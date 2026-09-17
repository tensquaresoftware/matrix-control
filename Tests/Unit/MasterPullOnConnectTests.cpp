#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MasterInboundApply.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/MasterPullOnConnectPolicy.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/MidiManager.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Util/ComboboxPatchSendDebouncer.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{

constexpr int kTestDebounceMs = 20;
constexpr juce::uint8 kMasterVersion = 0x03;

class TestAudioProcessorMasterPull : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessorMasterPull(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "M", std::move(layout))
    {
    }

    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "TestMasterPull"; }
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

struct QuietMasterApplyHarness
{
    Core::MasterModel model;
    Core::MidiOutboundQueue queue;
    Core::MidiActivityTracker tracker;
    SysExEncoder encoder;
    Core::MasterParameterSysExDispatcher dispatcher;
    Core::ComboboxPatchSendDebouncer debouncer;
    bool suppressMasterParameterSysEx = false;
    int enqueueCount = 0;
    std::unique_ptr<TestAudioProcessorMasterPull> processor;
    std::unique_ptr<Core::ApvtsMasterMapper> mapper;
    juce::String unisonId;
    juce::String masterTuneId;

    QuietMasterApplyHarness()
        : dispatcher(model,
                     [this](const juce::uint8* packedData)
                     {
                         ++enqueueCount;
                         Core::EditorPath editorPath(queue, tracker);
                         editorPath.enqueueSysEx(encoder.encodeMasterSysEx(kMasterVersion, packedData));
                     })
        , debouncer(kTestDebounceMs)
    {
        const auto choiceDescs = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        const auto intDescs = Core::ApvtsMasterMapper::buildIntDescriptors();

        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        for (const auto& d : choiceDescs)
        {
            if (d.parameterId == PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable)
            {
                unisonId = d.parameterId;
                layout.add(std::make_unique<juce::AudioParameterChoice>(
                    juce::ParameterID(d.parameterId, 1), d.displayName, d.choices, d.defaultIndex));
            }
        }
        for (const auto& d : intDescs)
        {
            if (d.parameterId == PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kMasterTune)
            {
                masterTuneId = d.parameterId;
                layout.add(std::make_unique<juce::AudioParameterInt>(
                    juce::ParameterID(d.parameterId, 1), d.displayName,
                    d.minValue, d.maxValue, d.defaultValue));
            }
        }

        processor = std::make_unique<TestAudioProcessorMasterPull>(std::move(layout));
        mapper = std::make_unique<Core::ApvtsMasterMapper>(processor->apvts, model);
    }

    void noteMasterParameterChanged()
    {
        if (suppressMasterParameterSysEx)
            return;

        debouncer.schedule([this]
                           {
                               if (! suppressMasterParameterSysEx)
                                   dispatcher.dispatchFull();
                           });
    }

    void cancelPending() noexcept { debouncer.cancel(); }

    void applyInboundMasterDump(const std::vector<juce::uint8>& packedMaster)
    {
        Core::applyQuietInboundMasterDump({
            model,
            *mapper,
            processor->apvts,
            packedMaster,
            [this] { cancelPending(); },
            suppressMasterParameterSysEx,
        });
    }
};

} // namespace

class MasterPullOnConnectTests : public juce::UnitTest
{
public:
    MasterPullOnConnectTests() : juce::UnitTest("MasterPullOnConnect") {}

