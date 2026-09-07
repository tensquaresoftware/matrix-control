#include <cstring>
#include <unordered_set>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Init/InitTemplateLoader.h"
#include "Core/Init/MasterModuleInitService.h"
#include "Core/Exceptions/ExceptionPropagator.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/MidiChannelMasterCodec.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"
#include "MasterModuleInitServiceTestSupport.h"

class MasterModuleInitServiceTests : public juce::UnitTest
{
public:
    MasterModuleInitServiceTests() : juce::UnitTest("MasterModuleInitService") {}

    void runTest() override
    {
        testInitMidiModulePreservesOtherModules();
        testInitVibratoModulePreservesOtherModules();
        testInitMiscModulePreservesOtherModules();
        testFallbackPathMetadata();
        testInvalidMasterFileFooterSeverity();
        testFooterPropagationOnFallback();
        testFooterClearsAfterSuccessfulInit();
        testInvalidInitPropertyIdNoOp();
        testApvtsCoherenceAfterMidiInit();
        testSuppressDuringApvtsPush();
    }

private:
    struct InitTestHarness
    {
        TestAudioProcessorMasterInit proc;
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper;
        SysExParser parser;
        SysExDecoder decoder;
        Core::InitTemplateLoader loader;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        Core::MasterParameterSysExDispatcher dispatcher;
        Core::MasterModuleInitService initService;
        juce::File templatesFolder;

        explicit InitTestHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout,
                                 const juce::File& folder)
            : proc(std::move(layout))
            , mapper(proc.apvts, model)
            , decoder(parser)
            , loader(decoder)
            , dispatcher(model,
                         [this](const juce::uint8* packedData)
                         {
                             Core::EditorPath editorPath(queue, tracker);
                             editorPath.enqueueSysEx(encoder.encodeMasterSysEx(0x03, packedData));
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
                       .getNonexistentChildFile("MatrixControlMasterModuleInit", "", false);
        jassert(dir.createDirectory());
        return dir;
    }

    static void copyFixtureToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = fixturesInitDir().getChildFile(fileName);
        source.copyFileTo(dir.getChildFile(fileName));
    }

