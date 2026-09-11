#include "StrigUnisonGateHelper.h"

#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/Widgets/ComboBox.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Helpers/UnisonKeyboardModePolicy.h"

namespace TSS::StrigUnisonGateHelper
{
    namespace
    {
        int itemIdForChoiceIndex(int choiceIndex) noexcept
        {
            return choiceIndex + 1;
        }

        void setStrigItemEnabled(ComboBox& combo, int strigChoiceIndex, bool enabled)
        {
            const int itemId = itemIdForChoiceIndex(strigChoiceIndex);
            if (combo.getNumItems() <= strigChoiceIndex)
                return;

            combo.setItemEnabled(itemId, enabled);
        }

        void fallbackFromStrigIfNeeded(juce::AudioProcessorValueTreeState& apvts,
                                       const juce::String& triggerParameterId,
                                       int strigChoiceIndex)
        {
            auto* param = apvts.getParameter(triggerParameterId);
            auto* choice = dynamic_cast<juce::AudioParameterChoice*>(param);
            if (choice == nullptr)
                return;

            if (choice->getIndex() != strigChoiceIndex)
                return;

            const int fallback = UnisonKeyboardModePolicy::firstNonStrigChoiceIndex(
                strigChoiceIndex, choice->choices.size());
            if (fallback == strigChoiceIndex)
                return;

            choice->beginChangeGesture();
            choice->setValueNotifyingHost(choice->convertTo0to1(static_cast<float>(fallback)));
            choice->endChangeGesture();

            GrayedControlHelper::setFooterInfoMessage(
                apvts,
                PluginDisplayNames::PatchEditSection::RampPortamentoModule::kStrigRequiresUnisonFooter);
        }
    }

    bool isKeyboardModeUnison(const juce::AudioProcessorValueTreeState& apvts)
    {
        const auto* keyboardModeParam = apvts.getParameter(
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);
        const auto* choiceParam = dynamic_cast<const juce::AudioParameterChoice*>(keyboardModeParam);
        return choiceParam != nullptr
            && UnisonKeyboardModePolicy::isUnisonKeyboardModeIndex(choiceParam->getIndex());
    }

    void refreshTriggerParameter(const TriggerRefreshRequest& request)
    {
        const auto* keyboardModeParam = request.apvts.getParameter(
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode);
        const auto* choiceParam = dynamic_cast<const juce::AudioParameterChoice*>(keyboardModeParam);
        const int keyboardModeIndex = choiceParam != nullptr ? choiceParam->getIndex() : -1;
        const bool strigAllowed = UnisonKeyboardModePolicy::isStrigAllowed(keyboardModeIndex);

        if (request.combo != nullptr)
            setStrigItemEnabled(*request.combo, request.strigChoiceIndex, strigAllowed);

        if (! strigAllowed && request.clearIfCurrentStrig)
            fallbackFromStrigIfNeeded(request.apvts, request.triggerParameterId, request.strigChoiceIndex);
    }

    KeyboardModeChangeListener::KeyboardModeChangeListener(
        juce::AudioProcessorValueTreeState& apvts,
        std::function<void(bool leftUnison)> onKeyboardModeChanged)
        : apvts_(apvts)
        , onKeyboardModeChanged_(std::move(onKeyboardModeChanged))
        , wasUnison_(isKeyboardModeUnison(apvts_))
    {
        apvts_.addParameterListener(
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
            this);
    }

    KeyboardModeChangeListener::~KeyboardModeChangeListener()
    {
        apvts_.removeParameterListener(
            PluginIDs::PatchEditSection::RampPortamentoModule::ParameterWidgets::kPortamentoKeyboardMode,
            this);
    }

    void KeyboardModeChangeListener::parameterChanged(const juce::String&, float)
    {
        const bool isUnison = isKeyboardModeUnison(apvts_);
        const bool leftUnison = wasUnison_ && ! isUnison;
        wasUnison_ = isUnison;

        if (onKeyboardModeChanged_)
            onKeyboardModeChanged_(leftUnison);
    }
}
