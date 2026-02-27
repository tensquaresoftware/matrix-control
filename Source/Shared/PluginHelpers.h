#pragma once

#include <initializer_list>

#include <juce_core/juce_core.h>

namespace PluginHelpers
{
    juce::StringArray makeStringArray(std::initializer_list<const char*> strings);
    juce::String getGroupDisplayName(const juce::String& groupId);
    juce::String getSectionDisplayName(const char* sectionId);
}
