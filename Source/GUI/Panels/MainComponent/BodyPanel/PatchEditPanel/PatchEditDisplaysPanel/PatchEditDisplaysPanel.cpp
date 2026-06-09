#include "PatchEditDisplaysPanel.h"

#include "InteractiveDisplayApvtsSync.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "GUI/Layout/Design/Design.h"


PatchEditDisplaysPanel::~PatchEditDisplaysPanel() = default;

PatchEditDisplaysPanel::PatchEditDisplaysPanel(TSS::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts_(&apvts)
    , envelope1Display_(TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                         TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                         TSS::envelopeDisplayLookFromSkin(skin))
    , envelope2Display_(TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                         TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                         TSS::envelopeDisplayLookFromSkin(skin))
    , envelope3Display_(TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                         TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                         TSS::envelopeDisplayLookFromSkin(skin))
    , trackGeneratorDisplay_(TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                             TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                             TSS::trackGeneratorDisplayLookFromSkin(skin))
    , patchNameModuleHeader_(TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                             TSS::Design::Atoms::Heights::kModuleHeader,
                             TSS::moduleHeaderLookFromSkin(skin),
                             TSS::ModuleHeader::ColourVariant::Blue,
                             PluginDisplayNames::PatchEditSection::PatchNameModule::kName)
    , patchNameDisplay_(TSS::Design::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                        TSS::Design::Atoms::Heights::kPatchNameDisplay,
                        TSS::patchNameDisplayLookFromSkin(skin))
    , apvtsSync_(std::make_unique<InteractiveDisplayApvtsSync>(
        apvts,
        envelope1Display_,
        envelope2Display_,
        envelope3Display_,
        trackGeneratorDisplay_))
{
    setOpaque(false);
    setSize(width_, height_);

    apvtsSync_->syncAllFromApvts();

    addAndMakeVisible(envelope1Display_);
    addAndMakeVisible(envelope2Display_);
    addAndMakeVisible(envelope3Display_);
    addAndMakeVisible(trackGeneratorDisplay_);
    addAndMakeVisible(patchNameModuleHeader_);
    addAndMakeVisible(patchNameDisplay_);
}

void PatchEditDisplaysPanel::connectSliderFastPaths(PatchEditTopModulesPanel& topModulesPanel,
                                                    PatchEditBottomModulesPanel& bottomModulesPanel)
{
    if (apvtsSync_ != nullptr)
        apvtsSync_->connectSliderFastPaths(topModulesPanel, bottomModulesPanel);
}

void PatchEditDisplaysPanel::resized()
{
    namespace PES = TSS::Design::Panels::Body::PatchEditSection;
    namespace PEM = PES::MiddleModules;
    const float sf = uiScale_;
    const int childWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(PEM::ChildModules::kWidth), sf);
    const int childHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(PEM::ChildModules::kHeight), sf);
    const float childStep = static_cast<float>(PEM::ChildModules::kWidth + PES::kInterModuleGap) * sf;
    const int paddingTop = TSS::ScaledLayout::scaledInt(
        static_cast<float>(PEM::PatchNameModule::kTopPadding), sf);
    const int moduleHeaderHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(TSS::Design::Atoms::Heights::kModuleHeader), sf);

    envelope1Display_.setBounds(0, 0, childWidth, childHeight);
    envelope2Display_.setBounds(juce::roundToInt(1.0f * childStep), 0, childWidth, childHeight);
    envelope3Display_.setBounds(juce::roundToInt(2.0f * childStep), 0, childWidth, childHeight);
    trackGeneratorDisplay_.setBounds(juce::roundToInt(3.0f * childStep), 0, childWidth, childHeight);

    const int patchNameSectionX = juce::roundToInt(4.0f * childStep);
    const int patchNameSectionW = getWidth() - patchNameSectionX;
    patchNameModuleHeader_.setBounds(patchNameSectionX, paddingTop,
                                    patchNameSectionW, moduleHeaderHeight);

    const int patchNameDisplayY = TSS::ScaledLayout::scaledInt(
        static_cast<float>(PEM::PatchNameModule::kTopPadding + TSS::Design::Atoms::Heights::kModuleHeader
            + PEM::PatchNameModule::kModuleHeaderToDisplayGap),
        sf);
    patchNameDisplay_.setBounds(
        patchNameSectionX,
        patchNameDisplayY,
        patchNameSectionW,
        TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Atoms::Heights::kPatchNameDisplay), sf));
}

void PatchEditDisplaysPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    const auto envelopeLook = TSS::envelopeDisplayLookFromSkin(skin);
    envelope1Display_.setLook(envelopeLook);
    envelope2Display_.setLook(envelopeLook);
    envelope3Display_.setLook(envelopeLook);
    trackGeneratorDisplay_.setLook(TSS::trackGeneratorDisplayLookFromSkin(skin));
    patchNameModuleHeader_.setLook(TSS::moduleHeaderLookFromSkin(skin));
    patchNameDisplay_.setLook(TSS::patchNameDisplayLookFromSkin(skin));
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
    patchNameModuleHeader_.setUiScale(uiScale_);
    patchNameDisplay_.setUiScale(uiScale_);
    
    resized();
    repaint();
}
