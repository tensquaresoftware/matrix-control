#pragma once

// Free helpers + options structs shared by PluginEditor.cpp and its companion .cpp files.

#include <vector>

#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace PluginEditorInternal
{
    /** "Label: value" row drawn above the message with values aligned on a shared column. */
    struct LabelledValueRow
    {
        juce::String label;
        juce::String value;
    };

    // ---- Ordered confirm alert: visual LTR Cancel / [middle] / primary (rightmost = default),
    //      Return always activates the primary button. See PluginEditorAlerts.cpp. ----
    struct OrderedConfirmAlertOptions
    {
        juce::MessageBoxIconType iconType;
        juce::String title;
        juce::String message;
        juce::String cancelLabel;
        juce::String primaryLabel;
        juce::Component* associatedComponent = nullptr;
        juce::String middleLabel = {};
        std::vector<LabelledValueRow> valueRows = {};
        /** Design px content width; 0 uses MatrixOrderedConfirmDialog default. */
        int designWidth = 0;
        /** When true, body left edge matches CANCEL (patch name mismatch). Default: ~10% inset. */
        bool alignBodyToCancel = false;
    };

    bool isMessageThread();

    // Matrix-chrome sync confirm. Semantic codes: Cancel/Escape -> 0, primary -> 1, middle -> 2.
    int showOrderedConfirmAlert(const OrderedConfirmAlertOptions& options);

    /** Bring the plugin/standalone UI forward before (or after) a sync OS modal. */
    void raiseUiBeforeModalDialog(juce::Component* associatedComponent);

    /** Raise → sync native directory FileChooser → raise. Empty File if cancelled. */
    juce::File browseForDirectorySync(juce::Component* associatedComponent,
                                      const juce::String& dialogTitle,
                                      const juce::File& startDirectory);

    /** Raise → sync native save FileChooser → raise. Empty File if cancelled.
        Always warns when overwriting an existing file. */
    juce::File browseForFileToSaveSync(juce::Component* associatedComponent,
                                       const juce::String& dialogTitle,
                                       const juce::File& startFileOrDirectory,
                                       const juce::String& filePatterns);

    /** Raise → sync native open FileChooser → raise. Empty File if cancelled. */
    juce::File browseForFileToOpenSync(juce::Component* associatedComponent,
                                       const juce::String& dialogTitle,
                                       const juce::File& startDirectory,
                                       const juce::String& filePatterns);

    // ---- Mutator Delete confirm with an optional "Don't ask again" checkbox. ----
    struct MutatorDeleteConfirmResult
    {
        bool confirmed = false;
        bool dontAskAgain = false;
    };

    // Matrix-chrome sync confirm with don't-ask-again toggle.
    // Codes: Cancel/Escape -> confirmed=false, Delete/Return -> confirmed=true.
    MutatorDeleteConfirmResult showMutatorDeleteConfirmAlert(juce::Component* associatedComponent);

    // Nearest preset scale id (PluginIDs::Settings::ScaleLevels) matching a computed UI scale,
    // or 0 when no preset matches within rounding.
    int matchingScaleIdForUiScale(float uiScale);

    /** EPROM TYPE popup-only Inquiry mark id from APVTS device properties (0 = none). */
    inline int epromInquiryPopupMarkerFromState(const juce::ValueTree& state)
    {
        const bool detected = static_cast<bool>(state.getProperty("deviceDetected", false));
        const auto version = state.getProperty("deviceVersion", juce::String()).toString();
        const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
            state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
        return Core::EpromTypePolicy::inquiryPopupMarkerId(
            detected, version, Core::EpromTypePolicy::deviceFamilyFromType(deviceType));
    }

    inline SettingsPanel::LiveDeviceStatus settingsLiveDeviceStatusFromState(
        const juce::ValueTree& state)
    {
        return {
            .deviceDetected = static_cast<bool>(state.getProperty("deviceDetected", false)),
            .deviceMidiUnresponsive = static_cast<bool>(
                state.getProperty(Core::kDeviceMidiUnresponsiveProperty, false)),
            .deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
                state.getProperty(MatrixDeviceTypes::kApvtsPropertyName)),
            .deviceVersion = state.getProperty("deviceVersion", juce::String()).toString(),
        };
    }

} // namespace PluginEditorInternal
