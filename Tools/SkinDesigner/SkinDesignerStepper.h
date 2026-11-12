#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

/** Horizontal prev/next stepper: one silhouette, two hit cells, filled chevrons. */
class SkinDesignerStepper final : public juce::Component,
                                  public juce::SettableTooltipClient
{
public:
    /** Slightly narrower than two glued 24px square buttons (48). */
    static constexpr int kPreferredWidth = 40;
    static constexpr int kPreferredHeight = 24; // SkinDesignerChrome::kButtonHeight

    std::function<void()> onDecrement;
    std::function<void()> onIncrement;

    void setDecrementEnabled(bool enabled);
    void setIncrementEnabled(bool enabled);
    void setDecrementTooltip(const juce::String& tip);
    void setIncrementTooltip(const juce::String& tip);

    bool isDecrementEnabled() const noexcept { return decrementEnabled_; }
    bool isIncrementEnabled() const noexcept { return incrementEnabled_; }

    void paint(juce::Graphics& g) override;
    void mouseMove(const juce::MouseEvent& e) override;
    void mouseEnter(const juce::MouseEvent& e) override;
    void mouseExit(const juce::MouseEvent& e) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

private:
    enum class Half
    {
        none,
        decrement,
        increment
    };

    Half halfAt(juce::Point<int> localPos) const noexcept;
    void setHover(Half half);
    void refreshTooltip(Half half);
    juce::Colour cellFill(Half half, bool enabled) const noexcept;
    void fillCell(juce::Graphics& g, juce::Rectangle<float> bounds, Half half, bool enabled) const;
    void drawChevron(juce::Graphics& g, juce::Rectangle<float> cell, bool pointLeft, bool enabled) const;

    bool decrementEnabled_ = true;
    bool incrementEnabled_ = true;
    juce::String decrementTooltip_;
    juce::String incrementTooltip_;
    Half hover_ = Half::none;
    Half pressed_ = Half::none;
};
