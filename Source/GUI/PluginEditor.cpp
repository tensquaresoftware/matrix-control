#include "PluginEditor.h"

#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/PluginProcessor.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "GUI/Settings/SettingsPanel.h"
#include "GUI/Settings/SettingsWindow.h"
#include "Skins/Skin.h"
#include "Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"

using TSS::SkinColourId;

class PluginEditor::HeaderRefreshTimer : private juce::Timer
{
public:
    HeaderRefreshTimer(PluginProcessor& processor, HeaderPanel& headerPanel, PluginEditor& owner)
        : processor_(processor)
        , headerPanel_(headerPanel)
        , owner_(owner)
    {
        startTimerHz(30);
    }

private:
    void timerCallback() override
    {
        if (processor_.isStandalone())
        {
            if (audioFromRefreshAttempts_ < 60)
            {
                const auto names = processor_.getAudioInputSourceNames();

                if (names.isEmpty())
                {
                    ++audioFromRefreshAttempts_;
                    owner_.refreshAudioFromCombo();
                }
            }

            if (auto* panel = owner_.getSettingsPanelIfOpen())
            {
                panel->getPeakIndicator().setLevel(
                    processor_.getAudioPassthroughProcessor().getPeakLevel());
            }
        }

        const auto& tracker = processor_.getMidiActivityTracker();
        headerPanel_.getInstrumentActivityLed().setLevel(
            tracker.getActivityLevel(Core::MidiActivityTracker::Path::kInstrument));
        headerPanel_.getEditorActivityLed().setLevel(
            tracker.getActivityLevel(Core::MidiActivityTracker::Path::kMidiFromInbound));
        headerPanel_.getMidiToActivityLed().setLevel(
            tracker.getActivityLevel(Core::MidiActivityTracker::Path::kOutbound));
    }

    PluginProcessor& processor_;
    HeaderPanel& headerPanel_;
    PluginEditor& owner_;
    int audioFromRefreshAttempts_ = 0;
};


PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , pluginProcessor(p)
{
    skinBlack_ = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
    skinCream_ = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);

    const int savedSkinId = pluginProcessor.getSkinVariantId();
    skin_ = (savedSkinId == PluginIDs::Settings::SkinVariants::kCream)
        ? skinCream_.get()
        : skinBlack_.get();

    widgetFactory_ = std::make_unique<WidgetFactory>(pluginProcessor.getApvts());
    layoutDimensions_ = WidgetFactory::buildGuiLayoutDimensions();
    TSS::ComboBox::setPopupLayoutDimensions(layoutDimensions_.popupMenu);

    setOpaque(true);
    setWantsKeyboardFocus(false);
    setInterceptsMouseClicks(true, true);

    const auto editorWidth = layoutDimensions_.editor.width;
    const auto editorHeight = layoutDimensions_.editor.height;

    mainComponent_ = std::make_unique<MainComponent>(
        *skin_, layoutDimensions_, *widgetFactory_, pluginProcessor.getApvts());
    addAndMakeVisible(*mainComponent_);

    mainComponent_->setBusReorderHandler(
        [this](int fromBus, int toBus)
        {
            pluginProcessor.swapMatrixModBusContents(fromBus, toBus);
        });

    testComponent_ = std::make_unique<TestComponent>(
        *skin_,
        pluginProcessor.getApvts(),
        pluginProcessor.getApvts().state,
        editorWidth,
        editorHeight);
    addChildComponent(*testComponent_);
    testComponent_->setVisible(false);

    updateSkin();

    auto& headerPanel = mainComponent_->getHeaderPanel();

    const int savedScaleId = pluginProcessor.getGuiScaleId();
    const float savedUiScale = PluginIDs::Settings::ScaleLevels::getUiScale(savedScaleId);
    applyUiScale(savedUiScale);

    restoreHeaderPanelFromState(headerPanel);
    wireHeaderPanel(headerPanel);

    headerPanel.setPluginMode(!pluginProcessor.isStandalone());
    headerPanel.refreshPortLists();

    const auto savedMidiInputPortId = pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString();
    headerPanel.selectMidiFromPort(savedMidiInputPortId);

    const auto savedMidiOutputPortId = pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString();
    headerPanel.selectMidiToPort(savedMidiOutputPortId);

    pluginProcessor.restoreMidiPortsForHost();

    if (pluginProcessor.isStandalone())
    {
        Core::StandaloneAudioInputRouter::enableInputMonitoring();

        const auto savedKeyboardFromPortId = pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        headerPanel.selectKeyboardFromPort(savedKeyboardFromPortId);
        pluginProcessor.setKeyboardFromPort(headerPanel.getSelectedKeyboardFromPortIdentifier());

        const auto savedAudioFromSourceId = pluginProcessor.getApvts().state.getProperty("audioFromSourceId", juce::String()).toString();
        if (savedAudioFromSourceId.isNotEmpty())
            pluginProcessor.setAudioFromSourceId(savedAudioFromSourceId);

        const float savedInputGainDb = static_cast<float>(
            pluginProcessor.getApvts().state.getProperty("inputGainDb", 0.0f));
        pluginProcessor.setInputGainDb(savedInputGainDb);
    }

    pluginProcessor.syncHardwareLatencyFromState();

    setResizable(false, false);

    headerPanel.getSettingsButton().onClick = [this]
    {
        if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
            closeSettingsWindow();
        else
            openSettingsWindow();
    };

    headerPanel.getUiElementsButton().onClick = [this, &headerPanel]
    {
        setUiElementsTestVisible(headerPanel.getUiElementsButton().getToggleState());
    };

    headerPanel.getMidiFromComboBox().onChange = [this, &headerPanel]
    {
        const auto previousPortId = pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString();
        const auto selectedPortId = headerPanel.getSelectedMidiFromPortIdentifier();

        if (pluginProcessor.setMidiInputPort(selectedPortId))
            return;

        headerPanel.selectMidiFromPort(previousPortId);
        if (previousPortId.isNotEmpty())
            pluginProcessor.setMidiInputPort(previousPortId);
    };

    headerPanel.getMidiToComboBox().onChange = [this, &headerPanel]
    {
        const auto previousPortId = pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString();
        const auto selectedPortId = headerPanel.getSelectedMidiToPortIdentifier();

        if (pluginProcessor.setMidiOutputPort(selectedPortId))
            return;

        headerPanel.selectMidiToPort(previousPortId);
        if (previousPortId.isNotEmpty())
            pluginProcessor.setMidiOutputPort(previousPortId);
    };

    headerPanel.getKeyboardFromComboBox().onChange = [this, &headerPanel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        const auto previousPortId = pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        const auto selectedPortId = headerPanel.getSelectedKeyboardFromPortIdentifier();

        if (pluginProcessor.setKeyboardFromPort(selectedPortId))
            return;

        headerPanel.selectKeyboardFromPort(previousPortId);
        if (previousPortId.isNotEmpty())
            pluginProcessor.setKeyboardFromPort(previousPortId);
    };

    headerRefreshTimer_ = std::make_unique<HeaderRefreshTimer>(pluginProcessor, headerPanel, *this);
    attachStandaloneAudioDeviceListener();

    syncUiScaleFromEditor();
    layoutUiElementsTestComponent();
    repaint();
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void PluginEditor::resized()
{
    const int baseWidth = layoutDimensions_.editor.width;
    if (baseWidth <= 0)
        return;

    if (auto* comp = mainComponent_.get())
        comp->setBounds(getLocalBounds());

    if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
        settingsWindow_->setBounds(getLocalBounds());

    layoutUiElementsTestComponent();
    syncUiScaleFromEditor();
}

void PluginEditor::syncUiScaleFromEditor()
{
    const int baseWidth = layoutDimensions_.editor.width;
    if (baseWidth <= 0)
        return;

    const float uiScale = TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth);

    if (auto* comp = mainComponent_.get())
        comp->setUiScale(uiScale);

    if (auto* comp = mainComponent_.get())
    {
        int matchingScaleId = 0;
        const int layoutPercentRounded = juce::roundToInt(uiScale * 100.0f);
        for (int id = PluginIDs::Settings::ScaleLevels::kMin; id <= PluginIDs::Settings::ScaleLevels::kMax; ++id)
        {
            const int presetPercentRounded = juce::roundToInt(
                PluginIDs::Settings::ScaleLevels::getUiScale(id) * 100.0f);
            if (layoutPercentRounded == presetPercentRounded)
            {
                matchingScaleId = id;
                break;
            }
        }

        if (matchingScaleId != 0)
            comp->getHeaderPanel().setCurrentUiScaleId(matchingScaleId);
    }

    if (settingsWindow_ != nullptr && settingsWindow_->isVisible())
        updateSettingsWindowLayout(uiScale);
}

