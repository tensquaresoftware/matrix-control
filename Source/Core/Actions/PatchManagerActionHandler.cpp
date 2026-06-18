#include "Core/Actions/PatchManagerActionHandler.h"

#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

    PatchManagerActionHandler::PatchManagerActionHandler(juce::AudioProcessorValueTreeState& apvts,
                                                         DeviceMemoryLimitsSupplier deviceMemoryLimits)
        : apvts_(apvts)
        , deviceMemoryLimits_(std::move(deviceMemoryLimits))
    {
    }

    void PatchManagerActionHandler::handleAction(const juce::String& propertyId, const juce::var&)
    {
        using namespace PluginIDs::PatchManagerSection;

        const auto limits = deviceMemoryLimits_();

        if (propertyId == InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch
            || propertyId == InternalPatchesModule::StandaloneWidgets::kLoadNextPatch)
        {
            const bool isNext = propertyId == InternalPatchesModule::StandaloneWidgets::kLoadNextPatch;
            const int direction = isNext ? 1 : -1;

            PatchCoordinates current;
            current.bank = static_cast<int>(apvts_.state.getProperty(
                InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
                limits.minBankNumber()));
            current.patch = static_cast<int>(apvts_.state.getProperty(
                InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
                limits.minPatchNumber()));

            const bool bankLocked = static_cast<bool>(apvts_.state.getProperty(
                BankUtilityModule::StateProperties::kBankLock,
                false));

            applyPatchCoordinates(limits.advancePatch(current, direction, bankLocked));
            return;
        }

        if (propertyId == InternalPatchesModule::StandaloneWidgets::kInitPatch
            || propertyId == InternalPatchesModule::StandaloneWidgets::kPastePatch
            || propertyId == InternalPatchesModule::StandaloneWidgets::kStorePatch)
        {
            return; // Story 7.3
        }

        if (propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadPreviousPatchFile
            || propertyId == ComputerPatchesModule::StandaloneWidgets::kLoadNextPatchFile
            || propertyId == ComputerPatchesModule::StandaloneWidgets::kSelectPatchFile
            || propertyId == ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder
            || propertyId == ComputerPatchesModule::StandaloneWidgets::kSavePatchAs
            || propertyId == ComputerPatchesModule::StandaloneWidgets::kSavePatchFile)
        {
            return; // Story 7.3 + Epic 4
        }

        if (!limits.hasBankConcept())
        {
            if (parseBankButtonIndex(propertyId) >= 0
                || propertyId == BankUtilityModule::StandaloneWidgets::kLockBank)
            {
                return;
            }
        }

        const int bankIndex = parseBankButtonIndex(propertyId);
        if (bankIndex >= 0)
        {
            apvts_.state.setProperty(BankUtilityModule::StateProperties::kSelectedBank, bankIndex, nullptr);
            apvts_.state.setProperty(InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber, bankIndex, nullptr);
            return;
        }

        if (propertyId == BankUtilityModule::StandaloneWidgets::kLockBank)
        {
            const bool currentLock = static_cast<bool>(apvts_.state.getProperty(
                BankUtilityModule::StateProperties::kBankLock,
                false));
            apvts_.state.setProperty(BankUtilityModule::StateProperties::kBankLock, !currentLock, nullptr);
        }
    }

    void PatchManagerActionHandler::applyPatchCoordinates(const PatchCoordinates& coordinates)
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentBankNumber,
            coordinates.bank,
            nullptr);
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
            coordinates.patch,
            nullptr);
    }

    int PatchManagerActionHandler::parseBankButtonIndex(const juce::String& propertyId) const
    {
        using namespace PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets;

        if (propertyId == kSelectBank0) return 0;
        if (propertyId == kSelectBank1) return 1;
        if (propertyId == kSelectBank2) return 2;
        if (propertyId == kSelectBank3) return 3;
        if (propertyId == kSelectBank4) return 4;
        if (propertyId == kSelectBank5) return 5;
        if (propertyId == kSelectBank6) return 6;
        if (propertyId == kSelectBank7) return 7;
        if (propertyId == kSelectBank8) return 8;
        if (propertyId == kSelectBank9) return 9;

        return -1;
    }

} // namespace Core
