#include "GettingStartedWizardDialog.h"

#include "Core/Services/EpromTypePolicy.h"
#include "GUI/Dialogs/GettingStartedWizardMetrics.h"
#include "GUI/Helpers/MidiPortComboPopulation.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Skins/Skin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

using GettingStartedWizard::Step;

void GettingStartedWizardDialog::buildStepControls(TSS::ISkin& skin)
{
    const auto comboStyle = TSS::ComboBox::Style::ButtonLike;
    // Dark-panel label colour — Matrix chrome body is always a dark plate.
    const auto labelLook = TSS::darkPanelLabelLookFromSkin(skin);
    const auto comboLook = TSS::comboBoxLookFromSkin(skin);
    const auto popupLook = TSS::popupMenuLookFromSkin(skin);

    auto makeLabel = [&](const char* text)
    {
        auto label = std::make_unique<TSS::Label>(kLabelWidth_, kControlHeight_, labelLook, text);
        addChildComponent(*label);
        return label;
    };
    auto makeCombo = [&]()
    {
        auto combo = std::make_unique<TSS::ComboBox>(
            kComboWidth_, kControlHeight_, comboLook, comboStyle);
        combo->setPopupMenuLook(popupLook);
        addChildComponent(*combo);
        return combo;
    };

    scaleLabel_ = makeLabel(PluginDisplayNames::Settings::kUiScaleRowLabel);
    scaleCombo_ = makeCombo();
    skinLabel_ = makeLabel(PluginDisplayNames::Settings::kSkinRowLabel);
    skinCombo_ = makeCombo();
    populateScaleAndSkinItems();

    midiFromLabel_ = makeLabel(PluginDisplayNames::Settings::kSynthFromLabel);
    midiFromCombo_ = makeCombo();
    midiToLabel_ = makeLabel(PluginDisplayNames::Settings::kSynthToLabel);
    midiToCombo_ = makeCombo();
    deviceLabel_ = makeLabel(PluginDisplayNames::FooterPanel::kDeviceLabel);
    deviceValueField_ = std::make_unique<TSS::ReadOnlyValueField>(skin);
    addChildComponent(*deviceValueField_);
    epromTypeLabel_ = makeLabel(PluginDisplayNames::Dialogs::EpromTypePrompt::kEpromTypeLabel);
    epromTypeCombo_ = makeCombo();

    keyboardFromLabel_ = makeLabel(PluginDisplayNames::Settings::kKeyboardFromLabel);
    keyboardFromCombo_ = makeCombo();
}

void GettingStartedWizardDialog::populateScaleAndSkinItems()
{
    using namespace PluginIDs::Settings::ScaleLevels;
    scaleCombo_->clear(juce::dontSendNotification);
    scaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k50, k50);
    scaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k75, k75);
    scaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k100, k100);
    scaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k125, k125);
    scaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k150, k150);
    scaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k175, k175);
    scaleCombo_->addItem(PluginDisplayNames::ChoiceLists::ScaleLevels::k200, k200);

    using namespace PluginIDs::Settings::SkinVariants;
    skinCombo_->clear(juce::dontSendNotification);
    skinCombo_->addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kBlack, kBlack);
    skinCombo_->addItem(PluginDisplayNames::ChoiceLists::SkinVariants::kCream, kCream);
}

void GettingStartedWizardDialog::applyControlLooks(TSS::ISkin& skin)
{
    const auto labelLook = TSS::darkPanelLabelLookFromSkin(skin);
    const auto comboLook = TSS::comboBoxLookFromSkin(skin);
    const auto popupLook = TSS::popupMenuLookFromSkin(skin);

    auto apply = [&](TSS::Label* label, TSS::ComboBox* combo)
    {
        if (label != nullptr)
            label->setLook(labelLook);
        if (combo != nullptr)
        {
            combo->setLook(comboLook);
            combo->setPopupMenuLook(popupLook);
        }
    };

    apply(scaleLabel_.get(), scaleCombo_.get());
    apply(skinLabel_.get(), skinCombo_.get());
    apply(midiFromLabel_.get(), midiFromCombo_.get());
    apply(midiToLabel_.get(), midiToCombo_.get());
    apply(deviceLabel_.get(), nullptr);
    if (deviceValueField_ != nullptr)
        deviceValueField_->setSkin(skin);
    apply(epromTypeLabel_.get(), epromTypeCombo_.get());
    apply(keyboardFromLabel_.get(), keyboardFromCombo_.get());
    if (audioPage_ != nullptr)
        audioPage_->setSkin(skin);
    refreshDeviceValueField();
}

void GettingStartedWizardDialog::wireControlCallbacks()
{
    wireAppearanceControlCallbacks();
    wireMidiControlCallbacks();
}

void GettingStartedWizardDialog::wireAppearanceControlCallbacks()
{
    scaleCombo_->onChange = [this]
    {
        if (suppressControlCallbacks_ || bindings_.onScaleChanged == nullptr)
            return;
        bindings_.onScaleChanged(scaleCombo_->getSelectedId());
    };
    skinCombo_->onChange = [this]
    {
        if (suppressControlCallbacks_ || bindings_.onSkinChanged == nullptr)
            return;
        bindings_.onSkinChanged(skinCombo_->getSelectedId());
    };
}

