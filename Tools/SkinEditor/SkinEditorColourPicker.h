#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "Tools/SkinEditor/SkinEditorChromeTokens.h"

/** Photoshop-style SV square + vertical hue + #hex + R/G/B. */
class SkinEditorColourPicker final : public juce::Component,
                                     private juce::TextEditor::Listener
{
public:
    SkinEditorColourPicker();

    /**
     * Panel height at `panelWidth` so the gap under #/R/G/B matches the gap above
     * them (kPanelContentPad), with the SV well sized from content width.
     * Flush to status (no trailing panel gutter).
     */
    static int preferredHeightForWidth(int panelWidth) noexcept
    {
        constexpr int kFieldRowH = 24;
        constexpr int kHueBarW = 18;
        constexpr int kSvHueGap = 8;
        const int contentW = panelWidth - 2 * SkinEditorChrome::kPanelContentPad;
        const int side = juce::jmax(0, contentW - kHueBarW - kSvHueGap);
        return SkinEditorChrome::kGutter + SkinEditorChrome::kPanelTitleHeight
               + SkinEditorChrome::kPanelContentPad + side + SkinEditorChrome::kPanelContentPad
               + kFieldRowH + SkinEditorChrome::kPanelContentPad;
    }

    void setColour(juce::Colour colour, juce::NotificationType notification);
    /** Idle SV/hue wells (#151515) and empty #/R/G/B until a Palette colour is selected. */
    void clearActiveColour();
    juce::Colour getColour() const noexcept { return colour_; }
    void setEnabledEditing(bool canEdit);

    std::function<void(juce::Colour)> onColourChanged;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    void textEditorReturnKeyPressed(juce::TextEditor&) override;
    void textEditorFocusLost(juce::TextEditor&) override;

    void syncEditorsFromColour();
    void clearEditors();
    void paintSvWell(juce::Graphics& g) const;
    void paintHueBar(juce::Graphics& g) const;
    void applyHexFromEditor();
    void applyRgbFromEditors();
    void updateFromSvDrag(juce::Point<float> pos);
    void updateFromHueDrag(juce::Point<float> pos);
    void applyHsvaFromDrag();
    void notifyChanged();

    juce::Colour colour_ { juce::Colours::black };
    float hue_ = 0.0f;
    float sat_ = 0.0f;
    float val_ = 0.0f;
    bool hasActiveColour_ = false;
    bool isSyncingEditors_ = false;
    bool editingEnabled_ = false;

    juce::Rectangle<int> svBounds_;
    juce::Rectangle<int> hueBounds_;
    juce::Rectangle<int> titleTabBounds_;

    juce::Label titleLabel_;
    juce::Label hexLabel_ { {}, "#" };
    juce::TextEditor hexEditor_;
    juce::Label rLabel_ { {}, "R" };
    juce::Label gLabel_ { {}, "G" };
    juce::Label bLabel_ { {}, "B" };
    juce::TextEditor rEditor_;
    juce::TextEditor gEditor_;
    juce::TextEditor bEditor_;
};
