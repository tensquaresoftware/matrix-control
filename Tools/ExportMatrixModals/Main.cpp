#include <functional>
#include <memory>
#include <vector>

#include <juce_audio_devices/juce_audio_devices.h>
#include <juce_gui_basics/juce_gui_basics.h>
#include <juce_graphics/juce_graphics.h>

#include "GUI/About/AboutWindow.h"
#include "GUI/Dialogs/AudioMidiSettingsWindow.h"
#include "GUI/Dialogs/BankTransferProgressDialog.h"
#include "GUI/Dialogs/EpromTypePromptDialog.h"
#include "GUI/Dialogs/MasterInitConfirmDialog.h"
#include "GUI/Dialogs/MasterM1kmLoadChoiceDialog.h"
#include "GUI/Dialogs/MatrixOrderedConfirmDialog.h"
#include "GUI/Dialogs/MutatorHistoryDefragConfirmDialog.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    using namespace PluginDisplayNames::Dialogs;

    juce::File outputRoot()
    {
        return juce::File::getSpecialLocation(juce::File::userDesktopDirectory).getChildFile("Modales");
    }

    juce::String scaleFolderName(float uiScale)
    {
        return juce::String(juce::roundToInt(uiScale * 100.0f)).paddedLeft('0', 3);
    }

    void savePng(const juce::Image& image, const juce::File& file)
    {
        file.getParentDirectory().createDirectory();
        if (file.existsAsFile())
            file.deleteFile();

        juce::FileOutputStream stream(file);
        if (! stream.openedOk())
            return;

        juce::PNGImageFormat png;
        png.writeImageToStream(image, stream);
    }

    void snapshotComponent(juce::Component& component, float uiScale, const juce::String& baseName)
    {
        const int hostW = juce::jmax(800, juce::roundToInt(1100.0f * uiScale));
        const int hostH = juce::jmax(700, juce::roundToInt(900.0f * uiScale));
        component.setSize(hostW, hostH);
        component.resized();
        component.repaint();

        // Force a paint into an off-screen image (works without adding to desktop).
        juce::Image image(juce::Image::ARGB, hostW, hostH, true);
        {
            juce::Graphics g(image);
            component.paintEntireComponent(g, true);
        }

        const auto dir = outputRoot().getChildFile(scaleFolderName(uiScale));
        savePng(image, dir.getChildFile(baseName + ".png"));
        juce::Logger::writeToLog("Wrote " + dir.getChildFile(baseName + ".png").getFullPathName());
    }

    PluginEditorInternal::OrderedConfirmAlertOptions ordered(const juce::String& title,
                                                             const juce::String& message,
                                                             const juce::String& cancel,
                                                             const juce::String& primary,
                                                             const juce::String& middle = {},
                                                             int designWidth = 0,
                                                             bool alignBodyToCancel = false)
    {
        return {
            .iconType = juce::MessageBoxIconType::WarningIcon,
            .title = title,
            .message = message,
            .cancelLabel = cancel,
            .primaryLabel = primary,
            .associatedComponent = nullptr,
            .middleLabel = middle,
            .valueRows = {},
            .designWidth = designWidth,
            .alignBodyToCancel = alignBodyToCancel
        };
    }

    void exportOrdered(TSS::ISkin& skin,
                       float uiScale,
                       const juce::String& baseName,
                       const PluginEditorInternal::OrderedConfirmAlertOptions& options)
    {
        MatrixOrderedConfirmDialog dialog(skin, uiScale, options);
        snapshotComponent(dialog, uiScale, baseName);
    }

    void exportAllAtScale(TSS::ISkin& skin, float uiScale, juce::AudioDeviceManager& deviceManager)
    {
        exportOrdered(skin,
                      uiScale,
                      "01-unsaved-patch-store",
                      ordered(UnsavedEditConfirm::kTitle,
                              UnsavedEditConfirm::kBodyStore,
                              UnsavedEditConfirm::kCancel,
                              UnsavedEditConfirm::kStore,
                              UnsavedEditConfirm::kDiscard));

        exportOrdered(skin,
                      uiScale,
                      "02-flush-mutation-history",
                      ordered(MutatorFlushConfirm::kTitle,
                              MutatorFlushConfirm::kBody,
                              MutatorFlushConfirm::kCancel,
                              MutatorFlushConfirm::kContinue));

        exportOrdered(skin,
                      uiScale,
                      "03-delete-init-template-patch",
                      ordered(DeleteInitTemplateConfirm::kTitle,
                              DeleteInitTemplateConfirm::kBodyPatch,
                              DeleteInitTemplateConfirm::kCancel,
                              DeleteInitTemplateConfirm::kDelete));

        exportOrdered(skin,
                      uiScale,
                      "04-import-bank",
                      ordered(BankImportConfirm::kTitle,
                              BankImportConfirm::kBody,
                              BankImportConfirm::kCancel,
                              BankImportConfirm::kContinue,
                              {},
                              360));

        exportOrdered(skin,
                      uiScale,
                      "05-paste-bank",
                      ordered(BankPasteConfirm::kTitle,
                              BankPasteConfirm::formatBody(3, 7),
                              BankPasteConfirm::kCancel,
                              BankPasteConfirm::kContinue));

        exportOrdered(skin,
                      uiScale,
                      "06-replace-export-folder",
                      ordered(BankExportOverwriteConfirm::kTitle,
                              BankExportOverwriteConfirm::kBody,
                              BankExportOverwriteConfirm::kCancel,
                              BankExportOverwriteConfirm::kContinue));

        exportOrdered(skin,
                      uiScale,
                      "07-invalid-patch-file-name",
                      ordered(InvalidSaveAsPatchName::kTitle,
                              InvalidSaveAsPatchName::kBody,
                              InvalidSaveAsPatchName::kOk,
                              InvalidSaveAsPatchName::kOk));

        {
            auto options = ordered(PatchNameReconciliation::kTitle,
                                   PatchNameReconciliation::kBody,
                                   PatchNameReconciliation::kCancel,
                                   PatchNameReconciliation::kFilename,
                                   PatchNameReconciliation::kInternal,
                                   400,
                                   true);
            options.valueRows = {
                { PatchNameReconciliation::kInternalNameLabel, "ANALOG" },
                { PatchNameReconciliation::kFilenameLabel, "MY-PATCH" }
            };
            exportOrdered(skin, uiScale, "08-patch-name-mismatch", options);
        }

        {
            MatrixMutatorDeleteConfirmDialog dialog(skin, uiScale);
            snapshotComponent(dialog, uiScale, "09-delete-mutation");
        }

        {
            MasterInitConfirmDialog dialog(skin, [] {});
            dialog.setUiScale(uiScale);
            dialog.prepareForShow("VIBRATO", [] {});
            snapshotComponent(dialog, uiScale, "10-reset-master-module");
        }

        {
            MasterInitConfirmDialog dialog(skin, [] {});
            dialog.setUiScale(uiScale);
            dialog.prepareForGlobalShow([] {});
            snapshotComponent(dialog, uiScale, "11-reset-all-master-modules");
        }

        {
            MutatorHistoryDefragConfirmDialog dialog(skin, [] {});
            dialog.setUiScale(uiScale);
            dialog.prepareForShow([] {});
            snapshotComponent(dialog, uiScale, "12-defrag-mutation-history");
        }

        {
            MasterM1kmLoadChoiceDialog dialog(skin, [] {});
            dialog.setUiScale(uiScale);
            dialog.prepareForShow([] {}, [] {});
            snapshotComponent(dialog, uiScale, "13-load-m1km-master");
        }

        {
            EpromTypePromptDialog dialog(skin, [] {});
            dialog.setUiScale(uiScale);
            EpromTypePromptDialog::PrepareForShowArgs args;
            args.includeFirmwareSuggestionHint = true;
            args.deviceStatus = { .deviceDetected = true,
                                  .deviceMidiUnresponsive = false,
                                  .deviceType = MatrixDeviceTypes::Type::kMatrix1000,
                                  .deviceVersion = "1.20" };
            args.onConfirm = [](int) {};
            args.onLater = [] {};
            dialog.prepareForShow(std::move(args));
            snapshotComponent(dialog, uiScale, "14-device-setup");
        }

        {
            BankTransferProgressDialog dialog(skin);
            dialog.setUiScale(uiScale);
            dialog.prepareForShow({
                .title = BankTransferProgress::kExportTitle,
                .message = BankTransferProgress::formatExportProgressMessage(2),
                .detail = "/Users/example/Banks/Bank-02",
                .totalSteps = 100,
                .onCancelRequested = [] {},
                .layout = BankTransferProgressDialog::ContentLayout::SingleLane
            });
            dialog.setProgress(42);
            snapshotComponent(dialog, uiScale, "15-bank-export-progress");
        }

        {
            BankTransferProgressDialog dialog(skin);
            dialog.setUiScale(uiScale);
            dialog.prepareForShow({
                .title = BankTransferProgress::kImportTitle,
                .message = "Reading bank safety copy from device:",
                .detail = "/Users/example/Banks/Import",
                .totalSteps = 100,
                .onCancelRequested = [] {},
                .layout = BankTransferProgressDialog::ContentLayout::DualLane
            });
            dialog.setProgress(30);
            dialog.beginSecondaryPhase("Writing patches to the device:", 100);
            dialog.setProgress(12);
            snapshotComponent(dialog, uiScale, "16-bank-import-progress");
        }

        {
            AboutWindow about(skin, [] {});
            about.setUiScale(uiScale);
            snapshotComponent(about, uiScale, "17-about");
        }

        {
            SettingsWindow settings(skin, false, nullptr, [] {});
            settings.setUiScale(uiScale);
            snapshotComponent(settings, uiScale, "18-settings");
        }

        {
            AudioMidiSettingsWindow::Config config;
            config.skin = &skin;
            config.deviceManager = &deviceManager;
            config.maxInputChannels = 2;
            config.maxOutputChannels = 2;
            config.peakLevelProvider = [] { return 0.35f; };
            config.onCloseRequested = [] {};
            AudioMidiSettingsWindow audio(std::move(config));
            audio.setUiScale(uiScale);
            snapshotComponent(audio, uiScale, "19-audio-settings");
        }
    }
}

int main(int argc, char* argv[])
{
    juce::ignoreUnused(argc, argv);
    juce::ScopedJuceInitialiser_GUI guiInit;

    const auto root = outputRoot();
    if (root.exists())
        root.deleteRecursively();
    root.createDirectory();

    auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
    juce::AudioDeviceManager deviceManager;
    deviceManager.initialiseWithDefaultDevices(0, 2);

    constexpr float scales[] = { 0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f };
    for (const float scale : scales)
        exportAllAtScale(*skin, scale, deviceManager);

    juce::Logger::writeToLog("Done. PNGs in " + root.getFullPathName());
    return 0;
}
