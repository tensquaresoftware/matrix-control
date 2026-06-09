#pragma once

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Widgets/ModulationBusCell.h"

namespace TSS
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
    MatrixModulationPanel(TSS::ISkin& skin, const MatrixModulationPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts);
    ~MatrixModulationPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

private:
    void beginBusReorderDrag(int sourceBus);
    void updateBusReorderDrag(juce::Point<int> positionInPanel);
    void finishBusReorderDrag(juce::Point<int> positionInPanel);
    void clearBusReorderDragState();
    int findBusIndexAtPanelPosition(juce::Point<int> positionInPanel) const;
    void setDropTargetBus(std::optional<int> busIndex);

    std::optional<int> dragSourceBus_;
    std::optional<int> dropTargetBus_;
    BusReorderHandler busReorderHandler_;
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

    void createInitAllBussesButton(TSS::ISkin& skin);

    MatrixModulationPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    juce::AudioProcessorValueTreeState& apvts_;

    std::unique_ptr<TSS::SectionHeader> sectionHeader_;
    std::unique_ptr<TSS::ModulationBusHeader> modulationBusHeader_;
    std::unique_ptr<TSS::Button> initAllBussesButton_;
    std::vector<std::unique_ptr<ModulationBusCell>> modulationBuses_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixModulationPanel)
};
