#include "FmTrackPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout FmTrackPanel::createLayout()
{
    return makePatchEditInitOnlyModuleLayout(
        PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
        PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
        {
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByEnv3,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmModByPressure,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5,
            PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput,
            ""
        });
}

FmTrackPanel::FmTrackPanel(const Config& config)
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

void FmTrackPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::PatchEditSection::FmTrackModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitOnly(*contextualHelpBinder_, moduleHeader_.get(), Help::kInit);

    static constexpr const char* kCellHelps[] = {
        Help::kFmAmount,
        Help::kFmModByEnv3,
        Help::kFmModByPressure,
        Help::kTrackPoint1,
        Help::kTrackPoint2,
        Help::kTrackPoint3,
        Help::kTrackPoint4,
        Help::kTrackPoint5,
        Help::kTrackInput,
        nullptr,
    };
    TSS::bindParameterCellHelps(*contextualHelpBinder_, *this, kCellHelps, std::size(kCellHelps));
}

