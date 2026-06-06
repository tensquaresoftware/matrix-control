#include "PluginEditor.h"

#include "Core/Audio/AudioPassthroughProcessor.h"
#include "Core/Audio/StandaloneAudioInputRouter.h"
#include "Core/MIDI/MidiActivityTracker.h"
#include "Core/PluginProcessor.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Skins/Skin.h"
#include "Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginIDs.h"

using tss::SkinColourId;

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

            headerPanel_.getPeakIndicator().setLevel(
                processor_.getAudioPassthroughProcessor().getPeakLevel());
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
    skinBlack_ = tss::Skin::create(tss::Skin::ColourVariant::Black);
    skinCream_ = tss::Skin::create(tss::Skin::ColourVariant::Cream);
    skin_ = skinBlack_.get();

    widgetFactory_ = std::make_unique<WidgetFactory>(pluginProcessor.getApvts());

    setOpaque(true);
    setWantsKeyboardFocus(false);
    setInterceptsMouseClicks(true, true);

    const auto editorWidth = PluginDesignDimensions::GUI::kWidth;
    const auto editorHeight = PluginDesignDimensions::GUI::kHeight;

    mainComponent_ = std::make_unique<MainComponent>(
        *skin_, editorWidth, editorHeight, *widgetFactory_, pluginProcessor.getApvts());
    addAndMakeVisible(*mainComponent_);

    testComponent_ = std::make_unique<TestComponent>(*skin_, pluginProcessor.getApvts().state, editorWidth, editorHeight);
    addChildComponent(*testComponent_);
    testComponent_->setVisible(false);

    updateSkin();

    auto& headerPanel = mainComponent_->getHeaderPanel();

    const int savedScaleId = pluginProcessor.getApvts().state.getProperty(
        PluginIDs::Settings::kGuiScaleId,
        PluginIDs::Settings::ScaleLevels::kDefault);
    const float savedUiScale = PluginIDs::Settings::ScaleLevels::getUiScale(savedScaleId);
    applyUiScale(savedUiScale);
    headerPanel.getUiScaleComboBox().setSelectedId(savedScaleId, juce::dontSendNotification);

    headerPanel.setPluginMode(!pluginProcessor.isStandalone());
    headerPanel.refreshPortLists();

    if (pluginProcessor.isStandalone())
        refreshAudioFromCombo();

    const auto savedMidiInputPortId = pluginProcessor.getApvts().state.getProperty("midiInputPortId", juce::String()).toString();
    headerPanel.selectMidiFromPort(savedMidiInputPortId);
    pluginProcessor.setMidiInputPort(savedMidiInputPortId);

    const auto savedMidiOutputPortId = pluginProcessor.getApvts().state.getProperty("midiOutputPortId", juce::String()).toString();
    headerPanel.selectMidiToPort(savedMidiOutputPortId);
    pluginProcessor.setMidiOutputPort(savedMidiOutputPortId);

    if (pluginProcessor.isStandalone())
    {
        Core::StandaloneAudioInputRouter::enableInputMonitoring();

        const auto savedKeyboardFromPortId = pluginProcessor.getApvts().state.getProperty("keyboardFromPortId", juce::String()).toString();
        headerPanel.selectKeyboardFromPort(savedKeyboardFromPortId);
        pluginProcessor.setKeyboardFromPort(headerPanel.getSelectedKeyboardFromPortIdentifier());

        const auto savedAudioFromSourceId = pluginProcessor.getApvts().state.getProperty("audioFromSourceId", juce::String()).toString();
        headerPanel.selectAudioFromSourceId(savedAudioFromSourceId);
        pluginProcessor.setAudioFromSourceId(headerPanel.getSelectedAudioFromSourceId());

        const float savedInputGainDb = static_cast<float>(
            pluginProcessor.getApvts().state.getProperty("inputGainDb", 0.0f));
        headerPanel.getInputGainSlider().setValue(savedInputGainDb, juce::dontSendNotification);
        pluginProcessor.setInputGainDb(savedInputGainDb);
    }

    const float savedHardwareLatencyMs = pluginProcessor.getHardwareLatencyMs();
    headerPanel.getHardwareLatencySlider().setValue(savedHardwareLatencyMs, juce::dontSendNotification);
    pluginProcessor.setHardwareLatencyMs(savedHardwareLatencyMs);

    setResizable(false, false);

    headerPanel.getSkinComboBox().onChange = [this, &headerPanel]
    {
        const auto selectedId = headerPanel.getSkinComboBox().getSelectedId();

        skin_ = (selectedId == 1) ? skinBlack_.get() : skinCream_.get();
        updateSkin();
    };

    headerPanel.getUiScaleComboBox().onChange = [this, &headerPanel]
    {
        const auto selectedId = headerPanel.getUiScaleComboBox().getSelectedId();
        const float uiScale = PluginIDs::Settings::ScaleLevels::getUiScale(selectedId);
        applyUiScale(uiScale);
        pluginProcessor.getApvts().state.setProperty(PluginIDs::Settings::kGuiScaleId, selectedId, nullptr);
    };

    headerPanel.getUiElementsButton().onClick = [this, &headerPanel]
    {
        setUiElementsTestVisible(headerPanel.getUiElementsButton().getToggleState());
    };

    headerPanel.getMidiFromComboBox().onChange = [this, &headerPanel]
    {
        pluginProcessor.setMidiInputPort(headerPanel.getSelectedMidiFromPortIdentifier());
    };

    headerPanel.getMidiToComboBox().onChange = [this, &headerPanel]
    {
        pluginProcessor.setMidiOutputPort(headerPanel.getSelectedMidiToPortIdentifier());
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

    headerPanel.getInputGainSlider().onValueChange = [this, &headerPanel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        pluginProcessor.setInputGainDb(static_cast<float>(headerPanel.getInputGainSlider().getValue()));
    };

    headerPanel.getHardwareLatencySlider().onValueChange = [this, &headerPanel]
    {
        pluginProcessor.setHardwareLatencyMs(static_cast<float>(headerPanel.getHardwareLatencySlider().getValue()));
    };

    headerPanel.getAudioFromComboBox().onChange = [this, &headerPanel]
    {
        if (!pluginProcessor.isStandalone())
            return;

        pluginProcessor.setAudioFromSourceId(headerPanel.getSelectedAudioFromSourceId());
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
    const int baseWidth = PluginDesignDimensions::GUI::kWidth;
    if (baseWidth <= 0)
        return;

    if (auto* comp = mainComponent_.get())
        comp->setBounds(getLocalBounds());

    layoutUiElementsTestComponent();
    syncUiScaleFromEditor();
}

void PluginEditor::syncUiScaleFromEditor()
{
    const int baseWidth = PluginDesignDimensions::GUI::kWidth;
    if (baseWidth <= 0)
        return;

    const float uiScale = tss::ScaledLayout::uiScaleFromEditorBounds(getWidth(), baseWidth);

    if (auto* comp = mainComponent_.get())
        comp->setUiScale(uiScale);

    auto& headerPanel = mainComponent_->getHeaderPanel();

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
        headerPanel.getUiScaleComboBox().setSelectedId(matchingScaleId, juce::dontSendNotification);
}

void PluginEditor::mouseDown(const juce::MouseEvent&)
{
    unfocusAllComponents();
}

void PluginEditor::updateSkin()
{
    if (auto* widget = mainComponent_.get())
        widget->setSkin(*skin_);

    repaint();
}

void PluginEditor::applyUiScale(float uiScale)
{
    const int baseWidth = PluginDesignDimensions::GUI::kWidth;
    const int baseHeight = PluginDesignDimensions::GUI::kHeight;

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
}

void PluginEditor::refreshAudioFromCombo()
{
    if (!pluginProcessor.isStandalone() || mainComponent_ == nullptr)
        return;

    auto& headerPanel = mainComponent_->getHeaderPanel();
    const auto previousSourceId = headerPanel.getSelectedAudioFromSourceId();
    const auto names = pluginProcessor.getAudioInputSourceNames();
    const auto ids = pluginProcessor.getAudioInputSourceIds();

    if (names.isEmpty() && ids.isEmpty())
        return;

    headerPanel.populateAudioFromCombo(names, ids);
    headerPanel.selectAudioFromSourceId(previousSourceId);

    const auto selectedSourceId = headerPanel.getSelectedAudioFromSourceId();

    if (selectedSourceId.isNotEmpty())
    {
        pluginProcessor.syncAudioPassthroughFromSourceId(selectedSourceId);
    }
    else if (!ids.isEmpty())
    {
        headerPanel.selectAudioFromSourceId(ids[0]);
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
