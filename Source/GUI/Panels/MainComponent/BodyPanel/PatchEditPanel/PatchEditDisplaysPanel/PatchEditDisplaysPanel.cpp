#include "PatchEditDisplaysPanel.h"

#include "InteractiveDisplayApvtsSync.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginDesignDimensions.h"


PatchEditDisplaysPanel::~PatchEditDisplaysPanel() = default;

PatchEditDisplaysPanel::PatchEditDisplaysPanel(tss::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts_(&apvts)
    , envelope1Display_(PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                         PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                         tss::envelopeDisplayLookFromSkin(skin))
    , envelope2Display_(PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                         PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                         tss::envelopeDisplayLookFromSkin(skin))
    , envelope3Display_(PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                         PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                         tss::envelopeDisplayLookFromSkin(skin))
    , trackGeneratorDisplay_(PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                             PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight,
                             tss::trackGeneratorDisplayLookFromSkin(skin))
    , patchNameModuleHeader_(PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                             PluginDesignDimensions::Widgets::Heights::kModuleHeader,
                             tss::moduleHeaderLookFromSkin(skin),
                             tss::ModuleHeader::ColourVariant::Blue,
                             PluginDisplayNames::PatchEditSection::PatchNameModule::kName)
    , patchNameDisplay_(PluginDesignDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                        PluginDesignDimensions::Widgets::Heights::kPatchNameDisplay,
                        tss::patchNameDisplayLookFromSkin(skin))
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
    namespace PES = PluginDesignDimensions::Panels::Body::PatchEditSection;
    namespace PEM = PES::MiddleModules;
    const float sf = uiScale_;
    const int childWidth = tss::ScaledLayout::scaledInt(static_cast<float>(PEM::ChildModules::kWidth), sf);
    const int childHeight = tss::ScaledLayout::scaledInt(static_cast<float>(PEM::ChildModules::kHeight), sf);
    const float childStep = static_cast<float>(PEM::ChildModules::kWidth + PES::kInterModuleGap) * sf;
    const int paddingTop = tss::ScaledLayout::scaledInt(
        static_cast<float>(PEM::PatchNameColumn::kTopPadding), sf);
    const int moduleHeaderHeight = tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDesignDimensions::Widgets::Heights::kModuleHeader), sf);

    envelope1Display_.setBounds(0, 0, childWidth, childHeight);
    envelope2Display_.setBounds(juce::roundToInt(1.0f * childStep), 0, childWidth, childHeight);
    envelope3Display_.setBounds(juce::roundToInt(2.0f * childStep), 0, childWidth, childHeight);
    trackGeneratorDisplay_.setBounds(juce::roundToInt(3.0f * childStep), 0, childWidth, childHeight);

    const int patchNameSectionX = juce::roundToInt(4.0f * childStep);
    const int patchNameSectionW = getWidth() - patchNameSectionX;
    patchNameModuleHeader_.setBounds(patchNameSectionX, paddingTop,
                                    patchNameSectionW, moduleHeaderHeight);

    const int patchNameDisplayY = tss::ScaledLayout::scaledInt(
        static_cast<float>(PEM::PatchNameColumn::kTopPadding + PluginDesignDimensions::Widgets::Heights::kModuleHeader
            + PEM::PatchNameColumn::kModuleHeaderToDisplayGap),
        sf);
    patchNameDisplay_.setBounds(
        patchNameSectionX,
        patchNameDisplayY,
        patchNameSectionW,
        tss::ScaledLayout::scaledInt(static_cast<float>(PluginDesignDimensions::Widgets::Heights::kPatchNameDisplay), sf));
}

void PatchEditDisplaysPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    const auto envelopeLook = tss::envelopeDisplayLookFromSkin(skin);
    envelope1Display_.setLook(envelopeLook);
    envelope2Display_.setLook(envelopeLook);
    envelope3Display_.setLook(envelopeLook);
    trackGeneratorDisplay_.setLook(tss::trackGeneratorDisplayLookFromSkin(skin));
    patchNameModuleHeader_.setLook(tss::moduleHeaderLookFromSkin(skin));
    patchNameDisplay_.setLook(tss::patchNameDisplayLookFromSkin(skin));
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
