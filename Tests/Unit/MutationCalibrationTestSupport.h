#pragma once

#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Services/PatchMutator/MutationAlgorithm.h"
#include "Core/Services/PatchMutator/MutationMatrixModPolicy.h"
#include "Core/Services/PatchMutator/MutationPostApply.h"
#include "PatchFixturePaths.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

// Shared fixtures for the playable-calibration suites: patch reads and writes by
// parameter id, Matrix Modulation bus setup, and one-shot mutation runs.
namespace MutationCalibrationTestSupport
{
    namespace Dco1Ids = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets;
    namespace Dco2Ids = PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets;
    namespace VcfVcaIds = PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets;
    namespace Env1Ids = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets;
    namespace Env2Ids = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets;
    namespace WaveSelectNames = PluginDisplayNames::ChoiceLists::WaveSelect;
    namespace SourceNames = PluginDisplayNames::ChoiceLists::ModulationBus::Source;
    namespace DestinationNames = PluginDisplayNames::ChoiceLists::ModulationBus::Destination;
    namespace TriggerModeNames = PluginDisplayNames::ChoiceLists::TriggerMode;
    namespace EnvelopeModeNames = PluginDisplayNames::ChoiceLists::EnvelopeMode;

    constexpr int kBusCount = ::Matrix1000Limits::kModulationBusCount;

    // Enough rolls that a policy violation cannot hide behind one lucky draw.
    constexpr int kTrialCount = 32;
    // Local stress pass only — keep off by default so CI stays in the seconds band.
    constexpr int kWideTrialCount = 128;
    constexpr bool kEnableWideTrials = false;

    inline int activeTrialCount() noexcept
    {
        return kEnableWideTrials ? kWideTrialCount : kTrialCount;
    }

    inline int readInt(const Core::PatchModel& patch, const char* parameterId)
    {
        const auto* descriptor = Core::findMutationIntDescriptor(parameterId);
        jassert(descriptor != nullptr);
        return patch.getValue(*descriptor);
    }

    inline void writeInt(Core::PatchModel& patch, const char* parameterId, int value)
    {
        const auto* descriptor = Core::findMutationIntDescriptor(parameterId);
        jassert(descriptor != nullptr);
        patch.setValue(*descriptor, juce::jlimit(descriptor->minValue, descriptor->maxValue, value));
    }

    inline void writeChoiceByName(Core::PatchModel& patch,
                                  const char* parameterId,
                                  const char* choiceName)
    {
        const auto* descriptor = Core::findMutationChoiceDescriptor(parameterId);
        jassert(descriptor != nullptr);
        const int index = descriptor->choices.indexOf(juce::String(choiceName));
        jassert(index >= 0);
        patch.setChoiceIndex(*descriptor, index);
    }

    inline bool isWaveSelectOff(const Core::PatchModel& patch, const char* parameterId)
    {
        const auto* descriptor = Core::findMutationChoiceDescriptor(parameterId);
        jassert(descriptor != nullptr);
        return patch.getChoiceIndex(*descriptor)
               == descriptor->choices.indexOf(juce::String(WaveSelectNames::kOff));
    }

    inline const PluginDescriptors::ChoiceParameterDescriptor& busSource(int busIndex)
    {
        return PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[
            static_cast<size_t>(busIndex)][0];
    }

    inline const PluginDescriptors::ChoiceParameterDescriptor& busDestination(int busIndex)
    {
        return PluginDescriptors::MatrixModulationSection::kModulationBusChoiceParameters[
            static_cast<size_t>(busIndex)][1];
    }

    inline const PluginDescriptors::IntParameterDescriptor& busAmount(int busIndex)
    {
        return PluginDescriptors::MatrixModulationSection::kModulationBusIntParameters[
            static_cast<size_t>(busIndex)][0];
    }

    struct BusRoute
    {
        int busIndex = 0;
        const char* sourceName = SourceNames::kNone;
        const char* destinationName = DestinationNames::kNone;
        int amount = 0;
    };

    inline void writeMatrixModBus(Core::PatchModel& patch, const BusRoute& route)
    {
        const auto& source = busSource(route.busIndex);
        const auto& destination = busDestination(route.busIndex);
        const int sourceIndex = source.choices.indexOf(juce::String(route.sourceName));
        const int destinationIndex = destination.choices.indexOf(juce::String(route.destinationName));
        jassert(sourceIndex >= 0);
        jassert(destinationIndex >= 0);
        if (sourceIndex < 0 || destinationIndex < 0)
            return;

        patch.setChoiceIndex(source, sourceIndex);
        patch.setChoiceIndex(destination, destinationIndex);
        patch.setValue(busAmount(route.busIndex), route.amount);
    }

    inline void clearAllMatrixModBuses(Core::PatchModel& patch)
    {
        for (int busIndex = 0; busIndex < kBusCount; ++busIndex)
            writeMatrixModBus(patch, BusRoute { busIndex });
    }

    inline Core::MutationRecipe makeRecipe(Core::MutationMode mode, Core::MutationPitchMode pitchMode)
    {
        Core::MutationRecipe recipe;
        recipe.mode = mode;
        recipe.pitchMode = pitchMode;
        recipe.applyModeCalibration();
        return recipe;
    }

    inline Core::PatchModel makeInitPatchModel()
    {
        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        return model;
    }

    inline Core::PatchModel mutated(const Core::PatchModel& seed,
                                    const Core::MutationRecipe& recipe,
                                    juce::uint32 rngSeed)
    {
        Core::PatchModel working;
        working.loadFrom(seed.data());

        Core::SeededRandom rng(rngSeed);
        Core::MutationAlgorithm algorithm;
        algorithm.apply(working, recipe, rng);
        return working;
    }