void PluginEditor::updateSettingsWindowLayout(float uiScale)
{
    if (settingsWindow_ == nullptr)
        return;

    settingsWindow_->setUiScale(uiScale);
    settingsWindow_->setBounds(getLocalBounds());
}

void PluginEditor::applySkinFromItemId(int skinItemId, bool persistToState)
{
    skin_ = (skinItemId == PluginIDs::Settings::SkinVariants::kBlack)
        ? skinBlack_.get()
        : skinCream_.get();
    updateSkin();

    if (persistToState)
        pluginProcessor.setSkinVariantId(skinItemId);

    if (auto* comp = mainComponent_.get())
        comp->getHeaderPanel().setCurrentSkinItemId(skinItemId);
}

void PluginEditor::applyUiScaleFromItemId(int scaleId, bool persistToState)
{
    const float uiScale = PluginIDs::Settings::ScaleLevels::getUiScale(scaleId);
    applyUiScale(uiScale);
    updateSettingsWindowLayout(uiScale);

    if (persistToState)
        pluginProcessor.setGuiScaleId(scaleId);

    if (auto* comp = mainComponent_.get())
        comp->getHeaderPanel().setCurrentUiScaleId(scaleId);
}

void PluginEditor::wireHeaderPanel(HeaderPanel& headerPanel)
{
    headerPanel.onSkinSelected = [this](int skinItemId)
    {
        applySkinFromItemId(skinItemId, true);
    };

    headerPanel.onUiScaleSelected = [this](int scaleId)
    {
        applyUiScaleFromItemId(scaleId, true);
    };

    headerPanel.onUiScaleReset = [this]
    {
        applyUiScaleFromItemId(PluginIDs::Settings::ScaleLevels::k100, true);
    };
}

void PluginEditor::restoreHeaderPanelFromState(HeaderPanel& headerPanel)
{
    headerPanel.setCurrentUiScaleId(pluginProcessor.getGuiScaleId());
    headerPanel.setCurrentSkinItemId(pluginProcessor.getSkinVariantId());
}

void PluginEditor::mouseDown(const juce::MouseEvent&)
{
    unfocusAllComponents();
}

void PluginEditor::updateSkin()
{
    if (auto* widget = mainComponent_.get())
        widget->setSkin(*skin_);

    if (settingsWindow_ != nullptr)
        settingsWindow_->setSkin(*skin_);

    if (testComponent_ != nullptr)
        testComponent_->setSkin(*skin_);

    repaint();
}

void PluginEditor::applyUiScale(float uiScale)
{
    const int baseWidth = layoutDimensions_.editor.width;
    const int baseHeight = layoutDimensions_.editor.height;

    setSize(juce::roundToInt(static_cast<float>(baseWidth) * uiScale),
            juce::roundToInt(static_cast<float>(baseHeight) * uiScale));
}

void PluginEditor::setUiElementsTestVisible(bool visible)
{
    if (uiElementsTestVisible_ == visible)
        return;

    uiElementsTestVisible_ = visible;

    if (auto* main = mainComponent_.get())
        main->setUiElementsTestVisible(visible);

    if (auto* test = testComponent_.get())
    {
        test->setVisible(visible);
        if (visible)
            test->toFront(false);
    }

    layoutUiElementsTestComponent();
    repaint();
}

void PluginEditor::layoutUiElementsTestComponent()
{
    if (!uiElementsTestVisible_ || mainComponent_ == nullptr || testComponent_ == nullptr)
        return;

    testComponent_->setBounds(mainComponent_->getUiElementsTestAreaBounds());
}

PluginEditor::~PluginEditor()
{
    detachStandaloneAudioDeviceListener();
    closeSettingsWindow();
}

SettingsPanel* PluginEditor::getSettingsPanelIfOpen()
{
    if (settingsWindow_ == nullptr || !settingsWindow_->isVisible())
        return nullptr;

    return &settingsWindow_->getSettingsPanel();
}

