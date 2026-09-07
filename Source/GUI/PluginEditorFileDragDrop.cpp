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

    if (files.size() != 1)
    {
        lastDragAssessedPath_.clear();
        panel->applyDragOverlay(false, {});
        return;
    }

    const juce::String path = files[0];
    if (path == lastDragAssessedPath_)
    {
        panel->applyDragOverlay(lastDragAssessedValid_, lastDragAssessedPreview_);
        return;
    }

    const auto assessment = pluginProcessor.getPatchFileService().assessSinglePatchSyxFile(
        juce::File(path));
    lastDragAssessedPath_ = path;
    lastDragAssessedValid_ = assessment.isValidSinglePatch;
    lastDragAssessedPreview_ = assessment.previewPrimaryName;
    panel->applyDragOverlay(lastDragAssessedValid_, lastDragAssessedPreview_);
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

    if (files.size() != 1)
    {
        TSS::GrayedControlHelper::setFooterWarningMessage(
            pluginProcessor.getApvts(), FooterMessages::kDropRejectedMultiFile);
        return;
    }

    pluginProcessor.loadDroppedComputerPatchFile(juce::File(files[0]));
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
