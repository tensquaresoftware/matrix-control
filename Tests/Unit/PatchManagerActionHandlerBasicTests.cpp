#include "PatchManagerActionHandlerTestSupport.h"

using namespace PatchManagerActionHandlerTestSupport;

class PatchManagerActionHandlerBasicTests : public juce::UnitTest
{
public:
    PatchManagerActionHandlerBasicTests() : juce::UnitTest("PatchManagerActionHandlerBasic") {}

    void runTest() override
    {
        testPasteRomBankBlocked();
        testPasteRamBankSuccess();
        testPasteMatrix6_sendsPatchSlot();
        testStoreRomBankBlocked();
        testStoreRamBankSuccess();
        testInitLoadsTemplateAndBufferToApvts();
        testInitBlankTemplate_assignsInitPatchName();
        testInitBlankCustomTemplate_assignsInitPatchName();
        testInitNamedTemplate_forcesInitSentinel();
        testInitDefaultDashNameTemplate_forcesInitSentinel();
        testInitMatrix1000_sendsEditBuffer();
        testInitMatrix6_sendsPatchToCurrentSlot();
        testInitRomBankBlocked();
        testInitCompareActiveBlocked();
        testBankSelectMatrix1000SetBank();
        testBankSelectMatrix6NoSetBank();
        testNavigationWithinBankNoSetBank();
        testAt99_fourNext_staysBank0();
        testAt99_next_wrapsToPatch0_sameBank();
        testUndefinedCoordinates_firstNext_landsOnBank0Patch0();
        testUndefinedCoordinates_firstPrev_landsOnBank0Patch0();
        testNavigationStaleSyncedBank_sendsSetBankOnFirstClick();
        testBankSelectMarksCoordinatesEstablished();
    }

private:
    static juce::File createTempInitTemplatesDir()
    {
        return juce::File::getSpecialLocation(juce::File::tempDirectory)
            .getNonexistentChildFile("MatrixControlInitPatchName", "", false);
    }

    bool writeNamedPatchInitSyx(const juce::File& templatesDir, const juce::String& patchName)
    {
        if (! templatesDir.createDirectory())
            return false;

        Core::PatchModel named;
        named.loadFrom(Core::InitDefaults::patchData());
        named.setName(patchName);

        SysExEncoder encoder;
        const auto syx = encoder.encodePatchSysEx(0, named.data());
        const auto file = templatesDir.getChildFile(Core::InitTemplateLoader::kPatchInitFileName);
        return file.replaceWithData(syx.getData(), syx.getSize());
    }

    void testPasteRomBankBlocked()
    {
        beginTest("paste_romBank_blocked");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 3, 12, false);
        harness.mapper.apvtsToBuffer();
        harness.clipboard.copyFullPatch(harness.model, "BANK 0 / PATCH 0");

        harness.handler.handleAction(InternalPatches::kPastePatch, juce::var());

