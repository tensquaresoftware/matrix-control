// Furtive left-footer contextual help for Patch Mutator controls (display-only overlay).

#include "PatchMutatorPanel.h"
#include "PatchMutatorPanelInternal.h"

#include "GUI/Helpers/ContextualHelpOverlay.h"
#include "GUI/MainComponent.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HierarchicalComboBox.h"
#include "GUI/Widgets/HierarchicalPopupMenu.h"
#include "GUI/Widgets/MultiColumnPopupMenu.h"
#include "GUI/Widgets/ScrollablePopupMenu.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using namespace PatchMutatorPanelInternal;
namespace MutatorHelp = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::ContextualHelp;

namespace
{
    constexpr int kContextualHelpClearDelayMs = 75;

    bool isMutatorPopupComponent(const juce::Component* component)
    {
        return dynamic_cast<const TSS::HierarchicalPopupMenu*>(component) != nullptr
            || dynamic_cast<const TSS::MultiColumnPopupMenu*>(component) != nullptr
            || dynamic_cast<const TSS::ScrollablePopupMenu*>(component) != nullptr;
    }

    bool isFocusInsideMutatorPopupMenu(const juce::Component* focused)
    {
        for (auto* c = focused; c != nullptr; c = c->getParentComponent())
        {
            if (isMutatorPopupComponent(c))
                return true;
        }

        return false;
    }

    bool isMutatorPopupModalActive()
    {
        for (int i = 0; i < juce::Component::getNumCurrentlyModalComponents(); ++i)
        {
            if (isMutatorPopupComponent(juce::Component::getCurrentlyModalComponent(i)))
                return true;
        }

        return false;
    }
}

void PatchMutatorPanel::registerContextualHelpBindings()
{
    const auto bind = [this](juce::Component* control, const char* helpText)
    {
        if (control == nullptr || helpText == nullptr)
            return;

        contextualHelpByControl_[control] = helpText;
        control->addMouseListener(this, true);
    };

    bind(modeComboBox_.get(), MutatorHelp::kMode);
    bind(pitchComboBox_.get(), MutatorHelp::kPitch);
    bind(historyComboBox_.get(), MutatorHelp::kHistory);
    bind(mutateButton_.get(), MutatorHelp::kMutate);
    bind(retryButton_.get(), MutatorHelp::kRetry);
    bind(historyPreviousButton_.get(), MutatorHelp::kHistoryPrevious);
    bind(historyNextButton_.get(), MutatorHelp::kHistoryNext);
    bind(compareButton_.get(), MutatorHelp::kCompare);
    bind(deleteButton_.get(), MutatorHelp::kDelete);
    bind(clearButton_.get(), MutatorHelp::kFlush);
    bind(exportButton_.get(), MutatorHelp::kExport);
    bind(dco1Toggle_.get(), MutatorHelp::kEnableDco1);
    bind(dco2Toggle_.get(), MutatorHelp::kEnableDco2);
    bind(vcfVcaToggle_.get(), MutatorHelp::kEnableVcfVca);
    bind(fmTrackToggle_.get(), MutatorHelp::kEnableFmTrack);
    bind(rampPortamentoToggle_.get(), MutatorHelp::kEnableRampPortamento);
    bind(env1Toggle_.get(), MutatorHelp::kEnableEnvelope1);
    bind(env2Toggle_.get(), MutatorHelp::kEnableEnvelope2);
    bind(env3Toggle_.get(), MutatorHelp::kEnableEnvelope3);
    bind(lfo1Toggle_.get(), MutatorHelp::kEnableLfo1);
    bind(lfo2Toggle_.get(), MutatorHelp::kEnableLfo2);
    bind(enableMatrixModToggle_.get(), MutatorHelp::kEnableMatrixMod);

    juce::Desktop::getInstance().addFocusChangeListener(this);
}

