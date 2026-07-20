#include "PatchEditDisplaysPanel.h"

#include "Modules/EnvelopeDisplayApvtsBinding.h"
#include "Modules/PatchNameDisplayPanel.h"
#include "Modules/TrackGeneratorDisplayApvtsBinding.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    using EnvIds = EnvelopeDisplayApvtsBinding::ParameterIds;
    using TrackIds = TrackGeneratorDisplayApvtsBinding::ParameterIds;

    const EnvIds kEnvelope1ParameterIds {{
        PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay,
        PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack,
        PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay,
        PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain,
        PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease
    }};

    const EnvIds kEnvelope2ParameterIds {{
        PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay,
        PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack,
        PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay,
        PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain,
        PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease
    }};

    const EnvIds kEnvelope3ParameterIds {{
        PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay,
        PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack,
        PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay,
        PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain,
        PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease
    }};

    const TrackIds kTrackGeneratorParameterIds {{
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint1,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint2,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint3,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint4,
        PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackPoint5
    }};
}

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
          dims_.moduleHeader,
          apvts))
    , envelope1Binding_(std::make_unique<EnvelopeDisplayApvtsBinding>(*apvts_, envelope1Display_, kEnvelope1ParameterIds))
    , envelope2Binding_(std::make_unique<EnvelopeDisplayApvtsBinding>(*apvts_, envelope2Display_, kEnvelope2ParameterIds))
    , envelope3Binding_(std::make_unique<EnvelopeDisplayApvtsBinding>(*apvts_, envelope3Display_, kEnvelope3ParameterIds))
    , trackGeneratorBinding_(std::make_unique<TrackGeneratorDisplayApvtsBinding>(
          *apvts_, trackGeneratorDisplay_, kTrackGeneratorParameterIds))
{
    setOpaque(false);
    setSize(dims_.width, dims_.height);

    addAndMakeVisible(envelope1Display_);
    addAndMakeVisible(envelope2Display_);
    addAndMakeVisible(envelope3Display_);
    addAndMakeVisible(trackGeneratorDisplay_);
    addAndMakeVisible(*patchNameDisplayPanel_);
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
