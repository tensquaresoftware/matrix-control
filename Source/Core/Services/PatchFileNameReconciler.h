#pragma once

#include <functional>
#include <optional>

#include <juce_core/juce_core.h>

namespace Core
{

    class PatchModel;

    enum class NameReconciliationChoice
    {
        kInternal,
        kFilename
    };

    struct PatchNameReconciliationResult
    {
        juce::String resolvedName;
        bool hadMismatch = false;
        bool usedFilename = false;
        bool cancelled = false;
    };

    struct PatchFileNameReconciler
    {
        using Picker = std::function<std::optional<NameReconciliationChoice>(
            juce::String internalSanitized,
            juce::String fileSanitized)>;

        static PatchNameReconciliationResult reconcile(PatchModel& model,
                                                       const juce::String& fileStem,
                                                       int policy,
                                                       const Picker& picker);
    };

} // namespace Core
