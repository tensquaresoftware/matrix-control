#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Helpers/ApvtsUndoableParameterAttachments.h"

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
    struct Config
    {
        TSS::ISkin& skin;
        int width = 0;
        int height = 0;
        const ModulationBusCellDimensions& dimensions;
        int busNumber = 0;
        WidgetFactory& factory;
        juce::AudioProcessorValueTreeState& apvts;
        juce::String sourceParamId;
        juce::String amountParamId;
        juce::String destinationParamId;
        juce::String busId;
    };

    explicit ModulationBusCell(const Config& config);
    ~ModulationBusCell() override;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;
    void mouseUp(const juce::MouseEvent& e) override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);
    int getDesignPanelHeight() const { return dimensions_.panelHeight; }
    int getBusNumber() const noexcept { return busNumber_; }

    using ReorderDragBeginFn = std::function<void(int busNumber)>;
    using ReorderDragMoveFn = std::function<void(juce::Point<int> positionInPanel)>;
    using ReorderDragEndFn = std::function<void(juce::Point<int> positionInPanel)>;

    void setReorderDragCallbacks(ReorderDragBeginFn onBegin,
                                 ReorderDragMoveFn onMove,
                                 ReorderDragEndFn onEnd);
    void setDropTargetHighlighted(bool highlighted);
    void setDragSourceHighlighted(bool highlighted);

    TSS::Label* getBusNumberLabel() const { return busNumberLabel_.get(); }
    TSS::ComboBox* getSourceComboBox() const { return sourceComboBox_.get(); }
    TSS::Slider* getAmountSlider() const { return amountSlider_.get(); }
    TSS::ComboBox* getDestinationComboBox() const { return destinationComboBox_.get(); }
    TSS::Button* getInitButton() const { return initButton_.get(); }

private:
    bool isBusNumberLabelHit(juce::Point<int> localPosition) const;
    juce::Point<int> toPanelPosition(juce::Point<int> localPosition) const;
    void finishReorderDrag(juce::Point<int> localPosition);
    void updateBusNumberLabelWeight();

    int busNumber_ { 0 };
    bool reorderDragPending_ { false };
    bool reorderDragActive_ { false };
    bool dropTargetHighlighted_ { false };
    bool dragSourceHighlighted_ { false };
    juce::Point<int> dragStartPosition_;
    ReorderDragBeginFn onReorderDragBegin_;
    ReorderDragMoveFn onReorderDragMove_;
    ReorderDragEndFn onReorderDragEnd_;

    ModulationBusCellDimensions dimensions_;
    void createBusNumberLabel();
    void createSourceComboBox(WidgetFactory& factory, const juce::String& sourceParamId);
    void createAmountSlider(WidgetFactory& factory, const juce::String& amountParamId);
    void createDestinationComboBox(const juce::String& destinationParamId);
    void createInitButton();
    void createSeparator();

    void layoutWidgetRow();
    void layoutSeparator(int yTop, int separatorHeight);

    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::Label> busNumberLabel_;
    std::unique_ptr<TSS::ComboBox> sourceComboBox_;
    std::unique_ptr<TSS::Slider> amountSlider_;
    std::unique_ptr<TSS::ComboBox> destinationComboBox_;
    std::unique_ptr<TSS::Button> initButton_;
    std::unique_ptr<TSS::HorizontalSeparator> separator_;

    std::unique_ptr<TSS::ApvtsUndoableComboBoxAttachment> sourceAttachment_;
    std::unique_ptr<TSS::ApvtsUndoableSliderAttachment> amountAttachment_;
    std::unique_ptr<TSS::ApvtsUndoableComboBoxAttachment> destinationAttachment_;

    juce::AudioProcessorValueTreeState& apvts_;
    juce::String busId_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ModulationBusCell)
};
