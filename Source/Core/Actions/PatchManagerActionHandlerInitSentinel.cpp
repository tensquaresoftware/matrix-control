#include "Core/Actions/PatchManagerActionHandler.h"

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileNameSanitizer.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    juce::String PatchManagerActionHandler::resolveSuggestedSaveStem() const
    {
        const auto raw = apvts_.state.getProperty(
            PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
            juce::String()).toString();

        if (PatchFileNameSanitizer::isInitPatchNameSentinel(raw))
            return {};

        return PatchFileNameSanitizer::sanitizeFileStem(raw);
    }

    bool PatchManagerActionHandler::isInitPatchNameSentinelActive() const
    {
        const auto raw = apvts_.state.getProperty(
            PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
            juce::String()).toString();

        if (PatchFileNameSanitizer::isInitPatchNameSentinel(raw))
            return true;

        return patchModel_ != nullptr
            && PatchFileNameSanitizer::isInitPatchNameSentinel(patchModel_->getName());
    }

    bool PatchManagerActionHandler::refuseSaveIfInitSentinelActive()
    {
        if (! isInitPatchNameSentinelActive())
            return false;

        publishSaveFailureFooter(PluginDisplayNames::Settings::FooterMessages::kRenameBeforeSave);
        return true;
    }

    bool PatchManagerActionHandler::tryDeferStoreForInitNameRequired()
    {
        if (! isInitPatchNameSentinelActive())
            return false;

        if (hooks_.requestNameRequiredBeforeStore)
            hooks_.requestNameRequiredBeforeStore();

        return true;
    }

} // namespace Core
