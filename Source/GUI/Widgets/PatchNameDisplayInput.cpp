#include "PatchNameDisplay.h"

#include "Core/Services/PatchFileNameSanitizer.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    void PatchNameDisplay::insertCharacterAtCaret(juce::juce_wchar character)
    {
        if (editBuffer_.length() >= kNameLength_)
            return;

        editBuffer_ = editBuffer_.substring(0, caretIndex_)
            + juce::String::charToString(character)
            + editBuffer_.substring(caretIndex_);
        ++caretIndex_;
        caretIndex_ = juce::jmin(caretIndex_, maxCaretIndex());
        restartCaretBlink();
    }

    void PatchNameDisplay::deleteCharacterBeforeCaret()
    {
        if (editBuffer_.isEmpty())
            return;

        // At max length the caret sits on the last character (no 9th slot). Backspace
        // must delete that last character, not the one before it.
        if (editBuffer_.length() == kNameLength_ && caretIndex_ == kNameLength_ - 1)
        {
            deleteCharacterAtCaret();
            return;
        }

        if (caretIndex_ <= 0)
            return;

        editBuffer_ = editBuffer_.substring(0, caretIndex_ - 1)
            + editBuffer_.substring(caretIndex_);
        --caretIndex_;
        restartCaretBlink();
    }

    void PatchNameDisplay::deleteCharacterAtCaret()
    {
        if (caretIndex_ >= editBuffer_.length())
            return;

        editBuffer_ = editBuffer_.substring(0, caretIndex_)
            + editBuffer_.substring(caretIndex_ + 1);
        caretIndex_ = juce::jmin(caretIndex_, maxCaretIndex());
        restartCaretBlink();
    }

    void PatchNameDisplay::moveCaret(int delta)
    {
        caretIndex_ = juce::jlimit(0, maxCaretIndex(), caretIndex_ + delta);
        clearIllegalCharacterPending();
        restartCaretBlink();
    }

    void PatchNameDisplay::clearIllegalCharacterPending()
    {
        if (! illegalCharPending_)
            return;

        illegalCharPending_ = false;
        if (onIllegalCharacterCleared_)
            onIllegalCharacterCleared_();
    }

    void PatchNameDisplay::restartCaretBlink()
    {
        caretOn_ = true;
        stopTimer();

        if (nameRequiredArmed_ && ! dragOverlayActive_)
            startTimerHz(kDragSecondaryBlinkHz_);
        else
            startTimer(kCaretBlinkIntervalMs_);

        repaint();
    }

    void PatchNameDisplay::timerCallback()
    {
        if (dragOverlayActive_ || showsNameRequiredSecondary())
            blinkSecondaryVisible_ = ! blinkSecondaryVisible_;

        if (editing_)
            caretOn_ = ! caretOn_;

        repaint();
    }

    bool PatchNameDisplay::showsNameRequiredSecondary() const noexcept
    {
        return nameRequiredArmed_ && ! dragOverlayActive_;
    }

    juce::String PatchNameDisplay::activeSecondaryText() const
    {
        if (showsNameRequiredSecondary())
        {
            return PluginDisplayNames::PatchEditSection::PatchNameModule::NameRequired::kSecondaryLabel;
        }

        return secondaryLabel_;
    }

    void PatchNameDisplay::updateHoverFromPosition(juce::Point<float> position)
    {
        const bool wasHovered = hoveredPrimary_;

        if (! editable_ || editing_ || nameRequiredArmed_ || dragOverlayActive_)
        {
            hoveredPrimary_ = false;
        }
        else
        {
            const auto layout = computeTextBlockLayout(getLocalBounds().toFloat());
            hoveredPrimary_ = layout.primaryRow.contains(position);
        }

        if (hoveredPrimary_ != wasHovered)
            repaint();
    }

    void PatchNameDisplay::mouseDown(const juce::MouseEvent& e)
    {
        if (! editing_)
            return;

        // Outside the display rectangle: name-required cancels (pending STORE abort);
        // normal rename commits.
        if (getScreenBounds().contains(e.getScreenPosition()))
            return;

        if (nameRequiredArmed_)
            cancelEdit();
        else
            commitEdit();
    }

    void PatchNameDisplay::mouseDoubleClick(const juce::MouseEvent& e)
    {
        if (! editable_ || nameRequiredArmed_ || dragOverlayActive_)
            return;

        const auto layout = computeTextBlockLayout(getLocalBounds().toFloat());
        if (! layout.primaryRow.contains(e.position))
            return;

        beginEdit();
    }

    void PatchNameDisplay::mouseEnter(const juce::MouseEvent& e)
    {
        updateHoverFromPosition(e.position);
    }

    void PatchNameDisplay::mouseExit(const juce::MouseEvent&)
    {
        if (! hoveredPrimary_)
            return;

        hoveredPrimary_ = false;
        repaint();
    }

    void PatchNameDisplay::mouseMove(const juce::MouseEvent& e)
    {
        updateHoverFromPosition(e.position);
    }

    bool PatchNameDisplay::handleTypedCharacter(juce::juce_wchar rawCharacter)
    {
        if (rawCharacter == 0)
            return false; // non-printable key (Tab, function keys...) — let default handling occur

        const auto typedCharacter = juce::CharacterFunctions::toUpperCase(rawCharacter);

        if (! Core::PatchFileNameSanitizer::isAllowedMatrixChar(typedCharacter))
        {
            illegalCharPending_ = true;
            if (onIllegalCharacter_)
                onIllegalCharacter_();
            return true;
        }

        clearIllegalCharacterPending();
        insertCharacterAtCaret(typedCharacter);
        return true;
    }

    bool PatchNameDisplay::keyPressed(const juce::KeyPress& key)
    {
        if (! editing_)
            return false;

        if (key == juce::KeyPress::returnKey)
        {
            commitEdit();
            return true;
        }

        if (key == juce::KeyPress::escapeKey)
        {
            cancelEdit();
            return true;
        }

        if (key.getKeyCode() == juce::KeyPress::leftKey)
        {
            moveCaret(-1);
            return true;
        }

        if (key.getKeyCode() == juce::KeyPress::rightKey)
        {
            moveCaret(1);
            return true;
        }

        // Normal text-field editing: Backspace removes the char before the caret and
        // shifts the tail left; Delete removes at the caret. Cap remains 8 chars.
        if (key.getKeyCode() == juce::KeyPress::backspaceKey)
        {
            clearIllegalCharacterPending();
            deleteCharacterBeforeCaret();
            return true;
        }

        if (key.getKeyCode() == juce::KeyPress::deleteKey)
        {
            clearIllegalCharacterPending();
            deleteCharacterAtCaret();
            return true;
        }

        return handleTypedCharacter(key.getTextCharacter());
    }

    void PatchNameDisplay::focusLost(juce::Component::FocusChangeType)
    {
        // Drag overlay suspends the edit session — do not treat that as cancel/commit.
        if (dragOverlayActive_ || ! editing_)
            return;

        // Name-required: blur cancels (pending STORE abort). Normal rename: blur commits.
        if (nameRequiredArmed_)
            cancelEdit();
        else
            commitEdit();
    }
}
