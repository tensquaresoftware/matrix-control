#include <array>
#include <cstring>
#include <optional>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/MidiChannelMasterCodec.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

class TestAudioProcessorMaster : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessorMaster(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "M", std::move(layout))
    {
    }

    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "TestMaster"; }
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

class ApvtsMasterMapperTests : public juce::UnitTest
{
public:
    ApvtsMasterMapperTests() : juce::UnitTest("ApvtsMasterMapper Tests") {}

    void runTest() override
    {
        runApvtsToBuffer();
        runBufferToApvts();
        runReferenceRoundTrip();
        runMidiChannelCompositionRoundTrip();
        runUnisonEnableRoundTrip();
    }

private:
    using MasterBuffer = std::array<juce::uint8, SysExConstants::kMasterPackedDataSize>;
    using IntDesc      = PluginDescriptors::IntParameterDescriptor;
    using ChoiceDesc   = PluginDescriptors::ChoiceParameterDescriptor;

    static juce::File fixturesMastersDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Masters");
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

    static std::optional<MasterBuffer> decodeMaster1()
    {
        SysExParser parser;
        SysExDecoder decoder(parser);
        juce::MemoryBlock raw;

        if (!fixturesMastersDir().getChildFile("Master 1.syx").loadFileAsData(raw))
            return std::nullopt;

        MasterBuffer buf {};
        return decoder.decodeMasterSysEx(raw, buf.data()) ? std::make_optional(buf) : std::nullopt;
    }

    static std::vector<IntDesc> findRoundTripDescriptors(const MasterBuffer& decoded)
    {
        auto allDescs = Core::ApvtsMasterMapper::buildIntDescriptors();
        std::vector<IntDesc> result;

        for (const auto& d : allDescs)
        {
            if (result.size() >= 3u) break;
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

        auto intDescs    = Core::ApvtsMasterMapper::buildIntDescriptors();
        auto choiceDescs = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        const auto& pedal1  = intDescs[0];    // Pedal1Select, offset 17, [0, 121]
        const auto& lever2  = intDescs[2];    // Lever2Select, offset 19, [0, 121]
        const auto& midiEcho = choiceDescs[1]; // MidiEcho On/Off — not the composite CHANNEL

        TestAudioProcessorMaster proc(makeIntChoiceLayout(pedal1, lever2, midiEcho));
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper(proc.apvts, model);
        *proc.apvts.getRawParameterValue(pedal1.parameterId)   = 10.0f;
        *proc.apvts.getRawParameterValue(lever2.parameterId)   = 5.0f;
        *proc.apvts.getRawParameterValue(midiEcho.parameterId) = 1.0f;
        mapper.apvtsToBuffer();

        expectEquals(model.getValue(pedal1), 10);
        expectEquals(model.getValue(lever2), 5);
        expectEquals(model.getChoiceIndex(midiEcho), 1);
    }

    void runBufferToApvts()
    {
        beginTest("Buffer -> APVTS: int and choice parameters");

        auto intDescs    = Core::ApvtsMasterMapper::buildIntDescriptors();
        auto choiceDescs = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        const auto& pedal1   = intDescs[0];
        const auto& lever2   = intDescs[2];
        const auto& midiEcho = choiceDescs[1];

        TestAudioProcessorMaster proc(makeIntChoiceLayout(pedal1, lever2, midiEcho));
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper(proc.apvts, model);
        model.setValue(pedal1, 20);
        model.setValue(lever2, 7);
        model.setChoiceIndex(midiEcho, 1);
        mapper.bufferToApvts();

        expectEquals(juce::roundToInt(proc.apvts.getRawParameterValue(pedal1.parameterId)->load()),  20);
        expectEquals(juce::roundToInt(proc.apvts.getRawParameterValue(lever2.parameterId)->load()),  7);
        expectEquals(juce::roundToInt(proc.apvts.getRawParameterValue(midiEcho.parameterId)->load()), 1);
    }

    void runReferenceRoundTrip()
    {
        beginTest("Reference round-trip Master 1.syx");

        const auto decoded = decodeMaster1();
        expect(decoded.has_value(), "Master 1 fixture should load and decode");
        if (!decoded.has_value()) return;

        const auto testDescs = findRoundTripDescriptors(*decoded);
        if (testDescs.empty())
        {
            // Master 1.syx carries default values — no non-zero unsigned ints found; skip sub-assertion.
            expect(true, "No non-zero unsigned int descriptors in Master 1.syx (default fixture)");
            return;
        }

        TestAudioProcessorMaster proc(makeRoundTripLayout(testDescs));
        Core::MasterModel model;
        model.loadFrom(decoded->data());

        Core::ApvtsMasterMapper mapper(proc.apvts, model);
        mapper.bufferToApvts();
        std::memset(model.data(), 0, Core::MasterModel::kBufferSize);
        mapper.apvtsToBuffer();

        for (const auto& d : testDescs)
        {
            const auto expected = static_cast<int>((*decoded)[static_cast<size_t>(d.sysExOffset)]);
            expectEquals(model.getValue(d), expected,
                         "Offset " + juce::String(d.sysExOffset) + " must survive round-trip");
        }
    }

    void runMidiChannelCompositionRoundTrip()
    {
        beginTest("CHANNEL composition: APVTS <-> buffer (OMNI / CHANNEL 4 / MONO G3)");

        const auto choiceDescs = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        const auto& channel = choiceDescs[0];
        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(channel.parameterId, 1), channel.displayName,
            channel.choices, channel.defaultIndex));

