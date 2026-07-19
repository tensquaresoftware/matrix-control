#include "MasterEditPanel.h"

#include "Modules/MidiPanel.h"
#include "Modules/VibratoPanel.h"
#include "Modules/MiscPanel.h"

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
    if (propertyName == MatrixDeviceTypes::kApvtsPropertyName
        || propertyName == "deviceDetected")
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

    // Align with Core master gate: only Matrix-1000 keeps MASTER interactive when detected.
    const bool shouldGray = deviceDetected && ! Core::isMasterEditAllowed(deviceDetected, deviceType);
    setMasterEditGrayed(shouldGray);
}

void MasterEditPanel::applyGrayedToChild(juce::Component* child, bool grayed)
{
    if (child == nullptr)
        return;

    TSS::GrayedControlHelper::applyGrayedAppearance(*child, grayed);
    child->setInterceptsMouseClicks(! grayed, ! grayed);
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
    TSS::GrayedControlHelper::setFooterInfoMessage(
        apvts_,
        PluginDisplayNames::MasterEditSection::kMatrix6PatchOnlyFooterMessage);
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
    const auto bounds = getLocalBounds();
    const float sf = uiScale_;

    const int sectionHeaderHeight = TSS::ScaledLayout::scaledInt(
        static_cast<float>(dims_.sectionHeaderHeight), sf);
    const int childWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.childModuleWidth), sf);
    const int midiH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.midiPanelHeight), sf);
    const int vibratoH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.vibratoPanelHeight), sf);
    const int miscH = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.miscPanelHeight), sf);
    const int interModuleGap = TSS::ScaledLayout::scaledInt(static_cast<float>(dims_.interModuleGap), sf);

    sectionHeader_->setBounds(bounds.getX(), bounds.getY(), bounds.getWidth(), sectionHeaderHeight);

    int y = bounds.getY() + sectionHeaderHeight;
    midiPanel_->setBounds(bounds.getX(), y, childWidth, midiH);
    y += midiH + interModuleGap;
    vibratoPanel_->setBounds(bounds.getX(), y, childWidth, vibratoH);
    y += vibratoH + interModuleGap;
    miscPanel_->setBounds(bounds.getX(), y, childWidth, miscH);
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
