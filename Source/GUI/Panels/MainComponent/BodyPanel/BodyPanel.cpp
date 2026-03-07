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
    const float sf = scalingFactor_;

    const int padding              = juce::roundToInt(static_cast<float>(padding_) * sf);
    const int patchEditPanelWidth  = juce::roundToInt(static_cast<float>(patchEditPanelWidth_) * sf);
    const int patchEditPanelHeight = juce::roundToInt(static_cast<float>(patchEditPanelHeight_) * sf);
    const int matrixW              = juce::roundToInt(static_cast<float>(matrixModulationPanelWidth_) * sf);
    const int matrixH              = juce::roundToInt(static_cast<float>(matrixModulationPanelHeight_) * sf);
    const int patchManagerW        = juce::roundToInt(static_cast<float>(patchManagerPanelWidth_) * sf);
    const int patchManagerH        = juce::roundToInt(static_cast<float>(patchManagerPanelHeight_) * sf);
    const int masterEditW          = juce::roundToInt(static_cast<float>(masterEditPanelWidth_) * sf);
    const int masterEditH          = juce::roundToInt(static_cast<float>(masterEditPanelHeight_) * sf);
    const int separatorW           = juce::roundToInt(static_cast<float>(PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard) * sf);
    const int separatorH           = juce::roundToInt(static_cast<float>(PluginDimensions::Widgets::Heights::kVerticalSeparator) * sf);

    // All X positions computed independently from float origin to avoid rounding accumulation.
    const float originX = static_cast<float>(bounds.getX() + padding);
    const int patchEditX        = bounds.getX() + padding;
    const int separator1X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_) * sf);
    const int matrixX           = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard) * sf);
    const int separator2X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard + matrixModulationPanelWidth_) * sf);
    const int masterEditX       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard + matrixModulationPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard) * sf);

    // Y positions computed independently from float origin.
    const float originY = static_cast<float>(bounds.getY() + padding);
    const int topY          = bounds.getY() + padding;
    const int patchManagerY = juce::roundToInt(originY + static_cast<float>(matrixModulationPanelHeight_) * sf);

    patchEditPanel_->setBounds(patchEditX, topY, patchEditPanelWidth, patchEditPanelHeight);
    verticalSeparator1_->setBounds(separator1X, topY, separatorW, separatorH);
    matrixModulationPanel_->setBounds(matrixX, topY, matrixW, matrixH);
    patchManagerPanel_->setBounds(matrixX, patchManagerY, patchManagerW, patchManagerH);
    verticalSeparator2_->setBounds(separator2X, topY, separatorW, separatorH);
    masterEditPanel_->setBounds(masterEditX, topY, masterEditW, masterEditH);
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
    
    if (verticalSeparator1_)
        verticalSeparator1_->setScalingFactor(scalingFactor_);
    if (verticalSeparator2_)
        verticalSeparator2_->setScalingFactor(scalingFactor_);
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

