#include "BodyPanel.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Widgets/VerticalSeparator.h"
#include "PatchEditPanel/PatchEditPanel.h"
#include "MatrixModulationPanel/MatrixModulationPanel.h"
#include "MasterEditPanel/MasterEditPanel.h"
#include "PatchManagerPanel/PatchManagerPanel.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/PluginDimensions.h"

using ::tss::VerticalSeparator;

BodyPanel::BodyPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
{
    setOpaque(true);
    patchEditPanel_ = std::make_unique<PatchEditPanel>(skin, widgetFactory, apvts);
    addAndMakeVisible(*patchEditPanel_);

    verticalSeparator1_ = std::make_unique<VerticalSeparator>(
        skin,
        PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard,
        PluginDimensions::Widgets::Heights::kVerticalSeparator
    );
    addAndMakeVisible(*verticalSeparator1_);

    matrixModulationPanel_ = std::make_unique<MatrixModulationPanel>(skin, widgetFactory, apvts);
    addAndMakeVisible(*matrixModulationPanel_);

    patchManagerPanel_ = std::make_unique<PatchManagerPanel>(skin, widgetFactory, apvts);
    addAndMakeVisible(*patchManagerPanel_);

    verticalSeparator2_ = std::make_unique<VerticalSeparator>(
        skin,
        PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard,
        PluginDimensions::Widgets::Heights::kVerticalSeparator
    );
    addAndMakeVisible(*verticalSeparator2_);

    masterEditPanel_ = std::make_unique<MasterEditPanel>(skin, widgetFactory, apvts);
    addAndMakeVisible(*masterEditPanel_);
}

BodyPanel::~BodyPanel() = default;

void BodyPanel::paint(juce::Graphics& g)
{
    if (skin_ != nullptr)
        g.fillAll(skin_->getBodyPanelBackgroundColour());
}

void BodyPanel::resized()
{
    const auto bounds = getLocalBounds();
    const auto padding = getPadding();
    
    const auto patchEditPanelX = padding;
    const auto patchEditPanelY = padding;
    patchEditPanel_->setBounds(
        bounds.getX() + patchEditPanelX,
        bounds.getY() + patchEditPanelY,
        PatchEditPanel::getWidth(),
        PatchEditPanel::getHeight()
    );
    
    const auto verticalSeparator1X = patchEditPanelX + PatchEditPanel::getWidth();
    const auto verticalSeparator1Y = padding;
    verticalSeparator1_->setTopLeftPosition(
        bounds.getX() + verticalSeparator1X,
        bounds.getY() + verticalSeparator1Y
    );
    
    const auto matrixModulationPanelX = verticalSeparator1X + verticalSeparator1_->getWidth();
    const auto matrixModulationPanelY = padding;
    matrixModulationPanel_->setBounds(
        bounds.getX() + matrixModulationPanelX,
        bounds.getY() + matrixModulationPanelY,
        MatrixModulationPanel::getWidth(),
        MatrixModulationPanel::getHeight()
    );
    
    const auto patchManagerPanelX = matrixModulationPanelX;
    const auto patchManagerPanelY = matrixModulationPanelY + MatrixModulationPanel::getHeight();
    patchManagerPanel_->setBounds(
        bounds.getX() + patchManagerPanelX,
        bounds.getY() + patchManagerPanelY,
        PatchManagerPanel::getWidth(),
        PatchManagerPanel::getHeight()
    );
    
    const auto verticalSeparator2X = matrixModulationPanelX + MatrixModulationPanel::getWidth();
    const auto verticalSeparator2Y = padding;
    verticalSeparator2_->setTopLeftPosition(
        bounds.getX() + verticalSeparator2X,
        bounds.getY() + verticalSeparator2Y
    );
    
    const auto masterEditPanelX = verticalSeparator2X + verticalSeparator2_->getWidth();
    const auto masterEditPanelY = padding;
    masterEditPanel_->setBounds(
        bounds.getX() + masterEditPanelX,
        bounds.getY() + masterEditPanelY,
        MasterEditPanel::getWidth(),
        MasterEditPanel::getHeight()
    );
}

void BodyPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* panel = patchEditPanel_.get())
        panel->setSkin(skin);

    if (auto* separator = verticalSeparator1_.get())
        separator->setSkin(skin);

    if (auto* panel = matrixModulationPanel_.get())
        panel->setSkin(skin);

    if (auto* separator = verticalSeparator2_.get())
        separator->setSkin(skin);

    if (auto* panel = masterEditPanel_.get())
        panel->setSkin(skin);

    if (auto* panel = patchManagerPanel_.get())
        panel->setSkin(skin);
}

