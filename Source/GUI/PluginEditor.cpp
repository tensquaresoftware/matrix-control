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
    
    setSize(getWidth(), getHeight());
    
    const auto editorWidth = getWidth();
    const auto editorHeight = getHeight();
    mainComponent = std::make_unique<MainComponent>(*skin_, editorWidth, editorHeight, *widgetFactory, pluginProcessor.getApvts());
    addAndMakeVisible(*mainComponent);
    
    if (auto* component = mainComponent.get())
    {
        component->setBounds(getLocalBounds());
    }
    
    auto& headerPanel = mainComponent->getHeaderPanel();
    
    headerPanel.getSkinComboBox().onChange = [this, &headerPanel]
    {
        const auto selectedId = headerPanel.getSkinComboBox().getSelectedId();
        
        skin_ = (selectedId == 1) ? skinBlack_.get() : skinCream_.get();
        updateSkin();
    };
    
    headerPanel.getZoomComboBox().onChange = [&headerPanel]
    {
        const auto selectedId = headerPanel.getZoomComboBox().getSelectedId();
        const float zoomFactor = PluginIDs::Settings::ZoomLevels::getZoomLevel(selectedId);
        
        // TODO: Implement zoom functionality (Phase 7)
        // For now, just log the selected zoom factor
        juce::ignoreUnused(zoomFactor);
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

