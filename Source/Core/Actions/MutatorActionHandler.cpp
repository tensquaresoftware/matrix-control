#include "Core/Actions/MutatorActionHandler.h"

#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    MutatorActionHandler::MutatorActionHandler(juce::AudioProcessorValueTreeState& apvts,
                                               PatchMutatorEnginePort* engine,
                                               ExportFolderPicker pickExportFolder,
                                               DefragLimitModalGate showDefragLimitModal,
                                               int historySelectionDebounceMs)
        : apvts_(apvts)
        , engine_(engine)
        , pickExportFolder_(std::move(pickExportFolder))
        , showDefragLimitModal_(std::move(showDefragLimitModal))
        , historySelectionDebouncer_(historySelectionDebounceMs)
    {
    }

    void MutatorActionHandler::handleAction(const juce::String& propertyId, const juce::var&)
    {
        using namespace PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

        if (propertyId == kMutate)
            handleMutate();
        else if (propertyId == kRetry)
            handleRetry();
        else if (propertyId == kCompare)
            handleCompare();
        else if (propertyId == kDelete)
            handleDelete();
        else if (propertyId == kClear)
            handleClear();
        else if (propertyId == kExport)
            handleExport();
    }

    void MutatorActionHandler::onHistorySelectionChanged()
    {
        if (engine_ == nullptr)
            return;

        historySelectionDebouncer_.schedule([this]()
        {
            if (engine_ != nullptr)
                engine_->auditionSelectedHistoryEntry();
        });
    }

    void MutatorActionHandler::handleMutate()
    {
        if (engine_ == nullptr)
            return;

        handleEngineResult(engine_->mutate());
    }

    void MutatorActionHandler::handleRetry()
    {
        if (engine_ == nullptr)
            return;

        handleEngineResult(engine_->retry());
    }

    void MutatorActionHandler::handleCompare()
    {
        if (engine_ == nullptr)
            return;

        handleEngineResult(engine_->toggleCompare());
    }

    void MutatorActionHandler::handleDelete()
    {
        if (engine_ == nullptr)
            return;

        handleEngineResult(engine_->deleteSelected());
    }

    void MutatorActionHandler::handleClear()
    {
        if (engine_ == nullptr)
            return;

        handleEngineResult(engine_->clearHistory());
    }

    void MutatorActionHandler::handleExport()
    {
        if (engine_ == nullptr || ! pickExportFolder_)
            return;

        const juce::File folder = pickExportFolder_();
        if (! folder.isDirectory())
            return;

        handleEngineResult(engine_->exportHistory(folder));
    }

    void MutatorActionHandler::propagateFooterMessage(const juce::String& message,
                                                      const juce::String& severity)
    {
        apvts_.state.setProperty("uiMessageText", message, nullptr);
        apvts_.state.setProperty("uiMessageSeverity", severity, nullptr);
    }

    void MutatorActionHandler::handleEngineResult(const MutatorActionResult& result)
    {
        if (! result.footerMessage.isEmpty())
            propagateFooterMessage(result.footerMessage, result.footerSeverity);

        if (result.defragModalRequested && showDefragLimitModal_)
        {
            showDefragLimitModal_([this]()
            {
                if (engine_ == nullptr)
                    return;

                handleEngineResult(engine_->defragHistory());
            });
        }
    }

} // namespace Core