void GettingStartedWizardDialog::wireMidiControlCallbacks()
{
    midiFromCombo_->onChange = [this]
    {
        if (suppressControlCallbacks_ || bindings_.onMidiFromChanged == nullptr)
            return;
        bindings_.onMidiFromChanged(TSS::MidiPortComboPopulation::selectedPortId(
            *midiFromCombo_, midiFromPortIdentifiers_));
        recomputeDeviceRow();
    };
    midiToCombo_->onChange = [this]
    {
        if (suppressControlCallbacks_ || bindings_.onMidiToChanged == nullptr)
            return;
        bindings_.onMidiToChanged(TSS::MidiPortComboPopulation::selectedPortId(
            *midiToCombo_, midiToPortIdentifiers_));
        recomputeDeviceRow();
    };
    epromTypeCombo_->onChange = [this]
    {
        if (suppressControlCallbacks_)
            return;
        epromComboTouchedByUser_ = true;
        if (bindings_.onEpromChanged != nullptr)
            bindings_.onEpromChanged(epromTypeCombo_->getSelectedId());
    };
    keyboardFromCombo_->onChange = [this]
    {
        if (suppressControlCallbacks_ || bindings_.onKeyboardFromChanged == nullptr)
            return;
        bindings_.onKeyboardFromChanged(TSS::MidiPortComboPopulation::selectedPortId(
            *keyboardFromCombo_, keyboardFromPortIdentifiers_));
    };
}

void GettingStartedWizardDialog::ensureAudioPage()
{
    if (isPluginMode_ || bindings_.audioDeviceManager == nullptr || audioPage_ != nullptr)
        return;

    audioPage_ = std::make_unique<SettingsAudioPage>(SettingsAudioPage::Config{
        .skin = skin_,
        .deviceManager = bindings_.audioDeviceManager,
        .peakLevelProvider = [this]() -> float
        {
            return bindings_.peakLevelProvider != nullptr ? bindings_.peakLevelProvider() : 0.0f;
        },
        .onSynthFromChanged = [this]
        {
            if (bindings_.onSynthFromChanged == nullptr || audioPage_ == nullptr)
                return;
            bindings_.onSynthFromChanged(audioPage_->getSelectedSynthFromSourceId());
        },
    });
    addChildComponent(*audioPage_);
}

void GettingStartedWizardDialog::updateControlVisibility()
{
    const bool showUi = step_ == Step::kUserInterface;
    const bool showSynth = step_ == Step::kSynthCommunication;
    const bool showKeyboard = step_ == Step::kMidiKeyboard && ! isPluginMode_;
    const bool showAudio = step_ == Step::kAudio && ! isPluginMode_;

    auto setVisible = [](juce::Component* c, bool visible)
    {
        if (c != nullptr)
            c->setVisible(visible);
    };

    setVisible(scaleLabel_.get(), showUi);
    setVisible(scaleCombo_.get(), showUi);
    setVisible(skinLabel_.get(), showUi);
    setVisible(skinCombo_.get(), showUi);

    setVisible(midiFromLabel_.get(), showSynth);
    setVisible(midiFromCombo_.get(), showSynth);
    setVisible(midiToLabel_.get(), showSynth);
    setVisible(midiToCombo_.get(), showSynth);
    setVisible(deviceLabel_.get(), showSynth);
    setVisible(deviceValueField_.get(), showSynth);
    setVisible(epromTypeLabel_.get(), showSynth);
    setVisible(epromTypeCombo_.get(), showSynth);

    setVisible(keyboardFromLabel_.get(), showKeyboard);
    setVisible(keyboardFromCombo_.get(), showKeyboard);

    if (showAudio)
        ensureAudioPage();
    setVisible(audioPage_.get(), showAudio);

    if (showSynth)
        recomputeDeviceRow();
    else
        stopTimer();
}

juce::Rectangle<int> GettingStartedWizardDialog::controlBandBounds(
    const DialogMatrixHelpers::ModalGeometry& geometry) const
{
    namespace Metrics = GettingStartedWizardMetrics;
    const int rowsHeight = juce::roundToInt(
        static_cast<float>(Metrics::reservedControlBandDesignHeight(step_, isPluginMode_))
        * uiScale_);
    if (rowsHeight <= 0)
        return {};

    return geometry.band.withSizeKeepingCentre(geometry.band.getWidth(), rowsHeight);
}

