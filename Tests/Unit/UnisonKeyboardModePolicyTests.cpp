#include <array>
#include <vector>

#include <juce_core/juce_core.h>

#include "Core/MIDI/EditorPath.h"
#include "Core/MIDI/MasterParameterSysExDispatcher.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/MIDI/PatchParameterSysExDispatcher.h"
#include "Core/MIDI/Queue/MidiOutboundQueue.h"
#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/MIDI/SysEx/SysExDecoder.h"
#include "Core/MIDI/SysEx/SysExEncoder.h"
#include "Core/MIDI/SysEx/SysExParser.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Helpers/UnisonKeyboardModePolicy.h"
#include "SysExWireCompliance.h"

namespace
{
    const PluginDescriptors::ChoiceParameterDescriptor* findChoice(
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& descriptors,
        const juce::String& parameterId)
    {
        for (const auto& d : descriptors)
        {
            if (d.parameterId == parameterId)
                return &d;
        }

        return nullptr;
    }

    struct UnisonDomainDescriptors
    {
        std::vector<PluginDescriptors::ChoiceParameterDescriptor> patchChoices;
        std::vector<PluginDescriptors::ChoiceParameterDescriptor> masterChoices;
        const PluginDescriptors::ChoiceParameterDescriptor* keyboard = nullptr;
        const PluginDescriptors::ChoiceParameterDescriptor* unison = nullptr;
    };

    UnisonDomainDescriptors loadUnisonDomainDescriptors()
    {
        UnisonDomainDescriptors loaded;
        loaded.patchChoices = Core::ApvtsPatchMapper::buildChoiceDescriptors();
        loaded.masterChoices = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        loaded.keyboard = findChoice(
            loaded.patchChoices,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);
        loaded.unison = findChoice(
            loaded.masterChoices,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable);
        return loaded;
    }
}

class UnisonKeyboardModePolicyTests : public juce::UnitTest
{
public:
    UnisonKeyboardModePolicyTests() : juce::UnitTest("Unison Keyboard Mode Policy") {}

    void runTest() override
    {
        keyboardModeChoiceOrderMatchesSysEx();
        inventoriedTriggerStrigChoiceIndices();
        legatoEnableAndGrayPredicates();
        strigAllowBlockAndFallbackIndex();
        masterOverrideBadgeVisibilityMatrix();
        independentPatchKeyboardModeAndMasterUnison();
        keyboardModeRemoteEditDoesNotTouchMasterUnison();
        masterUnisonFullDumpDoesNotTouchKeyboardMode();
        masterUnisonEnableSurvivesFullMasterEncodeDecode();
    }

private:
    void keyboardModeChoiceOrderMatchesSysEx()
    {
        beginTest("Keyboard Mode choices are REASGN/ROTATE/UNISON/REAROB at indices 0-3");

        const auto descriptors = Core::ApvtsPatchMapper::buildChoiceDescriptors();
        const auto* desc = findChoice(
            descriptors,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);

        expect(desc != nullptr);
        if (desc == nullptr)
            return;

        expectEquals(desc->sysExOffset, 8);
        expectEquals(desc->sysExId, 48);
        expectEquals(desc->defaultIndex, 1);
        expectEquals(desc->choices.size(), 4);
        expectEquals(desc->choices[0],
                     juce::String(PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kReasgn));
        expectEquals(desc->choices[1],
                     juce::String(PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRotate));
        expectEquals(desc->choices[2],
                     juce::String(PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kUnison));
        expectEquals(desc->choices[3],
                     juce::String(PluginDisplayNames::ChoiceLists::PortamentoKeyboardMode::kRearob));
        expectEquals(TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex, 2);
    }