void PluginEditor::openSettingsWindow()
{
    if (settingsWindow_ == nullptr)
    {
        const bool isPluginMode = !pluginProcessor.isStandalone();
        settingsWindow_ = std::make_unique<SettingsWindow>(
            *skin_,
            isPluginMode,
            [this](SettingsPanel& panel)
            {
                wireSettingsPanel(panel);
                restoreSettingsPanelFromState(panel);
            },
            [this] { closeSettingsWindow(); });
        addChildComponent(*settingsWindow_);
    }
    else
    {
        settingsWindow_->setSkin(*skin_);
        restoreSettingsPanelFromState(settingsWindow_->getSettingsPanel());
    }

    const int baseWidth = layoutDimensions_.editor.width;
    const float uiScale = (baseWidth > 0)
        ? TSS::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth)
        : 1.0f;
    updateSettingsWindowLayout(uiScale);

    settingsWindow_->setVisible(true);
    settingsWindow_->toFront(true);
    settingsWindow_->grabKeyboardFocus();
}

void PluginEditor::closeSettingsWindow()
{
    if (settingsWindow_ != nullptr)
        settingsWindow_->setVisible(false);
}

void PluginEditor::restoreSettingsPanelFromState(SettingsPanel& panel)
{
    panel.getHardwareLatencySlider().setValue(pluginProcessor.getHardwareLatencyMs(), juce::dontSendNotification);

    if (pluginProcessor.isStandalone())
    {
        refreshAudioFromCombo();

        const auto savedAudioFromSourceId = pluginProcessor.getApvts().state.getProperty("audioFromSourceId", juce::String()).toString();
        panel.selectAudioFromSourceId(savedAudioFromSourceId);

        const float savedInputGainDb = static_cast<float>(
            pluginProcessor.getApvts().state.getProperty("inputGainDb", 0.0f));
        panel.getInputGainSlider().setValue(savedInputGainDb, juce::dontSendNotification);
    }
}

void PluginEditor::wireSettingsPanel(SettingsPanel& panel)
{
    panel.getHardwareLatencySlider().onValueChange = [this, &panel]
    {
        pluginProcessor.setHardwareLatencyMs(static_cast<float>(panel.getHardwareLatencySlider().getValue()));
    };

    panel.getInputGainSlider().onValueChange = [this, &panel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        pluginProcessor.setInputGainDb(static_cast<float>(panel.getInputGainSlider().getValue()));
    };

    panel.getAudioFromComboBox().onChange = [this, &panel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        pluginProcessor.setAudioFromSourceId(panel.getSelectedAudioFromSourceId());
    };
}

void PluginEditor::refreshAudioFromCombo()
{
    if (!pluginProcessor.isStandalone())
        return;

    const auto names = pluginProcessor.getAudioInputSourceNames();
    const auto ids = pluginProcessor.getAudioInputSourceIds();

    if (names.isEmpty() && ids.isEmpty())
        return;

    juce::String previousSourceId;

    if (auto* panel = getSettingsPanelIfOpen())
        previousSourceId = panel->getSelectedAudioFromSourceId();
    else
        previousSourceId = pluginProcessor.getApvts().state.getProperty("audioFromSourceId", juce::String()).toString();

    if (auto* panel = getSettingsPanelIfOpen())
    {
        panel->populateAudioFromCombo(names, ids);
        panel->selectAudioFromSourceId(previousSourceId);
    }

    const auto selectedSourceId = previousSourceId.isNotEmpty()
        ? previousSourceId
        : (ids.isEmpty() ? juce::String() : ids[0]);

    if (selectedSourceId.isNotEmpty())
    {
        pluginProcessor.syncAudioPassthroughFromSourceId(selectedSourceId);
    }
    else if (!ids.isEmpty())
    {
        pluginProcessor.setAudioFromSourceId(ids[0]);
    }
}

void PluginEditor::attachStandaloneAudioDeviceListener()
{
    if (!pluginProcessor.isStandalone())
        return;

    Core::StandaloneAudioInputRouter::addAudioDeviceChangeListener(*this);
}

void PluginEditor::detachStandaloneAudioDeviceListener()
{
    Core::StandaloneAudioInputRouter::removeAudioDeviceChangeListener(*this);
}

void PluginEditor::changeListenerCallback(juce::ChangeBroadcaster*)
{
    refreshAudioFromCombo();

    if (pluginProcessor.isStandalone())
        Core::StandaloneAudioInputRouter::enableInputMonitoring();
}
