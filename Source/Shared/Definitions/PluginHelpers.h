#pragma once

#include <initializer_list>
#include <optional>

#include <juce_core/juce_core.h>

namespace PluginHelpers
{
    enum class ParameterWidgetKind { Slider, ComboBox };

    juce::StringArray makeStringArray(std::initializer_list<const char*> strings);
    juce::String getGroupDisplayName(const juce::String& groupId);
    juce::String getSectionDisplayName(const char* sectionId);
    std::optional<ParameterWidgetKind> resolveParameterWidgetKind(const juce::String& parameterId);
}