    void expectTriggerStrigAt(
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& descriptors,
        const char* parameterId,
        int expectedIndex,
        const char* strigLabel)
    {
        const auto* desc = findChoice(descriptors, parameterId);
        expect(desc != nullptr, juce::String(parameterId) + " descriptor missing");
        if (desc == nullptr)
            return;

        expect(expectedIndex >= 0 && expectedIndex < desc->choices.size(),
               juce::String(parameterId) + " STRIG index out of range");
        if (expectedIndex < 0 || expectedIndex >= desc->choices.size())
            return;

        expectEquals(desc->choices[expectedIndex], juce::String(strigLabel),
                     juce::String(parameterId) + " STRIG index mismatch");
    }

    void inventoriedTriggerStrigChoiceIndices()
    {
        beginTest("Inventoried trigger descriptors place STRIG at panel indices");

        const auto descriptors = Core::ApvtsPatchMapper::buildChoiceDescriptors();

        expectTriggerStrigAt(descriptors,
                             PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp1Trigger,
                             0,
                             PluginDisplayNames::ChoiceLists::RampTrigger::kStrig);
        expectTriggerStrigAt(descriptors,
                             PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kRamp2Trigger,
                             0,
                             PluginDisplayNames::ChoiceLists::RampTrigger::kStrig);
        expectTriggerStrigAt(descriptors,
                             PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
                             0,
                             PluginDisplayNames::ChoiceLists::TriggerMode::kStrig);
        expectTriggerStrigAt(descriptors,
                             PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
                             0,
                             PluginDisplayNames::ChoiceLists::TriggerMode::kStrig);
        expectTriggerStrigAt(descriptors,
                             PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
                             0,
                             PluginDisplayNames::ChoiceLists::TriggerMode::kStrig);
        expectTriggerStrigAt(descriptors,
                             PluginIDs::PatchEditSection::Lfo1Module::ParameterWidgets::kTriggerMode,
                             1,
                             PluginDisplayNames::ChoiceLists::LfoTriggerMode::kStrig);
        expectTriggerStrigAt(descriptors,
                             PluginIDs::PatchEditSection::Lfo2Module::ParameterWidgets::kTriggerMode,
                             1,
                             PluginDisplayNames::ChoiceLists::LfoTriggerMode::kStrig);
    }

    void legatoEnableAndGrayPredicates()
    {
        beginTest("LEGATO PORTA enabled only when Keyboard Mode index is UNISON");

        namespace Policy = TSS::UnisonKeyboardModePolicy;
        expect(Policy::isLegatoPortaEnabled(Policy::kUnisonKeyboardModeIndex));
        expect(! Policy::isLegatoPortaEnabled(0));
        expect(! Policy::isLegatoPortaEnabled(1));
        expect(! Policy::isLegatoPortaEnabled(3));
    }

    void strigAllowBlockAndFallbackIndex()
    {
        beginTest("STRIG allowed only in UNISON; fallback skips STRIG index");

        namespace Policy = TSS::UnisonKeyboardModePolicy;
        expect(Policy::isStrigAllowed(Policy::kUnisonKeyboardModeIndex));
        expect(! Policy::isStrigAllowed(0));
        expect(! Policy::isStrigAllowed(1));
        expect(! Policy::isStrigAllowed(3));

        // Ramp trigger: STRIG is index 0 → fallback is 1 (MTRIG).
        expectEquals(Policy::firstNonStrigChoiceIndex(0, 4), 1);
        // Env trigger: STRIG is index 0 as well.
        expectEquals(Policy::firstNonStrigChoiceIndex(0, 8), 1);
        // If STRIG were mid-list, pick the first other index.
        expectEquals(Policy::firstNonStrigChoiceIndex(2, 4), 0);
    }