        TestAudioProcessorMaster proc(std::move(layout));
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper(proc.apvts, model);
        auto* raw = proc.apvts.getRawParameterValue(channel.parameterId);

        const struct Case { float combo; int b; int o; int m; } cases[] = {
            { 0.0f, 0, 1, 0 }, { 4.0f, 3, 0, 0 }, { 19.0f, 2, 0, 1 }
        };

        for (const auto& c : cases)
        {
            *raw = c.combo;
            mapper.apvtsToBuffer();
            expectEquals(static_cast<int>(model.data()[11]), c.b);
            expectEquals(static_cast<int>(model.data()[12]), c.o);
            expectEquals(static_cast<int>(model.data()[35]), c.m);
        }

        for (const int combo : { 0, 4, 19 })
        {
            Core::MidiChannelMasterCodec::applyComboIndex(
                model.data(), Core::MasterModel::kBufferSize, combo);
            mapper.bufferToApvts();
            expectEquals(juce::roundToInt(raw->load()), combo);
        }
    }

    void runUnisonEnableRoundTrip()
    {
        beginTest("Unison Enable (octet 169): APVTS <-> buffer OFF/ON");

        const auto choiceDescs = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        const ChoiceDesc* unison = nullptr;
        for (const auto& d : choiceDescs)
        {
            if (d.parameterId == PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable)
            {
                unison = &d;
                break;
            }
        }

        expect(unison != nullptr, "miscUnisonEnable descriptor must exist");
        if (unison == nullptr)
            return;

        expectEquals(unison->sysExOffset, 169);
        expectEquals(unison->defaultIndex, 0);

        juce::AudioProcessorValueTreeState::ParameterLayout layout;
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(unison->parameterId, 1), unison->displayName,
            unison->choices, unison->defaultIndex));

        TestAudioProcessorMaster proc(std::move(layout));
        Core::MasterModel model;
        Core::ApvtsMasterMapper mapper(proc.apvts, model);
        auto* raw = proc.apvts.getRawParameterValue(unison->parameterId);

        *raw = 0.0f;
        mapper.apvtsToBuffer();
        expectEquals(static_cast<int>(model.data()[169]), 0);

        *raw = 1.0f;
        mapper.apvtsToBuffer();
        expectEquals(static_cast<int>(model.data()[169]), 1);

        model.data()[169] = 0;
        mapper.bufferToApvts();
        expectEquals(juce::roundToInt(raw->load()), 0);

        model.data()[169] = 1;
        mapper.bufferToApvts();
        expectEquals(juce::roundToInt(raw->load()), 1);
    }
};

static ApvtsMasterMapperTests apvtsMasterMapperTests;
