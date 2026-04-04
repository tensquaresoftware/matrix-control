#include <optional>

#include "Core/PluginProcessor.h"
#include "GUI/Layout/ScaledLayout.h"
#include "PluginEditor.h"
#include "Skins/Skin.h"
#include "Panels/MainComponent/HeaderPanel/HeaderPanel.h"
#include "Panels/MainComponent/BodyPanel/BodyPanel.h"
#include "Panels/MainComponent/FooterPanel/FooterPanel.h"
#include "Factories/WidgetFactory.h"

using tss::SkinColourId;


PluginEditor::PluginEditor(PluginProcessor& p)
    : AudioProcessorEditor(&p)
    , pluginProcessor(p)
{
    skinBlack_ = tss::Skin::create(tss::Skin::ColourVariant::Black);
    skinCream_ = tss::Skin::create(tss::Skin::ColourVariant::Cream);
    skin_ = skinBlack_.get();
    
    widgetFactory = std::make_unique<WidgetFactory>(pluginProcessor.getApvts());
    
    setOpaque(true);
    setWantsKeyboardFocus(false);
    setInterceptsMouseClicks(true, true);
    
    const auto editorWidth = PluginDimensions::GUI::kWidth;
    const auto editorHeight = PluginDimensions::GUI::kHeight;
    mainComponent = std::make_unique<MainComponent>(*skin_, editorWidth, editorHeight, *widgetFactory, pluginProcessor.getApvts());
    addAndMakeVisible(*mainComponent);
    
    if (auto* component = mainComponent.get())
    {
        component->setBounds(getLocalBounds());
    }

    updateSkin();

    auto& headerPanel = mainComponent->getHeaderPanel();
    
    const int savedScaleId = pluginProcessor.getApvts().state.getProperty(
        PluginIDs::Settings::kGuiScaleId, 
        PluginIDs::Settings::ScaleLevels::kDefault
    );
    const float savedScaleFactor = PluginIDs::Settings::ScaleLevels::getScaleFactor(savedScaleId);
    applyGuiScale(savedScaleFactor);
    headerPanel.getGuiScaleComboBox().setSelectedId(savedScaleId, juce::dontSendNotification);

    setResizable(true, false);
    const int baseWidth = PluginDimensions::GUI::kWidth;
    const int baseHeight = PluginDimensions::GUI::kHeight;
    constexpr float kMinScaleFactor = 0.5f;
    constexpr float kMaxScaleFactor = 4.0f;
    setResizeLimits(
        juce::roundToInt(static_cast<float>(baseWidth) * kMinScaleFactor),
        juce::roundToInt(static_cast<float>(baseHeight) * kMinScaleFactor),
        juce::roundToInt(static_cast<float>(baseWidth) * kMaxScaleFactor),
        juce::roundToInt(static_cast<float>(baseHeight) * kMaxScaleFactor));
    getConstrainer()->setFixedAspectRatio(
        static_cast<double>(baseWidth) / static_cast<double>(baseHeight));

    resizeCorner_ = std::make_unique<juce::ResizableCornerComponent>(this, getConstrainer());
    addAndMakeVisible(*resizeCorner_);
    resizeCorner_->setAlwaysOnTop(true);
    
    headerPanel.getSkinComboBox().onChange = [this, &headerPanel]
    {
        const auto selectedId = headerPanel.getSkinComboBox().getSelectedId();
        
        skin_ = (selectedId == 1) ? skinBlack_.get() : skinCream_.get();
        updateSkin();
    };
    
    headerPanel.getGuiScaleComboBox().onChange = [this, &headerPanel]
    {
        const auto selectedId = headerPanel.getGuiScaleComboBox().getSelectedId();
        const float scaleFactor = PluginIDs::Settings::ScaleLevels::getScaleFactor(selectedId);
        applyGuiScale(scaleFactor);
        headerPanel.setGuiScaleDisplayText(std::nullopt);
        pluginProcessor.getApvts().state.setProperty(PluginIDs::Settings::kGuiScaleId, selectedId, nullptr);
    };
    
    lastSyncedEditorWidth_ = getWidth();
    syncLayoutScaleFromEditor();
    startTimerHz(120);

    layoutResizeCorner();
    repaint();
}

PluginEditor::~PluginEditor()
{
    stopTimer();
}

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void PluginEditor::resized()
{
    const int baseWidth = PluginDimensions::GUI::kWidth;
    if (baseWidth <= 0)
        return;

    if (auto* comp = mainComponent.get())
        comp->setBounds(getLocalBounds());

    layoutResizeCorner();
}

void PluginEditor::layoutResizeCorner()
{
    if (resizeCorner_ == nullptr)
        return;

    bool hidden = false;
    if (auto* peer = getPeer())
        hidden = peer->isFullScreen() || peer->isKioskMode();

    resizeCorner_->setVisible(!hidden);

    if (hidden)
        return;

    const int s = PluginDimensions::GUI::kResizeCornerSize;
    resizeCorner_->setBounds(getWidth() - s, getHeight() - s, s, s);
}

void PluginEditor::timerCallback()
{
    const int w = getWidth();
    if (w == lastSyncedEditorWidth_)
        return;

    lastSyncedEditorWidth_ = w;
    syncLayoutScaleFromEditor();
}

void PluginEditor::syncLayoutScaleFromEditor()
{
    const int baseWidth = PluginDimensions::GUI::kWidth;
    if (baseWidth <= 0)
        return;

    // Layout and preset matching use editor pixel size vs design width only.
    // Host display scaling (getTransform) is not folded into layoutScale here.
    const float layoutScale = tss::ScaledLayout::layoutScaleFromEditorBounds(getWidth(), baseWidth);

    if (auto* comp = mainComponent.get())
        comp->setScalingFactor(layoutScale);

    auto& headerPanel = mainComponent->getHeaderPanel();

    int matchingScaleId = 0;
    const int layoutPercentRounded = juce::roundToInt(layoutScale * 100.0f);
    for (int id = PluginIDs::Settings::ScaleLevels::kMin; id <= PluginIDs::Settings::ScaleLevels::kMax; ++id)
    {
        const int presetPercentRounded = juce::roundToInt(
            PluginIDs::Settings::ScaleLevels::getScaleFactor(id) * 100.0f);
        if (layoutPercentRounded == presetPercentRounded)
        {
            matchingScaleId = id;
            break;
        }
    }

    if (matchingScaleId != 0)
    {
        headerPanel.setGuiScaleDisplayText(std::nullopt);
        headerPanel.getGuiScaleComboBox().setSelectedId(matchingScaleId, juce::dontSendNotification);
    }
    else
    {
        headerPanel.setGuiScaleDisplayText(
            juce::String::formatted("%.2f%%", layoutScale * 100.0f));
    }
}

void PluginEditor::mouseDown(const juce::MouseEvent&)
{
    unfocusAllComponents();
}

void PluginEditor::updateSkin()
{
    if (auto* widget = mainComponent.get())
        widget->setSkin(*skin_);
    repaint();
}

void PluginEditor::applyGuiScale(float scaleFactor)
{
    const int baseWidth = PluginDimensions::GUI::kWidth;
    const int baseHeight = PluginDimensions::GUI::kHeight;

    setSize(juce::roundToInt(static_cast<float>(baseWidth) * scaleFactor),
            juce::roundToInt(static_cast<float>(baseHeight) * scaleFactor));
}

