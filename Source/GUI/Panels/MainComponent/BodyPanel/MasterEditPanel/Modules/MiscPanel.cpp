#include "MiscPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout MiscPanel::createLayout()
{
    return makeMasterEditModuleLayout(
        PluginIDs::MasterEditSection::MiscModule::kGroupId,
        PluginIDs::MasterEditSection::MiscModule::StandaloneWidgets::kInit,
        {
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTranspose,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBendRange,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kVolumeInvertEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kBankLockEnable,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMemoryProtectEnable,
            ""
        });
}

MiscPanel::MiscPanel(const Config& config)
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

void MiscPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::MasterEditSection::MiscModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitOnly(*contextualHelpBinder_, moduleHeader_.get(), Help::kInit);

    static constexpr const char* kCellHelps[] = {
        Help::kMasterTune,
        Help::kMasterTranspose,
        Help::kBendRange,
        Help::kUnison,
        Help::kVolumeInvert,
        Help::kBankLock,
        Help::kMemoryProtect,
    };
    TSS::bindParameterCellHelps(*contextualHelpBinder_, *this, kCellHelps, std::size(kCellHelps));
}

