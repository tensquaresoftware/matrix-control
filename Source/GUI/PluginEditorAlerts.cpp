// Extracted from PluginEditor.cpp for modular maintenance.
// Ordered confirm alert + Mutator Delete confirm helpers (previously an anonymous namespace).

#include "PluginEditor.h"
#include "PluginEditorInternal.h"

#include "GUI/Dialogs/MatrixOrderedConfirmDialog.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace PluginEditorInternal
{

namespace
{
    struct ResolvedEditorChrome
    {
        TSS::ISkin* skin = nullptr;
        float uiScale = 1.0f;
        juce::Component* host = nullptr;
        std::unique_ptr<TSS::Skin> fallbackSkin;
    };

    ResolvedEditorChrome resolveEditorChrome(juce::Component* associatedComponent)
    {
        ResolvedEditorChrome resolved;
        resolved.host = associatedComponent;

        if (auto* editor = dynamic_cast<PluginEditor*>(associatedComponent))
        {
            resolved.skin = &editor->getActiveSkin();
            resolved.uiScale = editor->getAppliedUiScale();
            return resolved;
        }

        resolved.fallbackSkin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        resolved.skin = resolved.fallbackSkin.get();
        return resolved;
    }

    int runMatrixConfirmModal(juce::Component& dialog, juce::Component* host)
    {
        jassert(host != nullptr);
        if (host == nullptr)
            return 0;

        host->addAndMakeVisible(dialog);
        dialog.setBounds(host->getLocalBounds());
        dialog.toFront(true);
        dialog.enterModalState(true);
        dialog.grabKeyboardFocus();
        const int result = dialog.runModalLoop();
        host->removeChildComponent(&dialog);
        return result;
    }
}

bool isMessageThread()
{
    if (auto* mm = juce::MessageManager::getInstanceWithoutCreating())
        return mm->isThisTheMessageThread();

    return false;
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
    Semantic codes (stable across platforms): Cancel/Escape/OOR -> 0, primary -> 1, middle -> 2. */
int showOrderedConfirmAlert(const OrderedConfirmAlertOptions& options)
{
    jassert(options.cancelLabel.isNotEmpty());
    jassert(options.primaryLabel.isNotEmpty());
    jassert(isMessageThread());

    raiseUiBeforeModalDialog(options.associatedComponent);

   #if JUCE_MODAL_LOOPS_PERMITTED
    auto chrome = resolveEditorChrome(options.associatedComponent);
    if (chrome.host == nullptr || chrome.skin == nullptr)
        return 0;

    MatrixOrderedConfirmDialog dialog(*chrome.skin, chrome.uiScale, options);
    return runMatrixConfirmModal(dialog, chrome.host);
   #else
    jassertfalse;
    juce::ignoreUnused(options);
    return 0;
   #endif
}

MutatorDeleteConfirmResult showMutatorDeleteConfirmAlert(juce::Component* associatedComponent)
{
    raiseUiBeforeModalDialog(associatedComponent);

   #if JUCE_MODAL_LOOPS_PERMITTED
    auto chrome = resolveEditorChrome(associatedComponent);
    if (chrome.host == nullptr || chrome.skin == nullptr)
        return {};

    MatrixMutatorDeleteConfirmDialog dialog(*chrome.skin, chrome.uiScale);
    runMatrixConfirmModal(dialog, chrome.host);
    return dialog.getResult();
   #else
    jassertfalse;
    juce::ignoreUnused(associatedComponent);
    return {};
   #endif
}

} // namespace PluginEditorInternal
