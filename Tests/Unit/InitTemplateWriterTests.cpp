#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/InitTemplateWriter.h"
#include "Core/Init/MasterModuleInitService.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileService.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/ProjectPaths.h"
#include "MasterModuleInitServiceTestSupport.h"
#include "PatchManagerActionHandlerTestSupport.h"

class InitTemplateWriterTests : public juce::UnitTest
{
public:
    InitTemplateWriterTests() : juce::UnitTest("InitTemplateWriter") {}

    void runTest() override
    {
        projectPaths_initDirectoryUnderAppData();
        writePatchInit_blanksNameBytes();
        writeMasterInit_writesMasterInitSyx();
        writeAndLoadMasterInit_roundTrip();
        initAllModules_appliesTemplate();
        initAllModules_emptyFolderUsesHardcodedFallback();
        saveBlocked_whenSentinelActive();
        unsavedGateSaveBlocked_whenSentinelActive();
        deletePatchInit_removesFileAndLoadFallsBack();
        deleteMasterInit_removesFileAndLoadFallsBack();
        deletePatchInit_absentFileFails();
        deleteMasterInit_absentFileFails();
        initExists_reflectsWriteAndDelete();
    }

private:
    static juce::File fixturesInitDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Init");
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeMasterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (const auto& d : Core::ApvtsMasterMapper::buildIntDescriptors())
        {
            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.minValue, d.maxValue, d.defaultValue));
        }

        for (const auto& d : Core::ApvtsMasterMapper::buildChoiceDescriptors())
        {
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.choices, d.defaultIndex));
        }

        return layout;
    }

    static void fillModelWithDistinctValues(Core::MasterModel& model)
    {
        using namespace PluginDescriptors::MasterEditSection;

        for (const auto& d : MidiModule::kIntParameters)
            model.setValue(d, d.maxValue);

        for (const auto& d : VibratoModule::kIntParameters)
            model.setValue(d, d.minValue);

        for (const auto& d : MiscModule::kIntParameters)
            model.setValue(d, (d.minValue + d.maxValue) / 2);

        model.setChoiceIndex(MidiModule::kChoiceParameters[0], 2);
        model.setChoiceIndex(VibratoModule::kChoiceParameters[0], 3);
        model.setChoiceIndex(MiscModule::kChoiceParameters[0], 1);
    }

    static bool channelTripletMatch(const Core::MasterModel& lhs, const Core::MasterModel& rhs)
    {
        return lhs.data()[11] == rhs.data()[11]
            && lhs.data()[12] == rhs.data()[12]
            && lhs.data()[35] == rhs.data()[35];
    }

    static bool moduleBytesMatch(const Core::MasterModel& lhs,
                                 const Core::MasterModel& rhs,
                                 const juce::String& moduleGroupId)
    {
        using namespace PluginDescriptors::MasterEditSection;

        for (const auto& d : kIntParameters)
            if (d.parentGroupId == moduleGroupId && lhs.getValue(d) != rhs.getValue(d))
                return false;

        for (const auto& d : kChoiceParameters)
        {
            if (d.parentGroupId != moduleGroupId)
                continue;

            const bool channelMismatch =
                d.parameterId == PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel
                    ? ! channelTripletMatch(lhs, rhs)
                    : lhs.getChoiceIndex(d) != rhs.getChoiceIndex(d);
            if (channelMismatch)
                return false;
        }

        return true;
    }

    static void rewriteMasterInitWithOmniOn(const juce::File& templatesDir,
                                           SysExEncoder& encoder,
                                           Core::MasterModel& initTemplate)
    {
        initTemplate.data()[12] = 1;
        initTemplate.data()[35] = 0;
        const auto syx = encoder.encodeMasterSysEx(0x03, initTemplate.data());
        templatesDir.getChildFile(Core::InitTemplateLoader::kMasterInitFileName)
            .replaceWithData(syx.getData(), syx.getSize());
    }

    void projectPaths_initDirectoryUnderAppData()
    {
        beginTest("projectPaths_initDirectoryUnderAppData");

        const auto appData = ProjectPaths::getApplicationDataDirectory();
        const auto initDir = ProjectPaths::getInitTemplatesDirectory();

        expect(appData.getFullPathName().isNotEmpty());
        expect(initDir.isDirectory());
        expectEquals(initDir.getFileName(), juce::String("Init"));
        expect(initDir.isAChildOf(appData) || initDir == appData.getChildFile("Init"));
    }

    void writePatchInit_blanksNameBytes()
    {
        beginTest("writePatchInit_blanksNameBytes");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;
        Core::PatchFileService fileService(decoder);
        Core::InitTemplateLoader loader(decoder);

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("SENTINEL");

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitWriterPatch", "", true);
        expect(tempDir.createDirectory());

        const auto write = Core::InitTemplateWriter::writePatchInit(
            model, fileService, encoder, tempDir);
        expect(write.success);
        expect(Core::InitTemplateLoader::resolvePatchInitFile(tempDir).existsAsFile());

        Core::PatchModel loaded;
        const auto load = loader.loadPatch(loaded, tempDir);
        expect(load.success);
        expect(load.source == Core::InitTemplateSource::kUserFile);
        expect(loaded.getName().isEmpty());

        tempDir.deleteRecursively();
    }

    void writeMasterInit_writesMasterInitSyx()
    {
        beginTest("writeMasterInit_writesMasterInitSyx");

        SysExEncoder encoder;
        Core::MasterModel model;
        model.loadFrom(Core::InitDefaults::masterData());

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitWriterMasterInit", "", true);
        expect(tempDir.createDirectory());

        const auto write = Core::InitTemplateWriter::writeMasterInit(model, encoder, tempDir);
        expect(write.success);
        expect(Core::InitTemplateLoader::resolveMasterInitFile(tempDir).existsAsFile());
        expectEquals(write.infoMessage,
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterInitTemplateSaved));

        tempDir.deleteRecursively();
    }

    void writeAndLoadMasterInit_roundTrip()
    {
        beginTest("writeAndLoadMasterInit_roundTrip");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;
        Core::InitTemplateLoader loader(decoder);

        Core::MasterModel model;
        model.loadFrom(Core::InitDefaults::masterData());

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitWriterMaster", "", true);
        expect(tempDir.createDirectory());

        const auto write = Core::InitTemplateWriter::writeMasterInit(model, encoder, tempDir);
        expect(write.success);

        const auto target = Core::InitTemplateLoader::resolveMasterInitFile(tempDir);
        Core::MasterModel loaded;
        const auto load = Core::InitTemplateWriter::loadMasterFromUserFile(loaded, target, loader);
        expect(load.success);
        expect(load.source == Core::InitTemplateSource::kUserFile);
        expect(std::memcmp(loaded.data(), model.data(), Core::MasterModel::kBufferSize) == 0);

        tempDir.deleteRecursively();
    }

    void initAllModules_appliesTemplate()
    {
        beginTest("initAllModules_appliesTemplate");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;
        Core::InitTemplateLoader loader(decoder);

        TestAudioProcessorMasterInit proc(makeMasterLayout());
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper(proc.apvts, model);

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitAllMaster", "", true);
        expect(tempDir.createDirectory());
        expect(fixturesInitDir()
                   .getChildFile(Core::InitTemplateLoader::kMasterInitFileName)
                   .copyFileTo(Core::InitTemplateLoader::resolveMasterInitFile(tempDir)));

        Core::MasterModel initTemplate;
        loader.loadMaster(initTemplate, tempDir);
        rewriteMasterInitWithOmniOn(tempDir, encoder, initTemplate);
        loader.loadMaster(initTemplate, tempDir);

        fillModelWithDistinctValues(model);
        model.data()[12] = 0;
        model.data()[35] = 1;
        mapper.bufferToApvts();

        int dispatchCount = 0;
        Core::MasterParameterSysExDispatcher dispatcher(model, [&](const juce::uint8*) { ++dispatchCount; });
        Core::MasterModuleInitService service(model, mapper, loader, dispatcher, [&]() { return tempDir; });

        const auto result = service.initAllModules();
        expect(result.success && result.source == Core::InitTemplateSource::kUserFile);
        expect(dispatchCount == 1);

        using namespace PluginIDs::MasterEditSection;
        expect(moduleBytesMatch(model, initTemplate, MidiModule::kGroupId)
               && moduleBytesMatch(model, initTemplate, VibratoModule::kGroupId)
               && moduleBytesMatch(model, initTemplate, MiscModule::kGroupId));

        Core::MasterModel fromApvts;
        Core::ApvtsMasterMapper(proc.apvts, fromApvts).apvtsToBuffer();
        expect(std::memcmp(fromApvts.data(), model.data(), Core::MasterModel::kBufferSize) == 0);

        tempDir.deleteRecursively();
    }

    void initAllModules_emptyFolderUsesHardcodedFallback()
    {
        beginTest("initAllModules_emptyFolderUsesHardcodedFallback");

        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);

        TestAudioProcessorMasterInit proc(makeMasterLayout());
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper(proc.apvts, model);

        const auto emptyDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                  .getNonexistentChildFile("MatrixControlInitAllMasterEmpty", "", true);
        expect(emptyDir.createDirectory());
        expect(! Core::InitTemplateLoader::resolveMasterInitFile(emptyDir).existsAsFile());

        fillModelWithDistinctValues(model);
        model.data()[12] = 0;
        model.data()[35] = 1;
        mapper.bufferToApvts();

        int dispatchCount = 0;
        Core::MasterParameterSysExDispatcher dispatcher(model, [&](const juce::uint8*) { ++dispatchCount; });
        Core::MasterModuleInitService service(model, mapper, loader, dispatcher, [&]() { return emptyDir; });

        const auto result = service.initAllModules();
        expect(result.success && result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.infoMessage.isNotEmpty());
        expect(dispatchCount == 1);

        Core::MasterModel expectedDefaults;
        expectedDefaults.loadFrom(Core::InitDefaults::masterData());

        using namespace PluginIDs::MasterEditSection;
        expect(moduleBytesMatch(model, expectedDefaults, MidiModule::kGroupId)
               && moduleBytesMatch(model, expectedDefaults, VibratoModule::kGroupId)
               && moduleBytesMatch(model, expectedDefaults, MiscModule::kGroupId));

        Core::MasterModel fromApvts;
        Core::ApvtsMasterMapper(proc.apvts, fromApvts).apvtsToBuffer();
        expect(std::memcmp(fromApvts.data(), model.data(), Core::MasterModel::kBufferSize) == 0);

        emptyDir.deleteRecursively();
    }

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

    void deletePatchInit_removesFileAndLoadFallsBack()
    {
        beginTest("deletePatchInit_removesFileAndLoadFallsBack");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;
        Core::PatchFileService fileService(decoder);
        Core::InitTemplateLoader loader(decoder);

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("TEMPNAME");

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitDeletePatch", "", true);
        expect(tempDir.createDirectory());

        expect(Core::InitTemplateWriter::writePatchInit(model, fileService, encoder, tempDir).success);
        expect(Core::InitTemplateWriter::patchInitExists(tempDir));

        const auto deleted = Core::InitTemplateWriter::deletePatchInit(tempDir);
        expect(deleted.success);
        expectEquals(deleted.infoMessage,
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kPatchInitTemplateDeleted));
        expect(! Core::InitTemplateWriter::patchInitExists(tempDir));

        Core::PatchModel loaded;
        const auto load = loader.loadPatch(loaded, tempDir);
        expect(load.success);
        expect(load.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(load.fallbackReason == Core::InitTemplateFallbackReason::kFileMissing);
        expect(std::memcmp(loaded.data(), Core::InitDefaults::patchData(), Core::PatchModel::kBufferSize) == 0);

        tempDir.deleteRecursively();
    }

    void deleteMasterInit_removesFileAndLoadFallsBack()
    {
        beginTest("deleteMasterInit_removesFileAndLoadFallsBack");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;
        Core::InitTemplateLoader loader(decoder);

        Core::MasterModel model;
        model.loadFrom(Core::InitDefaults::masterData());

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitDeleteMaster", "", true);
        expect(tempDir.createDirectory());

        expect(Core::InitTemplateWriter::writeMasterInit(model, encoder, tempDir).success);
        expect(Core::InitTemplateWriter::masterInitExists(tempDir));

        const auto deleted = Core::InitTemplateWriter::deleteMasterInit(tempDir);
        expect(deleted.success);
        expectEquals(deleted.infoMessage,
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterInitTemplateDeleted));
        expect(! Core::InitTemplateWriter::masterInitExists(tempDir));

        Core::MasterModel loaded;
        const auto load = loader.loadMaster(loaded, tempDir);
        expect(load.success);
        expect(load.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(load.fallbackReason == Core::InitTemplateFallbackReason::kFileMissing);

        tempDir.deleteRecursively();
    }

    void deletePatchInit_absentFileFails()
    {
        beginTest("deletePatchInit_absentFileFails");

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitDeletePatchAbsent", "", true);
        expect(tempDir.createDirectory());
        expect(! Core::InitTemplateWriter::patchInitExists(tempDir));

        const auto deleted = Core::InitTemplateWriter::deletePatchInit(tempDir);
        expect(! deleted.success);
        expectEquals(deleted.infoMessage,
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kInitTemplateDeleteFailed));
        expect(! Core::InitTemplateWriter::patchInitExists(tempDir));

        tempDir.deleteRecursively();
    }

    void deleteMasterInit_absentFileFails()
    {
        beginTest("deleteMasterInit_absentFileFails");

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitDeleteMasterAbsent", "", true);
        expect(tempDir.createDirectory());
        expect(! Core::InitTemplateWriter::masterInitExists(tempDir));

        const auto deleted = Core::InitTemplateWriter::deleteMasterInit(tempDir);
        expect(! deleted.success);
        expectEquals(deleted.infoMessage,
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kInitTemplateDeleteFailed));
        expect(! Core::InitTemplateWriter::masterInitExists(tempDir));

        tempDir.deleteRecursively();
    }

    void initExists_reflectsWriteAndDelete()
    {
        beginTest("initExists_reflectsWriteAndDelete");

        SysExParser parser;
        SysExDecoder decoder(parser);
        SysExEncoder encoder;
        Core::PatchFileService fileService(decoder);

        Core::PatchModel patch;
        patch.loadFrom(Core::InitDefaults::patchData());
        Core::MasterModel master;
        master.loadFrom(Core::InitDefaults::masterData());

        const auto tempDir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                 .getNonexistentChildFile("MatrixControlInitExists", "", true);
        expect(tempDir.createDirectory());

        expect(! Core::InitTemplateWriter::patchInitExists(tempDir));
        expect(! Core::InitTemplateWriter::masterInitExists(tempDir));

        expect(Core::InitTemplateWriter::writePatchInit(patch, fileService, encoder, tempDir).success);
        expect(Core::InitTemplateWriter::writeMasterInit(master, encoder, tempDir).success);
        expect(Core::InitTemplateWriter::patchInitExists(tempDir));
        expect(Core::InitTemplateWriter::masterInitExists(tempDir));

        expect(Core::InitTemplateWriter::deletePatchInit(tempDir).success);
        expect(Core::InitTemplateWriter::deleteMasterInit(tempDir).success);
        expect(! Core::InitTemplateWriter::patchInitExists(tempDir));
        expect(! Core::InitTemplateWriter::masterInitExists(tempDir));

        tempDir.deleteRecursively();
    }
};

static InitTemplateWriterTests initTemplateWriterTests;
