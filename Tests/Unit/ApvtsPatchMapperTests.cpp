#include <array>
#include <cstring>
#include <optional>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

// Minimal AudioProcessor subclass for testing APVTS operations without pulling
// in GUI headers or plugin host infrastructure.
class TestAudioProcessor : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
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

class ApvtsPatchMapperTests : public juce::UnitTest
{
public:
    ApvtsPatchMapperTests() : juce::UnitTest("ApvtsPatchMapper Tests") {}

    void runTest() override
    {
        runApvtsToBuffer();
        runBufferToApvts();
        runMatrixModBus0ApvtsToBuffer();
        runReferenceRoundTrip();
    }

private:
    using PatchBuffer = std::array<juce::uint8, SysExConstants::kPatchPackedDataSize>;
    using IntDesc     = PluginDescriptors::IntParameterDescriptor;
    using ChoiceDesc  = PluginDescriptors::ChoiceParameterDescriptor;

    static juce::File fixturesPatchesDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Patches");
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeIntChoiceLayout(
        const IntDesc& d1, const IntDesc& d2, const ChoiceDesc& c1)
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(d1.parameterId, 1), d1.displayName,
            d1.minValue, d1.maxValue, d1.defaultValue));
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(d2.parameterId, 1), d2.displayName,
            d2.minValue, d2.maxValue, d2.defaultValue));
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(c1.parameterId, 1), c1.displayName,
            c1.choices, c1.defaultIndex));
        return layout;
    }

    static std::optional<PatchBuffer> decodePatch71()
    {
        SysExParser parser;
        SysExDecoder decoder(parser);
        juce::MemoryBlock raw;

        if (!fixturesPatchesDir().getChildFile("Patch 71.syx").loadFileAsData(raw))
            return std::nullopt;

        PatchBuffer buf {};
        return decoder.decodePatchSysEx(raw, buf.data()) ? std::make_optional(buf) : std::nullopt;
    }

    static std::vector<IntDesc> findRoundTripDescriptors(const PatchBuffer& decoded)
    {
        auto allDescs = Core::ApvtsPatchMapper::buildIntDescriptors();
        std::vector<IntDesc> result;

        for (const auto& d : allDescs)
        {
            if (result.size() >= 3) break;
            const auto val = static_cast<int>(decoded[static_cast<size_t>(d.sysExOffset)]);
            if (val > 0 && val >= d.minValue && val <= d.maxValue)
                result.push_back(d);
        }

        return result;
    }

    static juce::AudioProcessorValueTreeState::ParameterLayout makeRoundTripLayout(
        const std::vector<IntDesc>& descs)
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        for (const auto& d : descs)
            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(d.parameterId, 1), d.displayName,
                d.minValue, d.maxValue, d.defaultValue));
        return layout;
    }

    void runApvtsToBuffer()
    {
        beginTest("APVTS -> buffer: int and choice parameters");

        auto intDescs    = Core::ApvtsPatchMapper::buildIntDescriptors();
        auto choiceDescs = Core::ApvtsPatchMapper::buildChoiceDescriptors();
        const auto& freq = intDescs[0];    // dco1Frequency, offset 9, [0, 63]
        const auto& pw   = intDescs[2];    // dco1PulseWidth, offset 11, [0, 63]
        const auto& sync = choiceDescs[0]; // dco1Sync, offset 25, 4 choices

        TestAudioProcessor proc(makeIntChoiceLayout(freq, pw, sync));
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper(proc.apvts, model);
        *proc.apvts.getRawParameterValue(freq.parameterId) = 42.0f;
        *proc.apvts.getRawParameterValue(pw.parameterId)   = 27.0f;
        *proc.apvts.getRawParameterValue(sync.parameterId) = 2.0f;
        mapper.apvtsToBuffer();

        expectEquals(model.getValue(freq), 42);
        expectEquals(model.getValue(pw),   27);
        expectEquals(model.getChoiceIndex(sync), 2);
    }

    void runBufferToApvts()
    {
        beginTest("Buffer -> APVTS: int and choice parameters");

        auto intDescs    = Core::ApvtsPatchMapper::buildIntDescriptors();
        auto choiceDescs = Core::ApvtsPatchMapper::buildChoiceDescriptors();
        const auto& freq = intDescs[0];
        const auto& pw   = intDescs[2];
        const auto& sync = choiceDescs[0];

        TestAudioProcessor proc(makeIntChoiceLayout(freq, pw, sync));
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper(proc.apvts, model);
        model.setValue(freq, 35);
        model.setValue(pw,   50);
        model.setChoiceIndex(sync, 3);
        mapper.bufferToApvts();

        expectEquals(juce::roundToInt(proc.apvts.getRawParameterValue(freq.parameterId)->load()), 35);
        expectEquals(juce::roundToInt(proc.apvts.getRawParameterValue(pw.parameterId)->load()),   50);
        expectEquals(juce::roundToInt(proc.apvts.getRawParameterValue(sync.parameterId)->load()), 3);
    }

    void runMatrixModBus0ApvtsToBuffer()
    {
        beginTest("APVTS -> buffer: Matrix Mod bus 0 bytes 104–106");

        using namespace PluginDescriptors::MatrixModulationSection;

        const auto& amountDesc = kModulationBusIntParameters[0][0];
        const auto& sourceDesc = kModulationBusChoiceParameters[0][0];
        const auto& destinationDesc = kModulationBusChoiceParameters[0][1];

        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(std::make_unique<juce::AudioParameterInt>(
            juce::ParameterID(amountDesc.parameterId, 1), amountDesc.displayName,
            amountDesc.minValue, amountDesc.maxValue, amountDesc.defaultValue));
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(sourceDesc.parameterId, 1), sourceDesc.displayName,
            sourceDesc.choices, sourceDesc.defaultIndex));
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(destinationDesc.parameterId, 1), destinationDesc.displayName,
            destinationDesc.choices, destinationDesc.defaultIndex));

        TestAudioProcessor proc(std::move(layout));
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper(proc.apvts, model);

        *proc.apvts.getRawParameterValue(sourceDesc.parameterId) = 4.0f;
        *proc.apvts.getRawParameterValue(amountDesc.parameterId) = -15.0f;
        *proc.apvts.getRawParameterValue(destinationDesc.parameterId) = 9.0f;
        mapper.apvtsToBuffer();

        expectEquals(model.getChoiceIndex(sourceDesc), 4);
        expectEquals(model.getValue(amountDesc), -15);
        expectEquals(model.getChoiceIndex(destinationDesc), 9);
        expectEquals(static_cast<int>(model.data()[104]), 4);
        expectEquals(static_cast<int>(model.data()[105]), static_cast<int>(static_cast<juce::uint8>(-15)));
        expectEquals(static_cast<int>(model.data()[106]), 9);
    }

    void runReferenceRoundTrip()
    {
        beginTest("Reference round-trip Patch 71.syx");

        const auto decoded = decodePatch71();
        expect(decoded.has_value(), "Patch 71 fixture should load and decode");
        if (!decoded.has_value()) return;

        const auto testDescs = findRoundTripDescriptors(*decoded);
        expect(testDescs.size() >= 3, "Patch 71 must have at least 3 non-zero in-range bytes");
        if (testDescs.size() < 3) return;

        TestAudioProcessor proc(makeRoundTripLayout(testDescs));
        Core::PatchModel model;
        model.loadFrom(decoded->data());

        Core::ApvtsPatchMapper mapper(proc.apvts, model);
        mapper.bufferToApvts();
        std::memset(model.data(), 0, Core::PatchModel::kBufferSize);
        mapper.apvtsToBuffer();

        for (const auto& d : testDescs)
        {
            const auto expected = static_cast<int>((*decoded)[static_cast<size_t>(d.sysExOffset)]);
            expectEquals(model.getValue(d), expected,
                         "Offset " + juce::String(d.sysExOffset) + " must survive round-trip");
        }
    }
};

static ApvtsPatchMapperTests apvtsPatchMapperTests;
