#include "BodyPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/VerticalSeparator.h"
#include "PatchEditPanel/PatchEditPanel.h"
#include "MasterEditPanel/MasterEditPanel.h"
#include "SharedPanel/SharedPanel.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginDesignDimensions.h"

using tss::SkinColourId;

using ::tss::VerticalSeparator;

BodyPanel::BodyPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , padding_(PluginDesignDimensions::Panels::Body::kPadding)
    , patchEditPanelWidth_(PluginDesignDimensions::Panels::Body::PatchEditSection::kWidth)
    , patchEditPanelHeight_(PluginDesignDimensions::Panels::Body::PatchEditSection::kHeight)
    , masterEditPanelWidth_(PluginDesignDimensions::Panels::Body::MasterEditSection::kWidth)
    , masterEditPanelHeight_(PluginDesignDimensions::Panels::Body::MasterEditSection::kHeight)
    , skin_(&skin)
{
    setOpaque(true);
    patchEditPanel_ = std::make_unique<PatchEditPanel>(skin, patchEditPanelWidth_, patchEditPanelHeight_, widgetFactory, apvts);
    addAndMakeVisible(*patchEditPanel_);

    verticalSeparator1_ = std::make_unique<VerticalSeparator>(
        PluginDesignDimensions::Widgets::Widths::VerticalSeparator::kStandard,
        PluginDesignDimensions::Widgets::Heights::kVerticalSeparator,
        tss::verticalSeparatorLookFromSkin(skin));
    addAndMakeVisible(*verticalSeparator1_);

    sharedPanel_ = std::make_unique<SharedPanel>(
        skin,
        PluginDesignDimensions::Panels::Body::SharedColumn::kWidth,
        widgetFactory,
        apvts);
    addAndMakeVisible(*sharedPanel_);

    verticalSeparator2_ = std::make_unique<VerticalSeparator>(
        PluginDesignDimensions::Widgets::Widths::VerticalSeparator::kStandard,
        PluginDesignDimensions::Widgets::Heights::kVerticalSeparator,
        tss::verticalSeparatorLookFromSkin(skin));
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
    const float sf = uiScale_;

    const int padding              = tss::ScaledLayout::scaledInt(static_cast<float>(padding_), sf);
    const int patchEditPanelWidth  = tss::ScaledLayout::scaledInt(static_cast<float>(patchEditPanelWidth_), sf);
    const int patchEditPanelHeight = tss::ScaledLayout::scaledInt(static_cast<float>(patchEditPanelHeight_), sf);
    const int sharedColumnW        = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDesignDimensions::Panels::Body::SharedColumn::kWidth), sf);
    const int masterEditW          = tss::ScaledLayout::scaledInt(static_cast<float>(masterEditPanelWidth_), sf);
    const int masterEditH          = tss::ScaledLayout::scaledInt(static_cast<float>(masterEditPanelHeight_), sf);
    const int separatorW           = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDesignDimensions::Widgets::Widths::VerticalSeparator::kStandard), sf);
    const int separatorH           = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDesignDimensions::Widgets::Heights::kVerticalSeparator), sf);

    const int contentHeight = bounds.getHeight() - 2 * padding;

    // All X positions computed independently from float origin to avoid rounding accumulation.
    const float originX = static_cast<float>(bounds.getX() + padding);
    const int patchEditX        = bounds.getX() + padding;
    const int separator1X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_) * sf);
    const int sharedColumnX       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDesignDimensions::Widgets::Widths::VerticalSeparator::kStandard) * sf);
    const int separator2X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDesignDimensions::Widgets::Widths::VerticalSeparator::kStandard + PluginDesignDimensions::Panels::Body::SharedColumn::kWidth) * sf);
    const int masterEditX       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + PluginDesignDimensions::Widgets::Widths::VerticalSeparator::kStandard + PluginDesignDimensions::Panels::Body::SharedColumn::kWidth + PluginDesignDimensions::Widgets::Widths::VerticalSeparator::kStandard) * sf);

    const int topY = bounds.getY() + padding;

    patchEditPanel_->setBounds(patchEditX, topY, patchEditPanelWidth, patchEditPanelHeight);
    verticalSeparator1_->setBounds(separator1X, topY, separatorW, separatorH);
    sharedPanel_->setBounds(sharedColumnX, topY, sharedColumnW, contentHeight);
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
        sharedPanel_.get(),
        masterEditPanel_.get());
}

void BodyPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (verticalSeparator1_)
        verticalSeparator1_->setUiScale(uiScale_);
    if (verticalSeparator2_)
        verticalSeparator2_->setUiScale(uiScale_);
    if (patchEditPanel_)
        patchEditPanel_->setUiScale(uiScale_);
    if (sharedPanel_)
        sharedPanel_->setUiScale(uiScale_);
    if (masterEditPanel_)
        masterEditPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}

