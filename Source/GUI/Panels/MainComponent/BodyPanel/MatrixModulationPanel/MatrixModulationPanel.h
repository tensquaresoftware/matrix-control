#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Panels/Reusable/ModulationBusPanel.h"

namespace tss
{
    class ISkin;
    class SectionHeader;
    class ModulationBusHeader;
    class Button;
}

class WidgetFactory;

class MatrixModulationPanel : public juce::Component
{
public:
    MatrixModulationPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MatrixModulationPanel() override;

    void resized() override;
    void setSkin(tss::ISkin& skin);
    void setScalingFactor(float scalingFactor);

private:
    struct ModulationBusParameterArrays
    {
        std::array<const char*, Matrix1000Limits::kModulationBusCount> busIds;
        std::array<const char*, Matrix1000Limits::kModulationBusCount> sourceParameterIds;
        std::array<const char*, Matrix1000Limits::kModulationBusCount> amountParameterIds;
        std::array<const char*, Matrix1000Limits::kModulationBusCount> destinationParameterIds;
    };

    std::array<const char*, Matrix1000Limits::kModulationBusCount> createBusIds() const;
    std::array<const char*, Matrix1000Limits::kModulationBusCount> createSourceParameterIds() const;
    std::array<const char*, Matrix1000Limits::kModulationBusCount> createAmountParameterIds() const;
    std::array<const char*, Matrix1000Limits::kModulationBusCount> createDestinationParameterIds() const;
    ModulationBusParameterArrays createModulationBusParameterArrays() const;

    void createInitAllBussesButton(tss::ISkin& skin);

    int width_;
    int height_;
    int modulationBusHeight_;
    tss::ISkin* skin_;
    float scalingFactor_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<tss::SectionHeader> sectionHeader_;
    std::unique_ptr<tss::ModulationBusHeader> modulationBusHeader_;
    std::unique_ptr<tss::Button> initAllBussesButton_;
    std::vector<std::unique_ptr<ModulationBusPanel>> modulationBuses_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixModulationPanel)
};