    void masterOverrideBadgeVisibilityMatrix()
    {
        beginTest("Master override badge visibility matrix (device × Master Unison × Keyboard Mode)");

        namespace Policy = TSS::UnisonKeyboardModePolicy;
        using Type = MatrixDeviceTypes::Type;
        constexpr int kUnison = Policy::kUnisonKeyboardModeIndex;

        expect(Policy::shouldShowMasterOverrideBadge(Type::kMatrix1000, true, 0));
        expect(Policy::shouldShowMasterOverrideBadge(Type::kMatrix1000, true, 1));
        expect(Policy::shouldShowMasterOverrideBadge(Type::kMatrix1000, true, 3));
        expect(! Policy::shouldShowMasterOverrideBadge(Type::kMatrix1000, true, kUnison));
        expect(! Policy::shouldShowMasterOverrideBadge(Type::kMatrix1000, false, 0));
        expect(! Policy::shouldShowMasterOverrideBadge(Type::kMatrix6, true, 0));
        expect(! Policy::shouldShowMasterOverrideBadge(Type::kMatrix6R, true, 1));
        expect(! Policy::shouldShowMasterOverrideBadge(Type::kUnknown, true, 0));
    }

    void independentPatchKeyboardModeAndMasterUnison()
    {
        beginTest("Changing Keyboard Mode does not rewrite Master Unison octet 169 and vice versa");

        const auto domain = loadUnisonDomainDescriptors();
        expect(domain.keyboard != nullptr && domain.unison != nullptr);
        if (domain.keyboard == nullptr || domain.unison == nullptr)
            return;

        Core::PatchModel patchModel;
        Core::MasterModel masterModel;

        patchModel.setChoiceIndex(*domain.keyboard, 1);
        masterModel.setChoiceIndex(*domain.unison, 0);
        expectEquals(static_cast<int>(patchModel.data()[8]), 1);
        expectEquals(static_cast<int>(masterModel.data()[169]), 0);

        patchModel.setChoiceIndex(*domain.keyboard, TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
        expectEquals(static_cast<int>(patchModel.data()[8]),
                     TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
        expectEquals(static_cast<int>(masterModel.data()[169]), 0);

        masterModel.setChoiceIndex(*domain.unison, 1);
        expectEquals(static_cast<int>(masterModel.data()[169]), 1);
        expectEquals(static_cast<int>(patchModel.data()[8]),
                     TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
    }

    void expectDecodedMasterOctet169(const juce::MemoryBlock& sysEx, int expectedValue)
    {
        expect(SysExWireCompliance::assertMasterMessageMatches(sysEx, 0x03));

        SysExParser parser;
        SysExDecoder decoder(parser);
        std::array<juce::uint8, SysExConstants::kMasterPackedDataSize> decoded {};
        expect(decoder.decodeMasterSysEx(sysEx, decoded.data()));
        expectEquals(static_cast<int>(decoded[169]), expectedValue);
    }

    void expectRemoteEditQueued(Core::MidiOutboundQueue& queue,
                                int expectedParam,
                                int expectedValue)
    {
        auto msg = queue.dequeue();
        expect(msg.has_value());
        if (msg.has_value())
        {
            expect(SysExWireCompliance::assertRemoteEditMatches(
                msg->sysExData,
                static_cast<juce::uint8>(expectedParam),
                static_cast<juce::uint8>(expectedValue)));
        }
        expect(queue.isEmpty());
    }

    void seedMasterOnWithKeyboardUnison(Core::PatchModel& patchModel,
                                        Core::MasterModel& masterModel,
                                        const UnisonDomainDescriptors& domain)
    {
        masterModel.setChoiceIndex(*domain.unison, 1);
        patchModel.setChoiceIndex(*domain.keyboard,
                                  TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
    }

    void keyboardModeRemoteEditDoesNotTouchMasterUnison()
    {
        beginTest("Scenario A path: Keyboard Mode → Remote Edit 48 only; Master byte 169 untouched");

        const auto domain = loadUnisonDomainDescriptors();
        expect(domain.keyboard != nullptr && domain.unison != nullptr);
        if (domain.keyboard == nullptr || domain.unison == nullptr)
            return;

        Core::PatchModel patchModel;
        Core::MasterModel masterModel;
        seedMasterOnWithKeyboardUnison(patchModel, masterModel, domain);

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        int masterEnqueueCount = 0;
        int remoteEditCount = 0;
        int lastRemoteParam = -1;
        int lastRemoteValue = -1;

        Core::PatchParameterSysExDispatcher patchDispatcher(
            patchModel,
            [&](int parameterNumber, juce::uint8 packedValue)
            {
                ++remoteEditCount;
                lastRemoteParam = parameterNumber;
                lastRemoteValue = packedValue;
                Core::EditorPath(queue, tracker).enqueueSysEx(
                    encoder.encodeRemoteParameterEdit(
                        static_cast<juce::uint8>(parameterNumber), packedValue));
            });
        Core::MasterParameterSysExDispatcher masterDispatcher(
            masterModel, [&](const juce::uint8*) { ++masterEnqueueCount; });

        patchModel.setChoiceIndex(*domain.keyboard, 1); // leave UNISON → ROTATE
        patchDispatcher.dispatch(domain.keyboard->parameterId);
        masterDispatcher.dispatch(domain.keyboard->parameterId);

        expectEquals(remoteEditCount, 1);
        expectEquals(lastRemoteParam, 48);
        expectEquals(lastRemoteValue, 1);
        expectEquals(masterEnqueueCount, 0);
        expectEquals(static_cast<int>(masterModel.data()[169]), 1);
        expectRemoteEditQueued(queue, 48, 1);
    }

    void masterUnisonFullDumpDoesNotTouchKeyboardMode()
    {
        beginTest("Scenario B path: Master Unison OFF → full 0x03; patch byte 8 stays UNISON");

        const auto domain = loadUnisonDomainDescriptors();
        expect(domain.keyboard != nullptr && domain.unison != nullptr);
        if (domain.keyboard == nullptr || domain.unison == nullptr)
            return;

        Core::PatchModel patchModel;
        Core::MasterModel masterModel;
        patchModel.setChoiceIndex(*domain.keyboard, TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
        masterModel.setChoiceIndex(*domain.unison, 1);

        Core::MidiOutboundQueue queue;
        Core::MidiActivityTracker tracker;
        SysExEncoder encoder;
        int remoteEditCount = 0;

        Core::PatchParameterSysExDispatcher patchDispatcher(
            patchModel, [&](int, juce::uint8) { ++remoteEditCount; });
        Core::MasterParameterSysExDispatcher masterDispatcher(
            masterModel,
            [&](const juce::uint8* packedData)
            {
                Core::EditorPath editorPath(queue, tracker);
                editorPath.enqueueSysEx(encoder.encodeMasterSysEx(0x03, packedData));
            });

        masterModel.setChoiceIndex(*domain.unison, 0);
        masterDispatcher.dispatch(domain.unison->parameterId);
        patchDispatcher.dispatch(domain.unison->parameterId);

        expectEquals(remoteEditCount, 0);
        expectEquals(static_cast<int>(patchModel.data()[8]),
                     TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
        expectEquals(static_cast<int>(masterModel.data()[169]), 0);

        auto msg = queue.dequeue();
        expect(msg.has_value());
        if (msg.has_value())
            expectDecodedMasterOctet169(msg->sysExData, 0);
        expect(queue.isEmpty());
    }

    void masterUnisonEnableSurvivesFullMasterEncodeDecode()
    {
        beginTest("Unison Enable ON survives full Master 0x03 encode/decode at octet 169");

        const auto domain = loadUnisonDomainDescriptors();
        expect(domain.unison != nullptr);
        if (domain.unison == nullptr)
            return;

        Core::MasterModel model;
        model.setChoiceIndex(*domain.unison, 1);

        SysExEncoder encoder;
        const auto encoded = encoder.encodeMasterSysEx(0x03, model.data());
        expectDecodedMasterOctet169(encoded, 1);
    }
};

static UnisonKeyboardModePolicyTests unisonKeyboardModePolicyTests;
