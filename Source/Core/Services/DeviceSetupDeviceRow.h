#pragma once

#include <juce_core/juce_core.h>

#include "Core/MIDI/SysEx/SysExConstants.h"
#include "Core/Services/EpromTypePolicy.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{
    /** DEVICE row kind for the DEVICE SETUP assistant (footer-aligned chrome). */
    enum class DeviceSetupDeviceRowKind
    {
        kNotConnected,
        kSearching,
        kConnected
    };

    struct DeviceSetupDeviceRowInput
    {
        bool midiFromReady = false;
        bool midiToReady = false;
        bool deviceDetected = false;
        bool deviceMidiUnresponsive = false;
        bool searchingWindowActive = false;
        MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown;
    };

    struct DeviceSetupDeviceRowView
    {
        DeviceSetupDeviceRowKind kind = DeviceSetupDeviceRowKind::kNotConnected;
        bool identityOk = false;
        juce::String detailText;
    };

    /** Open guard for the one-shot DEVICE SETUP assistant. */
    inline bool shouldOpenDeviceSetupAssistant(bool promptDone, bool dialogAlreadyVisible) noexcept
    {
        return ! promptDone && ! dialogAlreadyVisible;
    }

    struct DeviceSetupFinishFlags
    {
        bool promptDone = true;
        bool promptPending = false;
    };

    /** SPECIFY LATER / Esc / outside-click: mark done, clear pending; EPROM unchanged by caller. */
    inline DeviceSetupFinishFlags deviceSetupSpecifyLaterFlags() noexcept
    {
        return { true, false };
    }

    struct DeviceSetupConfirmResult
    {
        int epromTypeId = PluginIDs::Settings::EpromType::kDefault;
        bool promptDone = true;
        bool promptPending = false;
    };

    /** CONFIRM: persist normalized EPROM selection and finish the one-shot prompt. */
    inline DeviceSetupConfirmResult deviceSetupConfirmResult(int selectedId) noexcept
    {
        return {
            EpromTypePolicy::normalize(selectedId),
            true,
            false,
        };
    }

    /** Prefer Inquiry suggestion unless the user already touched the combo. */
    inline int nextDeviceSetupEpromPreferredId(bool userTouched,
                                               int currentId,
                                               int preferredId) noexcept
    {
        if (userTouched)
            return currentId > 0 ? currentId : preferredId;

        return preferredId;
    }

    inline bool isDeviceSetupIdentityOk(bool deviceDetected,
                                        bool deviceMidiUnresponsive,
                                        MatrixDeviceTypes::Type deviceType) noexcept
    {
        if (! deviceDetected || deviceMidiUnresponsive)
            return false;

        return MatrixDeviceTypes::isSupportedMatrixDevice(deviceType);
    }

    /** True when both ports are ready and identity is not yet OK — SEARCHING may run. */
    inline bool shouldRunDeviceSetupSearchingWindow(bool midiFromReady,
                                                    bool midiToReady,
                                                    bool identityOk,
                                                    bool deviceMidiUnresponsive = false) noexcept
    {
        return midiFromReady && midiToReady && ! identityOk && ! deviceMidiUnresponsive;
    }

    struct DeviceSetupSearchingWindowState
    {
        bool active = false;
        bool exhausted = false;
        juce::String trackedFromId;
        juce::String trackedToId;
        juce::uint32 startedMs = 0;
    };

    struct DeviceSetupSearchingWindowInput
    {
        juce::String midiFromId;
        juce::String midiToId;
        bool identityOk = false;
        bool deviceMidiUnresponsive = false;
        juce::uint32 nowMs = 0;
        int timeoutMs = SysExConstants::kDefaultTimeoutMs;
    };

    struct DeviceSetupSearchingWindowUpdate
    {
        DeviceSetupSearchingWindowState state;
        bool shouldKickInquiry = false;
    };

    inline bool shouldStopDeviceSetupSearchingWindow(bool bothPortsReady,
                                                     bool identityOk,
                                                     bool deviceMidiUnresponsive) noexcept
    {
        return ! bothPortsReady || identityOk || deviceMidiUnresponsive;
    }

    inline bool shouldStartOrRestartDeviceSetupSearchingWindow(
        bool portPairChanged,
        bool previouslyActive,
        bool previouslyExhausted) noexcept
    {
        return portPairChanged || (! previouslyActive && ! previouslyExhausted);
    }

    inline DeviceSetupSearchingWindowUpdate stopDeviceSetupSearchingWindow(
        const DeviceSetupSearchingWindowState& previous,
        bool bothPortsReady,
        bool identityOk,
        bool deviceMidiUnresponsive) noexcept
    {
        DeviceSetupSearchingWindowUpdate update;
        update.state = previous;
        update.state.active = false;

        if (! bothPortsReady || identityOk)
            update.state.exhausted = false;
        else if (deviceMidiUnresponsive)
            update.state.exhausted = true;

        return update;
    }

    inline DeviceSetupSearchingWindowUpdate startDeviceSetupSearchingWindow(
        const DeviceSetupSearchingWindowState& previous,
        const juce::String& midiFromId,
        const juce::String& midiToId,
        juce::uint32 nowMs) noexcept
    {
        DeviceSetupSearchingWindowUpdate update;
        update.state = previous;
        update.state.active = true;
        update.state.exhausted = false;
        update.state.trackedFromId = midiFromId;
        update.state.trackedToId = midiToId;
        update.state.startedMs = nowMs;
        update.shouldKickInquiry = true;
        return update;
    }

    /**
     * Advance SEARCHING window: start/restart on ready port-pair change (clears exhausted),
     * exhaust after timeoutMs, stop when ports incomplete / identity OK / unresponsive.
     */
    inline DeviceSetupSearchingWindowUpdate advanceDeviceSetupSearchingWindow(
        const DeviceSetupSearchingWindowState& previous,
        const DeviceSetupSearchingWindowInput& input) noexcept
    {
        const bool bothReady = input.midiFromId.isNotEmpty() && input.midiToId.isNotEmpty();
        if (shouldStopDeviceSetupSearchingWindow(bothReady,
                                                 input.identityOk,
                                                 input.deviceMidiUnresponsive))
        {
            return stopDeviceSetupSearchingWindow(previous,
                                                  bothReady,
                                                  input.identityOk,
                                                  input.deviceMidiUnresponsive);
        }

        const bool pairChanged = input.midiFromId != previous.trackedFromId
            || input.midiToId != previous.trackedToId;
        if (shouldStartOrRestartDeviceSetupSearchingWindow(pairChanged,
                                                           previous.active,
                                                           previous.exhausted))
        {
            return startDeviceSetupSearchingWindow(previous,
                                                   input.midiFromId,
                                                   input.midiToId,
                                                   input.nowMs);
        }

        if (previous.active
            && (input.nowMs - previous.startedMs) >= static_cast<juce::uint32>(input.timeoutMs))
        {
            DeviceSetupSearchingWindowUpdate update;
            update.state = previous;
            update.state.active = false;
            update.state.exhausted = true;
            return update;
        }

        return { previous, false };
    }

    /** Build connected detail using a display-formatted version string (may be empty). */
    inline juce::String buildDeviceSetupConnectedDetail(MatrixDeviceTypes::Type deviceType,
                                                        const juce::String& versionDisplay)
    {
        if (! MatrixDeviceTypes::isSupportedMatrixDevice(deviceType))
            return PluginDisplayNames::FooterPanel::kDeviceUnknownDetail;

        juce::String detail = MatrixDeviceTypes::toDisplayString(deviceType).toUpperCase();
        if (versionDisplay.isNotEmpty())
            detail += " (V" + versionDisplay + ")";

        return detail;
    }

    inline DeviceSetupDeviceRowView resolveDeviceSetupDeviceRow(const DeviceSetupDeviceRowInput& input,
                                                                const juce::String& versionDisplay = {})
    {
        DeviceSetupDeviceRowView view;

        if (! input.midiFromReady || ! input.midiToReady)
        {
            view.kind = DeviceSetupDeviceRowKind::kNotConnected;
            view.identityOk = false;
            view.detailText = PluginDisplayNames::FooterPanel::kDeviceNotConnectedDetail;
            return view;
        }

        if (input.deviceMidiUnresponsive)
        {
            view.kind = DeviceSetupDeviceRowKind::kNotConnected;
            view.identityOk = false;
            view.detailText = PluginDisplayNames::FooterPanel::kDeviceNotConnectedDetail;
            return view;
        }

        const bool identityOk = isDeviceSetupIdentityOk(input.deviceDetected,
                                                        input.deviceMidiUnresponsive,
                                                        input.deviceType);
        if (identityOk || input.deviceDetected)
        {
            view.kind = DeviceSetupDeviceRowKind::kConnected;
            view.identityOk = identityOk;
            view.detailText = buildDeviceSetupConnectedDetail(input.deviceType, versionDisplay);
            return view;
        }

        if (input.searchingWindowActive)
        {
            view.kind = DeviceSetupDeviceRowKind::kSearching;
            view.identityOk = false;
            view.detailText = PluginDisplayNames::Dialogs::EpromTypePrompt::kSearching;
            return view;
        }

        view.kind = DeviceSetupDeviceRowKind::kNotConnected;
        view.identityOk = false;
        view.detailText = PluginDisplayNames::FooterPanel::kDeviceNotConnectedDetail;
        return view;
    }
}