    static bool sysExMatchesMasterMessage(const juce::MemoryBlock& block)
    {
        if (block.getSize() != SysExConstants::kMasterMessageLength)
            return false;

        const auto* data = static_cast<const juce::uint8*>(block.getData());
        return data[0] == SysExConstants::kSysExStart
            && data[1] == SysExConstants::kManufacturerIdOberheim
            && data[2] == SysExConstants::kDeviceIdMatrix1000
            && data[3] == SysExConstants::Opcode::kMasterParameterData
            && data[4] == 0x03
            && data[block.getSize() - 1] == SysExConstants::kSysExEnd;
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeFullMasterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (const auto& d : PluginDescriptors::MasterEditSection::kIntParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.minValue, d.maxValue, d.defaultValue));
        }

        for (const auto& d : PluginDescriptors::MasterEditSection::kChoiceParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.choices, d.defaultIndex));
        }

        return layout;
    }

    static Core::MasterModel makeInitTemplateModel(const juce::File& templatesFolder)
    {
        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);
        Core::MasterModel templateModel;
        loader.loadMaster(templateModel, templatesFolder);
        return templateModel;
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

    void testInitMidiModulePreservesOtherModules()
    {
        beginTest("initModule MIDI → MIDI reset, other modules unchanged, one 0x03");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        InitTestHarness harness(makeFullMasterLayout(), tempDir);
        auto initTemplate = makeInitTemplateModel(tempDir);
        rewriteMasterInitWithOmniOn(tempDir, harness.encoder, initTemplate);

        fillModelWithDistinctValues(harness.model);
        harness.model.data()[12] = 0;
        harness.model.data()[35] = 1; // Mono dirty — fails if init only copies byte 11

        const auto beforeVibrato = harness.model;
        const auto beforeMisc = harness.model;

        const auto result = harness.initService.initModule(Core::MasterModuleKind::kMidi);
        expect(result.success);
        expect(result.infoMessage.isEmpty());

        expectEquals(static_cast<int>(harness.model.data()[12]), 1);
        expectEquals(static_cast<int>(harness.model.data()[35]), 0);
        expect(moduleBytesMatch(harness.model, initTemplate, PluginIDs::MasterEditSection::MidiModule::kGroupId));
        expect(moduleBytesMatch(harness.model, beforeVibrato, PluginIDs::MasterEditSection::VibratoModule::kGroupId));
        expect(moduleBytesMatch(harness.model, beforeMisc, PluginIDs::MasterEditSection::MiscModule::kGroupId));

        auto msg = harness.queue.dequeue();
        expect(msg.has_value());
        expect(sysExMatchesMasterMessage(msg->sysExData));
        expect(harness.queue.isEmpty());
    }

    void testInitVibratoModulePreservesOtherModules()
    {
        beginTest("initModule Vibrato → Vibrato reset, other modules unchanged");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        InitTestHarness harness(makeFullMasterLayout(), tempDir);
        const auto initTemplate = makeInitTemplateModel(tempDir);

        fillModelWithDistinctValues(harness.model);
        const auto beforeMidi = harness.model;
        const auto beforeMisc = harness.model;

        harness.initService.initModule(Core::MasterModuleKind::kVibrato);

        expect(moduleBytesMatch(harness.model, initTemplate, PluginIDs::MasterEditSection::VibratoModule::kGroupId));
        expect(moduleBytesMatch(harness.model, beforeMidi, PluginIDs::MasterEditSection::MidiModule::kGroupId));
        expect(moduleBytesMatch(harness.model, beforeMisc, PluginIDs::MasterEditSection::MiscModule::kGroupId));

        expect(harness.queue.dequeue().has_value());
        expect(harness.queue.isEmpty());
    }

    void testInitMiscModulePreservesOtherModules()
    {
        beginTest("initModule Misc → Misc reset, other modules unchanged");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        InitTestHarness harness(makeFullMasterLayout(), tempDir);
        const auto initTemplate = makeInitTemplateModel(tempDir);

        fillModelWithDistinctValues(harness.model);
        const auto beforeMidi = harness.model;
        const auto beforeVibrato = harness.model;

        harness.initService.initModule(Core::MasterModuleKind::kMisc);

        expect(moduleBytesMatch(harness.model, initTemplate, PluginIDs::MasterEditSection::MiscModule::kGroupId));
        expect(moduleBytesMatch(harness.model, beforeMidi, PluginIDs::MasterEditSection::MidiModule::kGroupId));
        expect(moduleBytesMatch(harness.model, beforeVibrato, PluginIDs::MasterEditSection::VibratoModule::kGroupId));

        expect(harness.queue.dequeue().has_value());
        expect(harness.queue.isEmpty());
    }

    void testFallbackPathMetadata()
    {
        beginTest("empty templates folder → fallback metadata");

        const juce::File emptyFolder = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                           .getChildFile("MatrixControlMasterInitEmpty");
        emptyFolder.createDirectory();

        InitTestHarness harness(makeFullMasterLayout(), emptyFolder);

        const auto result = harness.initService.initModule(Core::MasterModuleKind::kMidi);

        expect(result.success);
        expect(result.source == Core::InitTemplateSource::kHardcodedFallback);
        expect(result.infoMessage.isNotEmpty());
        expect(harness.queue.dequeue().has_value());
    }

    void testApvtsCoherenceAfterMidiInit()
    {
        beginTest("APVTS values match post-init MasterModel");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        InitTestHarness harness(makeFullMasterLayout(), tempDir);
        fillModelWithDistinctValues(harness.model);

        harness.initService.initModule(Core::MasterModuleKind::kMidi);

        using namespace PluginDescriptors::MasterEditSection;

        for (const auto& d : MidiModule::kIntParameters)
        {
            const int modelValue = harness.model.getValue(d);
            expectEquals(juce::roundToInt(harness.proc.apvts.getRawParameterValue(d.parameterId)->load()),
                         modelValue);
        }

        while (harness.queue.dequeue().has_value()) {}
    }

    struct ProcessorPathHarness : TestAudioProcessorMasterInit, juce::ValueTree::Listener
    {
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper;
        SysExParser parser;
        SysExDecoder decoder;
        Core::InitTemplateLoader loader;
        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        Core::MasterParameterSysExDispatcher dispatcher;
        Core::MasterModuleInitService initService;
        std::unordered_set<juce::String> masterParameterIds_;
        bool suppressMasterParameterSysEx_ { false };
        juce::File templatesFolder;

        ProcessorPathHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout,
                             const juce::File& folder)
            : TestAudioProcessorMasterInit(std::move(layout))
            , mapper(apvts, model)
            , decoder(parser)
            , loader(decoder)
            , dispatcher(model,
                         [this](const juce::uint8* packedData)
                         {
                             Core::EditorPath editorPath(queue, tracker);
                             editorPath.enqueueSysEx(encoder.encodeMasterSysEx(0x03, packedData));
                         })
            , initService(model, mapper, loader, dispatcher, [this]() { return templatesFolder; })
            , templatesFolder(folder)
        {
            for (const auto& d : Core::ApvtsMasterMapper::buildIntDescriptors())
                masterParameterIds_.insert(d.parameterId);
            for (const auto& d : Core::ApvtsMasterMapper::buildChoiceDescriptors())
                masterParameterIds_.insert(d.parameterId);

            apvts.state.addListener(this);
        }

        ~ProcessorPathHarness() override
        {
            apvts.state.removeListener(this);
        }

        void valueTreePropertyChanged(juce::ValueTree& tree, const juce::Identifier& property) override
        {
            juce::ignoreUnused(tree);
            const auto parameterId = property.toString();

            if (masterParameterIds_.count(parameterId) > 0)
            {
                if (!suppressMasterParameterSysEx_)
                    mapper.apvtsToBuffer();

                if (!suppressMasterParameterSysEx_)
                    dispatcher.dispatch(parameterId);
            }

            handleMasterModuleInitPropertyChange(parameterId);
        }

        void propagateInitTemplateFooterMessage(const Core::InitTemplateLoadResult& result)
        {
            if (result.infoMessage.isEmpty())
            {
                ExceptionPropagator::clearMessage(apvts);
                return;
            }

            apvts.state.setProperty("uiMessageText", result.infoMessage, nullptr);

            const auto severity = (result.fallbackReason == Core::InitTemplateFallbackReason::kFileInvalid)
                ? juce::String("warning")
                : juce::String("info");
            apvts.state.setProperty("uiMessageSeverity", severity, nullptr);
        }

        void handleMasterModuleInitPropertyChange(const juce::String& propertyId)
        {
            using namespace PluginIDs::MasterEditSection;

            std::optional<Core::MasterModuleKind> moduleKind;

            if (propertyId == MidiModule::StandaloneWidgets::kInit)
                moduleKind = Core::MasterModuleKind::kMidi;
            else if (propertyId == VibratoModule::StandaloneWidgets::kInit)
                moduleKind = Core::MasterModuleKind::kVibrato;
            else if (propertyId == MiscModule::StandaloneWidgets::kInit)
                moduleKind = Core::MasterModuleKind::kMisc;

            if (!moduleKind.has_value())
                return;

            suppressMasterParameterSysEx_ = true;
            const auto result = initService.initModule(*moduleKind);
            suppressMasterParameterSysEx_ = false;

            propagateInitTemplateFooterMessage(result);
        }

        void runMidiInitViaPropertyStamp()
        {
            using namespace PluginIDs::MasterEditSection;
            apvts.state.setProperty(MidiModule::StandaloneWidgets::kInit,
                                    juce::Time::getCurrentTime().toMilliseconds(),
                                    nullptr);
        }
    };

    void testInvalidMasterFileFooterSeverity()
    {
        beginTest("invalid MasterInit.syx → fallback warning severity in footer");

        const auto tempDir = createTempTemplatesDir();
        tempDir.getChildFile(Core::InitTemplateLoader::kMasterInitFileName)
            .replaceWithText("not valid sysex");

        ProcessorPathHarness harness(makeFullMasterLayout(), tempDir);
        fillModelWithDistinctValues(harness.model);
        harness.mapper.bufferToApvts();

        harness.handleMasterModuleInitPropertyChange(
            PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit);

        expectEquals(harness.apvts.state.getProperty("uiMessageSeverity").toString(), juce::String("warning"));
        expect(harness.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());

        while (harness.queue.dequeue().has_value()) {}
        tempDir.deleteRecursively();
    }

    void testFooterPropagationOnFallback()
    {
        beginTest("empty templates folder → footer info severity on APVTS");

        const juce::File emptyFolder = juce::File::getSpecialLocation(juce::File::tempDirectory)
                                           .getChildFile("MatrixControlMasterInitEmptyFooter");
        emptyFolder.createDirectory();

        ProcessorPathHarness harness(makeFullMasterLayout(), emptyFolder);
        harness.handleMasterModuleInitPropertyChange(
            PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit);

        expectEquals(harness.apvts.state.getProperty("uiMessageSeverity").toString(), juce::String("info"));
        expect(harness.apvts.state.getProperty("uiMessageText").toString().isNotEmpty());

        while (harness.queue.dequeue().has_value()) {}
    }

    void testFooterClearsAfterSuccessfulInit()
    {
        beginTest("successful user-file init clears stale footer");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        ProcessorPathHarness harness(makeFullMasterLayout(), tempDir);
        harness.apvts.state.setProperty("uiMessageText", "stale warning", nullptr);
        harness.apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);

        harness.handleMasterModuleInitPropertyChange(
            PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit);

        expect(harness.apvts.state.getProperty("uiMessageText").toString().isEmpty());
        expect(harness.apvts.state.getProperty("uiMessageSeverity").toString().isEmpty());

        while (harness.queue.dequeue().has_value()) {}
        tempDir.deleteRecursively();
    }

    void testInvalidInitPropertyIdNoOp()
    {
        beginTest("unknown init property id → no model change, no SysEx");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        ProcessorPathHarness harness(makeFullMasterLayout(), tempDir);
        fillModelWithDistinctValues(harness.model);
        const auto before = harness.model;

        harness.handleMasterModuleInitPropertyChange("dco1Init");

        expect(moduleBytesMatch(harness.model, before, PluginIDs::MasterEditSection::MidiModule::kGroupId));
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }

    void testSuppressDuringApvtsPush()
    {
        beginTest("suppress flag — no 0x03 until single explicit dispatch");

        const auto tempDir = createTempTemplatesDir();
        copyFixtureToDir(tempDir, Core::InitTemplateLoader::kMasterInitFileName);

        ProcessorPathHarness harness(makeFullMasterLayout(), tempDir);
        fillModelWithDistinctValues(harness.model);
        harness.mapper.bufferToApvts();

        const auto initTemplate = makeInitTemplateModel(tempDir);
        harness.runMidiInitViaPropertyStamp();

        expect(moduleBytesMatch(harness.model, initTemplate, PluginIDs::MasterEditSection::MidiModule::kGroupId));

        using namespace PluginDescriptors::MasterEditSection;

        for (const auto& d : MidiModule::kIntParameters)
        {
            expectEquals(juce::roundToInt(harness.apvts.getRawParameterValue(d.parameterId)->load()),
                         harness.model.getValue(d));
        }

        for (const auto& d : MidiModule::kChoiceParameters)
        {
            expectEquals(
                juce::roundToInt(harness.apvts.getRawParameterValue(d.parameterId)->load()),
                d.parameterId == PluginIDs::MasterEditSection::MidiModule::ParameterWidgets::kChannel
                    ? Core::MidiChannelMasterCodec::readComboIndex(harness.model.data(),
                                                                  Core::MasterModel::kBufferSize)
                    : harness.model.getChoiceIndex(d));
        }

        expect(harness.queue.dequeue().has_value());
        expect(harness.queue.isEmpty());

        tempDir.deleteRecursively();
    }
};

static MasterModuleInitServiceTests masterModuleInitServiceTests;
