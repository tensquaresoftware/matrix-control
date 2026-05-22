#include "PatchEditDisplaysPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginDesignDimensions.h"


PatchEditDisplaysPanel::~PatchEditDisplaysPanel()
{
    if (apvts_ != nullptr && apvts_->state.isValid())
        apvts_->state.removeListener(this);
}

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
{
    setOpaque(false);
    setSize(width_, height_);

    apvts_->state.addListener(this);
    syncTrackGeneratorDisplayFromApvts();
    syncEnvelopeDisplaysFromApvts();

    addAndMakeVisible(envelope1Display_);
    addAndMakeVisible(envelope2Display_);
    addAndMakeVisible(envelope3Display_);
    addAndMakeVisible(trackGeneratorDisplay_);
    addAndMakeVisible(patchNameModuleHeader_);
    addAndMakeVisible(patchNameDisplay_);
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

void PatchEditDisplaysPanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                          const juce::Identifier& property)
{
    if (property != juce::Identifier(ApvtsTypes::kValue))
        return;

    const juce::String typeStr = treeWhosePropertyHasChanged.getType().toString();
    if (typeStr != "PARAM")
        return;

    const juce::var idVar = treeWhosePropertyHasChanged.getProperty("id");
    if (!idVar.isString())
        return;

    const juce::String paramId = idVar.toString();
    if (paramId == PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1)
    {
        trackGeneratorDisplay_.setTrackPoint1(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2)
    {
        trackGeneratorDisplay_.setTrackPoint2(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3)
    {
        trackGeneratorDisplay_.setTrackPoint3(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4)
    {
        trackGeneratorDisplay_.setTrackPoint4(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5)
    {
        trackGeneratorDisplay_.setTrackPoint5(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    
    if (paramId == PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay)
    {
        envelope1Display_.setDelay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack)
    {
        envelope1Display_.setAttack(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay)
    {
        envelope1Display_.setDecay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain)
    {
        envelope1Display_.setSustain(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease)
    {
        envelope1Display_.setRelease(getEnvParameterFromApvts(paramId), false);
        return;
    }
    
    if (paramId == PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay)
    {
        envelope2Display_.setDelay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack)
    {
        envelope2Display_.setAttack(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay)
    {
        envelope2Display_.setDecay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain)
    {
        envelope2Display_.setSustain(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease)
    {
        envelope2Display_.setRelease(getEnvParameterFromApvts(paramId), false);
        return;
    }
    
    if (paramId == PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay)
    {
        envelope3Display_.setDelay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack)
    {
        envelope3Display_.setAttack(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay)
    {
        envelope3Display_.setDecay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain)
    {
        envelope3Display_.setSustain(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease)
    {
        envelope3Display_.setRelease(getEnvParameterFromApvts(paramId), false);
    }
}

void PatchEditDisplaysPanel::syncTrackGeneratorDisplayFromApvts()
{
    if (apvts_ == nullptr)
        return;

    trackGeneratorDisplay_.setTrackPoint1(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1), false);
    trackGeneratorDisplay_.setTrackPoint2(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2), false);
    trackGeneratorDisplay_.setTrackPoint3(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3), false);
    trackGeneratorDisplay_.setTrackPoint4(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4), false);
    trackGeneratorDisplay_.setTrackPoint5(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5), false);
}

int PatchEditDisplaysPanel::getTrackPointValueFromApvts(const juce::String& parameterId) const
{
    if (apvts_ == nullptr)
        return 0;

    if (auto* param = apvts_->getParameter(parameterId))
    {
        const float normalised = param->getValue();
        return juce::jlimit(0, kTrackPointMax, juce::roundToInt(normalised * static_cast<float>(kTrackPointMax)));
    }

    return 0;
}

void PatchEditDisplaysPanel::syncEnvelopeDisplaysFromApvts()
{
    if (apvts_ == nullptr)
        return;
    
    envelope1Display_.setDelay(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay), false);
    envelope1Display_.setAttack(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack), false);
    envelope1Display_.setDecay(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay), false);
    envelope1Display_.setSustain(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain), false);
    envelope1Display_.setRelease(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease), false);
    
    envelope2Display_.setDelay(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay), false);
    envelope2Display_.setAttack(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack), false);
    envelope2Display_.setDecay(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay), false);
    envelope2Display_.setSustain(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain), false);
    envelope2Display_.setRelease(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease), false);
    
    envelope3Display_.setDelay(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay), false);
    envelope3Display_.setAttack(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack), false);
    envelope3Display_.setDecay(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay), false);
    envelope3Display_.setSustain(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain), false);
    envelope3Display_.setRelease(getEnvParameterFromApvts(PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease), false);
}

int PatchEditDisplaysPanel::getEnvParameterFromApvts(const juce::String& parameterId) const
{
    if (apvts_ == nullptr)
        return 0;
    
    if (auto* param = apvts_->getParameter(parameterId))
    {
        const float normalised = param->getValue();
        return juce::jlimit(0, kEnvParameterMax, juce::roundToInt(normalised * static_cast<float>(kEnvParameterMax)));
    }
    
    return 0;
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

