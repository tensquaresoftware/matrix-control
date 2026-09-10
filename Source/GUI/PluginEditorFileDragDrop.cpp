#include "PluginEditor.h"

#include "Core/Services/PatchFileService.h"
#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/MainComponent.h"
#include "GUI/Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/Modules/PatchNameDisplayPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.h"
#include "GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace
{
    namespace FooterMessages = PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::FooterMessages;

    bool pathLooksLikeSyx(const juce::String& path) noexcept
    {
        return juce::File(path).getFileExtension().equalsIgnoreCase(Core::PatchFileService::kSyxExtension);
    }

    // Lightweight drag heuristic: any directory and/or any .syx → accept overlay (no deep scan).
    bool selectionLooksAcceptable(const juce::StringArray& files) noexcept
    {
        for (const auto& path : files)
        {
            const juce::File file(path);

            if (file.isDirectory() || pathLooksLikeSyx(path))
                return true;
        }

        return false;
    }

    bool isSingleNonDirectorySyx(const juce::StringArray& files) noexcept
    {
        if (files.size() != 1)
            return false;

        const juce::File file(files[0]);
        return ! file.isDirectory() && pathLooksLikeSyx(files[0]);
    }
}

PatchNameDisplayPanel* PluginEditor::getPatchNameDisplayPanelIfPresent()
{
    if (mainComponent_ == nullptr)
        return nullptr;

    return &mainComponent_->getBodyPanel()
                .getPatchEditPanel()
                .getPatchEditDisplaysPanel()
                .getPatchNameDisplayPanel();
}

void PluginEditor::updatePatchNameDragOverlay(const juce::StringArray& files)
{
    auto* panel = getPatchNameDisplayPanelIfPresent();
    if (panel == nullptr)
        return;

    if (files.isEmpty() || ! selectionLooksAcceptable(files))
    {
        // Plural only when the selection itself has 2+ unloadable items (not a single bad .syx).
        const auto invalidKind = files.size() >= 2
            ? PatchNameDisplayPanel::DragOverlayKind::kInvalidPlural
            : PatchNameDisplayPanel::DragOverlayKind::kInvalid;
        const char* junkSentinel = files.size() >= 2 ? "\x01junks" : "\x01junk";

        if (lastDragAssessedPath_ == junkSentinel)
            return;

        lastDragAssessedPath_ = junkSentinel;
        lastDragAssessedValid_ = false;
        lastDragAssessedPreview_.clear();
        panel->applyDragOverlay(invalidKind);
        return;
    }

    if (! isSingleNonDirectorySyx(files))
    {
        constexpr const char* kSelectionDragSentinel = "\x01selection";
        if (lastDragAssessedPath_ == kSelectionDragSentinel)
            return;

        lastDragAssessedPath_ = kSelectionDragSentinel;
        lastDragAssessedValid_ = true;
        lastDragAssessedPreview_.clear();
        panel->applyDragOverlay(PatchNameDisplayPanel::DragOverlayKind::kValidSelection);
        return;
    }

    const juce::String path = files[0];
    if (path == lastDragAssessedPath_)
        return;

    const auto assessment = pluginProcessor.getPatchFileService().assessSinglePatchSyxFile(
        juce::File(path));
    lastDragAssessedPath_ = path;
    lastDragAssessedValid_ = assessment.isValidSinglePatch;
    lastDragAssessedPreview_ = assessment.previewPrimaryName;
    panel->applyDragOverlay(
        assessment.isValidSinglePatch ? PatchNameDisplayPanel::DragOverlayKind::kValidSingle
                                      : PatchNameDisplayPanel::DragOverlayKind::kInvalid,
        lastDragAssessedPreview_);
}

void PluginEditor::clearPatchNameDragOverlay()
{
    lastDragAssessedPath_.clear();

    if (auto* panel = getPatchNameDisplayPanelIfPresent())
        panel->clearDragOverlay();
}

void PluginEditor::handleSyxFilesDropped(const juce::StringArray& files)
{
    clearPatchNameDragOverlay();

    if (files.isEmpty())
        return;

    if (! selectionLooksAcceptable(files))
    {
        TSS::GrayedControlHelper::setFooterWarningMessage(
            pluginProcessor.getApvts(), FooterMessages::kDropRejectedNotSyx);
        return;
    }

    pluginProcessor.loadDroppedComputerPatchSelection(files);
}

bool PluginEditor::isInterestedInFileDrag(const juce::StringArray& files)
{
    return ! files.isEmpty();
}

void PluginEditor::fileDragEnter(const juce::StringArray& files, int, int)
{
    updatePatchNameDragOverlay(files);
}

void PluginEditor::fileDragMove(const juce::StringArray& files, int, int)
{
    updatePatchNameDragOverlay(files);
}

void PluginEditor::fileDragExit(const juce::StringArray&)
{
    clearPatchNameDragOverlay();
}

void PluginEditor::filesDropped(const juce::StringArray& files, int, int)
{
    handleSyxFilesDropped(files);
}
