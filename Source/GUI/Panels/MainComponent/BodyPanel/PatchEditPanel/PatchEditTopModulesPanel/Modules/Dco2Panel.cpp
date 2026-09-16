#include "Dco2Panel.h"

#include "GUI/Helpers/ContextualHelpBindingSupport.h"
#include "Shared/Definitions/PluginDisplayNames.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"


ModulePanelLayout Dco2Panel::createLayout()
{
    return makePatchEditModuleLayout(
        PluginIDs::PatchEditSection::Dco2Module::kGroupId,
        {
            .initWidgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kInit,
            .copyWidgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kCopy,
            .pasteWidgetId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPaste,
            .pasteEnabledPropertyId = PluginIDs::PatchEditSection::Dco2Module::StandaloneWidgets::kPasteEnabled
        },
        {
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequencyModByLfo1,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kDetune,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidth,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kPulseWidthModByLfo2,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveShape,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kWaveSelect,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kLevers,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyboardPortamento,
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kKeyClick
        });
}

Dco2Panel::Dco2Panel(const Config& config)
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

void Dco2Panel::registerContextualHelp()
{
    namespace Help = PluginDisplayNames::PatchEditSection::Dco2Module::ContextualHelp;

    contextualHelpBinder_ = std::make_unique<TSS::ContextualHelpBinder>(
        TSS::makeMainComponentFooterResolver(*this));

    TSS::bindModuleHeaderInitCopyPaste(*contextualHelpBinder_,
                                        moduleHeader_.get(),
                                        { Help::kInit, Help::kCopy, Help::kPaste });

    static constexpr const char* kCellHelps[] = {
        Help::kFrequency,
        Help::kFrequencyModByLfo1,
        Help::kDetune,
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

