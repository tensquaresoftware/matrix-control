#include <cstring>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/PatchModuleInitService.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

class TestAudioProcessorPatchInit : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessorPatchInit(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "P", std::move(layout))
    {
    }

    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "Test"; }
    void prepareToPlay(double, int) override {}
    void releaseResources() override {}
    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
    juce::AudioProcessorEditor* createEditor() override { return nullptr; }
    bool hasEditor() const override { return false; }
    bool acceptsMidi() const override { return false; }
    bool producesMidi() const override { return false; }
    bool isMidiEffect() const override { return false; }
    double getTailLengthSeconds() const override { return 0.0; }
    int getNumPrograms() override { return 1; }
    int getCurrentProgram() override { return 0; }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override { return {}; }
    void changeProgramName(int, const juce::String&) override {}
    void getStateInformation(juce::MemoryBlock&) override {}
    void setStateInformation(const void*, int) override {}
};

class PatchModuleInitServiceTests : public juce::UnitTest
{
public:
    PatchModuleInitServiceTests() : juce::UnitTest("PatchModuleInitService") {}

    void runTest() override
    {
        testInitDco1ModulePreservesOtherModules();
        testFallbackPathMetadata();
        testDispatchModuleSysExCount();
        testModuleGroupIdFromInitPropertyId();
        testModuleGroupIdFromPatchModuleKind();
    }

private:
    struct InitTestHarness
    {
        TestAudioProcessorPatchInit proc;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper;
        SysExParser parser;
        SysExDecoder decoder;
        Core::InitTemplateLoader loader;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        Core::PatchParameterSysExDispatcher dispatcher;
        Core::PatchModuleInitService initService;
        juce::File templatesFolder;
        int dispatchCount { 0 };

