#include "MiddlePanel.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/ApvtsTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginDimensions.h"


MiddlePanel::~MiddlePanel()
{
    if (apvts_ != nullptr && apvts_->state.isValid())
        apvts_->state.removeListener(this);
}

MiddlePanel::MiddlePanel(tss::ISkin& skin, int width, int height, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts_(&apvts)
    , envelope1Display_(PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth, PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight)
    , envelope2Display_(PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth, PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight)
    , envelope3Display_(PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth, PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight)
    , trackGeneratorDisplay_(PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth, PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kHeight)
    , patchNameModuleHeader_(PluginDisplayNames::PatchEditSection::PatchNameModule::kName,
                             PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                             PluginDimensions::Widgets::Heights::kModuleHeader,
                             tss::ModuleHeader::ColourVariant::Blue)
    , patchNameDisplay_(PluginDimensions::Panels::Body::PatchEditSection::MiddleModules::ChildModules::kWidth,
                        PluginDimensions::Widgets::Heights::kPatchNameDisplay)
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

void MiddlePanel::resized()
{
    using namespace PluginDimensions::Panels::Body::PatchEditSection::MiddleModules;
    const int childWidth = juce::roundToInt(static_cast<float>(ChildModules::kWidth) * scalingFactor_);
    const int childHeight = juce::roundToInt(static_cast<float>(ChildModules::kHeight) * scalingFactor_);
    const float childStep = static_cast<float>(ChildModules::kWidth + kSpacing) * scalingFactor_;
    const int paddingTop = juce::roundToInt(static_cast<float>(kPatchNameSectionPaddingTop) * scalingFactor_);
    const int moduleHeaderHeight = juce::roundToInt(static_cast<float>(PluginDimensions::Widgets::Heights::kModuleHeader) * scalingFactor_);

    envelope1Display_.setBounds(0, 0, childWidth, childHeight);
    envelope2Display_.setBounds(juce::roundToInt(1.0f * childStep), 0, childWidth, childHeight);
    envelope3Display_.setBounds(juce::roundToInt(2.0f * childStep), 0, childWidth, childHeight);
    trackGeneratorDisplay_.setBounds(juce::roundToInt(3.0f * childStep), 0, childWidth, childHeight);

    const int patchNameSectionX = juce::roundToInt(4.0f * childStep);
    const int patchNameSectionW = getWidth() - patchNameSectionX;
    patchNameModuleHeader_.setBounds(patchNameSectionX, paddingTop,
                                    patchNameSectionW, moduleHeaderHeight);

    // Y of patchNameDisplay computed independently to avoid rounding accumulation
    const int patchNameDisplayY = juce::roundToInt(static_cast<float>(kPatchNameSectionPaddingTop + PluginDimensions::Widgets::Heights::kModuleHeader + kPatchNameSectionSpacing) * scalingFactor_);
    patchNameDisplay_.setBounds(patchNameSectionX,
                               patchNameDisplayY,
                               patchNameSectionW,
                               juce::roundToInt(static_cast<float>(PluginDimensions::Widgets::Heights::kPatchNameDisplay) * scalingFactor_));
}

void MiddlePanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
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

void MiddlePanel::syncTrackGeneratorDisplayFromApvts()
{
    if (apvts_ == nullptr)
        return;

    trackGeneratorDisplay_.setTrackPoint1(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1), false);
    trackGeneratorDisplay_.setTrackPoint2(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2), false);
    trackGeneratorDisplay_.setTrackPoint3(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3), false);
    trackGeneratorDisplay_.setTrackPoint4(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4), false);
    trackGeneratorDisplay_.setTrackPoint5(getTrackPointValueFromApvts(PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5), false);
}

int MiddlePanel::getTrackPointValueFromApvts(const juce::String& parameterId) const
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

void MiddlePanel::syncEnvelopeDisplaysFromApvts()
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

int MiddlePanel::getEnvParameterFromApvts(const juce::String& parameterId) const
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

void MiddlePanel::setSkin(tss::ISkin& skin)
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

void MiddlePanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    
    envelope1Display_.setScalingFactor(scalingFactor_);
    envelope2Display_.setScalingFactor(scalingFactor_);
    envelope3Display_.setScalingFactor(scalingFactor_);
    trackGeneratorDisplay_.setScalingFactor(scalingFactor_);
    patchNameModuleHeader_.setScalingFactor(scalingFactor_);
    patchNameDisplay_.setScalingFactor(scalingFactor_);
    
    resized();
    repaint();
}

