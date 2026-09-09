#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Looks/WidgetLooks.h"

namespace TSS
{
    // Paint-only Patch Name display + inline rename editor. Deliberately NOT a JUCE
    // TextEditor overlay: custom caret paint, but editing semantics match a normal
    // text field (insert / backspace-shift / delete-shift) capped at 8 Matrix chars.
    //
    // Presentation priority (resolved in paint — not a Settings name-source mode):
    // drag overlay > name-required > Mutator/Compare secondary > idle primary.
    // Do not confuse with Core::PatchNameDisplay (musical vs hardware name source).
    class PatchNameDisplay : public juce::Component,
                             private juce::Timer
    {
    public:
        explicit PatchNameDisplay(int width, int height, const PatchNameDisplayLook& look);
        ~PatchNameDisplay() override;

        void setLook(const PatchNameDisplayLook& look);
        void setUiScale(float uiScale);

        // Primary line — the user patch name (8 chars max, display form).
        void setPatchName(const juce::String& patchName);

        // Secondary line — Mxx / Mxx-Ryy / INITIAL. Empty = single centred name (no history/Compare).
        // Suppressed while name-required is armed (restored when cleared).
        void setSecondaryLabel(const juce::String& secondaryLabel);

        // Gates the double-click affordance (ROM banks / Compare / etc. are not editable).
        void setEditable(bool editable);
        bool isEditable() const noexcept { return editable_; }

        void beginEdit();
        // Abandon: restores the previous name display, no commit callback fires.
        // Name-required: clears the mode and fires the cancel outcome.
        void cancelEdit();
        bool isEditing() const noexcept { return editing_; }

        // Drag-drop overlay: fixed primary, blinking secondary at 2 Hz. Does not touch APVTS.
        // Wins over name-required while active; clearing restores name-required if still armed.
        void showDragOverlay(const juce::String& primaryText, const juce::String& secondaryText);
        void clearDragOverlay();
        bool isDragOverlayActive() const noexcept { return dragOverlayActive_; }

        // Name-required (STORE gate UI): empty L1 + caret, blinking L2 "NAME REQUIRED".
        // Consumer arms/clears; does not require editable_ (STORE follow-up owns when to arm).
        void armNameRequired();
        void clearNameRequired();
        bool isNameRequiredArmed() const noexcept { return nameRequiredArmed_; }

        void onCommit(std::function<void(juce::String)> callback);
        void onIllegalCharacter(std::function<void()> callback);
        // Fires when a pending illegal-character footer should clear mid-edit
        // (valid key, arrow move, backspace/delete) — not an edit-session end.
        void onIllegalCharacterCleared(std::function<void()> callback);
        // Fires once per edit session end (commit or cancel) — panel uses this to clear
        // any footer error message it may have shown for an illegal character.
        void onEditEnded(std::function<void()> callback);
        // Name-required only: true = trim-non-empty commit; false = cancel or empty Return.
        void onNameRequiredOutcome(std::function<void(bool success)> callback);

        void paint(juce::Graphics& g) override;
        void mouseDown(const juce::MouseEvent& e) override;
        void mouseDoubleClick(const juce::MouseEvent& e) override;
        void mouseEnter(const juce::MouseEvent& e) override;
        void mouseExit(const juce::MouseEvent& e) override;
        void mouseMove(const juce::MouseEvent& e) override;
        bool keyPressed(const juce::KeyPress& key) override;
        void focusLost(juce::Component::FocusChangeType cause) override;

        int getWidth() const { return width_; }
        int getHeight() const { return height_; }

    private:
        struct TextBlockLayout
        {
            juce::Rectangle<float> primaryRow;
            juce::Rectangle<float> secondaryRow; // empty when single-line
            bool hasSecondary = false;
        };

        inline constexpr static int kBorderThickness_ = 2;
        inline constexpr static int kNameLength_ = 8;
        inline constexpr static int kCaretBlinkIntervalMs_ = 500;
        // Design-space gaps (scaled by uiScale_ at paint time).
        inline constexpr static float kDualLineGap_ = 2.0f;
        // Edit slots stay close to natural string pitch (tiny gap + minimal caret pad).
        inline constexpr static float kSlotGap_ = 0.0f;
        inline constexpr static float kCaretPadX_ = 1.0f;
        inline constexpr static float kCaretPadY_ = 1.0f;
        // Font metrics leave optical slack under the glyph — trim caret bottom so the
        // letter reads centred in the red rect.
        inline constexpr static float kCaretBottomTrim_ = 3.0f;
        inline constexpr static float kPrimaryIdleAlpha_ = 0.80f;
        inline constexpr static float kPrimaryHoverAlpha_ = 1.00f;
        inline constexpr static int kDragSecondaryBlinkHz_ = 2;

        PatchNameDisplayLook look_{};
        int width_;
        int height_;
        juce::String patchName_;
        juce::String secondaryLabel_;
        float uiScale_ = 1.0f;

        bool editable_ = false;
        bool editing_ = false;
        bool hoveredPrimary_ = false;
        bool listeningForOutsideClicks_ = false;
        bool illegalCharPending_ = false;
        bool dragOverlayActive_ = false;
        bool nameRequiredArmed_ = false;
        bool blinkSecondaryVisible_ = true;
        juce::String dragOverlayPrimary_;
        juce::String dragOverlaySecondary_;
        juce::String editBuffer_;
        int caretIndex_ = 0;
        bool caretOn_ = true;

        std::function<void(juce::String)> onCommit_;
        std::function<void()> onIllegalCharacter_;
        std::function<void()> onIllegalCharacterCleared_;
        std::function<void()> onEditEnded_;
        std::function<void(bool)> onNameRequiredOutcome_;

        void timerCallback() override;

        void commitEdit();
        void insertCharacterAtCaret(juce::juce_wchar character);
        void deleteCharacterBeforeCaret();
        void deleteCharacterAtCaret();
        void moveCaret(int delta);
        void restartCaretBlink();
        void clearIllegalCharacterPending();
        bool handleTypedCharacter(juce::juce_wchar rawCharacter);
        void updateHoverFromPosition(juce::Point<float> position);
        void attachOutsideClickListener();
        void detachOutsideClickListener();
        // Past-the-end caret is allowed only while length < 8 (room to type). At max
        // length the caret parks on the last real character — never a 9th slot.
        int maxCaretIndex() const noexcept;

        void enterNameRequiredEditSession();
        void suspendEditForDragOverlay();
        void endEditSessionVisuals();
        void notifyNameRequiredOutcome(bool success);
        bool showsNameRequiredSecondary() const noexcept;
        juce::String activeSecondaryText() const;

        TextBlockLayout computeTextBlockLayout(juce::Rectangle<float> bounds) const;
        juce::Colour primaryTextColour() const;
        juce::Font scaledPrimaryFont() const;
        juce::Font scaledSecondaryFont() const;

        void drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds);
        void drawNameSlots(juce::Graphics& g, const juce::Rectangle<float>& rowBounds);
        void drawEditingNameSlots(juce::Graphics& g,
                                  const juce::Rectangle<float>& rowBounds,
                                  const juce::Font& scaledFont);
        void paintEditSlotCharacter(juce::Graphics& g,
                                    const juce::Rectangle<float>& bounds,
                                    const juce::String& character,
                                    bool caretHere);
        void drawSecondaryText(juce::Graphics& g, const juce::Rectangle<float>& rowBounds);

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchNameDisplay)
    };
}
