#include "PatchNameDisplayPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/PatchNameDisplay.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    using PluginIDs::PatchEditSection::PatchNameModule::kPatchName;
    using PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName;
}

PatchNameDisplayPanel::PatchNameDisplayPanel(TSS::ISkin& skin,
                                             int width,
                                             int height,
                                             const PatchNameDisplayDimensions& patchNameDims,
                                             const ModuleHeaderDimensions& moduleHeaderDims,
                                             juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , patchNameDims_(patchNameDims)
    , moduleHeaderDims_(moduleHeaderDims)
    , apvts_(apvts)
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

    apvts_.state.addListener(this);
    syncFromApvtsState();
}

PatchNameDisplayPanel::~PatchNameDisplayPanel()
{
    apvts_.state.removeListener(this);
}

TSS::PatchNameDisplay& PatchNameDisplayPanel::getPatchNameDisplay()
{
    return *patchNameDisplay_;
}

void PatchNameDisplayPanel::resized()
{
    auto area = getLocalBounds();
    const float sf = uiScale_;

    area.removeFromTop(TSS::ScaledLayout::scaledInt(static_cast<float>(patchNameDims_.topPadding), sf));

    if (moduleHeader_ != nullptr)
        moduleHeader_->setBounds(area.removeFromTop(
            TSS::ScaledLayout::scaledInt(static_cast<float>(moduleHeaderDims_.height), sf)));

    area.removeFromTop(TSS::ScaledLayout::scaledInt(
        static_cast<float>(patchNameDims_.moduleHeaderToDisplayGap), sf));

    area.removeFromBottom(TSS::ScaledLayout::scaledInt(
        static_cast<float>(patchNameDims_.bottomPadding), sf));

    if (patchNameDisplay_ != nullptr)
        patchNameDisplay_->setBounds(area.removeFromTop(
            TSS::ScaledLayout::scaledInt(static_cast<float>(patchNameDims_.height), sf)));
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

void PatchNameDisplayPanel::valueTreePropertyChanged(juce::ValueTree&,
                                                     const juce::Identifier& property)
{
    if (property.toString() != kPatchName)
        return;

    syncFromApvtsState();
}

void PatchNameDisplayPanel::valueTreeRedirected(juce::ValueTree&)
{
    syncFromApvtsState();
}

void PatchNameDisplayPanel::syncFromApvtsState()
{
    if (patchNameDisplay_ == nullptr)
        return;

    auto name = apvts_.state.getProperty(kPatchName, juce::String(kDefaultPatchName)).toString();
    if (name.isEmpty())
        name = kDefaultPatchName;

    patchNameDisplay_->setPatchName(name);
}
