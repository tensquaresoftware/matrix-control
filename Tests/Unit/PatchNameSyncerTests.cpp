#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    class MinimalAudioProcessor : public juce::AudioProcessor
    {
    public:
        MinimalAudioProcessor()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", {})
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

    static juce::File fixturesPatchesDir()
    {
        return juce::File(MATRIX_TEST_FIXTURES_DIR).getChildFile("Patches");
    }
}

class PatchNameSyncerTests : public juce::UnitTest
{
public:
    PatchNameSyncerTests() : juce::UnitTest("PatchNameSyncer Tests") {}

    void runTest() override
    {
        runBufferToApvts();
        runApvtsToBuffer();
        runTruncation();
        runReferenceRoundTrip();
    }

private:
    using PatchBuffer = std::array<juce::uint8, Core::PatchModel::kBufferSize>;

    void runBufferToApvts()
    {
        beginTest("Buffer -> APVTS: name property reflects model");

        MinimalAudioProcessor proc;
        Core::PatchModel model;
        Core::PatchNameSyncer syncer(proc.apvts, model);

        model.setName("HELLO");
        syncer.bufferToApvts();

        const auto prop = proc.apvts.state
            .getProperty(PluginIDs::PatchEditSection::PatchNameModule::kPatchName)
            .toString();

        expectEquals(prop, model.getName());
    }

    void runApvtsToBuffer()
    {
        beginTest("APVTS -> buffer: model name reflects property");

        MinimalAudioProcessor proc;
        Core::PatchModel model;
        Core::PatchNameSyncer syncer(proc.apvts, model);

        proc.apvts.state.setProperty(
            PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
            juce::String("WORLD"),
            nullptr);
        syncer.apvtsToBuffer();

        expect(model.getName().isNotEmpty());
        expect(model.getName().containsOnly("ABCDEFGHIJKLMNOPQRSTUVWXYZ "
                                            "0123456789!\"#$%&'()*+,-./:;<=>?@[\\]^_`{|}~"));
    }

    void runTruncation()
    {
        beginTest("Truncation: 12-char input stores first 8 chars uppercase");

        MinimalAudioProcessor proc;
        Core::PatchModel model;
        Core::PatchNameSyncer syncer(proc.apvts, model);

        proc.apvts.state.setProperty(
            PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
            juce::String("abcdefghijkl"),
            nullptr);
        syncer.apvtsToBuffer();

        const auto name = model.getName();
        expectEquals(name.length(), Core::PatchModel::kNameLength);
        expectEquals(name.trimEnd(), juce::String("ABCDEFGH"));
    }

    void runReferenceRoundTrip()
    {
        beginTest("Reference round-trip Patch 71.syx: name property non-empty and <= 8 chars");

        SysExParser parser;
        SysExDecoder decoder(parser);
        juce::MemoryBlock raw;

        const bool loaded = fixturesPatchesDir().getChildFile("Patch 71.syx").loadFileAsData(raw);
        expect(loaded, "Patch 71 fixture should load");
        if (!loaded) return;

        PatchBuffer buf {};
        const bool decoded = decoder.decodePatchSysEx(raw, buf.data());
        expect(decoded, "Patch 71 fixture should decode");
        if (!decoded) return;

        MinimalAudioProcessor proc;
        Core::PatchModel model;
        model.loadFrom(buf.data());

        Core::PatchNameSyncer syncer(proc.apvts, model);
        syncer.bufferToApvts();

        const auto prop = proc.apvts.state
            .getProperty(PluginIDs::PatchEditSection::PatchNameModule::kPatchName)
            .toString();

        expect(prop.isNotEmpty(), "Patch name property should be non-empty after round-trip");
        expect(prop.length() <= Core::PatchModel::kNameLength,
               "Patch name property should be <= 8 chars");
    }
};

static PatchNameSyncerTests patchNameSyncerTests;
