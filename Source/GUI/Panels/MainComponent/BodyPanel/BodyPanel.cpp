#include "BodyPanel.h"

#include <vector>

#include "GUI/Layout/ScaledLayout.h"
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
    const float sf = displayScale_;

    const int padding              = tss::ScaledLayout::scaledInt(static_cast<float>(padding_), sf);
    const int patchEditPanelWidth  = tss::ScaledLayout::scaledInt(static_cast<float>(patchEditPanelWidth_), sf);
    const int patchEditPanelHeight = tss::ScaledLayout::scaledInt(static_cast<float>(patchEditPanelHeight_), sf);
    const int matrixW              = tss::ScaledLayout::scaledInt(static_cast<float>(matrixModulationPanelWidth_), sf);
    const int patchManagerW        = tss::ScaledLayout::scaledInt(static_cast<float>(patchManagerPanelWidth_), sf);
    const int masterEditW          = tss::ScaledLayout::scaledInt(static_cast<float>(masterEditPanelWidth_), sf);
    const int masterEditH          = tss::ScaledLayout::scaledInt(static_cast<float>(masterEditPanelHeight_), sf);
    const int separatorW           = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard), sf);
    const int separatorH           = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDimensions::Widgets::Heights::kVerticalSeparator), sf);

    const int contentHeight = bounds.getHeight() - 2 * padding;
    const std::vector<int> columnDesignHeights { matrixModulationPanelHeight_, patchManagerPanelHeight_ };
    const auto columnHeights = tss::ScaledLayout::distributeHeights(contentHeight, columnDesignHeights, sf, 1);
    const int matrixH = columnHeights[0];
    const int patchManagerH = columnHeights[1];

    // All X positions computed independently from float origin to avoid rounding accumulation.
    const float originX = static_cast<float>(bounds.getX() + padding);
    const int patchEditX        = bounds.getX() + padding;
    const int separator1X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_) * sf);
    const int matrixX           = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard) * sf);
    const int separator2X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard + matrixModulationPanelWidth_) * sf);
    const int masterEditX       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard + matrixModulationPanelWidth_ + PluginDimensions::Widgets::Widths::VerticalSeparator::kStandard) * sf);

    const int topY = bounds.getY() + padding;
    const int patchManagerY = topY + matrixH;

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

void BodyPanel::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(displayScale_, displayScale))
        return;
    
    displayScale_ = displayScale;
    
    if (verticalSeparator1_)
        verticalSeparator1_->setDisplayScale(displayScale_);
    if (verticalSeparator2_)
        verticalSeparator2_->setDisplayScale(displayScale_);
    if (patchEditPanel_)
        patchEditPanel_->setDisplayScale(displayScale_);
    if (matrixModulationPanel_)
        matrixModulationPanel_->setDisplayScale(displayScale_);
    if (patchManagerPanel_)
        patchManagerPanel_->setDisplayScale(displayScale_);
    if (masterEditPanel_)
        masterEditPanel_->setDisplayScale(displayScale_);
    
    resized();
    repaint();
}

