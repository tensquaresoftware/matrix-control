#include "PluginEditor.h"

#include <array>
#include <memory>

#include "Core/MIDI/MidiManager.h"
#include "Core/Models/MasterModel.h"
#include "Core/Services/MasterFileAssess.h"
#include "Core/Services/MasterM1kmCodec.h"
#include "Core/Services/MasterM1kmLoadPolicy.h"
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
    namespace MasterFooter = PluginDisplayNames::Settings::FooterMessages;

    struct DragOverlayCache
    {
        juce::String& path;
        bool& valid;
        juce::String& preview;
    };

    bool pathLooksLikePatchFile(const juce::String& path) noexcept
    {
        return Core::PatchFileService::hasSupportedPatchExtension(juce::File(path));
    }

    // Lightweight drag heuristic: any directory and/or any .syx/.m1kp → accept overlay (no deep scan).
    bool selectionLooksAcceptable(const juce::StringArray& files) noexcept
    {
        for (const auto& path : files)
        {
            const juce::File file(path);

            if (file.isDirectory() || pathLooksLikePatchFile(path))
                return true;
        }

        return false;
    }

    bool isSingleNonDirectoryFile(const juce::StringArray& files) noexcept
    {
        return files.size() == 1 && ! juce::File(files[0]).isDirectory();
    }

    bool isSingleNonDirectoryPatchFile(const juce::StringArray& files) noexcept
    {
        return isSingleNonDirectoryFile(files) && pathLooksLikePatchFile(files[0]);
    }

    bool isMasterExtensionCandidate(const juce::File& file) noexcept
    {
        return Core::MasterM1kmCodec::hasExtension(file)
            || Core::MasterFileAssess::hasSyxExtension(file);
    }

    bool isJunkDragSelection(const juce::StringArray& files) noexcept
    {
        if (files.isEmpty())
            return true;

        if (selectionLooksAcceptable(files))
            return false;

        return ! (isSingleNonDirectoryFile(files)
                  && Core::MasterM1kmCodec::hasExtension(juce::File(files[0])));
    }

    bool selectionIncludesMasterExtension(const juce::StringArray& files) noexcept
    {
        for (const auto& path : files)
        {
            if (isMasterExtensionCandidate(juce::File(path)))
                return true;
        }

        return false;
    }

    struct CachedOverlaySpec
    {
        juce::String cacheKey;
        PatchNameDisplayPanel::DragOverlayKind kind = PatchNameDisplayPanel::DragOverlayKind::kInvalid;
        bool isValid = false;
        juce::String previewPrimary;
    };

    void applyCachedDragOverlay(PatchNameDisplayPanel& panel,
                                DragOverlayCache& cache,
                                const CachedOverlaySpec& spec)
    {
        if (spec.cacheKey == cache.path)
            return;

        cache.path = spec.cacheKey;
        cache.valid = spec.isValid;
        cache.preview = spec.previewPrimary;
        panel.applyDragOverlay(spec.kind, spec.previewPrimary);
    }

    bool tryApplyMasterDragOverlay(PatchNameDisplayPanel& panel,
                                   DragOverlayCache& cache,
                                   const juce::StringArray& files,
                                   SysExDecoder& decoder)
    {
        if (! isSingleNonDirectoryFile(files) || ! isMasterExtensionCandidate(juce::File(files[0])))
            return false;

        const juce::String path = files[0];
        if (path == cache.path)
            return true;

        const auto masterAssessment = Core::MasterFileAssess::assess(juce::File(path), decoder);

        if (masterAssessment.isValidMaster)
        {
            applyCachedDragOverlay(panel, cache,
                                   { path, PatchNameDisplayPanel::DragOverlayKind::kValidMaster, true, {} });
            return true;
        }

        // Invalid .m1km is never a patch — BAD FILE with Blue / PATCH NAME chrome.
        if (Core::MasterM1kmCodec::hasExtension(juce::File(path)))
        {
            applyCachedDragOverlay(panel, cache,
                                   { path, PatchNameDisplayPanel::DragOverlayKind::kInvalid, false, {} });
            return true;
        }

        // Invalid-as-Master .syx may still be a valid patch — fall through.
        return false;
    }

    void applyPatchOrSelectionDragOverlay(PatchNameDisplayPanel& panel,
                                          DragOverlayCache& cache,
                                          const juce::StringArray& files,
                                          Core::PatchFileService& patchFileService)
    {
        if (! isSingleNonDirectoryPatchFile(files))
        {
            applyCachedDragOverlay(
                panel, cache,
                { "\x01selection", PatchNameDisplayPanel::DragOverlayKind::kValidSelection, true, {} });
            return;
        }

        const juce::String path = files[0];
        if (path == cache.path)
            return;

        const auto assessment = patchFileService.assessSinglePatchSyxFile(juce::File(path));
        applyCachedDragOverlay(
            panel, cache,
            { path,
              assessment.isValidSinglePatch ? PatchNameDisplayPanel::DragOverlayKind::kValidSingle
                                            : PatchNameDisplayPanel::DragOverlayKind::kInvalid,
              assessment.isValidSinglePatch,
              assessment.previewPrimaryName });
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
    if (isMasterM1kmLoadChoiceDialogVisible())
        return;

    auto* panel = getPatchNameDisplayPanelIfPresent();
    if (panel == nullptr)
        return;

    DragOverlayCache cache { lastDragAssessedPath_, lastDragAssessedValid_, lastDragAssessedPreview_ };

    if (isJunkDragSelection(files))
    {
        const auto invalidKind = files.size() >= 2
            ? PatchNameDisplayPanel::DragOverlayKind::kInvalidPlural
            : PatchNameDisplayPanel::DragOverlayKind::kInvalid;
        applyCachedDragOverlay(
            *panel, cache,
            { files.size() >= 2 ? "\x01junks" : "\x01junk", invalidKind, false, {} });
        return;
    }

    if (tryApplyMasterDragOverlay(*panel, cache, files,
                                  pluginProcessor.getMidiManager().getSysExDecoder()))
        return;

    applyPatchOrSelectionDragOverlay(*panel, cache, files, pluginProcessor.getPatchFileService());
}

