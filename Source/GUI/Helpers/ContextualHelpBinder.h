#pragma once

#include <functional>
#include <map>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

class FooterPanel;

namespace TSS
{
    /**
     * Shared furtive left-footer HELP wiring: control → string map, mouse enter/exit,
     * keyboard focus, anti-flicker delayed clear. Display-only — never writes APVTS sticky.
     */
    class ContextualHelpBinder : private juce::MouseListener,
                                 private juce::FocusChangeListener
    {
    public:
        using FooterResolver = std::function<FooterPanel*()>;
        using PopupDeferPredicate = std::function<bool(juce::Component* focused)>;
        using HostShowingPredicate = std::function<bool()>;

        explicit ContextualHelpBinder(FooterResolver resolveFooter);
        ~ContextualHelpBinder() override;

        void bind(juce::Component* control, const char* helpText);
        void unbindAll();

        /** Optional: keep overlay while combo / logo popups hold focus or stay modal. */
        void setPopupDeferPredicate(PopupDeferPredicate predicate);

        /** Optional: when false, focus churn schedules clear (e.g. hidden Mutator tab). */
        void setHostShowingPredicate(HostShowingPredicate predicate);

        /** Show HELP without a bound control (logo popup rows, etc.). */
        void showHelpText(const char* helpText);
        void scheduleClear();

    private:
        void mouseEnter(const juce::MouseEvent& event) override;
        void mouseExit(const juce::MouseEvent& event) override;
        void globalFocusChanged(juce::Component* focusedComponent) override;

        const char* helpTextForControl(juce::Component* control) const;
        juce::Component* resolveActiveControl() const;
        void showHelpFor(juce::Component* control);
        void applyClearIfIdle(int generation);
        bool shouldDeferClear(juce::Component* focused) const;
        FooterPanel* resolveFooter() const;

        FooterResolver resolveFooter_;
        PopupDeferPredicate popupDeferPredicate_;
        HostShowingPredicate hostShowingPredicate_;
        std::map<juce::Component*, const char*> helpByControl_;
        int clearGeneration_ = 0;
        int lastShownEpoch_ = 0;
        std::shared_ptr<bool> aliveFlag_;

        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(ContextualHelpBinder)
    };

    /** True when focus is inside a known project popup menu, or such a menu is modal. */
    bool shouldDeferContextualHelpClearForProjectPopup(juce::Component* focused);
}
