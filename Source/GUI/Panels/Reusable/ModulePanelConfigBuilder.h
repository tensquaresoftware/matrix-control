#pragma once

#include <initializer_list>
#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Widgets/ParameterCell.h"

enum class ModulePanelButtonSet { InitCopyPaste, InitOnly };
enum class ModulePanelModuleType { PatchEdit, MasterEdit };

struct ModulePanelLayout
{
    juce::String moduleId;
    ModulePanelButtonSet buttonSet = ModulePanelButtonSet::InitCopyPaste;
    ModulePanelModuleType moduleType = ModulePanelModuleType::PatchEdit;
    juce::String initWidgetId;
    juce::String copyWidgetId;
    juce::String pasteWidgetId;
    std::vector<juce::String> orderedParameterIds;
};

struct ModulePanelConfig
{
    juce::String moduleId;
    ModulePanelButtonSet buttonSet;
    ModulePanelModuleType moduleType;
    juce::String initWidgetId;
    juce::String copyWidgetId;
    juce::String pasteWidgetId;

    struct ParameterConfig
    {
        juce::String parameterId;
        ParameterCell::ParameterType parameterType;
    };

    std::vector<ParameterConfig> parameters;
};

ModulePanelConfig buildModulePanelConfig(const ModulePanelLayout& layout);

ModulePanelLayout makePatchEditModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    const juce::String& copyId,
    const juce::String& pasteId,
    std::initializer_list<const char*> orderedParameterIds);

ModulePanelLayout makePatchEditInitOnlyModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    std::initializer_list<const char*> orderedParameterIds);

ModulePanelLayout makeMasterEditModuleLayout(
    const juce::String& moduleGroupId,
    const juce::String& initId,
    std::initializer_list<const char*> orderedParameterIds);
