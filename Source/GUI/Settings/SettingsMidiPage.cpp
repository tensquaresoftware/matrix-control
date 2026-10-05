#include "SettingsMidiPage.h"

#include "GUI/Helpers/MidiActivityLedLevels.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

SettingsMidiPage::SettingsMidiPage(Config config)
    : config_(std::move(config))
    , skin_(config_.skin)
{
    jassert(config_.skin != nullptr);

    setOpaque(false);
    buildWidgets();
    wirePopupRefresh();
    updateKeyboardFromVisibility();
    refreshPortLists();
    setMonitoringActive(true);
}

SettingsMidiPage::~SettingsMidiPage()
{
    setMonitoringActive(false);
}

void SettingsMidiPage::buildWidgets()
{
    keyboardFromLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kKeyboardFromLabel);
    keyboardFromCombo_ = makeCombo(*skin_);
    keyboardFromLed_ = std::make_unique<TSS::Led>(SettingsShellMetrics::kPeakWidth,
                                                  SettingsShellMetrics::kPeakWidth);
    keyboardFromLed_->setSkin(*skin_);
    addAndMakeVisible(*keyboardFromLed_);

    synthFromLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kSynthFromLabel);
    synthFromCombo_ = makeCombo(*skin_);
    synthFromLed_ = std::make_unique<TSS::Led>(SettingsShellMetrics::kPeakWidth,
                                               SettingsShellMetrics::kPeakWidth);
    synthFromLed_->setSkin(*skin_);
    addAndMakeVisible(*synthFromLed_);

    synthToLabel_ = makeLabel(*skin_, PluginDisplayNames::Settings::kSynthToLabel);
    synthToCombo_ = makeCombo(*skin_);
    synthToLed_ = std::make_unique<TSS::Led>(SettingsShellMetrics::kPeakWidth,
                                             SettingsShellMetrics::kPeakWidth);
    synthToLed_->setSkin(*skin_);
    addAndMakeVisible(*synthToLed_);
}

void SettingsMidiPage::wirePopupRefresh()
{
    const auto refreshBeforeOpen = [this]
    {
        if (config_.onPortListsRefreshRequested)
            config_.onPortListsRefreshRequested();
        else
            refreshPortLists();
    };

    if (keyboardFromCombo_ != nullptr)
        keyboardFromCombo_->onAboutToShowPopup = refreshBeforeOpen;
    synthFromCombo_->onAboutToShowPopup = refreshBeforeOpen;
    synthToCombo_->onAboutToShowPopup = refreshBeforeOpen;
}

void SettingsMidiPage::updateKeyboardFromVisibility()
{
    const bool showKeyboard = SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode);
    keyboardFromLabel_->setVisible(showKeyboard);
    keyboardFromCombo_->setVisible(showKeyboard);
    keyboardFromLed_->setVisible(showKeyboard);
}

void SettingsMidiPage::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    keyboardFromLabel_->setLook(TSS::darkPanelLabelLookFromSkin(skin));
    keyboardFromCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    keyboardFromCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    keyboardFromLed_->setSkin(skin);
    synthFromLabel_->setLook(TSS::darkPanelLabelLookFromSkin(skin));
    synthFromCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    synthFromCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    synthFromLed_->setSkin(skin);
    synthToLabel_->setLook(TSS::darkPanelLabelLookFromSkin(skin));
    synthToCombo_->setLook(TSS::comboBoxLookFromSkin(skin));
    synthToCombo_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    synthToLed_->setSkin(skin);
}

void SettingsMidiPage::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
}

void SettingsMidiPage::setVisible(bool shouldBeVisible)
{
    Component::setVisible(shouldBeVisible);
    setMonitoringActive(shouldBeVisible);
}

void SettingsMidiPage::setMonitoringActive(bool shouldBeActive)
{
    if (shouldBeActive == monitoringActive_)
        return;

    monitoringActive_ = shouldBeActive;
    if (monitoringActive_)
        startTimerHz(30);
    else
        stopTimer();
}

void SettingsMidiPage::timerCallback()
{
    if (config_.activityTrackerProvider == nullptr)
        return;

    TSS::MidiActivityLedLevels::apply(config_.activityTrackerProvider(),
                                      *keyboardFromLed_,
                                      *synthFromLed_,
                                      *synthToLed_);
}

void SettingsMidiPage::populatePortLists(const juce::String& keepOpenSynthFromId,
                                         const juce::String& keepOpenSynthToId,
                                         const juce::String& keepOpenKeyboardFromId)
{
    TSS::MidiPortComboPopulation::populateInputPortCombo(
        *synthFromCombo_, synthFromPortIdentifiers_, keepOpenSynthFromId);
    TSS::MidiPortComboPopulation::populateOutputPortCombo(
        *synthToCombo_, synthToPortIdentifiers_, keepOpenSynthToId);

    if (SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode)
        && keyboardFromCombo_ != nullptr)
    {
        TSS::MidiPortComboPopulation::populateInputPortCombo(
            *keyboardFromCombo_, keyboardFromPortIdentifiers_, keepOpenKeyboardFromId);
    }
}

juce::String SettingsMidiPage::getSelectedSynthFromPortId() const
{
    return TSS::MidiPortComboPopulation::selectedPortId(*synthFromCombo_, synthFromPortIdentifiers_);
}

