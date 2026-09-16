#include "Dco1Panel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout Dco1Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Dco1Module::kGroupId,
        {
            .initWidgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
            .copyWidgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
            .pasteWidgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
            .pasteEnabledPropertyId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPasteEnabled
        },
        {
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequencyModByLfo1,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidth,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kPulseWidthModByLfo2,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveShape,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kLevers,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyboardPortamento,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kKeyClick
        });
}

Dco1Panel::Dco1Panel(const Config& config)
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

void Dco1Panel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::PatchEditSection::Dco1Module::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitCopyPaste(*contextualHelpBinder_,
                                        moduleHeader_.get(),
                                        { Help::kInit, Help::kCopy, Help::kPaste });

    static constexpr const char* kCellHelps[] = {
        Help::kFrequency,
        Help::kFrequencyModByLfo1,
        Help::kSync,
        Help::kPulseWidth,
        Help::kPulseWidthModByLfo2,
        Help::kWaveShape,
        Help::kWaveSelect,
        Help::kLevers,
        Help::kKeyboardPortamento,
        Help::kKeyClick,
    };
    TSS::bindParameterCellHelps(*contextualHelpBinder_, *this, kCellHelps, std::size(kCellHelps));
}

