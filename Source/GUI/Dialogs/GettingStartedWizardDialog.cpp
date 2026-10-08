#include "GettingStartedWizardDialog.h"

#include <algorithm>

#include "GUI/Dialogs/GettingStartedWizardMetrics.h"
#include "GUI/Skins/Skin.h"

using GettingStartedWizard::NavButton;
using GettingStartedWizard::Step;

namespace
{
    constexpr NavButton kAllNavButtons[] = {
        NavButton::kConfigureLater, NavButton::kContinue, NavButton::kPrevious,
        NavButton::kNext,           NavButton::kSkip,     NavButton::kFinish,
    };

    int scaledDesign(int designValue, float uiScale)
    {
        return juce::roundToInt(static_cast<float>(designValue) * uiScale);
    }
}

GettingStartedWizardDialog::GettingStartedWizardDialog(TSS::ISkin& skin,
                                                       bool isPluginMode,
                                                       std::function<void()> onDismissRequested)
    : onDismissRequested_(std::move(onDismissRequested))
    , skin_(&skin)
    , isPluginMode_(isPluginMode)
{
    setOpaque(false);
    setInterceptsMouseClicks(true, true);
    setWantsKeyboardFocus(true);

    for (const auto navButton : kAllNavButtons)
    {
        auto button = DialogMatrixHelpers::makeButton(
            skin,
            DialogMatrixHelpers::kDefaultButtonWidth,
            GettingStartedWizard::labelFor(navButton));
        button->setWantsKeyboardFocus(false);
        button->onClick = [this, navButton] { handleButton(navButton); };
        addChildComponent(*button);
        buttons_[static_cast<size_t>(navButton)] = std::move(button);
    }

    showStep(Step::kIntro);
}

TSS::Button& GettingStartedWizardDialog::buttonFor(NavButton button) const
{
    return *buttons_[static_cast<size_t>(button)];
}

void GettingStartedWizardDialog::prepareForShow(Step startStep)
{
    showStep(startStep);
}

void GettingStartedWizardDialog::showStep(Step step)
{
    step_ = GettingStartedWizard::coerceToApplicableStep(step, isPluginMode_);

    const auto shown = GettingStartedWizard::buttonsFor(step_, isPluginMode_);
    for (const auto navButton : kAllNavButtons)
    {
        const bool isShown = std::find(shown.begin(), shown.end(), navButton) != shown.end();
        buttonFor(navButton).setVisible(isShown);
    }

    resized();
    repaint();
}

void GettingStartedWizardDialog::handleButton(NavButton button)
{
    if (GettingStartedWizard::closesWizard(button))
    {
        requestDismiss();
        return;
    }

    const auto target = GettingStartedWizard::targetStepForNavButton(step_, button, isPluginMode_);
    if (target.has_value())
        showStep(*target);
}

void GettingStartedWizardDialog::requestDismiss()
{
    if (onDismissRequested_)
        onDismissRequested_();
}

void GettingStartedWizardDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    for (const auto navButton : kAllNavButtons)
        DialogMatrixHelpers::applyButtonSkin(buttonFor(navButton), skin);

    resized();
    repaint();
}

void GettingStartedWizardDialog::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    for (const auto navButton : kAllNavButtons)
        DialogMatrixHelpers::applyButtonUiScale(buttonFor(navButton), uiScale);

    resized();
    repaint();
}

DialogMatrixHelpers::ModalGeometry GettingStartedWizardDialog::computeGeometry() const
{
    namespace Metrics = GettingStartedWizardMetrics;
    return DialogMatrixHelpers::computeModalGeometry({
        .hostBounds = getLocalBounds(),
        .designWidth = Metrics::kDesignWidth,
        .uiScale = uiScale_,
        .bodyHeight = scaledDesign(Metrics::bodyDesignHeight(step_, isPluginMode_), uiScale_),
        .extraBandHeight = scaledDesign(
            Metrics::reservedControlBandDesignHeight(step_, isPluginMode_), uiScale_),
    });
}

void GettingStartedWizardDialog::paint(juce::Graphics& g)
{
    const auto geometry = computeGeometry();

    DialogMatrixHelpers::paintMatrixOverlayChrome({
        .g = g,
        .skin = *skin_,
        .dialogBounds = geometry.dialogBounds,
        .borderThickness = geometry.border,
        .titleBarHeight = geometry.titleBarHeight,
        .title = GettingStartedWizard::titleFor(step_),
        .uiScale = uiScale_,
    });

    g.setColour(skin_->getColour(TSS::SkinColourId::kDarkPanelText));
    DialogMatrixHelpers::paintBodyText(g,
                                       DialogMatrixHelpers::scaledModalBodyFont(*skin_, uiScale_),
                                       GettingStartedWizard::bodyFor(step_, isPluginMode_),
                                       geometry.textArea);
}

void GettingStartedWizardDialog::resized()
{
    std::vector<DialogMatrixHelpers::ButtonPlacement> placements;
    for (const auto navButton : GettingStartedWizard::buttonsFor(step_, isPluginMode_))
    {
        placements.push_back({
            &buttonFor(navButton),
            DialogMatrixHelpers::estimateButtonWidth(
                *skin_, GettingStartedWizard::labelFor(navButton), uiScale_),
        });
    }

    DialogMatrixHelpers::layoutCentredButtonRow(computeGeometry().buttonRow, uiScale_, placements);
}

void GettingStartedWizardDialog::mouseDown(const juce::MouseEvent& e)
{
    if (! computeGeometry().dialogBounds.contains(e.getPosition()))
        requestDismiss();
}

bool GettingStartedWizardDialog::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey)
    {
        requestDismiss();
        return true;
    }

    if (key == juce::KeyPress::returnKey)
    {
        handleButton(GettingStartedWizard::primaryButtonFor(step_, isPluginMode_));
        return true;
    }

    return Component::keyPressed(key);
}
