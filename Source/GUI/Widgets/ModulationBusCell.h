#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Widgets/ModulationBusCellDimensions.h"

namespace TSS
{
    class ISkin;
    class Label;
    class ComboBox;
    class Slider;
    class Button;
    class HorizontalSeparator;
}

class WidgetFactory;

class ModulationBusCell : public juce::Component
{
public:
    ModulationBusCell(TSS::ISkin& skin,
                      int width,
                      int height,
                      const ModulationBusCellDimensions& dimensions,
                      int busNumber,
                      WidgetFactory& factory,
                      juce::AudioProcessorValueTreeState& apvts,
                      const juce::String& sourceParamId,
                      const juce::String& amountParamId,
                      const juce::String& destinationParamId,
                      const juce::String& busId);
    ~ModulationBusCell() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    int getHeight() const { return dimensions_.panelHeight; }
    int getBusNumber() const noexcept { return busNumber_; }

    using ReorderDragBeginFn = std::function<void(int busNumber)>;
    using ReorderDragMoveFn = std::function<void(juce::Point<int> positionInPanel)>;
    using ReorderDragEndFn = std::function<void(juce::Point<int> positionInPanel)>;

    void setReorderDragCallbacks(ReorderDragBeginFn onBegin,
                                 ReorderDragMoveFn onMove,
                                 ReorderDragEndFn onEnd);
    void setDropTargetHighlighted(bool highlighted);
    void setDragSourceHighlighted(bool highlighted);

private:
    bool isBusNumberLabelHit(juce::Point<int> localPosition) const;
    juce::Point<int> toPanelPosition(juce::Point<int> localPosition) const;
    void finishReorderDrag(juce::Point<int> localPosition);

    int busNumber_ { 0 };
    bool reorderDragPending_ { false };
    bool reorderDragActive_ { false };
    bool dropTargetHighlighted_ { false };
    bool dragSourceHighlighted_ { false };
    juce::Point<int> dragStartPosition_;
    ReorderDragBeginFn onReorderDragBegin_;
    ReorderDragMoveFn onReorderDragMove_;
    ReorderDragEndFn onReorderDragEnd_;

    inline constexpr static float kReorderDragThresholdPx_ = 4.0f;
    ModulationBusCellDimensions dimensions_;
    void createBusNumberLabel(int busNumber, TSS::ISkin& skin);
    void createSourceComboBox(WidgetFactory& factory, TSS::ISkin& skin, const juce::String& sourceParamId, juce::AudioProcessorValueTreeState& apvts);
    void createAmountSlider(WidgetFactory& factory, TSS::ISkin& skin, const juce::String& amountParamId, juce::AudioProcessorValueTreeState& apvts);
    void createDestinationComboBox(int busNumber, TSS::ISkin& skin, const juce::String& destinationParamId, juce::AudioProcessorValueTreeState& apvts);
    void createInitButton(TSS::ISkin& skin, int busNumber);
    void createSeparator(TSS::ISkin& skin);

    void layoutWidgetRow();
    void layoutSeparator(int yTop, int separatorHeight);

    inline constexpr static int kGap_ = 5;

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::Label> busNumberLabel_;
    std::unique_ptr<TSS::ComboBox> sourceComboBox_;
    std::unique_ptr<TSS::Slider> amountSlider_;
    std::unique_ptr<TSS::ComboBox> destinationComboBox_;
    std::unique_ptr<TSS::Button> initButton_;
    std::unique_ptr<TSS::HorizontalSeparator> separator_;

    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> sourceAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> amountAttachment_;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment> destinationAttachment_;

    juce::AudioProcessorValueTreeState& apvts_;
    juce::String busId_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationBusCell)
};
