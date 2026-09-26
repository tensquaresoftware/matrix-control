// One-shot dev tool: builds PatchInit.syx and MasterInit.syx from PluginDescriptors
// default values (same source as APVTS layout). Run after changing descriptor defaults.
//
//   cmake --build <buildDir> --target GenerateInitFixtures
//   <buildDir>/GenerateInitFixtures_artefacts/<config>/GenerateInitFixtures
// Resolves output via ProjectPaths::getProjectRoot() — safe to run from any CWD.

#include <cstring>
#include <iostream>

#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsLayoutBuilder.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Shared/ProjectPaths.h"

namespace
{

class MinimalProcessor : public juce::AudioProcessor
{
public:
    explicit MinimalProcessor(juce::AudioProcessorValueTreeState::ParameterLayout layout)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, nullptr, "Init", std::move(layout))
    {
    }

    juce::AudioProcessorValueTreeState apvts;

    const juce::String getName() const override { return "GenerateInitFixtures"; }
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

void applyMasterNonDescriptorDefaults(Core::MasterModel& model)
{
    static_assert(Core::MasterModel::kBufferSize > 161,
                  "Master group-enable range must fit in packed buffer");

    constexpr size_t kGroupEnableStart = 36;
    constexpr size_t kGroupEnableEnd = 161;

    for (size_t i = kGroupEnableStart; i <= kGroupEnableEnd; ++i)
        model.data()[i] = 0xFF;
}

bool writeSysExFile(const juce::File& file, const juce::MemoryBlock& data)
{
    file.getParentDirectory().createDirectory();
    return file.replaceWithData(data.getData(), data.getSize());
}

juce::File fixturesInitDir()
{
    return ProjectPaths::getProjectRoot()
        .getChildFile("Tests")
        .getChildFile("Fixtures")
        .getChildFile("Matrix-Control")
        .getChildFile("Init");
}

bool writeOrFail(const juce::File& file, const juce::MemoryBlock& data, const char* label)
{
    if (writeSysExFile(file, data))
        return true;

    std::cerr << "Failed to write " << file.getFullPathName().toStdString() << " (" << label << ")\n";
    return false;
}

bool expectSysExSize(const juce::MemoryBlock& data, size_t expected, const char* label)
{
    if (data.getSize() == expected)
        return true;

    std::cerr << label << " SysEx size mismatch: expected " << expected
              << ", got " << data.getSize() << '\n';
    return false;
}

} // namespace

int main(int argc, char* argv[])
{
    juce::ignoreUnused(argc, argv);

    const auto outDir = fixturesInitDir();
    MinimalProcessor processor(ApvtsLayoutBuilder::createParameterLayout());

    Core::PatchModel patchModel;
    Core::ApvtsPatchMapper patchMapper(processor.apvts, patchModel);
    patchMapper.apvtsToBuffer();
    patchModel.setName("");

    Core::MasterModel masterModel;
    Core::ApvtsMasterMapper masterMapper(processor.apvts, masterModel);
    masterMapper.apvtsToBuffer();
    applyMasterNonDescriptorDefaults(masterModel);

    SysExEncoder encoder;
    const auto patchSysEx = encoder.encodePatchSysEx(0, patchModel.data());
    const auto masterSysEx = encoder.encodeMasterSysEx(0x03, masterModel.data());

    if (! expectSysExSize(patchSysEx, SysExConstants::kPatchMessageLength, "Patch")
        || ! expectSysExSize(masterSysEx, SysExConstants::kMasterMessageLength, "Master"))
        return 1;

    const auto patchFile = outDir.getChildFile("PatchInit.syx");
    const auto masterFile = outDir.getChildFile("MasterInit.syx");
    if (! writeOrFail(patchFile, patchSysEx, "PatchInit")
        || ! writeOrFail(masterFile, masterSysEx, "MasterInit"))
        return 1;

    std::cout << "Wrote " << patchFile.getFullPathName() << " (" << patchSysEx.getSize() << " bytes)\n";
    std::cout << "Wrote " << masterFile.getFullPathName() << " (" << masterSysEx.getSize() << " bytes)\n";
    return 0;
}
