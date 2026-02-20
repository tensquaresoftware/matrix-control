#include "MiddlePanel.h"

#include "GUI/Themes/Skin.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginDisplayNames.h"


MiddlePanel::~MiddlePanel()
{
    if (apvts_ != nullptr && apvts_->state.isValid())
        apvts_->state.removeListener(this);
}

MiddlePanel::MiddlePanel(tss::Skin& skin, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , apvts_(&apvts)
    , envelope1Display_(skin, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kWidth, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kHeight)
    , envelope2Display_(skin, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kWidth, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kHeight)
    , envelope3Display_(skin, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kWidth, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kHeight)
    , trackGeneratorDisplay_(skin, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kWidth, PluginDimensions::Panels::PatchEditPanel::MiddlePanel::ChildPanels::kHeight)
    , patchNameModuleHeader_(skin, PluginDescriptors::ModuleDisplayNames::kPatchName,
                             PluginDimensions::Panels::PatchEditPanel::MiddlePanel::PatchNameSection::kWidth,
                             PluginDimensions::Widgets::Heights::kModuleHeader,
                             tss::ModuleHeader::ColourVariant::Blue)
    , patchNameDisplay_(skin,
                        PluginDimensions::Panels::PatchEditPanel::MiddlePanel::PatchNameSection::kPatchNameDisplayWidth,
                        PluginDimensions::Panels::PatchEditPanel::MiddlePanel::PatchNameSection::kPatchNameDisplayHeight)
{
    setOpaque(false);
    setSize(getWidth(), getHeight());

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
    using namespace PluginDimensions::Panels::PatchEditPanel::MiddlePanel;
    const auto childWidth = ChildPanels::kWidth;
    const auto childHeight = ChildPanels::kHeight;

    envelope1Display_.setBounds(0, 0, childWidth, childHeight);
    envelope2Display_.setBounds(envelope1Display_.getBounds().getRight() + kSpacing, 0, childWidth, childHeight);
    envelope3Display_.setBounds(envelope2Display_.getBounds().getRight() + kSpacing, 0, childWidth, childHeight);
    trackGeneratorDisplay_.setBounds(envelope3Display_.getBounds().getRight() + kSpacing, 0, childWidth, childHeight);

    const auto patchNameSectionX = trackGeneratorDisplay_.getBounds().getRight() + kSpacing;
    const auto moduleHeaderHeight = PluginDimensions::Widgets::Heights::kModuleHeader;
    patchNameModuleHeader_.setBounds(patchNameSectionX, PatchNameSection::kPaddingTop,
                                    PatchNameSection::kWidth, moduleHeaderHeight);
    patchNameDisplay_.setBounds(patchNameSectionX, PatchNameSection::kPaddingTop + moduleHeaderHeight + PatchNameSection::kSpacing,
                               PatchNameSection::kPatchNameDisplayWidth, PatchNameSection::kPatchNameDisplayHeight);
}

void MiddlePanel::valueTreePropertyChanged(juce::ValueTree& treeWhosePropertyHasChanged,
                                          const juce::Identifier& property)
{
    if (property != juce::Identifier("value"))
        return;

    const juce::String typeStr = treeWhosePropertyHasChanged.getType().toString();
    if (typeStr != "PARAM")
        return;

    const juce::var idVar = treeWhosePropertyHasChanged.getProperty("id");
    if (!idVar.isString())
        return;

    const juce::String paramId = idVar.toString();
    if (paramId == PluginDescriptors::ParameterIds::kTrackPoint1)
    {
        trackGeneratorDisplay_.setTrackPoint1(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kTrackPoint2)
    {
        trackGeneratorDisplay_.setTrackPoint2(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kTrackPoint3)
    {
        trackGeneratorDisplay_.setTrackPoint3(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kTrackPoint4)
    {
        trackGeneratorDisplay_.setTrackPoint4(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kTrackPoint5)
    {
        trackGeneratorDisplay_.setTrackPoint5(getTrackPointValueFromApvts(paramId), false);
        return;
    }
    
    if (paramId == PluginDescriptors::ParameterIds::kEnv1Delay)
    {
        envelope1Display_.setDelay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv1Attack)
    {
        envelope1Display_.setAttack(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv1Decay)
    {
        envelope1Display_.setDecay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv1Sustain)
    {
        envelope1Display_.setSustain(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv1Release)
    {
        envelope1Display_.setRelease(getEnvParameterFromApvts(paramId), false);
        return;
    }
    
    if (paramId == PluginDescriptors::ParameterIds::kEnv2Delay)
    {
        envelope2Display_.setDelay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv2Attack)
    {
        envelope2Display_.setAttack(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv2Decay)
    {
        envelope2Display_.setDecay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv2Sustain)
    {
        envelope2Display_.setSustain(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv2Release)
    {
        envelope2Display_.setRelease(getEnvParameterFromApvts(paramId), false);
        return;
    }
    
    if (paramId == PluginDescriptors::ParameterIds::kEnv3Delay)
    {
        envelope3Display_.setDelay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv3Attack)
    {
        envelope3Display_.setAttack(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv3Decay)
    {
        envelope3Display_.setDecay(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv3Sustain)
    {
        envelope3Display_.setSustain(getEnvParameterFromApvts(paramId), false);
        return;
    }
    if (paramId == PluginDescriptors::ParameterIds::kEnv3Release)
    {
        envelope3Display_.setRelease(getEnvParameterFromApvts(paramId), false);
    }
}

void MiddlePanel::syncTrackGeneratorDisplayFromApvts()
{
    if (apvts_ == nullptr)
        return;

    trackGeneratorDisplay_.setTrackPoint1(getTrackPointValueFromApvts(PluginDescriptors::ParameterIds::kTrackPoint1), false);
    trackGeneratorDisplay_.setTrackPoint2(getTrackPointValueFromApvts(PluginDescriptors::ParameterIds::kTrackPoint2), false);
    trackGeneratorDisplay_.setTrackPoint3(getTrackPointValueFromApvts(PluginDescriptors::ParameterIds::kTrackPoint3), false);
    trackGeneratorDisplay_.setTrackPoint4(getTrackPointValueFromApvts(PluginDescriptors::ParameterIds::kTrackPoint4), false);
    trackGeneratorDisplay_.setTrackPoint5(getTrackPointValueFromApvts(PluginDescriptors::ParameterIds::kTrackPoint5), false);
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
    
    envelope1Display_.setDelay(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv1Delay), false);
    envelope1Display_.setAttack(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv1Attack), false);
    envelope1Display_.setDecay(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv1Decay), false);
    envelope1Display_.setSustain(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv1Sustain), false);
    envelope1Display_.setRelease(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv1Release), false);
    
    envelope2Display_.setDelay(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv2Delay), false);
    envelope2Display_.setAttack(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv2Attack), false);
    envelope2Display_.setDecay(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv2Decay), false);
    envelope2Display_.setSustain(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv2Sustain), false);
    envelope2Display_.setRelease(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv2Release), false);
    
    envelope3Display_.setDelay(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv3Delay), false);
    envelope3Display_.setAttack(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv3Attack), false);
    envelope3Display_.setDecay(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv3Decay), false);
    envelope3Display_.setSustain(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv3Sustain), false);
    envelope3Display_.setRelease(getEnvParameterFromApvts(PluginDescriptors::ParameterIds::kEnv3Release), false);
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

void MiddlePanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;
    envelope1Display_.setSkin(skin);
    envelope2Display_.setSkin(skin);
    envelope3Display_.setSkin(skin);
    trackGeneratorDisplay_.setSkin(skin);
    patchNameModuleHeader_.setSkin(skin);
    patchNameDisplay_.setSkin(skin);
}