        // ROM gating is visual-only — no warning footer.
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(harness.queue.isEmpty());
    }

    void testPasteRamBankSuccess()
    {
        beginTest("paste_ramBank_success");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 7, false);
        harness.mapper.apvtsToBuffer();
        harness.clipboard.copyFullPatch(harness.model, "BANK 0 / PATCH 0");

        harness.handler.handleAction(InternalPatches::kPastePatch, juce::var());

        expect(harness.clipboard.canPasteFullPatch());
        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(queued.editBufferPatch);
    }

    void testPasteMatrix6_sendsPatchSlot()
    {
        beginTest("paste_matrix6_sendsPatchSlot");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6));
        initializePatchManagerState(harness.proc.apvts.state, 0, 7, false);
        harness.mapper.apvtsToBuffer();
        harness.clipboard.copyFullPatch(harness.model, "BANK 0 / PATCH 0");

        harness.handler.handleAction(InternalPatches::kPastePatch, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(queued.patchData);
        expect(!queued.editBufferPatch);
        expectEquals(queued.patchNumber, 7);
    }

    void testStoreRomBankBlocked()
    {
        beginTest("store_romBank_blocked");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 5, 1, false);

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(harness.queue.isEmpty());
    }

    void testStoreRamBankSuccess()
    {
        beginTest("store_ramBank_success");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 42, false);

        harness.proc.apvts.state.setProperty(
            PatchManager::StateProperties::kNavigationFocus,
            PatchManager::NavigationFocus::kComputer,
            nullptr);

        harness.handler.handleAction(InternalPatches::kStorePatch, juce::var());

        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 1);
        expect(queued.patchData);
        expect(!queued.editBufferPatch);
        expectEquals(harness.nameRequiredBeforeStoreHookState->calls, 0);
    }

    void testInitLoadsTemplateAndBufferToApvts()
    {
        beginTest("initFullPatch_loadsTemplate");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expect(!harness.suppressPatchSysEx);
        expect(!harness.suppressMatrixModSysEx);
        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());

        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(queued.editBufferPatch);
        expectEquals(queued.patchSysExCount, 1);
    }

    void testInitBlankTemplate_assignsInitPatchName()
    {
        beginTest("init_blankTemplate_assignsInitPatchName");

        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName;
        using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expectEquals(harness.model.getName(), juce::String(kInitPatchName));
        expectEquals(harness.proc.apvts.state.getProperty(kPatchName).toString(),
                     juce::String(kInitPatchName));
    }

    void testInitBlankCustomTemplate_assignsInitPatchName()
    {
        beginTest("init_blankCustomTemplate_assignsInitPatchName");

        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName;
        using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;

        const auto tempDir = createTempInitTemplatesDir();
        const bool wrote = writeNamedPatchInitSyx(tempDir, {});
        expect(wrote, "Blank-name PatchInit.syx should write");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.initTemplatesFolder = tempDir;
        if (wrote)
            harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expectEquals(harness.model.getName(), juce::String(kInitPatchName));
        expectEquals(harness.proc.apvts.state.getProperty(kPatchName).toString(),
                     juce::String(kInitPatchName));

        tempDir.deleteRecursively();
    }

    void testInitNamedTemplate_forcesInitSentinel()
    {
        beginTest("init_namedTemplate_forcesInitSentinel");

        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName;
        using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;

        const auto tempDir = createTempInitTemplatesDir();
        const bool wrote = writeNamedPatchInitSyx(tempDir, "MYINIT");
        expect(wrote, "Named PatchInit.syx should write");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.initTemplatesFolder = tempDir;
        if (wrote)
            harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expectEquals(harness.model.getName(), juce::String(kInitPatchName));
        expectEquals(harness.proc.apvts.state.getProperty(kPatchName).toString(),
                     juce::String(kInitPatchName));

        tempDir.deleteRecursively();
    }

    void testInitDefaultDashNameTemplate_forcesInitSentinel()
    {
        beginTest("init_defaultDashNameTemplate_forcesInitSentinel");

        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName;
        using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kInitPatchName;
        using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;

        const auto tempDir = createTempInitTemplatesDir();
        const bool wrote = writeNamedPatchInitSyx(tempDir, kDefaultPatchName);
        expect(wrote, "Dash-name PatchInit.syx should write");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        harness.initTemplatesFolder = tempDir;
        if (wrote)
            harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expectEquals(harness.model.getName(), juce::String(kInitPatchName));
        expectEquals(harness.proc.apvts.state.getProperty(kPatchName).toString(),
                     juce::String(kInitPatchName));

        tempDir.deleteRecursively();
    }

    void testInitMatrix1000_sendsEditBuffer()
    {
        beginTest("init_matrix1000_sendsEditBuffer");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 12, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(!queued.patchData);
        expect(queued.editBufferPatch);
        expectEquals(queued.patchSysExCount, 1);
    }

    void testInitMatrix6_sendsPatchToCurrentSlot()
    {
        beginTest("init_matrix6_sendsPatchToCurrentSlot");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6));
        initializePatchManagerState(harness.proc.apvts.state, 0, 42, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        const auto queued = scanQueue(harness.queue);
        expect(queued.patchData);
        expect(!queued.editBufferPatch);
        expectEquals(queued.patchNumber, 42);
        expectEquals(queued.patchSysExCount, 1);
    }

    void testInitRomBankBlocked()
    {
        beginTest("init_romBank_blocked");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 5, 1, false);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expect(harness.proc.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(harness.proc.apvts.state.getProperty("uiMessageSeverity").toString().isEmpty());
        expect(harness.queue.isEmpty());
    }

    void testInitCompareActiveBlocked()
    {
        beginTest("init_compareActive_blocked");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 1, 12, false);
        harness.proc.apvts.state.setProperty(MutatorState::kCompareActive, true, nullptr);

        harness.handler.handleAction(InternalPatches::kInitPatch, juce::var());

        expect(harness.queue.isEmpty());
    }

    void testBankSelectMatrix1000SetBank()
    {
        beginTest("bankSelect_matrix1000_setBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank3, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 3);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 3);
    }

    void testBankSelectMatrix6NoSetBank()
    {
        beginTest("bankSelect_matrix6_noSetBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix6));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, false);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank3, juce::var());

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expect(!static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expect(harness.queue.isEmpty());
    }

    void testNavigationWithinBankNoSetBank()
    {
        beginTest("navigation_withinBank_noSetBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 5, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 6);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        const auto queued = scanQueue(harness.queue);
        expect(!queued.setBank);
    }

    void testAt99_fourNext_staysBank0()
    {
        beginTest("at99_fourNext_staysBank0");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state,
                                    0,
                                    Matrix1000Limits::kMaxPatchNumber,
                                    true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        for (int step = 0; step < 4; ++step)
            fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 3);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        const auto queued = scanQueue(harness.queue);
        expect(!queued.setBank);
    }

    void testAt99_next_wrapsToPatch0_sameBank()
    {
        beginTest("at99_next_wrapsToPatch0_sameBank");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state,
                                    0,
                                    Matrix1000Limits::kMaxPatchNumber,
                                    true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 0);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     Matrix1000Limits::kMinPatchNumber);
        const auto queued = scanQueue(harness.queue);
        expect(!queued.setBank);
    }

    void testUndefinedCoordinates_firstNext_landsOnBank0Patch0()
    {
        beginTest("undefinedCoordinates_firstNext_landsOnBank0Patch0");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        // Nothing to step from yet, so the first Next claims the lowest slot instead of 0/01.
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)),
                     Matrix1000Limits::kMinBankNumber);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     Matrix1000Limits::kMinPatchNumber);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 0);
    }

    void testUndefinedCoordinates_firstPrev_landsOnBank0Patch0()
    {
        beginTest("undefinedCoordinates_firstPrev_landsOnBank0Patch0");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 0, false);

        fireInternalPatchNavigation(harness, InternalPatches::kLoadPreviousPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)),
                     Matrix1000Limits::kMinBankNumber);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     Matrix1000Limits::kMinPatchNumber);
        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         BankUtility::StateProperties::kSelectedBank)),
                     Matrix1000Limits::kMinBankNumber);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 0);
    }

    void testNavigationStaleSyncedBank_sendsSetBankOnFirstClick()
    {
        beginTest("navigation_staleSyncedBank_sendsSetBankOnFirstClick");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 9, 93, true);
        harness.patchSelectionMidiSync.resetLastSyncedBank(0);

        fireInternalPatchNavigation(harness, InternalPatches::kLoadNextPatch);

        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 9);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)), 94);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 9);
    }

    void testBankSelectMarksCoordinatesEstablished()
    {
        beginTest("bankSelect_marksCoordinatesEstablished");

        HandlerHarness harness(Core::DeviceMemoryLimits::resolve(MatrixDeviceTypes::Type::kMatrix1000));
        initializePatchManagerState(harness.proc.apvts.state, 0, 4, false);

        harness.handler.handleAction(BankUtility::StandaloneWidgets::kSelectBank3, juce::var());

        expect(static_cast<bool>(harness.proc.apvts.state.getProperty(
            PatchManager::StateProperties::kPatchCoordinatesEstablished)));
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(
                         PatchManager::StateProperties::kNavigationFocus)),
                     PatchManager::NavigationFocus::kInternal);
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentBankNumber)), 3);
        // Selecting a bank lands on its first slot, whatever the previous patch was.
        expectEquals(static_cast<int>(harness.proc.apvts.state.getProperty(InternalPatches::kCurrentPatchNumber)),
                     Matrix1000Limits::kMinPatchNumber);
        const auto queued = scanQueue(harness.queue);
        expect(queued.setBank);
        expectEquals(queued.setBankValue, 3);
    }
};

static PatchManagerActionHandlerBasicTests patchManagerActionHandlerBasicTests;
