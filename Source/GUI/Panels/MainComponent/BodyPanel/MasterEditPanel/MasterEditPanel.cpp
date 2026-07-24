#include "MasterEditPanel.h"

#include "Modules/MidiPanel.h"
#include "Modules/VibratoPanel.h"
#include "Modules/MiscPanel.h"

#include "Core/MIDI/EditorOutboundGate.h"
#include "Core/MIDI/MasterEditGate.h"
#include "Core/Services/DeviceTypeRegistry.h"
#include "GUI/Helpers/GrayedControlHelper.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/SectionHeader.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"
#include "GUI/Factories/WidgetFactory.h"

namespace
{
    void setSubtreeKeyboardInteractionEnabled(juce::Component& root, bool enabled)
    {
        root.setWantsKeyboardFocus(enabled);
        root.setMouseClickGrabsKeyboardFocus(enabled);

        for (int i = 0; i < root.getNumChildComponents(); ++i)
        {
            if (auto* child = root.getChildComponent(i))
                setSubtreeKeyboardInteractionEnabled(*child, enabled);
        }
    }

    const char* masterBlockedFooterMessage(MatrixDeviceTypes::Type deviceType)
    {
        if (MatrixDeviceTypes::isMatrix6Family(deviceType))
            return PluginDisplayNames::MasterEditSection::kMatrix6PatchOnlyFooterMessage;

        return PluginDisplayNames::MasterEditSection::kUnsupportedDeviceFooterMessage;
    }
}