    inline Core::MutationRecipe makeFullyEnabledRecipe(Core::MutationMode mode,
                                                       Core::MutationPitchMode pitchMode)
    {
        auto recipe = makeRecipe(mode, pitchMode);
        recipe.enableDco1 = true;
        recipe.enableDco2 = true;
        recipe.enableVcfVca = true;
        recipe.enableFmTrack = true;
        recipe.enableRampPortamento = true;
        recipe.enableEnvelope1 = true;
        recipe.enableEnvelope2 = true;
        recipe.enableEnvelope3 = true;
        recipe.enableLfo1 = true;
        recipe.enableLfo2 = true;
        recipe.enableMatrixMod = true;
        return recipe;
    }

    inline juce::File fixturesRoot()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR);
    }

    // Relative path under Tests/Fixtures (e.g. "Matrix-Control/Patches/Factory/BANK 2/P00. OBXA-11.syx").
    inline bool loadPatchFromFixtureRelative(const juce::String& relativePath,
                                             Core::PatchModel& outModel)
    {
        juce::MemoryBlock raw;
        if (! PatchTestFixtures::resolveFixtureFile(relativePath).loadFileAsData(raw))
            return false;

        SysExParser parser;
        SysExDecoder decoder(parser);
        juce::uint8 decoded[SysExConstants::kPatchPackedDataSize] = {};
        if (! decoder.decodePatchSysEx(raw, decoded))
            return false;

        outModel.loadFrom(decoded);
        return true;
    }

    // Spec corpus: banks 2–9, two patches each (exact filenames).
    inline constexpr const char* kRomCorpusRelativePaths[] = {
        "Matrix-Control/Patches/Factory/BANK 2/P00. OBXA-11.syx",
        "Matrix-Control/Patches/Factory/BANK 2/P29. AMBIANCE.syx",
        "Matrix-Control/Patches/Factory/BANK 3/P03. HALO.syx",
        "Matrix-Control/Patches/Factory/BANK 3/P99. SYNCAGE.syx",
        "Matrix-Control/Patches/Factory/BANK 4/P34. CS-80.syx",
        "Matrix-Control/Patches/Factory/BANK 4/P19. BIGBRA$$.syx",
        "Matrix-Control/Patches/Factory/BANK 5/P11. SYNBASS.syx",
        "Matrix-Control/Patches/Factory/BANK 5/P68. OSC SYNC.syx",
        "Matrix-Control/Patches/Factory/BANK 6/P10. 1000STRG.syx",
        "Matrix-Control/Patches/Factory/BANK 6/P79. SOLEMN.syx",
        "Matrix-Control/Patches/Factory/BANK 7/P04. ARP-2600.syx",
        "Matrix-Control/Patches/Factory/BANK 7/P73. SLAP 2.syx",
        "Matrix-Control/Patches/Factory/BANK 8/P03. BANJO.syx",
        "Matrix-Control/Patches/Factory/BANK 8/P73. NOISE-DN.syx",
        "Matrix-Control/Patches/Factory/BANK 9/P01. MIKPIANO.syx",
        "Matrix-Control/Patches/Factory/BANK 9/P11. OB8 JUMP.syx",
    };

    inline Core::PatchModel makeTrapDeadAmplitude()
    {
        auto patch = makeInitPatchModel();
        clearAllMatrixModBuses(patch);
        writeChoiceByName(patch, Dco1Ids::kWaveSelect, WaveSelectNames::kWave);
        writeChoiceByName(patch, Dco2Ids::kWaveSelect, WaveSelectNames::kWave);
        writeInt(patch, VcfVcaIds::kVca1Volume, 0);
        writeInt(patch, VcfVcaIds::kVca2ModByEnv2, 0);
        writeInt(patch, Env2Ids::kAmplitude, 0);
        return patch;
    }

    inline Core::PatchModel makeTrapDualWaveOff()
    {
        auto patch = makeInitPatchModel();
        clearAllMatrixModBuses(patch);
        writeChoiceByName(patch, Dco1Ids::kWaveSelect, WaveSelectNames::kOff);
        writeChoiceByName(patch, Dco2Ids::kWaveSelect, WaveSelectNames::kOff);
        return patch;
    }

    inline Core::PatchModel makeTrapFilterSmother()
    {
        auto patch = makeInitPatchModel();
        clearAllMatrixModBuses(patch);
        writeInt(patch, VcfVcaIds::kFrequency, 0);
        writeInt(patch, VcfVcaIds::kResonance, 0);
        return patch;
    }

    inline Core::PatchModel makeTrapMixTowardSilentDco()
    {
        auto patch = makeInitPatchModel();
        clearAllMatrixModBuses(patch);
        writeChoiceByName(patch, Dco1Ids::kWaveSelect, WaveSelectNames::kOff);
        writeChoiceByName(patch, Dco2Ids::kWaveSelect, WaveSelectNames::kWave);
        // Mix polarity: 63 = DCO 1 only — points at the silent oscillator.
        writeInt(patch, VcfVcaIds::kBalance, 63);
        return patch;
    }

    inline Core::PatchModel makeTrapNegativeMmTowardVca1()
    {
        auto patch = makeInitPatchModel();
        clearAllMatrixModBuses(patch);
        // Low base volume so a deep negative MM amount can actually smother amplitude.
        writeInt(patch, VcfVcaIds::kVca1Volume, 10);
        writeMatrixModBus(patch,
                          { 0, SourceNames::kLfo1, DestinationNames::kVca1Volume, -63 });
        return patch;
    }
} // namespace MutationCalibrationTestSupport