void PatchMutatorPanel::unregisterContextualHelpBindings()
{
    juce::Desktop::getInstance().removeFocusChangeListener(this);

    for (const auto& entry : contextualHelpByControl_)
    {
        if (entry.first != nullptr)
            entry.first->removeMouseListener(this);
    }

    contextualHelpByControl_.clear();
    ++contextualHelpClearGeneration_;

    if (auto* footer = resolveFooterPanel())
        footer->clearContextualHelpOverlay();
}

FooterPanel* PatchMutatorPanel::resolveFooterPanel() const
{
    if (auto* main = findParentComponentOfClass<MainComponent>())
        return &main->getFooterPanel();

    return nullptr;
}

const char* PatchMutatorPanel::helpTextForControl(juce::Component* control) const
{
    for (auto* c = control; c != nullptr; c = c->getParentComponent())
    {
        const auto it = contextualHelpByControl_.find(c);
        if (it != contextualHelpByControl_.end())
            return it->second;
    }

    return nullptr;
}

juce::Component* PatchMutatorPanel::resolveActiveContextualHelpControl() const
{
    for (const auto& entry : contextualHelpByControl_)
    {
        auto* control = entry.first;
        if (control != nullptr && control->hasKeyboardFocus(true))
            return control;
    }

    for (const auto& entry : contextualHelpByControl_)
    {
        auto* control = entry.first;
        if (control != nullptr && control->isMouseOver(true))
            return control;
    }

    return nullptr;
}

void PatchMutatorPanel::showContextualHelpFor(juce::Component* control)
{
    const auto* helpText = helpTextForControl(control);
    if (helpText == nullptr)
        return;

    ++contextualHelpClearGeneration_;

    if (auto* footer = resolveFooterPanel())
        footer->setContextualHelpOverlay(helpText);
}

void PatchMutatorPanel::scheduleContextualHelpClear()
{
    const int generation = ++contextualHelpClearGeneration_;
    juce::Timer::callAfterDelay(kContextualHelpClearDelayMs,
                                [safeThis = juce::Component::SafePointer<PatchMutatorPanel>(this),
                                 generation]
                                {
                                    if (safeThis != nullptr)
                                        safeThis->applyContextualHelpClearIfIdle(generation);
                                });
}

void PatchMutatorPanel::applyContextualHelpClearIfIdle(int generation)
{
    const bool anyActive = resolveActiveContextualHelpControl() != nullptr;
    if (! TSS::shouldClearContextualHelpOverlay(generation,
                                                contextualHelpClearGeneration_,
                                                anyActive))
    {
        if (generation == contextualHelpClearGeneration_ && anyActive)
            showContextualHelpFor(resolveActiveContextualHelpControl());
        return;
    }

    const bool focusInPopup =
        isFocusInsideMutatorPopupMenu(juce::Component::getCurrentlyFocusedComponent());
    if (TSS::shouldDeferContextualHelpClearForMutatorPopup(focusInPopup,
                                                           isMutatorPopupModalActive()))
    {
        scheduleContextualHelpClear();
        return;
    }

    if (auto* footer = resolveFooterPanel())
        footer->clearContextualHelpOverlay();
}

void PatchMutatorPanel::mouseEnter(const juce::MouseEvent& event)
{
    showContextualHelpFor(event.eventComponent);
}

void PatchMutatorPanel::mouseExit(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    scheduleContextualHelpClear();
}

void PatchMutatorPanel::globalFocusChanged(juce::Component* focusedComponent)
{
    if (! isShowing())
    {
        scheduleContextualHelpClear();
        return;
    }

    if (helpTextForControl(focusedComponent) != nullptr)
    {
        showContextualHelpFor(focusedComponent);
        return;
    }

    if (TSS::shouldDeferContextualHelpClearForMutatorPopup(
            isFocusInsideMutatorPopupMenu(focusedComponent),
            isMutatorPopupModalActive()))
    {
        return;
    }

    scheduleContextualHelpClear();
}
