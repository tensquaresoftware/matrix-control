#include "PatchNameDisplay.h"

#include "Core/Services/PatchFileNameSanitizer.h"
#include "Core/Services/PatchNameEditRules.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    PatchNameDisplay::PatchNameDisplay(int width, int height, const PatchNameDisplayLook& look)
        : look_(look)
        , width_(width)
        , height_(height)
        , patchName_(PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName)
    {
        setOpaque(false);
        setSize(width_, height_);
        setWantsKeyboardFocus(true);
    }

    PatchNameDisplay::~PatchNameDisplay()
    {
        stopTimer();
        detachOutsideClickListener();
    }

    void PatchNameDisplay::setLook(const PatchNameDisplayLook& look)
    {
        look_ = look;
        repaint();
    }

    void PatchNameDisplay::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void PatchNameDisplay::setPatchName(const juce::String& patchName)
    {
        if (dragOverlayActive_)
        {
            patchName_ = patchName;
            return;
        }

        patchName_ = patchName;
        repaint();
    }

    void PatchNameDisplay::setSecondaryLabel(const juce::String& secondaryLabel)
    {
        if (dragOverlayActive_)
        {
            secondaryLabel_ = secondaryLabel;
            return;
        }

        if (secondaryLabel_ == secondaryLabel)
            return;

        secondaryLabel_ = secondaryLabel;
        repaint();
    }

    void PatchNameDisplay::setEditable(bool editable)
    {
        if (editable_ == editable)
            return;

        editable_ = editable;

        if (! editable_)
        {
            hoveredPrimary_ = false;
            if (editing_)
                cancelEdit();
        }

        repaint();
    }

    void PatchNameDisplay::onCommit(std::function<void(juce::String)> callback)
    {
        onCommit_ = std::move(callback);
    }

    void PatchNameDisplay::onIllegalCharacter(std::function<void()> callback)
    {
        onIllegalCharacter_ = std::move(callback);
    }

    void PatchNameDisplay::onIllegalCharacterCleared(std::function<void()> callback)
    {
        onIllegalCharacterCleared_ = std::move(callback);
    }

    void PatchNameDisplay::onEditEnded(std::function<void()> callback)
    {
        onEditEnded_ = std::move(callback);
    }

    void PatchNameDisplay::beginEdit()
    {
        if (! editable_ || editing_ || dragOverlayActive_)
            return;

        // Fresh empty field — do not preload the current patch name.
        editBuffer_.clear();
        caretIndex_ = 0;
        caretOn_ = true;
        illegalCharPending_ = false;
        editing_ = true;

        grabKeyboardFocus();
        attachOutsideClickListener();
        startTimer(kCaretBlinkIntervalMs_);
        repaint();
    }

    void PatchNameDisplay::cancelEdit()
    {
        if (! editing_)
            return;

        stopTimer();
        detachOutsideClickListener();
        editing_ = false;
        illegalCharPending_ = false;
        repaint();

        if (onEditEnded_)
            onEditEnded_();
    }

    void PatchNameDisplay::commitEdit()
    {
        if (! editing_)
            return;

        const auto resolvedName = Core::PatchNameEditRules::resolveCommittedPatchName(editBuffer_, patchName_);

        stopTimer();
        detachOutsideClickListener();
        editing_ = false;
        illegalCharPending_ = false;
        repaint();

        if (onCommit_)
            onCommit_(resolvedName);

        if (onEditEnded_)
            onEditEnded_();
    }

    void PatchNameDisplay::attachOutsideClickListener()
    {
        if (listeningForOutsideClicks_)
            return;

        juce::Desktop::getInstance().addGlobalMouseListener(this);
        listeningForOutsideClicks_ = true;
    }

    void PatchNameDisplay::detachOutsideClickListener()
    {
        if (! listeningForOutsideClicks_)
            return;

        juce::Desktop::getInstance().removeGlobalMouseListener(this);
        listeningForOutsideClicks_ = false;
    }

    int PatchNameDisplay::maxCaretIndex() const noexcept
    {
        if (editBuffer_.isEmpty())
            return 0;

        if (editBuffer_.length() >= kNameLength_)
            return kNameLength_ - 1;

        return editBuffer_.length();
    }

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
        startTimer(kCaretBlinkIntervalMs_);
        repaint();
    }

    void PatchNameDisplay::timerCallback()
    {
        if (dragOverlayActive_)
        {
            dragSecondaryVisible_ = ! dragSecondaryVisible_;
            repaint();
            return;
        }

        caretOn_ = ! caretOn_;
        repaint();
    }

    void PatchNameDisplay::updateHoverFromPosition(juce::Point<float> position)
    {
        const bool wasHovered = hoveredPrimary_;

        if (! editable_ || editing_)
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

        // Commit when the click is outside this display rectangle (e.g. module header
        // or anywhere else in the UI). Clicks inside the afficheur keep editing.
        if (getScreenBounds().contains(e.getScreenPosition()))
            return;

        commitEdit();
    }

    void PatchNameDisplay::mouseDoubleClick(const juce::MouseEvent& e)
    {
        if (! editable_)
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

        const auto rawCharacter = key.getTextCharacter();
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

    void PatchNameDisplay::focusLost(juce::Component::FocusChangeType)
    {
        // Keyboard focus leaving the display also commits (e.g. Tab). Outside clicks are
        // handled by the global mouse listener against this component's screen bounds.
        commitEdit();
    }
}
