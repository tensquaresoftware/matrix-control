#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "Tools/SkinEditor/SkinEditorChromeTokens.h"

/** Photoshop-style SV square + vertical hue + #hex + R/G/B + S/B sliders. */
class SkinEditorColourPicker final : public juce::Component,
                                     private juce::TextEditor::Listener,
                                     private juce::Slider::Listener
{
public:
    SkinEditorColourPicker();
    ~SkinEditorColourPicker() override;

    /**
     * Panel height at `panelWidth` so the SV→#/R/G/B gap matches kPanelContentPad,
     * with the SV well sized from width. Gap under #/R/G/B and bottom content pad
     * are each 5px tighter than kPanelContentPad (smoke). Flush to status.
     */
    static int preferredHeightForWidth(int panelWidth) noexcept
    {
        constexpr int kFieldRowH = 24;
        constexpr int kHueBarW = 18;
        constexpr int kSvHueGap = 8;
        const int contentW = panelWidth - 2 * SkinEditorChrome::kPanelContentPad;
        const int side = juce::jmax(0, contentW - kHueBarW - kSvHueGap);
        // #/RGB, then Saturation + Brightness stacked with no inter-row gap.
        // Gap under #/R/G/B and bottom pad: 5px tighter than kPanelContentPad (smoke).
        constexpr int kTightPad = SkinEditorChrome::kPanelContentPad - 5;
        return SkinEditorChrome::kGutter + SkinEditorChrome::kPanelTitleHeight
               + SkinEditorChrome::kPanelContentPad + side
               + SkinEditorChrome::kPanelContentPad + kFieldRowH
               + kTightPad + 2 * kFieldRowH
               + kTightPad;
    }

    void setColour(juce::Colour colour, juce::NotificationType notification);
    /** Idle SV/hue wells (#151515) and empty #/R/G/B until a Palette colour is selected. */
    void clearActiveColour();
    juce::Colour getColour() const noexcept { return colour_; }
    void setEnabledEditing(bool canEdit);

    std::function<void(juce::Colour)> onColourChanged;
    /** Continuous SV / Hue / S/B drag started — parent may coalesce expensive refresh. */
    std::function<void()> onColourGestureBegan;
    /** Continuous drag ended — parent should flush coalesced refresh/push. */
    std::function<void()> onColourGestureEnded;
    /** Escape while COLOR has focus - clear mapping / editing mode in main. */
    std::function<bool()> onEscapeRequested;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    enum class DragTarget
    {
        kNone,
        kSv,
        kHue
    };

    struct EscapeKeyForwarder final : public juce::KeyListener
    {
        std::function<bool()>* handler = nullptr;
        bool keyPressed(const juce::KeyPress& key, juce::Component*) override
        {
            if (key == juce::KeyPress::escapeKey && handler != nullptr && *handler != nullptr)
                return (*handler)();
            return false;
        }
    };

    void textEditorReturnKeyPressed(juce::TextEditor&) override;
    void textEditorFocusLost(juce::TextEditor&) override;
    void sliderValueChanged(juce::Slider*) override;
    void sliderDragStarted(juce::Slider*) override;
    void sliderDragEnded(juce::Slider*) override;

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
    void endWellDragIfActive();
    void endSliderGestureIfActive();
    void styleAxisSlider(juce::Slider& slider);
    void layoutHexRgbRow(int rowY);
    void layoutAxisRows(int rowY);

    juce::Colour colour_ { juce::Colours::black };
    float hue_ = 0.0f;
    float sat_ = 0.0f;
    float val_ = 0.0f;
    bool hasActiveColour_ = false;
    bool isSyncingEditors_ = false;
    bool editingEnabled_ = false;
    bool sliderGestureActive_ = false;
    DragTarget dragTarget_ = DragTarget::kNone;

    juce::Rectangle<int> svBounds_;
    juce::Rectangle<int> hueBounds_;
    juce::Rectangle<int> titleTabBounds_;

    EscapeKeyForwarder escapeForwarder_;

    juce::Label titleLabel_;
    juce::Label hexLabel_ { {}, "#" };
    juce::TextEditor hexEditor_;
    juce::Label rLabel_ { {}, "R" };
    juce::Label gLabel_ { {}, "G" };
    juce::Label bLabel_ { {}, "B" };
    juce::TextEditor rEditor_;
    juce::TextEditor gEditor_;
    juce::TextEditor bEditor_;
    juce::Label saturationLabel_ { {}, "Saturation:" };
    juce::Label brightnessLabel_ { {}, "Brightness:" };
    juce::Slider saturationSlider_;
    juce::Slider brightnessSlider_;
};
