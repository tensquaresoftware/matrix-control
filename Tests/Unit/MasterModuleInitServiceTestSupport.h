#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Init/InitTemplateLoader.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/MidiChannelMasterCodec.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"
#include "PatchFixturePaths.h"

// Minimal AudioProcessor host for MasterModuleInitService unit tests.
class TestAudioProcessorMasterInit : public juce::AudioProcessor
{
public:
    explicit TestAudioProcessorMasterInit(juce::AudioProcessorValueTreeState::ParameterLayout layout)
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

namespace MasterModuleInitTestFixtures
{
    inline juce::File createTempTemplatesDir()
    {
        auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory)
                       .getNonexistentChildFile("MatrixControlMasterModuleInit", "", false);
        if (! dir.createDirectory())
            return {};
        return dir;
    }

    // Release-safe: unchecked copyFileTo can leave empty/missing templates under NDEBUG.
    inline bool copyFixtureToDir(const juce::File& dir, const juce::String& fileName)
    {
        const auto source = PatchTestFixtures::initFixturesDir().getChildFile(fileName);
        if (! source.existsAsFile() || source.getSize() <= 0)
            return false;

        juce::MemoryBlock bytes;
        if (! source.loadFileAsData(bytes) || bytes.getSize() == 0)
            return false;

        const auto destination = dir.getChildFile(fileName);
        if (! destination.replaceWithData(bytes.getData(), bytes.getSize()))
            return false;

        return destination.existsAsFile()
            && destination.getSize() == static_cast<juce::int64>(bytes.getSize());
    }

    inline bool sysExMatchesMasterMessage(const juce::MemoryBlock& block)
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

    inline juce::AudioProcessorValueTreeState::ParameterLayout makeFullMasterLayout()
    {
        juce::AudioProcessorValueTreeState::ParameterLayout layout;

        for (const auto& d : PluginDescriptors::MasterEditSection::kIntParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterInt>(
                juce::ParameterID(d.parameterId, 1), d.hostName,
                d.minValue, d.maxValue, d.defaultValue));
        }

        for (const auto& d : PluginDescriptors::MasterEditSection::kChoiceParameters)
        {
            layout.add(std::make_unique<juce::AudioParameterChoice>(
                juce::ParameterID(d.parameterId, 1), d.hostName,
                d.choices, d.defaultIndex));
        }

        return layout;
    }

    inline Core::MasterModel makeInitTemplateModel(const juce::File& templatesFolder)
    {
        SysExParser parser;
        SysExDecoder decoder(parser);
        Core::InitTemplateLoader loader(decoder);
        Core::MasterModel templateModel;
        loader.loadMaster(templateModel, templatesFolder);
        return templateModel;
    }

    inline void rewriteMasterInitWithOmniOn(const juce::File& templatesDir,
                                            SysExEncoder& encoder,
                                            Core::MasterModel& initTemplate)
    {
        initTemplate.data()[12] = 1;
        initTemplate.data()[35] = 0;
        const auto syx = encoder.encodeMasterSysEx(0x03, initTemplate.data());
        templatesDir.getChildFile(Core::InitTemplateLoader::kMasterInitFileName)
            .replaceWithData(syx.getData(), syx.getSize());
    }
}

namespace MasterModelTestHelpers
{
    inline void fillModelWithDistinctValues(Core::MasterModel& model)
    {
        using namespace PluginDescriptors::MasterEditSection;
        namespace MidiIds = PluginIDs::MasterEditSection::MidiModule;
        namespace VibratoIds = PluginIDs::MasterEditSection::VibratoModule;
        namespace MiscIds = PluginIDs::MasterEditSection::MiscModule;

        for (const auto& d : Core::ApvtsMasterMapper::buildIntDescriptors())
        {
            if (d.parentGroupId == MidiIds::kGroupId)
                model.setValue(d, d.maxValue);
            else if (d.parentGroupId == VibratoIds::kGroupId)
                model.setValue(d, d.minValue);
            else if (d.parentGroupId == MiscIds::kGroupId)
                model.setValue(d, (d.minValue + d.maxValue) / 2);
        }

        model.setChoiceIndex(MidiModule::kChoiceParameters[0], 2);
        model.setChoiceIndex(VibratoModule::kChoiceParameters[0], 3);
        model.setChoiceIndex(MiscModule::kChoiceParameters[0], 1);
    }

    inline bool channelTripletMatch(const Core::MasterModel& lhs, const Core::MasterModel& rhs)
    {
        return lhs.data()[11] == rhs.data()[11]
            && lhs.data()[12] == rhs.data()[12]
            && lhs.data()[35] == rhs.data()[35];
    }

    inline bool moduleBytesMatch(const Core::MasterModel& lhs,
                                 const Core::MasterModel& rhs,
                                 const juce::String& moduleGroupId)
    {
        using namespace PluginDescriptors::MasterEditSection;

        for (const auto& d : Core::ApvtsMasterMapper::buildIntDescriptors())
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
}