    void runTest() override
    {
        policy_skipsMatrix6FamilyAndHeartbeat();
        policy_pullsOnFreshDetectAndReconnect();
        apply_loadsUnisonAndMasterTuneUnderSuppress();
        apply_cancelsPendingDebounceWithoutStalePush();
        apply_leavesPatchKeyboardModeUntouched();
        apply_rejectsWrongSizeWithoutMutating();
        deliver_successClearsFailureFooterAndApplies();
        deliver_wrongSizePublishesFooterWithoutApply();
        deliver_emptyDumpPublishesFooterWithoutApply();
        seam_inquirySuccessArmsPullHeartbeatDoesNot();
        cancel_invokesPendingCallbackWithEmpty();
    }

private:
    void policy_skipsMatrix6FamilyAndHeartbeat()
    {
        beginTest("policy: Matrix-6/6R skip; heartbeat while already M-1000 skips");

        expect(! Core::shouldPullMasterAfterDeviceInquirySuccess(
            false, MatrixDeviceTypes::Type::kUnknown, MatrixDeviceTypes::Type::kMatrix6));
        expect(! Core::shouldPullMasterAfterDeviceInquirySuccess(
            false, MatrixDeviceTypes::Type::kUnknown, MatrixDeviceTypes::Type::kMatrix6R));
        expect(! Core::shouldPullMasterAfterDeviceInquirySuccess(
            true,
            MatrixDeviceTypes::Type::kMatrix1000,
            MatrixDeviceTypes::Type::kMatrix1000));
        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            true,
            MatrixDeviceTypes::Type::kMatrix1000,
            MatrixDeviceTypes::Type::kMatrix1000,
            true),
               "Port-pair reconnect must force Master pull even while still flagged detected");
    }

    void policy_pullsOnFreshDetectAndReconnect()
    {
        beginTest("policy: fresh detect, reconnect, and 6/6R→M-1000 promotion pull");

        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            false, MatrixDeviceTypes::Type::kUnknown, MatrixDeviceTypes::Type::kMatrix1000));
        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            false, MatrixDeviceTypes::Type::kMatrix1000, MatrixDeviceTypes::Type::kMatrix1000));
        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            true, MatrixDeviceTypes::Type::kUnknown, MatrixDeviceTypes::Type::kMatrix1000));
        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            true, MatrixDeviceTypes::Type::kMatrix6, MatrixDeviceTypes::Type::kMatrix1000));
        expect(Core::shouldPullMasterAfterDeviceInquirySuccess(
            true, MatrixDeviceTypes::Type::kMatrix6R, MatrixDeviceTypes::Type::kMatrix1000));
    }

    static std::vector<juce::uint8> makeMasterDump(juce::uint8 unisonEnable, juce::uint8 masterTune)
    {
        std::vector<juce::uint8> dump(SysExConstants::kMasterPackedDataSize, 0);
        dump[169] = unisonEnable;
        dump[8] = masterTune;
        return dump;
    }

    void apply_loadsUnisonAndMasterTuneUnderSuppress()
    {
        beginTest("apply: Unison Enable + Master Tune load under suppress (no outbound)");

        QuietMasterApplyHarness harness;
        expect(harness.unisonId.isNotEmpty());
        expect(harness.masterTuneId.isNotEmpty());

        auto* unisonRaw = harness.processor->apvts.getRawParameterValue(harness.unisonId);
        auto* tuneRaw = harness.processor->apvts.getRawParameterValue(harness.masterTuneId);
        expect(unisonRaw != nullptr && tuneRaw != nullptr);
        if (unisonRaw == nullptr || tuneRaw == nullptr)
            return;

        *unisonRaw = 0.0f;
        *tuneRaw = 0.0f;
        harness.mapper->apvtsToBuffer();

        harness.applyInboundMasterDump(makeMasterDump(1, 17));

        expectEquals(static_cast<int>(harness.model.data()[169]), 1);
        expectEquals(static_cast<int>(harness.model.data()[8]), 17);
        expectEquals(juce::roundToInt(unisonRaw->load()), 1);
        expectEquals(juce::roundToInt(tuneRaw->load()), 17);
        expectEquals(harness.enqueueCount, 0);
    }

    void apply_cancelsPendingDebounceWithoutStalePush()
    {
        beginTest("apply: cancels pending Master debounce so stale push cannot fight dump");

        QuietMasterApplyHarness harness;
        harness.noteMasterParameterChanged();
        expect(harness.debouncer.isPending());

        harness.applyInboundMasterDump(makeMasterDump(1, 10));
        expect(! harness.debouncer.isPending());
        expectEquals(harness.enqueueCount, 0);

        harness.debouncer.flushPendingSynchronouslyForTests();
        expectEquals(harness.enqueueCount, 0);
    }

    void apply_leavesPatchKeyboardModeUntouched()
    {
        beginTest("apply: does not rewrite patch Keyboard Mode (byte 8 / independence)");

        QuietMasterApplyHarness harness;
        Core::PatchModel patch;
        patch.data()[8] = 2; // UNISON keyboard mode

        harness.applyInboundMasterDump(makeMasterDump(1, 33));

        expectEquals(static_cast<int>(patch.data()[8]), 2);
        expectEquals(static_cast<int>(harness.model.data()[169]), 1);
    }

    void apply_rejectsWrongSizeWithoutMutating()
    {
        beginTest("apply: wrong-size dump leaves Master unchanged");

        QuietMasterApplyHarness harness;
        harness.model.data()[169] = 0;
        harness.model.data()[8] = 7;
        harness.mapper->bufferToApvts();

        std::vector<juce::uint8> bad(10, 1);
        harness.applyInboundMasterDump(bad);

        expectEquals(static_cast<int>(harness.model.data()[169]), 0);
        expectEquals(static_cast<int>(harness.model.data()[8]), 7);
        expectEquals(harness.enqueueCount, 0);
    }

    void deliver_successClearsFailureFooterAndApplies()
    {
        beginTest("deliver: valid dump clears kMasterPullFailed and invokes apply handler");

        QuietMasterApplyHarness harness;
        MidiManager manager(harness.processor->apvts, harness.queue, harness.tracker);

        harness.processor->apvts.state.setProperty(
            "uiMessageText",
            juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterPullFailed),
            nullptr);
        harness.processor->apvts.state.setProperty("uiMessageSeverity", "warning", nullptr);

        std::vector<juce::uint8> received;
        manager.setMasterPullApplyHandler(
            [&](std::vector<juce::uint8> packed)
            {
                received = std::move(packed);
            });

        const auto dump = makeMasterDump(1, 12);
        manager.deliverMasterPullResult(dump);

        expect(received.size() == dump.size(), "Apply handler must receive the dump");
        expect(received == dump, "Apply handler payload must match delivered dump");
        expect(harness.processor->apvts.state.getProperty("uiMessageText").toString()
                   != PluginDisplayNames::Settings::FooterMessages::kMasterPullFailed,
               "Success must clear sticky Master pull failure footer");
    }

    void deliver_wrongSizePublishesFooterWithoutApply()
    {
        beginTest("deliver: non-empty wrong-size dump publishes footer and skips apply");

        QuietMasterApplyHarness harness;
        MidiManager manager(harness.processor->apvts, harness.queue, harness.tracker);

        bool applyCalled = false;
        manager.setMasterPullApplyHandler(
            [&](std::vector<juce::uint8>)
            {
                applyCalled = true;
            });

        std::vector<juce::uint8> bad(32, 1);
        manager.deliverMasterPullResult(std::move(bad));

        expect(! applyCalled, "Wrong-size Master pull must not invoke apply handler");
        expectEquals(
            harness.processor->apvts.state.getProperty("uiMessageText").toString(),
            juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterPullFailed));
    }

    void deliver_emptyDumpPublishesFooterWithoutApply()
    {
        beginTest("failure: empty dump publishes footer and skips apply handler");

        QuietMasterApplyHarness harness;
        MidiManager manager(harness.processor->apvts, harness.queue, harness.tracker);

        bool applyCalled = false;
        manager.setMasterPullApplyHandler(
            [&](std::vector<juce::uint8>)
            {
                applyCalled = true;
            });

        manager.deliverMasterPullResult({});

        expect(! applyCalled, "Empty Master pull must not invoke apply handler");
        expectEquals(
            harness.processor->apvts.state.getProperty("uiMessageText").toString(),
            juce::String(PluginDisplayNames::Settings::FooterMessages::kMasterPullFailed));
        expectEquals(
            harness.processor->apvts.state.getProperty("uiMessageSeverity").toString(),
            juce::String("warning"));
    }

    void seam_inquirySuccessArmsPullHeartbeatDoesNot()
    {
        beginTest("seam: inquiry success arms Master pull; heartbeat-style inputs do not");

        QuietMasterApplyHarness harness;
        MidiManager manager(harness.processor->apvts, harness.queue, harness.tracker);

        harness.processor->apvts.state.setProperty("deviceDetected", true, nullptr);
        harness.processor->apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                                   MatrixDeviceTypes::kMatrix1000Id,
                                                   nullptr);

        expect(manager.maybePullMasterAfterInquirySuccess(
                   false,
                   MatrixDeviceTypes::Type::kUnknown,
                   MatrixDeviceTypes::Type::kMatrix1000),
               "Fresh M-1000 detect must arm async Master pull");

        manager.cancelPendingSysExRequest();

        expect(! manager.maybePullMasterAfterInquirySuccess(
                    true,
                    MatrixDeviceTypes::Type::kMatrix1000,
                    MatrixDeviceTypes::Type::kMatrix1000),
               "Heartbeat while already M-1000 must not arm Master pull");

        expect(manager.maybePullMasterAfterInquirySuccess(
                   true,
                   MatrixDeviceTypes::Type::kMatrix1000,
                   MatrixDeviceTypes::Type::kMatrix1000,
                   true),
               "Port-pair reconnect force must arm Master pull while still flagged detected");
    }

    void cancel_invokesPendingCallbackWithEmpty()
    {
        beginTest("cancel: pending async callback is invoked with empty vector");

        QuietMasterApplyHarness harness;
        MidiManager manager(harness.processor->apvts, harness.queue, harness.tracker);

        harness.processor->apvts.state.setProperty("deviceDetected", true, nullptr);
        harness.processor->apvts.state.setProperty(MatrixDeviceTypes::kApvtsPropertyName,
                                                   MatrixDeviceTypes::kMatrix1000Id,
                                                   nullptr);

        bool callbackRan = false;
        bool callbackEmpty = false;
        // Long settle keeps the request pending until cancel (no hardware needed).
        manager.requestMasterDataAsync(
            [&](std::vector<juce::uint8> packed)
            {
                callbackRan = true;
                callbackEmpty = packed.empty();
            },
            5000,
            5000);

        manager.cancelPendingSysExRequest();

        expect(callbackRan, "Cancel must invoke the pending async callback");
        expect(callbackEmpty, "Cancelled async request must deliver an empty vector");
    }
};

static MasterPullOnConnectTests masterPullOnConnectTests;
