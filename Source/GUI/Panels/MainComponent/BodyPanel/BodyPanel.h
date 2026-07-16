#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_audio_processors/juce_audio_processors.h>

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Widgets/ModuleHeader.h"

namespace Core
{
    class PatchFileService;
}

namespace TSS
{
    class ISkin;
    class VerticalSeparator;
    class CompareLockBinder;
}

class PatchEditPanel;
class SharedPanel;
class MasterEditPanel;
class WidgetFactory;

class BodyPanel : public juce::Component
{
public:
    BodyPanel(TSS::ISkin& skin,
              const GuiLayoutDimensions& layoutDimensions,
              WidgetFactory& widgetFactory,
              juce::AudioProcessorValueTreeState& apvts,
              const Core::PatchFileService& patchFileService);
    ~BodyPanel() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    using BusReorderHandler = std::function<void(int fromBus, int toBus)>;

    void setBusReorderHandler(BusReorderHandler handler);

    void setMasterInitConfirmationGate(TSS::ModuleHeader::InitConfirmationGate gate);

private:
    BodyPanelDimensions dims_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<PatchEditPanel> patchEditPanel_;
    std::unique_ptr<TSS::VerticalSeparator> verticalSeparator1_;
    std::unique_ptr<SharedPanel> sharedPanel_;
    std::unique_ptr<TSS::VerticalSeparator> verticalSeparator2_;
    std::unique_ptr<MasterEditPanel> masterEditPanel_;
    std::unique_ptr<TSS::CompareLockBinder> compareLockBinder_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BodyPanel)
};
