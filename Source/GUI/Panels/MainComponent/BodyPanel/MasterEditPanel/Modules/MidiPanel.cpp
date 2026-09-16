#include "MidiPanel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout MidiPanel::createLayout()
{
    return makeMasterEditModuleLayout(
        PluginIDs::MasterEditSection::MidiModule::kGroupId,
        PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
        {
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kMidiEcho,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kControllers,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPatchChanges,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal1Select,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kPedal2Select,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever2Select,
            PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kLever3Select
        });
}

MidiPanel::MidiPanel(const Config& config)
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

void MidiPanel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::MasterEditSection::MidiModule::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitOnly(*contextualHelpBinder_, moduleHeader_.get(), Help::kInit);

    static constexpr const char* kCellHelps[] = {
        Help::kChannel,
        Help::kMidiEcho,
        Help::kControllers,
        Help::kPatchChanges,
        Help::kPedal1Select,
        Help::kPedal2Select,
        Help::kLever2Select,
        Help::kLever3Select,
    };
    TSS::bindParameterCellHelps(*contextualHelpBinder_, *this, kCellHelps, std::size(kCellHelps));
}

