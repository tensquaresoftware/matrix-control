#include "Core/PluginProcessor.h"
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
        pluginProcessor.getApvts().state.setProperty(PluginIDs::Settings::kGuiScaleId, selectedId, nullptr);
    };
    
    repaint();
}

PluginEditor::~PluginEditor() = default;

void PluginEditor::paint(juce::Graphics& g)
{
    g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void PluginEditor::resized()
{
    if (auto* component = mainComponent.get())
        component->setBounds(getLocalBounds());
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
    if (auto* component = mainComponent.get())
    {
        component->setScalingFactor(scaleFactor);
    }
    
    const int baseWidth = PluginDimensions::GUI::kWidth;
    const int baseHeight = PluginDimensions::GUI::kHeight;
    
    setSize(juce::roundToInt(static_cast<float>(baseWidth) * scaleFactor), 
            juce::roundToInt(static_cast<float>(baseHeight) * scaleFactor));
}

