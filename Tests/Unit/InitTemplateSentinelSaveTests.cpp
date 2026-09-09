#include <juce_core/juce_core.h>

#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "PatchManagerActionHandlerTestSupport.h"

class InitTemplateSentinelSaveTests : public juce::UnitTest
{
public:
    InitTemplateSentinelSaveTests() : juce::UnitTest("InitTemplateSentinelSave") {}

    void runTest() override
    {
        saveBlocked_whenSentinelActive();
        unsavedGateSaveBlocked_whenSentinelActive();
    }

private:
    void saveBlocked_whenSentinelActive()
    {
        beginTest("saveBlocked_whenSentinelActive");

        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName;
        using PluginDisplayNames::Settings::FooterMessages::kRenameBeforeSave;
        using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;
        using namespace PatchManagerActionHandlerTestSupport;
        namespace ComputerWidgets = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets;

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());
        expectEquals(harness.proc.apvts.state.getProperty(kPatchName).toString(),
                     juce::String(kInitPatchName));

        bool saveAsPickerCalled = false;
        harness.pickSaveFileCallback = [&](juce::File, juce::String)
        {
            saveAsPickerCalled = true;
            return juce::File();
        };

        harness.handler.handleAction(ComputerWidgets::kSavePatchAs, juce::var());
        expect(! saveAsPickerCalled);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(kRenameBeforeSave));

        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        const auto target = tempDir.getChildFile("Patch 71.syx");
        const auto sizeBefore = target.getSize();
        const auto modBefore = target.getLastModificationTime();

        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(ComputerWidgets::kSelectPatchFile, 1, nullptr);
        // Keep sentinel after scan wiring — INIT already set the name.
        harness.proc.apvts.state.setProperty(kPatchName, kInitPatchName, nullptr);
        harness.model.setName(kInitPatchName);

        harness.handler.handleAction(ComputerWidgets::kSavePatchFile, juce::var());

        expectEquals(target.getSize(), sizeBefore);
        expect(target.getLastModificationTime() == modBefore);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(kRenameBeforeSave));

        tempDir.deleteRecursively();
    }

    void unsavedGateSaveBlocked_whenSentinelActive()
    {
        beginTest("unsavedGateSaveBlocked_whenSentinelActive");

        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName;
        using PluginDisplayNames::Settings::FooterMessages::kRenameBeforeSave;
        using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;
        using namespace PatchManagerActionHandlerTestSupport;
        namespace ComputerWidgets = PluginIDs::PatchManagerSection::ComputerPatchesModule::StandaloneWidgets;

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        const auto tempDir = createTempScanDir();
        expect(tempDir.createDirectory());
        copyFixturePatchToDir(tempDir, "Patch 71.syx");
        const auto target = tempDir.getChildFile("Patch 71.syx");

        setupComputerPatchesScan(harness, tempDir);
        harness.proc.apvts.state.setProperty(ComputerWidgets::kSelectPatchFile, 1, nullptr);
        simulateSelectPatchFileDispatch(harness);
        expect(harness.handler.hasUsableKnownSyxPath());

        harness.proc.apvts.state.setProperty(kPatchName, kInitPatchName, nullptr);
        harness.model.setName(kInitPatchName);

        const auto sizeBefore = target.getSize();
        const auto modBefore = target.getLastModificationTime();

        expect(! harness.handler.tryPersistCurrentPatchFromUnsavedGate(
            Core::UnsavedEditPersistKind::kSave));

        expectEquals(target.getSize(), sizeBefore);
        expect(target.getLastModificationTime() == modBefore);
        expectEquals(harness.proc.apvts.state.getProperty("uiMessageText").toString(),
                     juce::String(kRenameBeforeSave));

        tempDir.deleteRecursively();
    }
};

static InitTemplateSentinelSaveTests initTemplateSentinelSaveTests;
