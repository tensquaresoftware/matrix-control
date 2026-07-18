#include "GUI/Helpers/CompareLockBinder.h"

#include "Core/MIDI/EditorOutboundGate.h"
#include "GUI/Helpers/GrayedControlHelper.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;
    namespace FooterCopy = PluginDisplayNames::FooterPanel;

    constexpr float kLockedAlpha = 0.5f;
    const juce::Identifier kDeviceDetectedId("deviceDetected");

    void applySectionLock(juce::Component& component, bool locked)
    {
        // JUCE 8 getComponentAt() always descends into children when the parent hit-tests true.
        // setInterceptsMouseClicks(true, false) therefore only dims visually if paired with alpha —
        // it does NOT block child interaction. Use (false, false) so hitTest fails on the whole
        // subtree and clicks fall through to the parent (non-interactive while locked).
        component.setInterceptsMouseClicks(! locked, ! locked);
        component.setAlpha(locked ? kLockedAlpha : 1.0f);

        if (locked)
            component.giveAwayKeyboardFocus();
    }
}

namespace TSS
{
    CompareLockBinder::CompareLockBinder(juce::AudioProcessorValueTreeState& apvts,
                                         std::vector<juce::Component*> lockedComponents,
                                         bool lockOnCompare)
        : apvts_(apvts)
        , lockOnCompare_(lockOnCompare)
    {
        locked_.reserve(lockedComponents.size());
        for (auto* component : lockedComponents)
            locked_.emplace_back(component);

        apvts_.state.addListener(this);
        apply();
    }

    CompareLockBinder::~CompareLockBinder()
    {
        apvts_.state.removeListener(this);
    }

    void CompareLockBinder::valueTreePropertyChanged(juce::ValueTree&, const juce::Identifier& property)
    {
        if (property.toString() == MutatorState::kCompareActive || property == kDeviceDetectedId)
            apply();
    }

    void CompareLockBinder::valueTreeRedirected(juce::ValueTree&)
    {
        apply();
    }

    void CompareLockBinder::syncDeviceLockFooter(bool deviceDetected)
    {
        if (! deviceDetected)
        {
            GrayedControlHelper::setFooterInfoMessage(apvts_, FooterCopy::kDeviceLockGuidance);
            return;
        }

        if (apvts_.state.getProperty("uiMessageText").toString()
            == juce::String(FooterCopy::kDeviceLockGuidance))
        {
            apvts_.state.setProperty("uiMessageText", juce::String(), nullptr);
        }
    }

    void CompareLockBinder::apply()
    {
        const bool compareActive = static_cast<bool>(
            apvts_.state.getProperty(MutatorState::kCompareActive, false));
        const bool deviceDetected = static_cast<bool>(
            apvts_.state.getProperty(kDeviceDetectedId, false));

        const bool locked = lockOnCompare_
                                ? Core::isSectionLocked(deviceDetected, compareActive)
                                : ! deviceDetected;

        for (auto& safeComponent : locked_)
        {
            if (auto* component = safeComponent.getComponent())
                applySectionLock(*component, locked);
        }

        syncDeviceLockFooter(deviceDetected);
    }
}
