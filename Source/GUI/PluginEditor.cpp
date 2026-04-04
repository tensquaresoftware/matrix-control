#include "PluginEditor.h"

#include "Core/PluginProcessor.h"
#include "GUI/Layout/ScaledLayout.h"
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
        component->setBounds(getLocalBounds());

    updateSkin();

    auto& headerPanel = mainComponent->getHeaderPanel();

    const int savedScaleId = pluginProcessor.getApvts().state.getProperty(
        PluginIDs::Settings::kGuiScaleId,
        PluginIDs::Settings::ScaleLevels::kDefault);
    const float savedDisplayScale = PluginIDs::Settings::ScaleLevels::getDisplayScale(savedScaleId);
    applyDisplayScale(savedDisplayScale);
    headerPanel.getGuiScaleComboBox().setSelectedId(savedScaleId, juce::dontSendNotification);

    setResizable(false, false);

    headerPanel.getSkinComboBox().onChange = [this, &headerPanel]
    {
        const auto selectedId = headerPanel.getSkinComboBox().getSelectedId();

        skin_ = (selectedId == 1) ? skinBlack_.get() : skinCream_.get();
        updateSkin();
    };

    headerPanel.getGuiScaleComboBox().onChange = [this, &headerPanel]
    {
        const auto selectedId = headerPanel.getGuiScaleComboBox().getSelectedId();
        const float displayScale = PluginIDs::Settings::ScaleLevels::getDisplayScale(selectedId);
        applyDisplayScale(displayScale);
        pluginProcessor.getApvts().state.setProperty(PluginIDs::Settings::kGuiScaleId, selectedId, nullptr);
    };

    syncDisplayScaleFromEditor();
    repaint();
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

    syncDisplayScaleFromEditor();
}

void PluginEditor::syncDisplayScaleFromEditor()
{
    const int baseWidth = PluginDimensions::GUI::kWidth;
    if (baseWidth <= 0)
        return;

    // UI display scale from editor width vs design width. Host HiDPI (Component::setScaleFactor) is separate.
    const float displayScale = tss::ScaledLayout::displayScaleFromEditorBounds(getWidth(), baseWidth);

    if (auto* comp = mainComponent.get())
        comp->setDisplayScale(displayScale);

    auto& headerPanel = mainComponent->getHeaderPanel();

    int matchingScaleId = 0;
    const int layoutPercentRounded = juce::roundToInt(displayScale * 100.0f);
    for (int id = PluginIDs::Settings::ScaleLevels::kMin; id <= PluginIDs::Settings::ScaleLevels::kMax; ++id)
    {
        const int presetPercentRounded = juce::roundToInt(
            PluginIDs::Settings::ScaleLevels::getDisplayScale(id) * 100.0f);
        if (layoutPercentRounded == presetPercentRounded)
        {
            matchingScaleId = id;
            break;
        }
    }

    if (matchingScaleId != 0)
        headerPanel.getGuiScaleComboBox().setSelectedId(matchingScaleId, juce::dontSendNotification);
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

void PluginEditor::applyDisplayScale(float displayScale)
{
    const int baseWidth = PluginDimensions::GUI::kWidth;
    const int baseHeight = PluginDimensions::GUI::kHeight;

    setSize(juce::roundToInt(static_cast<float>(baseWidth) * displayScale),
            juce::roundToInt(static_cast<float>(baseHeight) * displayScale));
}
