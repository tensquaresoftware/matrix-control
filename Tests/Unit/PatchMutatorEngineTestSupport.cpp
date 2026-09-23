#include "PatchMutatorEngineTestSupport.h"

Core::MutationHistoryStore& PatchMutatorEngineTestAccess::historyStore(Core::PatchMutatorEngine& engine)
{
    return engine.historyStore_;
}

void PatchMutatorEngineTestAccess::applySelectionFromApvts(Core::PatchMutatorEngine& engine)
{
    engine.applySelectionFromApvts();
}

Core::PatchModel PatchMutatorEngineTestAccess::resolveAuditionBuffer(Core::PatchMutatorEngine& engine)
{
    return engine.resolveAuditionBuffer();
}

namespace PatchMutatorEngineTestSupport
{
    TestAudioProcessorMutator::TestAudioProcessorMutator()
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "P", ApvtsFactory::createParameterLayout())
    {
    }

    EngineHarness::EngineHarness()
        : mapper(proc.apvts, model)
        , patchNameSyncer(proc.apvts, model)
        , midiManager(proc.apvts, queue, tracker)
        , decoder(parser)
        , patchFileService(decoder)
        , engine(Core::PatchMutatorEngine::Dependencies {
                     &model, &mapper, &patchNameSyncer, &midiManager, proc.apvts,
                     [this]() { return currentPatchNumber; },
                     [this]() { return deviceLimits; },
                     &patchFileService, &midiManager.getSysExEncoder() },
                 Core::ActionExecutionHooks {
                     .setSuppressMatrixModSysEx = [this](bool s) { suppressMatrixModSysEx = s; },
                     .setSuppressPatchSysEx = [this](bool s) { suppressPatchSysEx = s; } })
    {
        proc.apvts.state.setProperty("deviceDetected", true, nullptr);
        proc.apvts.state.setProperty("deviceType", "Matrix-1000", nullptr);
        model.loadFrom(Core::InitDefaults::patchData());
    }

    void EngineHarness::setRecipe(int amount, int random, bool enableDco1)
    {
        proc.apvts.state.setProperty(PatchMutator::kAmount, amount, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kRandom, random, nullptr);
        // Legacy Amount / Random no longer reach the algorithm. WILD + FREE is the MODE
        // equivalent of the wide-open jitter these tests were written against.
        setMode(Core::MutationMode::kWild, Core::MutationPitchMode::kFree);
        proc.apvts.state.setProperty(PatchMutator::kEnableDco1, enableDco1, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableDco2, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableVcfVca, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableFmTrack, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableRampPortamento, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope1, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope2, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableEnvelope3, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableLfo1, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableLfo2, false, nullptr);
        proc.apvts.state.setProperty(PatchMutator::kEnableMatrixMod, false, nullptr);
    }

    void EngineHarness::setMode(Core::MutationMode mode, Core::MutationPitchMode pitchMode)
    {
        proc.apvts.state.setProperty(PatchMutator::kMode, static_cast<int>(mode), nullptr);
        proc.apvts.state.setProperty(PatchMutator::kPitch, static_cast<int>(pitchMode), nullptr);
    }

    Core::MutationHistoryStore& EngineHarness::store()
    {
        return PatchMutatorEngineTestAccess::historyStore(engine);
    }

    void EngineHarness::applySelectionFromApvts()
    {
        PatchMutatorEngineTestAccess::applySelectionFromApvts(engine);
    }

    Core::PatchModel EngineHarness::resolveAuditionBuffer()
    {
        return PatchMutatorEngineTestAccess::resolveAuditionBuffer(engine);
    }

    int countPatchSysExMessages(Core::MidiOutboundQueue& queue)
    {
        int count = 0;

        while (! queue.isEmpty())
        {
            const auto msg = queue.dequeue();
            if (! msg.has_value())
                break;

            if (msg->category != Core::MidiOutboundQueue::MessageCategory::kSysEx)
                continue;

            const auto& block = msg->sysExData;
            if (block.getSize() < 4)
                continue;

            const auto* data = static_cast<const juce::uint8*>(block.getData());
            if (data[3] == SysExConstants::Opcode::kSinglePatchData
                || data[3] == SysExConstants::Opcode::kSinglePatchToEditBuffer)
                ++count;
        }

        return count;
    }

    FullPatchOpcodeCounts countFullPatchOpcodes(Core::MidiOutboundQueue& queue)
    {
        FullPatchOpcodeCounts counts;

        while (! queue.isEmpty())
        {
            const auto msg = queue.dequeue();
            if (! msg.has_value())
                break;

            if (msg->category != Core::MidiOutboundQueue::MessageCategory::kSysEx)
                continue;

            const auto& block = msg->sysExData;
            if (block.getSize() < 4)
                continue;

            const auto* data = static_cast<const juce::uint8*>(block.getData());
            if (data[3] == SysExConstants::Opcode::kSinglePatchData)
                ++counts.slotWrite;
            else if (data[3] == SysExConstants::Opcode::kSinglePatchToEditBuffer)
                ++counts.editBuffer;
            else if (data[3] == SysExConstants::Opcode::kRemoteParameterEdit)
                ++counts.remoteParamEdit;
            else if (data[3] == SysExConstants::Opcode::kRemoteParameterEditMatrix)
                ++counts.matrixModEdit;
        }

        return counts;
    }

    Core::PatchModel makeDistinctBuffer(int seed)
    {
        Core::PatchModel patch;
        const auto marker = static_cast<juce::uint8>(seed & 0xFF);
        std::memset(patch.data(), marker, Core::PatchModel::kBufferSize);
        patch.data()[8] = marker;
        Core::MutationNaming::applyPatchName(patch, 0);
        return patch;
    }

    juce::File makeTempExportDir()
    {
        auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                       .getNonexistentChildFile("MatrixControlMutatorExport", "", false);
        dir.createDirectory();
        return dir;
    }

    void expectActionEnabledMirrors(juce::UnitTest& test,
                                    const EngineHarness& harness,
                                    ActionEnabledExpectations expected)
    {
        const auto& state = harness.proc.apvts.state;
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kMutateEnabled)) == expected.mutate);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kRetryEnabled)) == expected.retry);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kExportEnabled))
                    == expected.exportEnabled);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kDeleteEnabled))
                    == expected.deleteEnabled);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kClearEnabled)) == expected.clear);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kMutateAllocationBlocked, false))
                    == expected.mutateAllocationBlocked);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kRetryAllocationBlocked, false))
                    == expected.retryAllocationBlocked);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kMutateDefragRecovery, false))
                    == expected.mutateDefragRecovery);
        test.expect(static_cast<bool>(state.getProperty(MutatorState::kRetryDefragRecovery, false))
                    == expected.retryDefragRecovery);
    }
}
