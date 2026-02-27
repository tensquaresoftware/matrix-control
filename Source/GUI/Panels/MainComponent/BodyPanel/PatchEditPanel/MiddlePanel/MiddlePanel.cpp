#include "MiddlePanel.h"

#include "GUI/Skins/Skin.h"
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
    , envelope1Display_(skin, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kWidth, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kHeight)
    , envelope2Display_(skin, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kWidth, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kHeight)
    , envelope3Display_(skin, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kWidth, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kHeight)
    , trackGeneratorDisplay_(skin, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kWidth, PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kHeight)
    , patchNameModuleHeader_(skin, PluginDisplayNames::PatchEditSection::PatchNameModule::kName,
                             PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kWidth,
                             PluginDimensions::Widgets::Heights::kModuleHeader,
                             tss::ModuleHeader::ColourVariant::Blue)
    , patchNameDisplay_(skin,
                        PluginDimensions::Panels::Body::PatchEdit::Middle::ChildPanels::kWidth,
                        PluginDimensions::Panels::Body::PatchEdit::Middle::kPatchNameDisplayHeight)
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
    using namespace PluginDimensions::Panels::Body::PatchEdit::Middle;
    const auto childWidth = ChildPanels::kWidth;
    const auto childHeight = ChildPanels::kHeight;

    envelope1Display_.setBounds(0, 0, childWidth, childHeight);
    envelope2Display_.setBounds(envelope1Display_.getBounds().getRight() + kSpacing, 0, childWidth, childHeight);
    envelope3Display_.setBounds(envelope2Display_.getBounds().getRight() + kSpacing, 0, childWidth, childHeight);
    trackGeneratorDisplay_.setBounds(envelope3Display_.getBounds().getRight() + kSpacing, 0, childWidth, childHeight);

    const auto patchNameSectionX = trackGeneratorDisplay_.getBounds().getRight() + kSpacing;
    const auto moduleHeaderHeight = PluginDimensions::Widgets::Heights::kModuleHeader;
    patchNameModuleHeader_.setBounds(patchNameSectionX, kPatchNameSectionPaddingTop,
                                    patchNameDisplay_.getWidth(), moduleHeaderHeight);
    patchNameDisplay_.setBounds(patchNameSectionX,
                               kPatchNameSectionPaddingTop + moduleHeaderHeight + kPatchNameSectionSpacing,
                               patchNameDisplay_.getWidth(),
                               patchNameDisplay_.getHeight());
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