void GettingStartedWizardDialog::placeControlRow(juce::Rectangle<int> band,
                                                 int rowIndex,
                                                 TSS::Label& label,
                                                 juce::Component& field)
{
    const int controlHeight = juce::roundToInt(static_cast<float>(kControlHeight_) * uiScale_);
    const int labelWidth = juce::roundToInt(static_cast<float>(kLabelWidth_) * uiScale_);
    const int comboWidth = juce::roundToInt(static_cast<float>(kComboWidth_) * uiScale_);
    const int rowGap = juce::roundToInt(static_cast<float>(kRowGap_) * uiScale_);
    const int rowWidth = labelWidth + comboWidth;
    const auto centred = band.withSizeKeepingCentre(rowWidth, band.getHeight());
    const int y = centred.getY() + rowIndex * (controlHeight + rowGap);

    label.setBounds(centred.getX(), y, labelWidth, controlHeight);
    label.setUiScale(uiScale_);
    field.setBounds(centred.getX() + labelWidth, y, comboWidth, controlHeight);
    if (auto* combo = dynamic_cast<TSS::ComboBox*>(&field))
        combo->setUiScale(uiScale_);
    else if (auto* value = dynamic_cast<TSS::ReadOnlyValueField*>(&field))
        value->setUiScale(uiScale_);
}

void GettingStartedWizardDialog::layoutStepControls(juce::Rectangle<int> band)
{
    if (band.isEmpty())
        return;

    if (step_ == Step::kUserInterface)
    {
        placeControlRow(band, 0, *scaleLabel_, *scaleCombo_);
        placeControlRow(band, 1, *skinLabel_, *skinCombo_);
        return;
    }

    if (step_ == Step::kSynthCommunication)
    {
        placeControlRow(band, 0, *midiFromLabel_, *midiFromCombo_);
        placeControlRow(band, 1, *midiToLabel_, *midiToCombo_);
        placeControlRow(band, 2, *deviceLabel_, *deviceValueField_);
        placeControlRow(band, 3, *epromTypeLabel_, *epromTypeCombo_);
        return;
    }

    if (step_ == Step::kMidiKeyboard && ! isPluginMode_)
    {
        placeControlRow(band, 0, *keyboardFromLabel_, *keyboardFromCombo_);
        return;
    }

    if (step_ == Step::kAudio && ! isPluginMode_ && audioPage_ != nullptr)
    {
        const int pageWidth = juce::roundToInt(
            static_cast<float>(SettingsShellMetrics::kLabelWidth
                               + SettingsShellMetrics::kControlColumnWidth)
            * uiScale_);
        audioPage_->setBounds(band.withSizeKeepingCentre(pageWidth, band.getHeight()));
        audioPage_->setUiScale(uiScale_);
    }
}

void GettingStartedWizardDialog::populateMidiPortLists()
{
    TSS::MidiPortComboPopulation::populateInputPortCombo(*midiFromCombo_, midiFromPortIdentifiers_);
    TSS::MidiPortComboPopulation::populateOutputPortCombo(*midiToCombo_, midiToPortIdentifiers_);
}

void GettingStartedWizardDialog::populateEpromItems(MatrixDeviceTypes::Type deviceType,
                                                    int preferredSelectedId)
{
    const auto family = Core::EpromTypePolicy::deviceFamilyFromType(deviceType);
    const int selectedId = Core::EpromTypePolicy::coerceForDeviceFamily(preferredSelectedId, family);
    const int markerId = Core::EpromTypePolicy::inquiryPopupMarkerId(
        liveStatus_.deviceDetected, liveStatus_.deviceVersion, family);
    const juce::ScopedValueSetter<bool> guard(suppressControlCallbacks_, true);
    epromTypeCombo_->clear(juce::dontSendNotification);
    Core::EpromTypePolicy::forEachValidItem(family, [this](int id)
    {
        epromTypeCombo_->addItem(Core::EpromTypePolicy::displayNameForId(id), id);
    });
    epromTypeCombo_->setPopupOnlyMarkedItem(markerId);
    epromTypeCombo_->setSelectedId(selectedId, juce::dontSendNotification);
}

void GettingStartedWizardDialog::syncPortsFromHost(const juce::String& midiFromPortId,
                                                   const juce::String& midiToPortId,
                                                   bool repopulateLists)
{
    const juce::ScopedValueSetter<bool> guard(suppressControlCallbacks_, true);
    if (repopulateLists)
        populateMidiPortLists();

    TSS::MidiPortComboPopulation::selectPortInCombo(
        *midiFromCombo_, midiFromPortIdentifiers_, midiFromPortId);
    TSS::MidiPortComboPopulation::selectPortInCombo(
        *midiToCombo_, midiToPortIdentifiers_, midiToPortId);
    recomputeDeviceRow();
}

void GettingStartedWizardDialog::syncKeyboardFromHost(const juce::String& keyboardFromPortId,
                                                      bool repopulateLists)
{
    if (keyboardFromCombo_ == nullptr)
        return;

    const juce::ScopedValueSetter<bool> guard(suppressControlCallbacks_, true);
    if (repopulateLists)
        TSS::MidiPortComboPopulation::populateInputPortCombo(*keyboardFromCombo_,
                                                             keyboardFromPortIdentifiers_);
    TSS::MidiPortComboPopulation::selectPortInCombo(
        *keyboardFromCombo_, keyboardFromPortIdentifiers_, keyboardFromPortId);
}

