#include "PatchNameDisplayPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "Shared/Definitions/PluginDisplayNames.h"

PatchNameDisplayPanel::PatchNameDisplayPanel(TSS::ISkin& skin,
                                             int width,
                                             int height,
                                             const PatchNameDisplayDimensions& patchNameDims,
                                             const ModuleHeaderDimensions& moduleHeaderDims)
    : width_(width)
    , height_(height)
    , patchNameDims_(patchNameDims)
    , moduleHeaderDims_(moduleHeaderDims)
    , moduleHeader_(std::make_unique<TSS::ModuleHeader>(
          patchNameDims_.width,
          moduleHeaderDims_.height,
          TSS::moduleHeaderLookFromSkin(skin),
          TSS::ModuleHeader::ColourVariant::Blue,
          PluginDisplayNames::PatchEditSection::PatchNameModule::kName,
          moduleHeaderDims_))
    , patchNameDisplay_(std::make_unique<TSS::PatchNameDisplay>(
          patchNameDims_.width,
          patchNameDims_.height,
          TSS::patchNameDisplayLookFromSkin(skin)))
{
    setOpaque(false);
    setSize(width_, height_);
    addAndMakeVisible(*moduleHeader_);
    addAndMakeVisible(*patchNameDisplay_);
}

PatchNameDisplayPanel::~PatchNameDisplayPanel() = default;

TSS::PatchNameDisplay& PatchNameDisplayPanel::getPatchNameDisplay()
{
    return *patchNameDisplay_;
}

void PatchNameDisplayPanel::resized()
{
    const float sf = uiScale_;
    const int w = getWidth();
    const int topPad = TSS::ScaledLayout::scaledInt(static_cast<float>(patchNameDims_.topPadding), sf);
    const int headerH = TSS::ScaledLayout::scaledInt(static_cast<float>(moduleHeaderDims_.height), sf);
    const int bottomPad = TSS::ScaledLayout::scaledInt(static_cast<float>(patchNameDims_.bottomPadding), sf);
    const int displayH = TSS::ScaledLayout::scaledInt(static_cast<float>(patchNameDims_.height), sf);
    const int displayY = getHeight() - bottomPad - displayH;

    if (moduleHeader_ != nullptr)
        moduleHeader_->setBounds(0, topPad, w, headerH);

    if (patchNameDisplay_ != nullptr)
        patchNameDisplay_->setBounds(0, displayY, w, displayH);
}

void PatchNameDisplayPanel::setSkin(TSS::ISkin& skin)
{
    if (moduleHeader_ != nullptr)
        moduleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));
    if (patchNameDisplay_ != nullptr)
        patchNameDisplay_->setLook(TSS::patchNameDisplayLookFromSkin(skin));
}

void PatchNameDisplayPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;

    if (moduleHeader_ != nullptr)
        moduleHeader_->setUiScale(uiScale_);
    if (patchNameDisplay_ != nullptr)
        patchNameDisplay_->setUiScale(uiScale_);

    resized();
    repaint();
}
