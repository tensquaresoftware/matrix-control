#include "PatchEditDisplaysPanel.h"

#include "Modules/InteractiveDisplayApvtsSync.h"
#include "Modules/PatchNameDisplayPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"

PatchEditDisplaysPanel::~PatchEditDisplaysPanel() = default;

PatchEditDisplaysPanel::PatchEditDisplaysPanel(TSS::ISkin& skin, const PatchEditDisplaysPanelDimensions& dims, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(&apvts)
    , envelope1Display_(dims_.childBand, TSS::envelopeDisplayLookFromSkin(skin))
    , envelope2Display_(dims_.childBand, TSS::envelopeDisplayLookFromSkin(skin))
    , envelope3Display_(dims_.childBand, TSS::envelopeDisplayLookFromSkin(skin))
    , trackGeneratorDisplay_(dims_.trackGeneratorBand, TSS::trackGeneratorDisplayLookFromSkin(skin))
    , patchNameDisplayPanel_(std::make_unique<PatchNameDisplayPanel>(
          skin,
          dims_.patchName.width,
          dims_.childBand.height,
          dims_.patchName,
          dims_.moduleHeader))
    , apvtsSync_(std::make_unique<InteractiveDisplayApvtsSync>(
        apvts,
        envelope1Display_,
        envelope2Display_,
        envelope3Display_,
        trackGeneratorDisplay_))
{
    setOpaque(false);
    setSize(dims_.width, dims_.height);

    apvtsSync_->syncAllFromApvts();

    addAndMakeVisible(envelope1Display_);
    addAndMakeVisible(envelope2Display_);
    addAndMakeVisible(envelope3Display_);
    addAndMakeVisible(trackGeneratorDisplay_);
    addAndMakeVisible(*patchNameDisplayPanel_);
}

void PatchEditDisplaysPanel::connectSliderFastPaths(PatchEditTopModulesPanel& topModulesPanel,
                                                    PatchEditBottomModulesPanel& bottomModulesPanel)
{
    if (apvtsSync_ != nullptr)
        apvtsSync_->connectSliderFastPaths(topModulesPanel, bottomModulesPanel);
}

void PatchEditDisplaysPanel::resized()
{
    const float sf = uiScale_;
    const int childWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.childBand.width), sf);
    const int childHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.childBand.height), sf);
    const float childStep = static_cast<float>(dims_.childBand.width + dims_.interModuleGap) * sf;

    envelope1Display_.setBounds(0, 0, childWidth, childHeight);
    envelope2Display_.setBounds(juce::roundToInt(1.0f * childStep), 0, childWidth, childHeight);
    envelope3Display_.setBounds(juce::roundToInt(2.0f * childStep), 0, childWidth, childHeight);
    trackGeneratorDisplay_.setBounds(juce::roundToInt(3.0f * childStep), 0, childWidth, childHeight);

    const int patchNameSectionX = juce::roundToInt(4.0f * childStep);
    const int patchNameSectionW = getWidth() - patchNameSectionX;
    const int patchNameSectionH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.childBand.height), sf);

    if (patchNameDisplayPanel_ != nullptr)
        patchNameDisplayPanel_->setBounds(patchNameSectionX, 0, patchNameSectionW, patchNameSectionH);
}

void PatchEditDisplaysPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    const auto envelopeLook = TSS::envelopeDisplayLookFromSkin(skin);
    envelope1Display_.setLook(envelopeLook);
    envelope2Display_.setLook(envelopeLook);
    envelope3Display_.setLook(envelopeLook);
    trackGeneratorDisplay_.setLook(TSS::trackGeneratorDisplayLookFromSkin(skin));
    if (patchNameDisplayPanel_ != nullptr)
        patchNameDisplayPanel_->setSkin(skin);
}

void PatchEditDisplaysPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    envelope1Display_.setUiScale(uiScale_);
    envelope2Display_.setUiScale(uiScale_);
    envelope3Display_.setUiScale(uiScale_);
    trackGeneratorDisplay_.setUiScale(uiScale_);
    if (patchNameDisplayPanel_ != nullptr)
        patchNameDisplayPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}
