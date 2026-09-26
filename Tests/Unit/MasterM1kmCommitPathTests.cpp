#include <cstring>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Services/MasterM1kmCodec.h"
#include "Core/Services/MasterM1kmLoadPolicy.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "MasterModuleInitServiceTestSupport.h"
#include "PatchFixturePaths.h"

namespace
{
    juce::AudioProcessorValueTreeState::ParameterLayout makeMasterLayout()
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

    bool decodeStudyFixture(const juce::String& fileName, juce::uint8* packedOut172)
    {
        const auto file = PatchTestFixtures::matrix1000EditorMastersDir().getChildFile(fileName);
        if (! file.existsAsFile())
            return false;

        juce::MemoryBlock data;
        if (! file.loadFileAsData(data))
            return false;

        return Core::MasterM1kmCodec::decodeToPacked(data, packedOut172);
    }

    int countQueuedMasterSysEx(Core::MidiOutboundQueue& queue)
    {
        int count = 0;

        while (! queue.isEmpty())
        {
            const auto msg = queue.dequeue();
            if (! msg.has_value())
                break;

            if (msg->sysExData.getSize() != SysExConstants::kMasterMessageLength)
                continue;

            const auto* bytes = static_cast<const juce::uint8*>(msg->sysExData.getData());
            if (bytes[3] == SysExConstants::Opcode::kMasterParameterData)
                ++count;
        }

        return count;
    }

    // Mirrors PluginProcessor::commitMasterUserLoadToApvtsAndSynth without the processor TU.
    void commitLikeSettingsLoad(Core::ApvtsMasterMapper& mapper,
                                juce::AudioProcessorValueTreeState& apvts,
                                Core::MasterParameterSysExDispatcher& dispatcher)
    {
        mapper.bufferToApvts();
        (void) apvts.copyState(); // flush deferred APVTS→ValueTree sync
        dispatcher.dispatchFull();
        apvts.state.setProperty("uiMessageText",
                               PluginDisplayNames::Settings::FooterMessages::kMasterLoaded,
                               nullptr);
        apvts.state.setProperty("uiMessageSeverity", juce::String("info"), nullptr);
    }
} // namespace

class MasterM1kmCommitPathTests : public juce::UnitTest
{
public:
    MasterM1kmCommitPathTests() : juce::UnitTest("MasterM1kmCommitPath") {}

    void runTest() override
    {
        settingsOnly_pushesApvtsDispatchesAndFooter();
    }

private:
    void settingsOnly_pushesApvtsDispatchesAndFooter()
    {
        beginTest("settingsOnly_pushesApvtsDispatchesAndFooter");

        juce::uint8 packed[Core::MasterM1kmCodec::kInt16Count] = {};
        expect(decodeStudyFixture("All Groups Off.m1km", packed));

        TestAudioProcessorMasterInit proc(makeMasterLayout());
        Core::MasterModel model;
        model.loadFrom(Core::InitDefaults::masterData());
        Core::ApvtsMasterMapper mapper(proc.apvts, model);

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        int enqueueCount = 0;
        Core::MasterParameterSysExDispatcher dispatcher(
            model,
            [&](const juce::uint8* packedData)
            {
                ++enqueueCount;
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeMasterSysEx(0x03, packedData));
            });

        Core::MasterM1kmLoadPolicy::loadPackedIntoModel(
            model, packed, Core::MasterM1kmGroupsPolicy::kMasterSettingsOnly,
            Core::InitDefaults::masterData());
        commitLikeSettingsLoad(mapper, proc.apvts, dispatcher);

        expectEquals(enqueueCount, 1);
        expectEquals(countQueuedMasterSysEx(queue), 1);

        expectEquals(proc.apvts.state.getProperty("uiMessageText").toString()
                         + "|" + proc.apvts.state.getProperty("uiMessageSeverity").toString(),
                     juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterLoaded) + "|info");

        const auto* defaults = Core::InitDefaults::masterData();
        expect(std::memcmp(model.data() + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           defaults + Core::MasterM1kmLoadPolicy::kGroupsOffset,
                           Core::MasterM1kmLoadPolicy::kGroupsCount)
                   == 0
               && std::memcmp(model.data() + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                              defaults + Core::MasterM1kmLoadPolicy::kCascadeOffset,
                              Core::MasterM1kmLoadPolicy::kCascadeCount)
                      == 0);
    }
};

static MasterM1kmCommitPathTests masterM1kmCommitPathTests;
