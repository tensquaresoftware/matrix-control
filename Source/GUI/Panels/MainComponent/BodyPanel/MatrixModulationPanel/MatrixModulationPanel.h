#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/PluginDescriptors.h"
#include "Shared/PluginDimensions.h"
#include "GUI/Panels/Reusable/ModulationBusPanel.h"

namespace tss
{
    class Skin;
    class SectionHeader;
    class ModulationBusHeader;
    class Button;
}

class WidgetFactory;

class MatrixModulationPanel : public juce::Component
{
public:
    MatrixModulationPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MatrixModulationPanel() override;

    void resized() override;
    void setSkin(tss::Skin& skin);

    static int getWidth() { return PluginDimensions::Panels::Body::MatrixModulation::kWidth; }
    static int getHeight() { return PluginDimensions::Panels::Body::MatrixModulation::kHeight; }

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

    void createInitAllBussesButton(tss::Skin& skin);

    tss::Skin* skin_;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<tss::SectionHeader> sectionHeader_;
    std::unique_ptr<tss::ModulationBusHeader> modulationBusHeader_;
    std::unique_ptr<tss::Button> initAllBussesButton_;
    std::vector<std::unique_ptr<ModulationBusPanel>> modulationBuses_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixModulationPanel)
};

