#pragma once

#include <array>
#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Dialogs/GettingStartedWizardFlow.h"
#include "GUI/Widgets/Button.h"

namespace TSS
{
    class ISkin;
}

/** GETTING STARTED wizard shell (GS-2): Matrix overlay chrome, step title in the title band,
    frozen help copy, and Previous / Next style navigation over the format-applicable steps.
    Step control bodies, durable flags and auto-open policy are GS-3. */
class GettingStartedWizardDialog : public juce::Component
{
public:
    GettingStartedWizardDialog(TSS::ISkin& skin,
                               bool isPluginMode,
                               std::function<void()> onDismissRequested);

    /** Shows `startStep` (coerced to an applicable step). */
    void prepareForShow(GettingStartedWizard::Step startStep);

    GettingStartedWizard::Step getCurrentStep() const noexcept { return step_; }

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    static constexpr size_t kNavButtonCount = 6;

    DialogMatrixHelpers::ModalGeometry computeGeometry() const;
    TSS::Button& buttonFor(GettingStartedWizard::NavButton button) const;
    void showStep(GettingStartedWizard::Step step);
    void handleButton(GettingStartedWizard::NavButton button);
    void requestDismiss();

    std::function<void()> onDismissRequested_;
    TSS::ISkin* skin_;
    bool isPluginMode_;
    float uiScale_ = 1.0f;
    GettingStartedWizard::Step step_ = GettingStartedWizard::Step::kIntro;
    std::array<std::unique_ptr<TSS::Button>, kNavButtonCount> buttons_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(GettingStartedWizardDialog)
};