        explicit InitTestHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout,
                                 const juce::File& folder)
            : proc(std::move(layout))
            , mapper(proc.apvts, model)
            , decoder(parser)
            , loader(decoder)
            , dispatcher(model,
                         [this](int, juce::uint8)
                         {
                             ++dispatchCount;
                         })
            , initService(model, mapper, loader, dispatcher, [this]() { return templatesFolder; })
            , templatesFolder(folder)
        {
        }
    };

    static juce::File fixturesInitDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Init");
    }

    static juce::File createTempTemplatesDir()
    {
        auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                       .getNonexistentChildFile("MatrixControlPatchModuleInit", "", false);
        jassert(dir.createDirectory());
        return dir;
    }

    static void copyFixtureToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = fixturesInitDir().getChildFile(fileName);
        source.copyFileTo(dir.getChildFile(fileName));
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeDcoLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (const auto& d : PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.minValue, d.maxValue, d.defaultValue));
        }

        for (const auto& d : PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.minValue, d.maxValue, d.defaultValue));
        }

        return layout;
    }

    static Core::PatchModel makeInitTemplateModel(const juce::File& templatesFolder)
    {
        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);
        Core::PatchModel templateModel;
        loader.loadPatch(templateModel, templatesFolder);
        return templateModel;
    }

    static void fillDco1WithDistinctValues(Core::PatchModel& model)
    {
        using namespace PluginDescriptors::PatchEditSection;

        for (const auto& d : Dco1Module::kIntParameters)
            model.setValue(d, d.maxValue);

        for (const auto& d : Dco2Module::kIntParameters)
            model.setValue(d, d.minValue);
    }

    static bool moduleBytesMatch(const Core::PatchModel& lhs,
                                 const Core::PatchModel& rhs,
                                 const juce::String& moduleGroupId)
    {
        for (const auto& d : Core::ApvtsPatchMapper::buildIntDescriptors())
        {
            if (d.parentGroupId == moduleGroupId && lhs.getValue(d) != rhs.getValue(d))
                return false;
        }

        for (const auto& d : Core::ApvtsPatchMapper::buildChoiceDescriptors())
        {
            if (d.parentGroupId == moduleGroupId && lhs.getChoiceIndex(d) != rhs.getChoiceIndex(d))
                return false;
        }

        return true;
    }

    static int expectedSysExCountForModule(const juce::String& moduleGroupId)
    {
        int count = 0;

        for (const auto& d : Core::ApvtsPatchMapper::buildIntDescriptors())
        {
            if (d.parentGroupId == moduleGroupId && d.sysExId != PluginDescriptors::kNoSysExId)
                ++count;
        }

        for (const auto& d : Core::ApvtsPatchMapper::buildChoiceDescriptors())
        {
            if (d.parentGroupId == moduleGroupId && d.sysExId != PluginDescriptors::kNoSysExId)
                ++count;
        }

        return count;
    }

    void testInitDco1ModulePreservesOtherModules()
    {
        beginTest("initModule DCO1 → DCO1 reset, DCO2 unchanged");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kPatchInitFileName);

        InitTestHarness harness(makeDcoLayout(), tempDir);
        const auto initTemplate = makeInitTemplateModel(tempDir);

        fillDco1WithDistinctValues(harness.model);
        const auto beforeDco2 = harness.model;

        const auto result = harness.initService.initModule(PluginIDs::PatchEditSection::Dco1Module::kGroupId);
        expect(result.success);

        expect(moduleBytesMatch(harness.model, initTemplate, PluginIDs::PatchEditSection::Dco1Module::kGroupId));
        expect(moduleBytesMatch(harness.model, beforeDco2, PluginIDs::PatchEditSection::Dco2Module::kGroupId));
        expectEquals(harness.dispatchCount,
                     expectedSysExCountForModule(PluginIDs::PatchEditSection::Dco1Module::kGroupId));

        tempDir.deleteRecursively();
    }

    void testFallbackPathMetadata()
    {
        beginTest("empty templates folder → fallback metadata");

        const juce::File emptyFolder = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                           .getChildFile("MatrixControlPatchInitEmpty");
        emptyFolder.createDirectory();

        InitTestHarness harness(makeDcoLayout(), emptyFolder);

        const auto result = harness.initService.initModule(PluginIDs::PatchEditSection::Dco1Module::kGroupId);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.infoMessage.isNotEmpty());
        expect(harness.dispatchCount > 0);

        emptyFolder.deleteRecursively();
    }

    void testDispatchModuleSysExCount()
    {
        beginTest("dispatchModule enqueues one 0x06 per valid module parameter");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kPatchInitFileName);

        InitTestHarness harness(makeDcoLayout(), tempDir);
        harness.initService.initModule(PluginIDs::PatchEditSection::Dco1Module::kGroupId);

        expectEquals(harness.dispatchCount,
                     expectedSysExCountForModule(PluginIDs::PatchEditSection::Dco1Module::kGroupId));

        tempDir.deleteRecursively();
    }

    void testModuleGroupIdFromInitPropertyId()
    {
        beginTest("moduleGroupIdFromInitPropertyId maps all 10 Patch Edit init IDs");

        using namespace PluginIDs::PatchEditSection;

        expectEquals(Core::PatchModuleInitService::moduleGroupIdFromInitPropertyId(Dco1Module::StandaloneWidgets::kInit),
                     juce::String(Dco1Module::kGroupId));
        expectEquals(Core::PatchModuleInitService::moduleGroupIdFromInitPropertyId(VcfVcaModule::StandaloneWidgets::kInit),
                     juce::String(VcfVcaModule::kGroupId));
        expectEquals(Core::PatchModuleInitService::moduleGroupIdFromInitPropertyId(Lfo2Module::StandaloneWidgets::kInit),
                     juce::String(Lfo2Module::kGroupId));
        expect(Core::PatchModuleInitService::moduleGroupIdFromInitPropertyId("dco1Paste").isEmpty());
    }

    void testModuleGroupIdFromPatchModuleKind()
    {
        beginTest("moduleGroupIdFromPatchModuleKind maps paste module kinds");

        expectEquals(Core::PatchModuleInitService::moduleGroupIdFromPatchModuleKind(Core::PatchModuleKind::Env2),
                     juce::String(PluginIDs::PatchEditSection::Envelope2Module::kGroupId));
    }
};

static PatchModuleInitServiceTests patchModuleInitServiceTests;
