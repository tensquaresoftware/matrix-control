#include "BodyPanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/VerticalSeparator.h"
#include "PatchEditPanel/PatchEditPanel.h"
#include "MatrixModulationPanel/MatrixModulationPanel.h"
#include "MasterEditPanel/MasterEditPanel.h"
#include "PatchManagerPanel/PatchManagerPanel.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginDimensions.h"

using tss::SkinColourId;

using ::tss::VerticalSeparator;

BodyPanel::BodyPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , padding_(PluginDimensions::Panels::Body::kPadding)
    , patchEditPanelWidth_(PluginDimensions::Panels::Body::PatchEditSection::kWidth)
    , patchEditPanelHeight_(PluginDimensions::Panels::Body::PatchEditSection::kHeight)
    , matrixModulationPanelWidth_(PluginDimensions::Panels::Body::MatrixModulationSection::kWidth)
    , matrixModulationPanelHeight_(PluginDimensions::Panels::Body::MatrixModulationSection::kHeight)
    , patchManagerPanelWidth_(PluginDimensions::Panels::Body::PatchManagerSection::kWidth)
    , patchManagerPanelHeight_(PluginDimensions::Panels::Body::PatchManagerSection::kHeight)
    , masterEditPanelWidth_(PluginDimensions::Panels::Body::MasterEditSection::kWidth)
    , masterEditPanelHeight_(PluginDimensions::Panels::Body::MasterEditSection::kHeight)
    , skin_(&skin)
{
    setOpaque(true);
    patchEditPanel_ = std::make_unique<PatchEditPanel>(skin, patchEditPanelWidth_, patchEditPanelHeight_, widgetFactory, apvts);
    addAndMakeVisible(*patchEditPanel_);

    verticalSeparator1_ = std::make_unique<VerticalSeparator>(
        PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard,
        PluginDimensions::Widgets::Heights::kVerticalSeparator
    );
    addAndMakeVisible(*verticalSeparator1_);

    matrixModulationPanel_ = std::make_unique<MatrixModulationPanel>(skin, matrixModulationPanelWidth_, matrixModulationPanelHeight_, widgetFactory, apvts);
    addAndMakeVisible(*matrixModulationPanel_);

    patchManagerPanel_ = std::make_unique<PatchManagerPanel>(skin, patchManagerPanelWidth_, patchManagerPanelHeight_, widgetFactory, apvts);
    addAndMakeVisible(*patchManagerPanel_);

    verticalSeparator2_ = std::make_unique<VerticalSeparator>(
        PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard,
        PluginDimensions::Widgets::Heights::kVerticalSeparator
    );
    addAndMakeVisible(*verticalSeparator2_);

    masterEditPanel_ = std::make_unique<MasterEditPanel>(skin, masterEditPanelWidth_, masterEditPanelHeight_, widgetFactory, apvts);
    addAndMakeVisible(*masterEditPanel_);
}

BodyPanel::~BodyPanel() = default;

void BodyPanel::paint(juce::Graphics& g)
{
    if (skin_ != nullptr)
        g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void BodyPanel::resized()
{
    const auto bounds = getLocalBounds();
    
    const auto patchEditPanelX = padding_;
    const auto patchEditPanelY = padding_;
    patchEditPanel_->setBounds(
        bounds.getX() + patchEditPanelX,
        bounds.getY() + patchEditPanelY,
        patchEditPanelWidth_,
        patchEditPanelHeight_
    );
    
    const auto verticalSeparator1X = patchEditPanelX + patchEditPanelWidth_;
    const auto verticalSeparator1Y = padding_;
    verticalSeparator1_->setTopLeftPosition(
        bounds.getX() + verticalSeparator1X,
        bounds.getY() + verticalSeparator1Y
    );
    
    const auto matrixModulationPanelX = verticalSeparator1X + verticalSeparator1_->getWidth();
    const auto matrixModulationPanelY = padding_;
    matrixModulationPanel_->setBounds(
        bounds.getX() + matrixModulationPanelX,
        bounds.getY() + matrixModulationPanelY,
        matrixModulationPanelWidth_,
        matrixModulationPanelHeight_
    );
    
    const auto patchManagerPanelX = matrixModulationPanelX;
    const auto patchManagerPanelY = matrixModulationPanelY + matrixModulationPanelHeight_;
    patchManagerPanel_->setBounds(
        bounds.getX() + patchManagerPanelX,
        bounds.getY() + patchManagerPanelY,
        patchManagerPanelWidth_,
        patchManagerPanelHeight_
    );
    
    const auto verticalSeparator2X = matrixModulationPanelX + matrixModulationPanelWidth_;
    const auto verticalSeparator2Y = padding_;
    verticalSeparator2_->setTopLeftPosition(
        bounds.getX() + verticalSeparator2X,
        bounds.getY() + verticalSeparator2Y
    );
    
    const auto masterEditPanelX = verticalSeparator2X + verticalSeparator2_->getWidth();
    const auto masterEditPanelY = padding_;
    masterEditPanel_->setBounds(
        bounds.getX() + masterEditPanelX,
        bounds.getY() + masterEditPanelY,
        masterEditPanelWidth_,
        masterEditPanelHeight_
    );
}

void BodyPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    verticalSeparator1_->setLook(tss::verticalSeparatorLookFromSkin(skin));
    verticalSeparator2_->setLook(tss::verticalSeparatorLookFromSkin(skin));
    tss::propagateSkin(skin,
        patchEditPanel_.get(),
        matrixModulationPanel_.get(),
        masterEditPanel_.get(),
        patchManagerPanel_.get());
}

void BodyPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    
    if (patchEditPanel_)
        patchEditPanel_->setScalingFactor(scalingFactor_);
    if (matrixModulationPanel_)
        matrixModulationPanel_->setScalingFactor(scalingFactor_);
    if (patchManagerPanel_)
        patchManagerPanel_->setScalingFactor(scalingFactor_);
    if (masterEditPanel_)
        masterEditPanel_->setScalingFactor(scalingFactor_);
    
    resized();
    repaint();
}

