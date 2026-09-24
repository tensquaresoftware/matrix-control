#include "Core/Actions/PatchManagerActionHandler.h"

#include "Core/Services/PatchFileService.h"

namespace Core
{
    bool PatchManagerActionHandler::confirmM1kpSiblingSyxOverwriteIfNeeded(
        const juce::File& originFile,
        const juce::File& writeTarget) const
    {
        if (! originFile.getFileExtension().equalsIgnoreCase(PatchFileService::kM1kpExtension)
            || ! writeTarget.existsAsFile())
            return true;

        return m1kpSiblingSyxOverwriteConfirmGate_ != nullptr
            && m1kpSiblingSyxOverwriteConfirmGate_(writeTarget.getFileName());
    }
} // namespace Core
