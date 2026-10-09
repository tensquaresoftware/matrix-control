#include "GettingStartedWizardDialog.h"

#include <algorithm>

#include "GUI/Dialogs/GettingStartedWizardMetrics.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
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

    buildStepControls(skin);
    wireControlCallbacks();
    showStep(Step::kIntro);
}

GettingStartedWizardDialog::~GettingStartedWizardDialog()
{
    stopTimer();
}

TSS::Button& GettingStartedWizardDialog::buttonFor(NavButton button) const
{
    return *buttons_[static_cast<size_t>(button)];
}

void GettingStartedWizardDialog::prepareForShow(Step startStep, HostBindings bindings)
{
    bindings_ = std::move(bindings);
    includeFirmwareSuggestionHint_ = bindings_.includeFirmwareSuggestionHint;
    useAudioResumeCopy_ = bindings_.useAudioResumeCopy;
    liveStatus_ = bindings_.deviceStatus;
    searchingWindow_ = {};
    searchingDotFrame_ = 0;
    epromComboTouchedByUser_ = false;
    stopTimer();

    {
        const juce::ScopedValueSetter<bool> guard(suppressControlCallbacks_, true);
        if (scaleCombo_ != nullptr)
            scaleCombo_->setSelectedId(bindings_.scaleId, juce::dontSendNotification);
        if (skinCombo_ != nullptr)
            skinCombo_->setSelectedId(bindings_.skinId, juce::dontSendNotification);
    }

    syncPortsFromHost(bindings_.midiFromPortId, bindings_.midiToPortId, true);
    populateEpromItems(liveStatus_.deviceType, bindings_.preferredEpromTypeId);

    if (keyboardFromCombo_ != nullptr)
    {
        const juce::ScopedValueSetter<bool> guard(suppressControlCallbacks_, true);
        TSS::MidiPortComboPopulation::populateInputPortCombo(*keyboardFromCombo_,
                                                             keyboardFromPortIdentifiers_);
        TSS::MidiPortComboPopulation::selectPortInCombo(
            *keyboardFromCombo_, keyboardFromPortIdentifiers_, bindings_.keyboardFromPortId);
    }

    ensureAudioPage();
    populateSynthFromChannels(bindings_.synthFromChannelNames,
                              bindings_.synthFromChannelIds,
                              bindings_.selectedSynthFromSourceId);

    showStep(startStep);
}

void GettingStartedWizardDialog::stopLiveTimers()
{
    stopTimer();
    searchingDotFrame_ = 0;
    if (audioPage_ != nullptr)
        audioPage_->setMonitoringActive(false);
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

    if (step_ == Step::kSynthCommunication)
    {
        const int preferred = bindings_.resolvePreferredEpromTypeId != nullptr
            ? bindings_.resolvePreferredEpromTypeId()
            : bindings_.preferredEpromTypeId;
        refreshEpromSuggestion(liveStatus_.deviceType, preferred);
    }

    if (step_ == Step::kAudio && ! isPluginMode_)
    {
        ensureAudioPage();
        if (bindings_.refreshSynthFromCatalog != nullptr)
            bindings_.refreshSynthFromCatalog();
        else
            populateSynthFromChannels(bindings_.synthFromChannelNames,
                                      bindings_.synthFromChannelIds,
                                      bindings_.selectedSynthFromSourceId);
    }

    updateControlVisibility();
    resized();
    repaint();
}

void GettingStartedWizardDialog::handleButton(NavButton button)
{
    if (button == NavButton::kConfigureLater)
    {
        if (bindings_.onConfigureLater)
            bindings_.onConfigureLater();
        requestDismiss();
        return;
    }

    if (button == NavButton::kContinue && step_ == Step::kIntro && bindings_.onContinuedFromIntro)
        bindings_.onContinuedFromIntro();

    if (GettingStartedWizard::marksStepDone(step_, button) && bindings_.onContentStepCompleted)
        bindings_.onContentStepCompleted(step_);

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
    stopTimer();
    if (onDismissRequested_)
        onDismissRequested_();
}

void GettingStartedWizardDialog::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    for (const auto navButton : kAllNavButtons)
        DialogMatrixHelpers::applyButtonSkin(buttonFor(navButton), skin);

    applyControlLooks(skin);
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
    const auto body = bodyText();
    const int bodyWidth = DialogMatrixHelpers::bodyTextWidthFor(
        DialogMatrixHelpers::contentWidthFor(Metrics::kDesignWidth, uiScale_));
    // Use measured text height (not the per-step planning floor). Short variants must not invent
    // a second blank below the copy; the single gap before controls is kGapBeforeButtons (24 px).
    // Do not clamp to maxBodyDesignHeightBelowSettings: STEP 2 with the firmware-suggestion
    // suffix is the tallest body and that ceiling (~91 px) squeezes fitted text, which visually
    // shortens the gap above SYNTH FROM compared with other steps.
    const int measuredBody = DialogMatrixHelpers::measureBodyHeight(
        DialogMatrixHelpers::scaledModalBodyFont(*skin_, uiScale_), body, bodyWidth);
    const int bodyHeight = juce::jmax(measuredBody, 1);

    return DialogMatrixHelpers::computeModalGeometry({
        .hostBounds = getLocalBounds(),
        .designWidth = Metrics::kDesignWidth,
        .uiScale = uiScale_,
        .bodyHeight = bodyHeight,
        .extraBandHeight = scaledDesign(
            Metrics::reservedControlBandDesignHeight(step_, isPluginMode_), uiScale_),
    });
}

juce::String GettingStartedWizardDialog::bodyText() const
{
    return GettingStartedWizard::bodyFor(step_, isPluginMode_, {
        .includeFirmwareSuggestionSuffix = includeFirmwareSuggestionHint_
            && step_ == Step::kSynthCommunication,
        .useAudioResumeCopy = useAudioResumeCopy_ && step_ == Step::kAudio,
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
                                       bodyText(),
                                       geometry.textArea);
}

void GettingStartedWizardDialog::resized()
{
    const auto geometry = computeGeometry();

    std::vector<DialogMatrixHelpers::ButtonPlacement> placements;
    for (const auto navButton : GettingStartedWizard::buttonsFor(step_, isPluginMode_))
    {
        placements.push_back({
            &buttonFor(navButton),
            DialogMatrixHelpers::estimateButtonWidth(
                *skin_, GettingStartedWizard::labelFor(navButton), uiScale_),
        });
    }

    DialogMatrixHelpers::layoutCentredButtonRow(geometry.buttonRow, uiScale_, placements);
    layoutStepControls(controlBandBounds(geometry));
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