void PluginEditor::clearPatchNameDragOverlay()
{
    lastDragAssessedPath_.clear();

    if (auto* panel = getPatchNameDisplayPanelIfPresent())
        panel->clearDragOverlay();
}

void PluginEditor::handleMasterFileDropped(const juce::File& file)
{
    clearPatchNameDragOverlay();

    if (Core::MasterM1kmCodec::hasExtension(file))
    {
        auto packed = std::make_shared<std::array<juce::uint8, Core::MasterModel::kBufferSize>>();
        if (! pluginProcessor.tryDecodeMasterM1kmUserFile(file, packed->data()))
            return;

        openMasterM1kmLoadChoiceDialog(
            [this, packed]
            {
                pluginProcessor.commitMasterM1kmUserLoad(
                    packed->data(), Core::MasterM1kmGroupsPolicy::kMasterSettingsOnly);
            },
            [this, packed]
            {
                pluginProcessor.commitMasterM1kmUserLoad(
                    packed->data(), Core::MasterM1kmGroupsPolicy::kFullMaster);
            });
        return;
    }

    pluginProcessor.loadMasterFromUserFile(file);
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
    if (isMasterM1kmLoadChoiceDialogVisible())
        return;

    updatePatchNameDragOverlay(files);
}

void PluginEditor::fileDragMove(const juce::StringArray& files, int, int)
{
    if (isMasterM1kmLoadChoiceDialogVisible())
        return;

    updatePatchNameDragOverlay(files);
}

void PluginEditor::fileDragExit(const juce::StringArray&)
{
    clearPatchNameDragOverlay();
}

void PluginEditor::filesDropped(const juce::StringArray& files, int, int)
{
    if (isMasterM1kmLoadChoiceDialogVisible())
    {
        clearPatchNameDragOverlay();
        return;
    }

    if (files.isEmpty())
    {
        clearPatchNameDragOverlay();
        return;
    }

    // V1 Master drop: exactly one file. Multi-file never commits a Master.
    if (files.size() >= 2)
    {
        if (selectionIncludesMasterExtension(files) && ! selectionLooksAcceptable(files))
        {
            clearPatchNameDragOverlay();
            TSS::GrayedControlHelper::setFooterWarningMessage(
                pluginProcessor.getApvts(), MasterFooter::kDropRejectedMasterMulti);
            return;
        }

        handleSyxFilesDropped(files);
        return;
    }

    if (isSingleNonDirectoryFile(files) && isMasterExtensionCandidate(juce::File(files[0])))
    {
        const juce::File file(files[0]);
        const auto masterAssessment = Core::MasterFileAssess::assess(
            file, pluginProcessor.getMidiManager().getSysExDecoder());

        if (masterAssessment.isValidMaster)
        {
            handleMasterFileDropped(file);
            return;
        }

        if (Core::MasterM1kmCodec::hasExtension(file))
        {
            clearPatchNameDragOverlay();
            TSS::GrayedControlHelper::setFooterWarningMessage(
                pluginProcessor.getApvts(), MasterFooter::kDropRejectedNotMaster);
            return;
        }
    }

    handleSyxFilesDropped(files);
}