MasterEditPanel::MasterEditPanel(TSS::ISkin& skin, const MasterEditPanelDimensions& dims, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : dims_(dims)
    , skin_(&skin)
    , apvts_(apvts)
    , sectionHeader_(std::make_unique<TSS::SectionHeader>(
        dims_.sectionHeaderWidth,
        dims_.sectionHeaderHeight,
        TSS::sectionHeaderLookFromSkin(skin),
        PluginHelpers::getSectionDisplayName(PluginIDs::MasterEditSection::kGroupId),
        TSS::SectionHeader::ColourVariant::Orange))
    , midiPanel_(std::make_unique<MidiPanel>(skin, dims_.childModuleWidth, dims_.midiPanelHeight, widgetFactory, apvts, dims_.moduleHeader, dims_.parameterCell))
    , vibratoPanel_(std::make_unique<VibratoPanel>(skin, dims_.childModuleWidth, dims_.vibratoPanelHeight, widgetFactory, apvts, dims_.moduleHeader, dims_.parameterCell))
    , miscPanel_(std::make_unique<MiscPanel>(skin, dims_.childModuleWidth, dims_.miscPanelHeight, widgetFactory, apvts, dims_.moduleHeader, dims_.parameterCell))
{
    setOpaque(false);
    addAndMakeVisible(*sectionHeader_);
    addAndMakeVisible(*midiPanel_);
    addAndMakeVisible(*vibratoPanel_);
    addAndMakeVisible(*miscPanel_);

    apvts_.state.addListener(this);
    refreshDeviceGating();

    setSize(dims_.width, dims_.height);
}

MasterEditPanel::~MasterEditPanel()
{
    apvts_.state.removeListener(this);
}

void MasterEditPanel::valueTreePropertyChanged(juce::ValueTree&,
                                               const juce::Identifier& property)
{
    const auto propertyName = property.toString();
    namespace MutatorState = PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties;

    if (propertyName == MatrixDeviceTypes::kApvtsPropertyName
        || propertyName == "deviceDetected"
        || propertyName == MutatorState::kCompareActive)
    {
        refreshDeviceGating();
    }
}

void MasterEditPanel::valueTreeRedirected(juce::ValueTree&)
{
    refreshDeviceGating();
}

void MasterEditPanel::refreshDeviceGating()
{
    const bool deviceDetected = static_cast<bool>(apvts_.state.getProperty("deviceDetected", false));
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts_.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    const bool compareActive = static_cast<bool>(apvts_.state.getProperty(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StateProperties::kCompareActive,
        false));

    // Root Compare/device lock already dims the panel — skip child gray to avoid ~0.25 alpha.
    const bool rootLocked = Core::isSectionLocked(deviceDetected, compareActive);
    const bool shouldGray = ! rootLocked
        && deviceDetected
        && ! Core::isMasterEditAllowed(deviceDetected, deviceType);
    setMasterEditGrayed(shouldGray);
}

void MasterEditPanel::applyGrayedToChild(juce::Component* child, bool grayed)
{
    if (child == nullptr)
        return;

    TSS::GrayedControlHelper::applyGrayedAppearance(*child, grayed);
    child->setInterceptsMouseClicks(! grayed, ! grayed);
    setSubtreeKeyboardInteractionEnabled(*child, ! grayed);
}

void MasterEditPanel::setMasterEditGrayed(bool grayed)
{
    masterEditGrayed_ = grayed;

    applyGrayedToChild(sectionHeader_.get(), grayed);
    applyGrayedToChild(midiPanel_.get(), grayed);
    applyGrayedToChild(vibratoPanel_.get(), grayed);
    applyGrayedToChild(miscPanel_.get(), grayed);

    if (grayed)
        giveAwayKeyboardFocus();

    repaint();
}

void MasterEditPanel::showMatrix6PatchOnlyFooterMessage()
{
    const auto deviceType = Core::DeviceTypeRegistry::fromApvtsProperty(
        apvts_.state.getProperty(MatrixDeviceTypes::kApvtsPropertyName));
    TSS::GrayedControlHelper::setFooterInfoMessage(apvts_, masterBlockedFooterMessage(deviceType));
}

void MasterEditPanel::mouseDown(const juce::MouseEvent& event)
{
    if (! masterEditGrayed_)
    {
        juce::Component::mouseDown(event);
        return;
    }

    showMatrix6PatchOnlyFooterMessage();
}

void MasterEditPanel::resized()
{
    auto area = getLocalBounds();
    const float sf = uiScale_;

    const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.sectionHeaderHeight), sf);
    const int childWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.childModuleWidth), sf);
    const int midiH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.midiPanelHeight), sf);
    const int vibratoH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.vibratoPanelHeight), sf);
    const int miscH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.miscPanelHeight), sf);
    const int interModuleGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.interModuleGap), sf);

    sectionHeader_->setBounds(area.removeFromTop(sectionHeaderHeight));

    midiPanel_->setBounds(area.removeFromTop(midiH).withWidth(childWidth));
    area.removeFromTop(interModuleGap);
    vibratoPanel_->setBounds(area.removeFromTop(vibratoH).withWidth(childWidth));
    area.removeFromTop(interModuleGap);
    miscPanel_->setBounds(area.removeFromTop(miscH).withWidth(childWidth));
}

void MasterEditPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    sectionHeader_->setLook(TSS::sectionHeaderLookFromSkin(skin));
    TSS::propagateSkin(skin,
        midiPanel_.get(),
        vibratoPanel_.get(),
        miscPanel_.get());

    if (masterEditGrayed_)
        setMasterEditGrayed(true);
}

void MasterEditPanel::setInitConfirmationGate(TSS::ModuleHeader::InitConfirmationGate gate)
{
    if (midiPanel_)
        midiPanel_->setInitConfirmationGate(gate);
    if (vibratoPanel_)
        vibratoPanel_->setInitConfirmationGate(gate);
    if (miscPanel_)
        miscPanel_->setInitConfirmationGate(gate);
}

void MasterEditPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    
    if (sectionHeader_)
        sectionHeader_->setUiScale(uiScale_);
    if (midiPanel_)
        midiPanel_->setUiScale(uiScale_);
    if (vibratoPanel_)
        vibratoPanel_->setUiScale(uiScale_);
    if (miscPanel_)
        miscPanel_->setUiScale(uiScale_);
    
    resized();
    repaint();
}
