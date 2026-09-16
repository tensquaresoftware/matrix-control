#include "PatchMutatorEngineTestSupport.h"

#include "Core/Services/PatchMutator/PatchMutatorEngineInternal.h"

using namespace PatchMutatorEngineTestSupport;

class PatchMutatorEngineExportTests : public juce::UnitTest
{
public:
    PatchMutatorEngineExportTests() : juce::UnitTest("PatchMutatorEngineExport") {}

    void runTest() override
    {
        mutate_firstRoot_freezesExportBasename();
        resetSessionForPatchLoad_clearsFrozenBasename();
        refreshFrozenExportBasename_afterRename_updatesBasename();
        refreshFrozenExportBasename_noFrozenBasename_noOp();
        export_withFrozenBasename_createsSessionSubfolder();
        export_existingSessionFolder_requestsCollisionModal();
        exportResolved_keep_writesIndexedFolder();
        exportResolved_cancel_setsPrefixedFooter();
        export_writesLiveUserName_notMxxLabel();
        export_afterRename_createsNewFolderKeepsOld();
        export_emptyHistory_blocked();
        export_success_footer();
        export_nonWritableFolder_blocked();
        export_failedFallback_prefixesErrorMessage();
        export_doesNotMutateStore();
        export_noSysEx();
    }

private:
    void mutate_firstRoot_freezesExportBasename()
    {
        beginTest("mutate_firstRoot_freezesExportBasename");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(8, 25); });
        harness.model.setName("OB-VOX");

        expect(harness.engine.mutate().success);
        expectEquals(harness.store().getFrozenExportBasename(), juce::String("OB-VOX @ B8-P25"));
    }

    void resetSessionForPatchLoad_clearsFrozenBasename()
    {
        beginTest("resetSessionForPatchLoad_clearsFrozenBasename");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(1, 2); });

        expect(harness.engine.mutate().success);
        expect(harness.store().hasFrozenExportBasename());

        harness.engine.resetSessionForPatchLoad();
        expect(! harness.store().hasFrozenExportBasename());
    }

    void refreshFrozenExportBasename_afterRename_updatesBasename()
    {
        beginTest("refreshFrozenExportBasename_afterRename_updatesBasename");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(1, 73); });
        harness.model.setName("WARMPAD");

        expect(harness.engine.mutate().success);
        expectEquals(harness.store().getFrozenExportBasename(), juce::String("WARMPAD @ B1-P73"));

        harness.engine.refreshFrozenExportBasename("COLDPAD");
        expectEquals(harness.store().getFrozenExportBasename(), juce::String("COLDPAD @ B1-P73"));
    }

    void refreshFrozenExportBasename_noFrozenBasename_noOp()
    {
        beginTest("refreshFrozenExportBasename_noFrozenBasename_noOp");

        EngineHarness harness;
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(1, 73); });

        harness.engine.refreshFrozenExportBasename("COLDPAD");
        expect(! harness.store().hasFrozenExportBasename());
    }

    void export_withFrozenBasename_createsSessionSubfolder()
    {
        beginTest("export_withFrozenBasename_createsSessionSubfolder");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(8, 25); });
        harness.model.setName("OB-VOX");
        expect(harness.engine.mutate().success);

        const auto tempDir = makeTempExportDir();
        const auto result = harness.engine.exportHistory(tempDir);

        expect(result.success);
        expect(! result.exportCollisionModalRequested);
        const auto sessionFolder = tempDir.getChildFile("OB-VOX @ B8-P25");
        expect(sessionFolder.isDirectory());
        expect(sessionFolder.getChildFile("M00.syx").existsAsFile());
        expect(! sessionFolder.getChildFile("M00").isDirectory());
        expectEquals(result.footerMessage,
                     juce::String("PATCH MUTATOR: Exported 2 mutation file(s) to ")
                         + sessionFolder.getFullPathName() + ".");
        expectEquals(result.footerSeverity, juce::String("info"));

        tempDir.deleteRecursively();
    }

    void export_existingSessionFolder_requestsCollisionModal()
    {
        beginTest("export_existingSessionFolder_requestsCollisionModal");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(8, 25); });
        harness.model.setName("OB-VOX");
        expect(harness.engine.mutate().success);

        const auto tempDir = makeTempExportDir();
        expect(tempDir.getChildFile("OB-VOX @ B8-P25").createDirectory());

        const auto result = harness.engine.exportHistory(tempDir);
        expect(result.exportCollisionModalRequested);
        expect(! result.success);

        tempDir.deleteRecursively();
    }

    void exportResolved_keep_writesIndexedFolder()
    {
        beginTest("exportResolved_keep_writesIndexedFolder");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(8, 25); });
        harness.model.setName("OB-VOX");
        expect(harness.engine.mutate().success);

        const auto tempDir = makeTempExportDir();
        expect(tempDir.getChildFile("OB-VOX @ B8-P25").createDirectory());

        const auto result = harness.engine.exportHistoryResolved(
            tempDir, Core::ExportCollisionResolution::kKeep);

        expect(result.success);
        expect(tempDir.getChildFile("OB-VOX @ B8-P25-2").isDirectory());
        expectEquals(result.footerMessage,
                     juce::String("PATCH MUTATOR: Exported 2 mutation file(s) to ")
                         + tempDir.getChildFile("OB-VOX @ B8-P25-2").getFullPathName() + ".");

        tempDir.deleteRecursively();
    }

    void exportResolved_cancel_setsPrefixedFooter()
    {
        beginTest("exportResolved_cancel_setsPrefixedFooter");

        EngineHarness harness;
        const auto tempDir = makeTempExportDir();

        const auto result = harness.engine.exportHistoryResolved(
            tempDir, Core::ExportCollisionResolution::kCancel);

        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Export cancelled."));
        expectEquals(result.footerSeverity, juce::String("info"));

        tempDir.deleteRecursively();
    }

    void export_writesLiveUserName_notMxxLabel()
    {
        beginTest("export_writesLiveUserName_notMxxLabel");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(8, 25); });
        harness.model.setName("WARMPAD");
        expect(harness.engine.mutate().success);

        const auto tempDir = makeTempExportDir();
        expect(harness.engine.exportHistory(tempDir).success);

        const auto exportedFile = tempDir.getChildFile("WARMPAD @ B8-P25")
                                       .getChildFile("M00.syx");
        expect(exportedFile.existsAsFile());

        juce::uint8 packed[SysExConstants::kPatchPackedDataSize] = {};
        expect(harness.patchFileService.loadPatchSysExFile(exportedFile, packed).success);

        Core::PatchModel decoded;
        decoded.loadFrom(packed);
        expectEquals(decoded.getName(), juce::String("WARMPAD"));

        tempDir.deleteRecursively();
    }

    void export_afterRename_createsNewFolderKeepsOld()
    {
        beginTest("export_afterRename_createsNewFolderKeepsOld");

        EngineHarness harness;
        harness.setRecipe(100, 100, true);
        harness.engine.setPatchLoadContextProvider(
            []() { return Core::PatchLoadContext::deviceMemory(1, 73); });
        harness.model.setName("WARMPAD");
        expect(harness.engine.mutate().success);

        const auto tempDir = makeTempExportDir();
        expect(harness.engine.exportHistory(tempDir).success);
        expect(tempDir.getChildFile("WARMPAD @ B1-P73").isDirectory());

        harness.model.setName("COLDPAD");
        harness.engine.refreshFrozenExportBasename("COLDPAD");

        expect(harness.engine.exportHistory(tempDir).success);
        expect(tempDir.getChildFile("COLDPAD @ B1-P73").isDirectory());
        expect(tempDir.getChildFile("WARMPAD @ B1-P73").isDirectory());

        tempDir.deleteRecursively();
    }

    void export_emptyHistory_blocked()
    {
        beginTest("export_emptyHistory_blocked");

        EngineHarness harness;
        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlMutatorExport", "", false);
        expect(tempDir.createDirectory());

        const auto result = harness.engine.exportHistory(tempDir);

        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Mutation history is empty."));
        expectEquals(result.footerSeverity, juce::String("warning"));
        expectEquals(tempDir.getNumberOfChildFiles(0), 0);

        tempDir.deleteRecursively();
    }

    void export_success_footer()
    {
        beginTest("export_success_footer");

        EngineHarness harness;
        harness.setRecipe(50, 50);

        expect(harness.engine.mutate().success);

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlMutatorExport", "", false);
        expect(tempDir.createDirectory());

        const auto result = harness.engine.exportHistory(tempDir);

        expect(result.success);
        expectEquals(result.footerSeverity, juce::String("info"));
        expectEquals(result.footerMessage,
                     juce::String("PATCH MUTATOR: Exported 2 mutation file(s) to ")
                         + tempDir.getFullPathName() + ".");

        tempDir.deleteRecursively();
    }

    void export_nonWritableFolder_blocked()
    {
        beginTest("export_nonWritableFolder_blocked");

        EngineHarness harness;
        harness.setRecipe(50, 50);
        expect(harness.engine.mutate().success);

        const auto missing = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getChildFile("MatrixControlMissingMutatorExportFolder");

        const auto result = harness.engine.exportHistory(missing);

        expect(! result.success);
        expectEquals(result.footerMessage, juce::String("PATCH MUTATOR: Export folder is not writable."));
        expectEquals(result.footerSeverity, juce::String("warning"));
    }

    void export_failedFallback_prefixesErrorMessage()
    {
        beginTest("export_failedFallback_prefixesErrorMessage");

        Core::PatchFileExportResult emptyError;
        emptyError.success = false;
        const auto emptyFallback = PatchMutatorEngineInternal::makeExportHistoryResult(
            emptyError, juce::File("/tmp"));
        expect(! emptyFallback.success);
        expectEquals(emptyFallback.footerMessage,
                     juce::String("PATCH MUTATOR: Mutation export failed."));
        expectEquals(emptyFallback.footerSeverity, juce::String("warning"));

        Core::PatchFileExportResult rawError;
        rawError.success = false;
        rawError.errorMessage = "Folder not writable";
        const auto prefixed = PatchMutatorEngineInternal::makeExportHistoryResult(
            rawError, juce::File("/tmp"));
        expect(! prefixed.success);
        expectEquals(prefixed.footerMessage, juce::String("PATCH MUTATOR: Folder not writable"));
        expectEquals(prefixed.footerSeverity, juce::String("warning"));
    }

    void export_doesNotMutateStore()
    {
        beginTest("export_doesNotMutateStore");

        EngineHarness harness;
        harness.setRecipe(50, 50);
        expect(harness.engine.mutate().success);

        const auto rootCountBefore = harness.store().rootCount();

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlMutatorExport", "", false);
        expect(tempDir.createDirectory());
        expect(harness.engine.exportHistory(tempDir).success);

        expectEquals(harness.store().rootCount(), rootCountBefore);

        tempDir.deleteRecursively();
    }

    void export_noSysEx()
    {
        beginTest("export_noSysEx");

        EngineHarness harness;
        harness.setRecipe(50, 50);
        expect(harness.engine.mutate().success);

        while (! harness.queue.isEmpty())
            (void) harness.queue.dequeue();

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlMutatorExport", "", false);
        expect(tempDir.createDirectory());
        expect(harness.engine.exportHistory(tempDir).success);

        expectEquals(countPatchSysExMessages(harness.queue), 0);

        tempDir.deleteRecursively();
    }

};

static PatchMutatorEngineExportTests patchMutatorEngineExportTests;
