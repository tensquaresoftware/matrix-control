#pragma once

// Free helpers + options structs shared by PluginEditor.cpp and its companion .cpp files.

#include <juce_gui_basics/juce_gui_basics.h>

namespace PluginEditorInternal
{
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
    };

    bool isMessageThread();

    /** True when macOS NSAlert places the first registered button as rightmost default. */
    bool usesMacOsNativeAlertButtonOrder();

    /** Prevent non-primary AlertWindow buttons from consuming Return when focused. */
    void configureOrderedAlertButtons(juce::AlertWindow& alert,
                                      const juce::String& cancelLabel,
                                      const juce::String& primaryLabel,
                                      const juce::String& middleLabel);

    // Semantic result codes (stable across platforms): Cancel/Escape/OOR -> 0, primary -> 1, middle -> 2.
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

    // Uses AlertWindow on all platforms (native NSAlert cannot host a checkbox).
    // Codes: Cancel/Escape -> confirmed=false, Delete/Return -> confirmed=true.
    MutatorDeleteConfirmResult showMutatorDeleteConfirmAlert(juce::Component* associatedComponent);

    // Nearest preset scale id (PluginIDs::Settings::ScaleLevels) matching a computed UI scale,
    // or 0 when no preset matches within rounding.
    int matchingScaleIdForUiScale(float uiScale);

} // namespace PluginEditorInternal
