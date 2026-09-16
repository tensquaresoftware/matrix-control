#include "VibratoPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout VibratoPanel::createLayout()
{
    return makeMasterEditModuleLayout(
        PluginIDs::MasterEditSection::VibratoModule::kGroupId,
        PluginIDs::MasterEditSection::VibratoModule::StandaloneWidgets::kInit,
        {
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeed,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kWaveform,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmplitude,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModSource,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kSpeedModAmount,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModSource,
            PluginIDs::MasterEditSection::VibratoModule::ParameterWidgets::kAmpModAmount
        });
}

VibratoPanel::VibratoPanel(const Config& config)
    : BaseModulePanel(BaseModulePanel::Config{
          .skin = config.skin,
          .widgetFactory = config.widgetFactory,
          .apvts = config.apvts,
          .layout = createLayout(),
          .width = config.width,
          .height = config.height,
          .moduleHeaderDims = config.moduleHeaderDims,
          .parameterCellDims = config.parameterCellDims})
{
    registerContextualHelp();
}

void VibratoPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::MasterEditSection::VibratoModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitOnly(*contextualHelpBinder_, moduleHeader_.get(), Help::kInit);

    static constexpr const char* kCellHelps[] = {
        Help::kSpeed,
        Help::kWaveform,
        Help::kAmplitude,
        Help::kSpeedModSource,
        Help::kSpeedModAmount,
        Help::kAmpModSource,
        Help::kAmpModAmount,
    };
    TSS::bindParameterCellHelps(*contextualHelpBinder_, *this, kCellHelps, std::size(kCellHelps));
}

