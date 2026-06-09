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
#include "GUI/Layout/Design/Design.h"

using TSS::SkinColourId;

using ::TSS::VerticalSeparator;

BodyPanel::BodyPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , padding_(TSS::Design::Panels::Body::kPadding)
    , patchEditPanelWidth_(TSS::Design::Panels::Body::PatchEditSection::kWidth)
    , patchEditPanelHeight_(TSS::Design::Panels::Body::PatchEditSection::kHeight)
    , masterEditPanelWidth_(TSS::Design::Panels::Body::MasterEditSection::kWidth)
    , masterEditPanelHeight_(TSS::Design::Panels::Body::MasterEditSection::kHeight)
    , skin_(&skin)
{
    setOpaque(true);
    patchEditPanel_ = std::make_unique<PatchEditPanel>(skin, patchEditPanelWidth_, patchEditPanelHeight_, widgetFactory, apvts);
    addAndMakeVisible(*patchEditPanel_);

    verticalSeparator1_ = std::make_unique<VerticalSeparator>(
        TSS::Design::Atoms::Widths::VerticalSeparator::kStandard,
        TSS::Design::PanelWidgets::Heights::kVerticalSeparator,
        TSS::verticalSeparatorLookFromSkin(skin));
    addAndMakeVisible(*verticalSeparator1_);

    sharedPanel_ = std::make_unique<SharedPanel>(
        skin,
        TSS::Design::Panels::Body::SharedColumn::kWidth,
        widgetFactory,
        apvts);
    addAndMakeVisible(*sharedPanel_);

    verticalSeparator2_ = std::make_unique<VerticalSeparator>(
        TSS::Design::Atoms::Widths::VerticalSeparator::kStandard,
        TSS::Design::PanelWidgets::Heights::kVerticalSeparator,
        TSS::verticalSeparatorLookFromSkin(skin));
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

    const int padding              = TSS::ScaledLayout::scaledInt(static_cast<float>(padding_), sf);
    const int patchEditPanelWidth  = TSS::ScaledLayout::scaledInt(static_cast<float>(patchEditPanelWidth_), sf);
    const int patchEditPanelHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(patchEditPanelHeight_), sf);
    const int sharedColumnW        = TSS::ScaledLayout::scaledInt(
        static_cast<float>(TSS::Design::Panels::Body::SharedColumn::kWidth), sf);
    const int masterEditW          = TSS::ScaledLayout::scaledInt(static_cast<float>(masterEditPanelWidth_), sf);
    const int masterEditH          = TSS::ScaledLayout::scaledInt(static_cast<float>(masterEditPanelHeight_), sf);
    const int separatorW           = TSS::ScaledLayout::scaledInt(
        static_cast<float>(TSS::Design::Atoms::Widths::VerticalSeparator::kStandard), sf);
    const int separatorH           = TSS::ScaledLayout::scaledInt(
        static_cast<float>(TSS::Design::PanelWidgets::Heights::kVerticalSeparator), sf);

    const int contentHeight = bounds.getHeight() - 2 * padding;

    // All X positions computed independently from float origin to avoid rounding accumulation.
    const float originX = static_cast<float>(bounds.getX() + padding);
    const int patchEditX        = bounds.getX() + padding;
    const int separator1X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_) * sf);
    const int sharedColumnX       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + TSS::Design::Atoms::Widths::VerticalSeparator::kStandard) * sf);
    const int separator2X       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + TSS::Design::Atoms::Widths::VerticalSeparator::kStandard + TSS::Design::Panels::Body::SharedColumn::kWidth) * sf);
    const int masterEditX       = juce::roundToInt(originX + static_cast<float>(patchEditPanelWidth_ + TSS::Design::Atoms::Widths::VerticalSeparator::kStandard + TSS::Design::Panels::Body::SharedColumn::kWidth + TSS::Design::Atoms::Widths::VerticalSeparator::kStandard) * sf);

    const int topY = bounds.getY() + padding;

    patchEditPanel_->setBounds(patchEditX, topY, patchEditPanelWidth, patchEditPanelHeight);
    verticalSeparator1_->setBounds(separator1X, topY, separatorW, separatorH);
    sharedPanel_->setBounds(sharedColumnX, topY, sharedColumnW, contentHeight);
    verticalSeparator2_->setBounds(separator2X, topY, separatorW, separatorH);
    masterEditPanel_->setBounds(masterEditX, topY, masterEditW, masterEditH);
}

void BodyPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    verticalSeparator1_->setLook(TSS::verticalSeparatorLookFromSkin(skin));
    verticalSeparator2_->setLook(TSS::verticalSeparatorLookFromSkin(skin));
    TSS::propagateSkin(skin,
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

void BodyPanel::setBusReorderHandler(BusReorderHandler handler)
{
    if (sharedPanel_ != nullptr)
        sharedPanel_->setBusReorderHandler(std::move(handler));
}
