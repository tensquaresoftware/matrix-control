#include "PluginHelpers.h"

#include "PluginDescriptors.h"

namespace PluginHelpers
{
    juce::StringArray makeStringArray(std::initializer_list<const char*> strings)
    {
        juce::StringArray result;
        for (const char* str : strings)
        {
            result.add(str);
        }
        return result;
    }

    juce::String getGroupDisplayName(const juce::String& groupId)
    {
        for (const auto& group : PluginDescriptors::kAllApvtsGroups)
        {
            if (group.groupId == groupId)
            {
                return group.displayName;
            }
        }
        return groupId;
    }

    juce::String getSectionDisplayName(const char* sectionId)
    {
        return getGroupDisplayName(sectionId);
    }
}
