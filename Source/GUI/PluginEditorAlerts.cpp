// Extracted from PluginEditor.cpp for modular maintenance.
// Ordered confirm alert + Mutator Delete confirm helpers (previously an anonymous namespace).

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "Shared/Definitions/PluginDisplayNames.h"

namespace PluginEditorInternal
{

bool isMessageThread()
{
    if (auto* mm = juce::MessageManager::getInstanceWithoutCreating())
        return mm->isThisTheMessageThread();

    return false;
}

bool usesMacOsNativeAlertButtonOrder()
{
   #if JUCE_MAC
    return juce::LookAndFeel::getDefaultLookAndFeel().isUsingNativeAlertWindows();
   #else
    return false;
   #endif
}

void configureOrderedAlertButtons(juce::AlertWindow& alert,
                                  const juce::String& cancelLabel,
                                  const juce::String& primaryLabel,
                                  const juce::String& middleLabel)
{
    auto* cancel = alert.getButton(cancelLabel);
    auto* primary = alert.getButton(primaryLabel);
    jassert(cancel != nullptr);
    jassert(primary != nullptr);

    if (cancel != nullptr)
        cancel->setWantsKeyboardFocus(false);

    if (middleLabel.isNotEmpty())
    {
        auto* middle = alert.getButton(middleLabel);
        jassert(middle != nullptr);

        if (middle != nullptr)
            middle->setWantsKeyboardFocus(false);
    }

    // Primary keeps keyboard focus so Return activates it; Cancel/middle are click-only.
    if (primary != nullptr)
        primary->grabKeyboardFocus();
}

void raiseUiBeforeModalDialog(juce::Component* associatedComponent)
{
    if (associatedComponent != nullptr)
    {
        if (auto* top = associatedComponent->getTopLevelComponent())
            top->toFront(true);
        else
            associatedComponent->toFront(true);
    }

   #if JUCE_MAC
    juce::Process::makeForegroundProcess();
   #endif
}

juce::File browseForDirectorySync(juce::Component* associatedComponent,
                                  const juce::String& dialogTitle,
                                  const juce::File& startDirectory)
{
    const juce::Component::SafePointer<juce::Component> safe(associatedComponent);
    raiseUiBeforeModalDialog(safe.getComponent());

    juce::FileChooser chooser(dialogTitle,
                              startDirectory,
                              juce::String(),
                              true,
                              false,
                              safe.getComponent());

    const bool ok = chooser.browseForDirectory();
    raiseUiBeforeModalDialog(safe.getComponent());

    if (! ok)
        return {};

    return chooser.getResult();
}

juce::File browseForFileToSaveSync(juce::Component* associatedComponent,
                                   const juce::String& dialogTitle,
                                   const juce::File& startFileOrDirectory,
                                   const juce::String& filePatterns)
{
    const juce::Component::SafePointer<juce::Component> safe(associatedComponent);
    raiseUiBeforeModalDialog(safe.getComponent());

    juce::FileChooser chooser(dialogTitle,
                              startFileOrDirectory,
                              filePatterns,
                              true,
                              false,
                              safe.getComponent());

    const bool ok = chooser.browseForFileToSave(true);
    raiseUiBeforeModalDialog(safe.getComponent());

    if (! ok)
        return {};

    return chooser.getResult();
}

juce::File browseForFileToOpenSync(juce::Component* associatedComponent,
                                   const juce::String& dialogTitle,
                                   const juce::File& startDirectory,
                                   const juce::String& filePatterns)
{
    const juce::Component::SafePointer<juce::Component> safe(associatedComponent);
    raiseUiBeforeModalDialog(safe.getComponent());

    juce::FileChooser chooser(dialogTitle,
                              startDirectory,
                              filePatterns,
                              true,
                              false,
                              safe.getComponent());

    const bool ok = chooser.browseForFileToOpen();
    raiseUiBeforeModalDialog(safe.getComponent());

    if (! ok)
        return {};

    return chooser.getResult();
}

/** Visual LTR: Cancel -> [middle] -> primary (rightmost = default).
    Semantic codes (stable across platforms): Cancel/Escape/OOR -> 0, primary -> 1, middle -> 2.

    macOS native: register primary-first (NSAlert rightmost-first).
    Windows/Linux: controlled AlertWindow with Cancel-first layout and Return/Escape wired
    by role -- preferred over TaskDialog so LTR + Enter=primary both hold. */
int showOrderedConfirmAlert(const OrderedConfirmAlertOptions& options)
{
    jassert(options.cancelLabel.isNotEmpty());
    jassert(options.primaryLabel.isNotEmpty());

    const bool hasMiddle = options.middleLabel.isNotEmpty();

    // Native FileChooser can leave Matrix-Control behind; raise before the next modal.
    raiseUiBeforeModalDialog(options.associatedComponent);

    if (usesMacOsNativeAlertButtonOrder())
    {
        auto messageBoxOptions = juce::MessageBoxOptions()
                                     .withIconType(options.iconType)
                                     .withTitle(options.title)
                                     .withMessage(options.message)
                                     .withButton(options.primaryLabel);

        if (hasMiddle)
            messageBoxOptions = messageBoxOptions.withButton(options.middleLabel);

        messageBoxOptions = messageBoxOptions.withButton(options.cancelLabel)
                                .withAssociatedComponent(options.associatedComponent);

        const int numButtons = messageBoxOptions.getNumButtons();
        const int raw = juce::NativeMessageBox::show(messageBoxOptions);

        if (raw < 0 || raw >= numButtons)
            return 0;

        return (raw + 1) % numButtons;
    }

   #if JUCE_MODAL_LOOPS_PERMITTED
    juce::AlertWindow alert(options.title, options.message, options.iconType, options.associatedComponent);
    alert.addButton(options.cancelLabel, 0, juce::KeyPress(juce::KeyPress::escapeKey));

    if (hasMiddle)
        alert.addButton(options.middleLabel, 2);

    alert.addButton(options.primaryLabel, 1, juce::KeyPress(juce::KeyPress::returnKey));
    configureOrderedAlertButtons(alert, options.cancelLabel, options.primaryLabel, options.middleLabel);
    return alert.runModalLoop();
   #else
    jassertfalse;
    juce::ignoreUnused(options, hasMiddle);
    return 0;
   #endif
}

MutatorDeleteConfirmResult showMutatorDeleteConfirmAlert(juce::Component* associatedComponent)
{
    namespace Dialog = PluginDisplayNames::Dialogs::MutatorDeleteConfirm;

    raiseUiBeforeModalDialog(associatedComponent);

   #if JUCE_MODAL_LOOPS_PERMITTED
    juce::AlertWindow alert(Dialog::kTitle,
                            Dialog::kBody,
                            juce::MessageBoxIconType::WarningIcon,
                            associatedComponent);
    juce::ToggleButton dontAskAgain(Dialog::kDontAskAgain);
    dontAskAgain.setSize(360, 24);
    // AlertWindow paints customComponent->getName() above the control; clear it so
    // "Don't ask again" appears only once (ToggleButton text), not as a duplicate label.
    dontAskAgain.setName({});
    // Keep Enter on Delete (primary); checkbox is click-only, same as Cancel.
    dontAskAgain.setWantsKeyboardFocus(false);
    dontAskAgain.setMouseClickGrabsKeyboardFocus(false);
    alert.addCustomComponent(&dontAskAgain);
    alert.addButton(Dialog::kCancel, 0, juce::KeyPress(juce::KeyPress::escapeKey));
    alert.addButton(Dialog::kDelete, 1, juce::KeyPress(juce::KeyPress::returnKey));
    configureOrderedAlertButtons(alert, Dialog::kCancel, Dialog::kDelete, {});

    const int result = alert.runModalLoop();
    return { result == 1, dontAskAgain.getToggleState() };
   #else
    jassertfalse;
    juce::ignoreUnused(associatedComponent);
    return {};
   #endif
}

} // namespace PluginEditorInternal
