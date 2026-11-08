// Extracted from InternalPatchesPanel.cpp for modular maintenance.
// Widget construction and initial wiring for the Internal Patches module.

#include "InternalPatchesPanel.h"

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/SkinValues.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/NumberBox.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

#include <juce_core/juce_core.h>

namespace
{
    void dispatchTimestampAction(juce::AudioProcessorValueTreeState& apvts, const juce::Identifier& propertyId)
    {
        apvts.state.setProperty(propertyId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    }
}

std::unique_ptr<TSS::Button> InternalPatchesPanel::makeTimestampActionButton(
    TSS::ISkin& skin,
    WidgetFactory& widgetFactory,
    const juce::Identifier& actionId)
{
    auto button = widgetFactory.createStandaloneButton(actionId.toString(), skin, dims_.buttons.height);
    button->onClick = [this, actionId]
    {
        dispatchTimestampAction(apvts_, actionId);
    };
    addAndMakeVisible(*button);
    return button;
}

void InternalPatchesPanel::setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory, const juce::String& moduleId)
{
    moduleHeader = std::make_unique<TSS::ModuleHeader>(
        dims_.moduleHeader.patchManagerTitleBandWidth,
        dims_.moduleHeader.height,
        TSS::moduleHeaderLookFromSkin(skin),
        TSS::ModuleHeader::ColourVariant::Blue,
        widgetFactory.getGroupDisplayName(moduleId),
        dims_.moduleHeader);
    moduleHeader->setTitleClickHandler([this]
    {
        dispatchTimestampAction(
            apvts_,
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kHeaderClick);
    });
    addAndMakeVisible(*moduleHeader);
}

void InternalPatchesPanel::setupBrowserGroupLabel(TSS::ISkin& skin)
{
    browserGroupLabel = std::make_unique<TSS::GroupLabel>(
        dims_.groupLabels.internalPatchesBrowserWidth,
        dims_.groupLabels.height,
        TSS::groupLabelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kBrowser);
    addAndMakeVisible(*browserGroupLabel);
}

void InternalPatchesPanel::setupLoadPreviousPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadPreviousPatchButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadPreviousPatch);
}

void InternalPatchesPanel::setupLoadNextPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    loadNextPatchButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kLoadNextPatch);
}

void InternalPatchesPanel::setupCurrentBankNumberBox(TSS::ISkin& skin)
{
    currentBankNumber = std::make_unique<TSS::NumberBox>(
        TSS::numberBoxLookFromSkin(skin),
        TSS::NumberBox::Config {
            .width = dims_.numberBoxes.bankNumberWidth,
            .height = dims_.numberBoxes.height,
            .editable = false,
            .minValue = Matrix1000Limits::kMinBankNumber,
            .maxValue = Matrix1000Limits::kMaxBankNumber });
    addAndMakeVisible(*currentBankNumber);
}

void InternalPatchesPanel::setupCurrentPatchNumberBox(TSS::ISkin& skin)
{
    currentPatchNumber = std::make_unique<TSS::NumberBox>(
        TSS::numberBoxLookFromSkin(skin),
        TSS::NumberBox::Config {
            .width = dims_.numberBoxes.patchNumberWidth,
            .height = dims_.numberBoxes.height,
            .editable = true,
            .minValue = Matrix1000Limits::kMinPatchNumber,
            .maxValue = Matrix1000Limits::kMaxPatchNumber });

    // Push value to APVTS via property (same pattern as buttons)
    currentPatchNumber->setOnValueChanged([this](int newValue)
    {
        // Avoid feedback loop when the value already came from the ValueTree
        const auto currentPropertyValue = apvts_.state.getProperty(
            PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber, -1);

        if (static_cast<int>(currentPropertyValue) != newValue)
        {
            apvts_.state.setProperty(
                PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCurrentPatchNumber,
                newValue,
                nullptr);
        }
    });

    addAndMakeVisible(*currentPatchNumber);
}

void InternalPatchesPanel::setupMemoryGroupLabel(TSS::ISkin& skin)
{
    memoryGroupLabel = std::make_unique<TSS::GroupLabel>(
        dims_.groupLabels.internalPatchesMemoryWidth,
        dims_.groupLabels.height,
        TSS::groupLabelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kMemory);
    addAndMakeVisible(*memoryGroupLabel);
}

void InternalPatchesPanel::setupInitPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    initPatchButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch);
}

void InternalPatchesPanel::setupCopyPatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    copyPatchButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch);
}

void InternalPatchesPanel::setupPastePatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    pastePatchButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch);
}

void InternalPatchesPanel::setupStorePatchButton(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    storePatchButton_ = makeTimestampActionButton(
        skin,
        widgetFactory,
        PluginIDs::PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch);
}
