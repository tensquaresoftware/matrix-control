#include "ModulePanelConfigBuilder.h"

#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Exceptions/WidgetFactoryExceptions.h"

namespace
{
    ModulePanelLayout makeModuleLayout(
        const juce::String& moduleGroupId,
        ModulePanelButtonSet buttonSet,
        ModulePanelModuleType moduleType,
        const juce::String& initId,
        const juce::String& copyId,
        const juce::String& pasteId,
        std::initializer_list<const char*> orderedParameterIds)
    {
        ModulePanelLayout layout;
        layout.moduleId = moduleGroupId;
        layout.buttonSet = buttonSet;
        layout.moduleType = moduleType;
        layout.initWidgetId = initId;
        layout.copyWidgetId = copyId;
        layout.pasteWidgetId = pasteId;
        layout.orderedParameterIds.reserve(orderedParameterIds.size());

        for (const auto* parameterId : orderedParameterIds)
            layout.orderedParameterIds.emplace_back(parameterId);

        return layout;
    }
}

ModulePanelConfig buildModulePanelConfig(const ModulePanelLayout& layout)
{
    ModulePanelConfig config;
    config.moduleId = layout.moduleId;
    config.buttonSet = layout.buttonSet;
    config.moduleType = layout.moduleType;
    config.initWidgetId = layout.initWidgetId;
    config.copyWidgetId = layout.copyWidgetId;
    config.pasteWidgetId = layout.pasteWidgetId;
    config.parameters.reserve(layout.orderedParameterIds.size());

    for (const auto& parameterId : layout.orderedParameterIds)
    {
        if (parameterId.isEmpty())
        {
            config.parameters.push_back({ juce::String(), ParameterCell::ParameterType::None });
            continue;
        }

        const auto widgetKind = PluginHelpers::resolveParameterWidgetKind(parameterId);
        if (! widgetKind.has_value())
        {
            jassertfalse;
            throw ParameterNotFoundException(parameterId);
        }

        const auto parameterType = (*widgetKind == PluginHelpers::ParameterWidgetKind::Slider)
            ? ParameterCell::ParameterType::Slider
            : ParameterCell::ParameterType::ComboBox;

        config.parameters.push_back({ parameterId, parameterType });
    }

    return config;
}

ModulePanelLayout makePatchEditModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    const juce::String& copyId,
    const juce::String& pasteId,
    std::initializer_list<const char*> orderedParameterIds)
{
    return makeModuleLayout(
        moduleGroupId,
        ModulePanelButtonSet::InitCopyPaste,
        ModulePanelModuleType::PatchEdit,
        initId,
        copyId,
        pasteId,
        orderedParameterIds);
}

ModulePanelLayout makePatchEditInitOnlyModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    std::initializer_list<const char*> orderedParameterIds)
{
    return makeModuleLayout(
        moduleGroupId,
        ModulePanelButtonSet::InitOnly,
        ModulePanelModuleType::PatchEdit,
        initId,
        juce::String(),
        juce::String(),
        orderedParameterIds);
}

ModulePanelLayout makeMasterEditModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    std::initializer_list<const char*> orderedParameterIds)
{
    return makeModuleLayout(
        moduleGroupId,
        ModulePanelButtonSet::InitOnly,
        ModulePanelModuleType::MasterEdit,
        initId,
        juce::String(),
        juce::String(),
        orderedParameterIds);
}
