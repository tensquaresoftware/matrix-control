#include "Core/Services/PatchFileNameReconciler.h"

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    namespace
    {
        PatchNameReconciliationResult makeMatchResult(const juce::String& name)
        {
            PatchNameReconciliationResult result;
            result.resolvedName = name;
            return result;
        }

        PatchNameReconciliationResult makeMismatchResult(const juce::String& name, bool usedFilename)
        {
            PatchNameReconciliationResult result;
            result.resolvedName = name;
            result.hadMismatch = true;
            result.usedFilename = usedFilename;
            return result;
        }

        PatchNameReconciliationResult applyChoice(PatchModel& model,
                                                  const juce::String& internalSanitized,
                                                  const juce::String& fileSanitized,
                                                  NameReconciliationChoice choice)
        {
            if (choice == NameReconciliationChoice::kFilename)
            {
                model.setName(fileSanitized);
                return makeMismatchResult(fileSanitized, true);
            }

            model.setName(internalSanitized);
            return makeMismatchResult(internalSanitized, false);
        }

        PatchNameReconciliationResult makeCancelledResult()
        {
            PatchNameReconciliationResult cancelled;
            cancelled.cancelled = true;
            return cancelled;
        }

        PatchNameReconciliationResult reconcileAskOnce(PatchModel& model,
                                                     const juce::String& internalSanitized,
                                                     const juce::String& fileSanitized,
                                                     const PatchFileNameReconciler::Picker& picker)
        {
            if (! picker)
                return makeCancelledResult();

            const auto choice = picker(internalSanitized, fileSanitized);
            if (! choice.has_value())
                return makeCancelledResult();

            return applyChoice(model, internalSanitized, fileSanitized, *choice);
        }
    }

    PatchNameReconciliationResult PatchFileNameReconciler::reconcile(PatchModel& model,
                                                                     const juce::String& fileStem,
                                                                     int policy,
                                                                     const Picker& picker)
    {
        using namespace PluginIDs::Settings::NameReconciliationPolicy;

        const auto internalSanitized = PatchFileNameSanitizer::sanitizeToMatrixName(model.getName());
        const auto fileSanitized = PatchFileNameSanitizer::sanitizeFileStem(fileStem);

        if (internalSanitized.equalsIgnoreCase(fileSanitized))
            return makeMatchResult(internalSanitized);

        if (policy == kPreferFilename)
        {
            model.setName(fileSanitized);
            return makeMismatchResult(fileSanitized, true);
        }

        if (policy == kAskOncePerLoad)
            return reconcileAskOnce(model, internalSanitized, fileSanitized, picker);

        model.setName(internalSanitized);
        return makeMismatchResult(internalSanitized, false);
    }

} // namespace Core
