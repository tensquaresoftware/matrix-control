#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Widgets/ModuleHeader.h"

namespace Core
{
    class PatchFileService;
}

namespace TSS
{
    class Skin;
}

class WidgetFactory;

class MainComponent : public juce::Component
{
public:
    MainComponent(TSS::Skin& skin,
                  const GuiLayoutDimensions& layoutDimensions,
                  WidgetFactory& widgetFactory,
                  juce::AudioProcessorValueTreeState& apvts,
                  const Core::PatchFileService& patchFileService);
    ~MainComponent() override = default;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::Skin& skin);
    void setUiScale(float uiScale);

    HeaderPanel& getHeaderPanel() { return headerPanel; }
    BodyPanel& getBodyPanel() { return bodyPanel; }
    FooterPanel& getFooterPanel() { return footerPanel; }

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

    void setMasterInitConfirmationGate(TSS::ModuleHeader::InitConfirmationGate gate);

#if JUCE_DEBUG
    void setUiElementsTestVisible(bool visible);
    juce::Rectangle<int> getUiElementsTestAreaBounds() const;
#endif

private:
    TSS::Skin* skin_;
    GuiLayoutDimensions layoutDimensions_;
    float uiScale_ = 1.0f;
#if JUCE_DEBUG
    bool uiElementsTestVisible_ = false;
    int uiElementsTestAreaY_ = 0;
#endif

    HeaderPanel headerPanel;
    BodyPanel bodyPanel;
    FooterPanel footerPanel;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainComponent)
};