juce::String SettingsMidiPage::getSelectedSynthToPortId() const
{
    return TSS::MidiPortComboPopulation::selectedPortId(*synthToCombo_, synthToPortIdentifiers_);
}

juce::String SettingsMidiPage::getSelectedKeyboardFromPortId() const
{
    if (! SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode)
        || keyboardFromCombo_ == nullptr)
        return {};

    return TSS::MidiPortComboPopulation::selectedPortId(*keyboardFromCombo_,
                                                        keyboardFromPortIdentifiers_);
}

void SettingsMidiPage::selectSynthFromPort(const juce::String& portId)
{
    TSS::MidiPortComboPopulation::selectPortInCombo(
        *synthFromCombo_, synthFromPortIdentifiers_, portId);
}

void SettingsMidiPage::selectSynthToPort(const juce::String& portId)
{
    TSS::MidiPortComboPopulation::selectPortInCombo(
        *synthToCombo_, synthToPortIdentifiers_, portId);
}

void SettingsMidiPage::selectKeyboardFromPort(const juce::String& portId)
{
    if (! SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode)
        || keyboardFromCombo_ == nullptr)
        return;

    TSS::MidiPortComboPopulation::selectPortInCombo(
        *keyboardFromCombo_, keyboardFromPortIdentifiers_, portId);
}

bool SettingsMidiPage::isAnyPortPopupOpen() const noexcept
{
    if (synthFromCombo_->isPopupOpen() || synthToCombo_->isPopupOpen())
        return true;

    return keyboardFromCombo_ != nullptr && keyboardFromCombo_->isPopupOpen();
}

void SettingsMidiPage::registerContextualHelp(TSS::ContextualHelpBinder& binder)
{
    namespace Help = PluginDisplayNames::Settings::ContextualHelp;

    if (SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode)
        && keyboardFromCombo_ != nullptr)
    {
        binder.bind(keyboardFromCombo_.get(), Help::kKeyboardFrom);
        binder.bind(keyboardFromLed_.get(), Help::kKeyboardFromActivityLed);
    }

    binder.bind(synthFromCombo_.get(), Help::kMidiSynthFrom);
    binder.bind(synthFromLed_.get(), Help::kMidiSynthFromActivityLed);
    binder.bind(synthToCombo_.get(), Help::kMidiSynthTo);
    binder.bind(synthToLed_.get(), Help::kMidiSynthToActivityLed);
}

void SettingsMidiPage::paint(juce::Graphics&)
{
}

void SettingsMidiPage::resized()
{
    const int rowGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kRowGap), uiScale_);
    const int labelWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kLabelWidth), uiScale_);
    const int controlHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kControlHeight), uiScale_);
    const int comboWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kSynthFromComboWidth), uiScale_);
    const int ledWidth = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPeakWidth), uiScale_);
    const int ledGap = TSS::ScaledLayout::scaledInt(
        static_cast<float>(SettingsShellMetrics::kPeakGap), uiScale_);

    auto bounds = getLocalBounds();

    const auto placeRow = [&](TSS::Label& label, TSS::ComboBox& combo, TSS::Led& led, bool visible)
    {
        if (! visible)
            return;

        auto row = bounds.removeFromTop(controlHeight);
        label.setBounds(row.getX(), row.getY(), labelWidth, controlHeight);
        label.setUiScale(uiScale_);
        const int controlX = row.getX() + labelWidth;
        combo.setBounds(controlX, row.getY(), comboWidth, controlHeight);
        combo.setUiScale(uiScale_);
        const int ledY = row.getY() + (controlHeight - ledWidth) / 2;
        led.setBounds(controlX + comboWidth + ledGap, ledY, ledWidth, ledWidth);
        led.setUiScale(uiScale_);
        bounds.removeFromTop(rowGap);
    };

    placeRow(*keyboardFromLabel_,
             *keyboardFromCombo_,
             *keyboardFromLed_,
             SettingsShellMetrics::showsKeyboardFromRow(config_.isPluginMode));
    placeRow(*synthFromLabel_, *synthFromCombo_, *synthFromLed_, true);
    placeRow(*synthToLabel_, *synthToCombo_, *synthToLed_, true);
}

std::unique_ptr<TSS::Label> SettingsMidiPage::makeLabel(TSS::ISkin& skin, const juce::String& text)
{
    auto label = std::make_unique<TSS::Label>(SettingsShellMetrics::kLabelWidth,
                                              SettingsShellMetrics::kControlHeight,
                                              TSS::darkPanelLabelLookFromSkin(skin),
                                              text);
    addAndMakeVisible(*label);
    return label;
}

std::unique_ptr<TSS::ComboBox> SettingsMidiPage::makeCombo(TSS::ISkin& skin)
{
    auto combo = std::make_unique<TSS::ComboBox>(SettingsShellMetrics::kSynthFromComboWidth,
                                                 SettingsShellMetrics::kControlHeight,
                                                 TSS::comboBoxLookFromSkin(skin),
                                                 TSS::ComboBox::Style::ButtonLike);
    combo->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    combo->setUsesPortSentinelPopupChrome(true);
    addAndMakeVisible(*combo);
    return combo;
}
