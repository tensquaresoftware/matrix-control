#include <vector>

#include <juce_core/juce_core.h>

#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Helpers/UnisonKeyboardModePolicy.h"

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

        const auto patchChoices = Core::ApvtsPatchMapper::buildChoiceDescriptors();
        const auto masterChoices = Core::ApvtsMasterMapper::buildChoiceDescriptors();
        const auto* keyboardDesc = findChoice(
            patchChoices,
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);
        const auto* unisonDesc = findChoice(
            masterChoices,
            PluginIDs::MasterEditSection::MiscModule::ParameterWidgets::kUnisonEnable);

        expect(keyboardDesc != nullptr && unisonDesc != nullptr);
        if (keyboardDesc == nullptr || unisonDesc == nullptr)
            return;

        Core::PatchModel patchModel;
        Core::MasterModel masterModel;

        patchModel.setChoiceIndex(*keyboardDesc, 1);
        masterModel.setChoiceIndex(*unisonDesc, 0);
        expectEquals(static_cast<int>(patchModel.data()[8]), 1);
        expectEquals(static_cast<int>(masterModel.data()[169]), 0);

        patchModel.setChoiceIndex(*keyboardDesc, TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
        expectEquals(static_cast<int>(patchModel.data()[8]),
                     TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
        expectEquals(static_cast<int>(masterModel.data()[169]), 0);

        masterModel.setChoiceIndex(*unisonDesc, 1);
        expectEquals(static_cast<int>(masterModel.data()[169]), 1);
        expectEquals(static_cast<int>(patchModel.data()[8]),
                     TSS::UnisonKeyboardModePolicy::kUnisonKeyboardModeIndex);
    }
};

static UnisonKeyboardModePolicyTests unisonKeyboardModePolicyTests;
