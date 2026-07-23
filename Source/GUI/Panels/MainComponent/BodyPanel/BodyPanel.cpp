#include "BodyPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/VerticalSeparator.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Helpers/CompareLockBinder.h"
#include "PatchEditPanel/PatchEditPanel.h"
#include "MasterEditPanel/MasterEditPanel.h"
#include "SharedPanel/SharedPanel.h"
#include "GUI/Factories/WidgetFactory.h"

using TSS::SkinColourId;

using ::TSS::VerticalSeparator;

BodyPanel::BodyPanel(TSS::ISkin& skin,
                     const GuiLayoutDimensions& layoutDimensions,
                     WidgetFactory& widgetFactory,
                     juce::AudioProcessorValueTreeState& apvts,
                     const Core::PatchFileService& patchFileService)
    : dims_(layoutDimensions.body)
    , skin_(&skin)
{
    setOpaque(true);
    patchEditPanel_ = std::make_unique<PatchEditPanel>(
        skin, dims_.patchEdit, layoutDimensions.patchEditParameterCell, layoutDimensions.patchEditModuleHeader, widgetFactory, apvts);
    addAndMakeVisible(*patchEditPanel_);

    verticalSeparator1_ = std::make_unique<VerticalSeparator>(
        dims_.separators.verticalStandardWidth,
        dims_.separators.verticalStandardHeight,
        TSS::verticalSeparatorLookFromSkin(skin),
        dims_.separators);
    addAndMakeVisible(*verticalSeparator1_);

    sharedPanel_ = std::make_unique<SharedPanel>(skin, dims_.shared, widgetFactory, apvts, patchFileService);
    addAndMakeVisible(*sharedPanel_);

    verticalSeparator2_ = std::make_unique<VerticalSeparator>(
        dims_.separators.verticalStandardWidth,
        dims_.separators.verticalStandardHeight,
        TSS::verticalSeparatorLookFromSkin(skin),
        dims_.separators);
    addAndMakeVisible(*verticalSeparator2_);

    masterEditPanel_ = std::make_unique<MasterEditPanel>(skin, dims_.masterEdit, widgetFactory, apvts);
    addAndMakeVisible(*masterEditPanel_);

    compareLockBinder_ = std::make_unique<TSS::CompareLockBinder>(
        apvts,
        std::vector<juce::Component*>{ patchEditPanel_.get(), masterEditPanel_.get() });
}

BodyPanel::~BodyPanel() = default;

void BodyPanel::paint(juce::Graphics& g)
{
    if (skin_ != nullptr)
        g.fillAll(skin_->getColour(SkinColourId::kBodyPanelBackground));
}

void BodyPanel::resized()
{
    auto area = getLocalBounds();
    const float sf = uiScale_;

    const int padding              = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.padding), sf);
    const int patchEditPanelWidth  = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.patchEditWidth), sf);
    const int patchEditPanelHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.patchEditHeight), sf);
    const int sharedColumnW        = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.sharedColumnWidth), sf);
    const int masterEditW          = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.masterEditWidth), sf);
    const int masterEditH          = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.masterEditHeight), sf);
    const int separatorW           = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.separators.verticalStandardWidth), sf);
    const int separatorH           = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.separators.verticalStandardHeight), sf);
    const int sharedColumnHeight   = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.sharedColumnHeight), sf);

    area.removeFromLeft(padding);
    area.removeFromRight(padding);

    const int topY = padding;

    const auto patchEditBounds = area.removeFromLeft(patchEditPanelWidth);
    patchEditPanel_->setBounds(patchEditBounds.getX(), topY, patchEditBounds.getWidth(), patchEditPanelHeight);

    // Full Body height (not content band): joins Header/Footer thick borders through padding gutters.
    const auto separator1Bounds = area.removeFromLeft(separatorW);
    verticalSeparator1_->setBounds(separator1Bounds.getX(), 0, separator1Bounds.getWidth(), separatorH);

    const auto sharedBounds = area.removeFromLeft(sharedColumnW);
    sharedPanel_->setBounds(sharedBounds.getX(), topY, sharedBounds.getWidth(), sharedColumnHeight);

    const auto separator2Bounds = area.removeFromLeft(separatorW);
    verticalSeparator2_->setBounds(separator2Bounds.getX(), 0, separator2Bounds.getWidth(), separatorH);

    const auto masterEditBounds = area.removeFromLeft(masterEditW);
    masterEditPanel_->setBounds(masterEditBounds.getX(), topY, masterEditBounds.getWidth(), masterEditH);
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

void BodyPanel::setMasterInitConfirmationGate(TSS::ModuleHeader::InitConfirmationGate gate)
{
    if (masterEditPanel_ != nullptr)
        masterEditPanel_->setInitConfirmationGate(std::move(gate));
}
