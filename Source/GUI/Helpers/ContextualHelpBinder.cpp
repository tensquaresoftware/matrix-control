#include "GUI/Helpers/ContextualHelpBinder.h"

#include "GUI/Helpers/ContextualHelpOverlay.h"
#include "GUI/Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "GUI/Widgets/HeaderLogoPopupMenu.h"
#include "GUI/Widgets/HierarchicalPopupMenu.h"
#include "GUI/Widgets/MultiColumnPopupMenu.h"
#include "GUI/Widgets/ScrollablePopupMenu.h"

namespace
{
    constexpr int kContextualHelpClearDelayMs = 75;

    bool isProjectPopupComponent(const juce::Component* component)
    {
        return dynamic_cast<const TSS::HierarchicalPopupMenu*>(component) != nullptr
            || dynamic_cast<const TSS::MultiColumnPopupMenu*>(component) != nullptr
            || dynamic_cast<const TSS::ScrollablePopupMenu*>(component) != nullptr
            || dynamic_cast<const TSS::HeaderLogoPopupMenu*>(component) != nullptr;
    }

    bool isFocusInsideProjectPopup(const juce::Component* focused)
    {
        for (auto* c = focused; c != nullptr; c = c->getParentComponent())
        {
            if (isProjectPopupComponent(c))
                return true;
        }

        return false;
    }

    bool isProjectPopupModalActive()
    {
        for (int i = 0; i < juce::Component::getNumCurrentlyModalComponents(); ++i)
        {
            if (isProjectPopupComponent(juce::Component::getCurrentlyModalComponent(i)))
                return true;
        }

        return false;
    }
}

namespace TSS
{
    bool shouldDeferContextualHelpClearForProjectPopup(juce::Component* focused)
    {
        return shouldDeferContextualHelpClearForMutatorPopup(isFocusInsideProjectPopup(focused),
                                                             isProjectPopupModalActive());
    }

    ContextualHelpBinder::ContextualHelpBinder(FooterResolver resolveFooter)
        : resolveFooter_(std::move(resolveFooter))
    {
        jassert(resolveFooter_ != nullptr);
        popupDeferPredicate_ = [](juce::Component* focused)
        {
            return shouldDeferContextualHelpClearForProjectPopup(focused);
        };
        aliveFlag_ = std::make_shared<bool>(true);
        juce::Desktop::getInstance().addFocusChangeListener(this);
    }

    ContextualHelpBinder::~ContextualHelpBinder()
    {
        if (aliveFlag_ != nullptr)
            *aliveFlag_ = false;

        unbindAll();
        juce::Desktop::getInstance().removeFocusChangeListener(this);
    }

    void ContextualHelpBinder::bind(juce::Component* control, const char* helpText)
    {
        if (control == nullptr || helpText == nullptr)
            return;

        const bool alreadyBound = helpByControl_.find(control) != helpByControl_.end();
        helpByControl_[control] = helpText;

        if (! alreadyBound)
            control->addMouseListener(this, true);
    }

    void ContextualHelpBinder::unbindAll()
    {
        for (const auto& entry : helpByControl_)
        {
            if (entry.first != nullptr)
                entry.first->removeMouseListener(this);
        }

        helpByControl_.clear();
        ++clearGeneration_;

        if (auto* footer = resolveFooter())
            footer->clearContextualHelpOverlayIfEpoch(lastShownEpoch_);

        lastShownEpoch_ = 0;
    }

    void ContextualHelpBinder::setPopupDeferPredicate(PopupDeferPredicate predicate)
    {
        popupDeferPredicate_ = std::move(predicate);
    }

    void ContextualHelpBinder::setHostShowingPredicate(HostShowingPredicate predicate)
    {
        hostShowingPredicate_ = std::move(predicate);
    }

    void ContextualHelpBinder::showHelpText(const char* helpText)
    {
        if (helpText == nullptr)
            return;

        ++clearGeneration_;

        if (auto* footer = resolveFooter())
            lastShownEpoch_ = footer->setContextualHelpOverlay(helpText);
    }

    FooterPanel* ContextualHelpBinder::resolveFooter() const
    {
        return resolveFooter_ != nullptr ? resolveFooter_() : nullptr;
    }

    const char* ContextualHelpBinder::helpTextForControl(juce::Component* control) const
    {
        for (auto* c = control; c != nullptr; c = c->getParentComponent())
        {
            const auto it = helpByControl_.find(c);
            if (it != helpByControl_.end())
                return it->second;
        }

        return nullptr;
    }

    juce::Component* ContextualHelpBinder::resolveActiveControl() const
    {
        for (const auto& entry : helpByControl_)
        {
            auto* control = entry.first;
            if (control != nullptr && control->hasKeyboardFocus(true))
                return control;
        }

        for (const auto& entry : helpByControl_)
        {
            auto* control = entry.first;
            if (control != nullptr && control->isMouseOver(true))
                return control;
        }

        return nullptr;
    }

    void ContextualHelpBinder::showHelpFor(juce::Component* control)
    {
        const auto* helpText = helpTextForControl(control);
        if (helpText == nullptr)
            return;

        ++clearGeneration_;

        if (auto* footer = resolveFooter())
            lastShownEpoch_ = footer->setContextualHelpOverlay(helpText);
    }

    void ContextualHelpBinder::scheduleClear()
    {
        const int generation = ++clearGeneration_;
        const auto alive = aliveFlag_;
        juce::Timer::callAfterDelay(kContextualHelpClearDelayMs,
                                    [alive, binder = this, generation]
                                    {
                                        if (alive == nullptr || ! *alive)
                                            return;

                                        binder->applyClearIfIdle(generation);
                                    });
    }

    void ContextualHelpBinder::applyClearIfIdle(int generation)
    {
        const bool anyActive = resolveActiveControl() != nullptr;
        if (! shouldClearContextualHelpOverlay(generation, clearGeneration_, anyActive))
        {
            if (generation == clearGeneration_ && anyActive)
                showHelpFor(resolveActiveControl());
            return;
        }

        if (shouldDeferClear(juce::Component::getCurrentlyFocusedComponent()))
        {
            scheduleClear();
            return;
        }

        if (auto* footer = resolveFooter())
            footer->clearContextualHelpOverlayIfEpoch(lastShownEpoch_);
    }

    bool ContextualHelpBinder::shouldDeferClear(juce::Component* focused) const
    {
        return popupDeferPredicate_ != nullptr && popupDeferPredicate_(focused);
    }

    void ContextualHelpBinder::mouseEnter(const juce::MouseEvent& event)
    {
        showHelpFor(event.eventComponent);
    }

    void ContextualHelpBinder::mouseExit(const juce::MouseEvent& event)
    {
        juce::ignoreUnused(event);
        scheduleClear();
    }

    void ContextualHelpBinder::globalFocusChanged(juce::Component* focusedComponent)
    {
        if (hostShowingPredicate_ != nullptr && ! hostShowingPredicate_())
        {
            scheduleClear();
            return;
        }

        if (helpTextForControl(focusedComponent) != nullptr)
        {
            showHelpFor(focusedComponent);
            return;
        }

        if (shouldDeferClear(focusedComponent))
            return;

        scheduleClear();
    }
}
