#include "ModuleHeaderPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Button.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"


ModuleHeaderPanel::~ModuleHeaderPanel() = default;

ModuleHeaderPanel::ModuleHeaderPanel(tss::ISkin& skin,
                                     WidgetFactory& factory,
                                     const juce::String& moduleId,
                                     ButtonSet buttonSet,
                                     ModuleType moduleType,
                                     juce::AudioProcessorValueTreeState& apvts,
                                     const juce::String& initWidgetId,
                                     const juce::String& copyWidgetId,
                                     const juce::String& pasteWidgetId)
    : skin_(&skin)
    , buttonSet_(buttonSet)
    , moduleType_(moduleType)
    , apvts_(apvts)
{
    setOpaque(false);
    createModuleHeader(skin, factory, moduleId);
    createInitButton(skin, factory, initWidgetId);

    if (buttonSet == ButtonSet::InitCopyPaste)
        createCopyPasteButtons(skin, factory, copyWidgetId, pasteWidgetId);
}

void ModuleHeaderPanel::createModuleHeader(tss::ISkin&, WidgetFactory& factory, const juce::String& moduleId)
{
    const auto moduleHeaderWidth = (moduleType_ == ModuleType::PatchEdit)
        ? PluginDimensions::Widgets::Widths::ModuleHeader::kPatchEditModule
        : PluginDimensions::Widgets::Widths::ModuleHeader::kMasterEditModule;
    
    const auto colourVariant = (moduleType_ == ModuleType::PatchEdit)
        ? tss::ModuleHeader::ColourVariant::Blue
        : tss::ModuleHeader::ColourVariant::Orange;

    moduleHeader_ = std::make_unique<tss::ModuleHeader>(
        factory.getGroupDisplayName(moduleId),
        moduleHeaderWidth,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        colourVariant);
    addAndMakeVisible(*moduleHeader_);
}

void ModuleHeaderPanel::createInitButton(tss::ISkin& skin, WidgetFactory& factory, const juce::String& initWidgetId)
{
    initButton_ = factory.createStandaloneButton(initWidgetId, skin, PluginDimensions::Widgets::Heights::kButton);
    initButton_->onClick = [this, initWidgetId]
    {
        apvts_.state.setProperty(initWidgetId,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*initButton_);
}

void ModuleHeaderPanel::createCopyPasteButtons(tss::ISkin& skin, WidgetFactory& factory, const juce::String& copyWidgetId, const juce::String& pasteWidgetId)
{
    copyButton_ = factory.createStandaloneButton(copyWidgetId, skin, PluginDimensions::Widgets::Heights::kButton);
    copyButton_->onClick = [this, copyWidgetId]
    {
        apvts_.state.setProperty(copyWidgetId,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*copyButton_);

    pasteButton_ = factory.createStandaloneButton(pasteWidgetId, skin, PluginDimensions::Widgets::Heights::kButton);
    pasteButton_->onClick = [this, pasteWidgetId]
    {
        apvts_.state.setProperty(pasteWidgetId,
                                juce::Time::getCurrentTime().toMilliseconds(),
                                nullptr);
    };
    addAndMakeVisible(*pasteButton_);
}

void ModuleHeaderPanel::resized()
{
    layoutModuleHeader();

    if (buttonSet_ == ButtonSet::InitCopyPaste)
        layoutInitCopyPasteButtons();
    else
        layoutInitOnlyButtons();
    
    if (initButton_)
        initButton_->setScalingFactor(scalingFactor_);
    if (copyButton_)
        copyButton_->setScalingFactor(scalingFactor_);
    if (pasteButton_)
        pasteButton_->setScalingFactor(scalingFactor_);
}

void ModuleHeaderPanel::layoutModuleHeader()
{
    const auto moduleHeaderWidth = (moduleType_ == ModuleType::PatchEdit)
        ? PluginDimensions::Widgets::Widths::ModuleHeader::kPatchEditModule
        : PluginDimensions::Widgets::Widths::ModuleHeader::kMasterEditModule;
    const auto moduleHeaderHeight = PluginDimensions::Widgets::Heights::kModuleHeader;
    const int y = 0;

    if (auto* header = moduleHeader_.get())
        header->setBounds(0, y, moduleHeaderWidth, moduleHeaderHeight);
}

void ModuleHeaderPanel::layoutInitOnlyButtons()
{
    const auto initButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto panelWidth = getWidth();
    const int y = 0;

    if (auto* button = initButton_.get())
        button->setBounds(panelWidth - initButtonWidth, y, initButtonWidth, buttonHeight);
}

void ModuleHeaderPanel::layoutInitCopyPasteButtons()
{
    const auto initButtonWidth = PluginDimensions::Widgets::Widths::Button::kInit;
    const auto copyButtonWidth = PluginDimensions::Widgets::Widths::Button::kCopy;
    const auto pasteButtonWidth = PluginDimensions::Widgets::Widths::Button::kPaste;
    const auto buttonHeight = PluginDimensions::Widgets::Heights::kButton;
    const auto panelWidth = getWidth();
    const int y = 0;

    if (auto* button = pasteButton_.get())
        button->setBounds(panelWidth - pasteButtonWidth, y, pasteButtonWidth, buttonHeight);

    if (auto* button = copyButton_.get())
        button->setBounds(panelWidth - pasteButtonWidth - copyButtonWidth, y, copyButtonWidth, buttonHeight);

    if (auto* button = initButton_.get())
        button->setBounds(panelWidth - pasteButtonWidth - copyButtonWidth - initButtonWidth, y, initButtonWidth, buttonHeight);
}

void ModuleHeaderPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    if (moduleHeader_)
        moduleHeader_->setLook(tss::moduleHeaderLookFromSkin(skin));
    if (initButton_)
        initButton_->setLook(tss::buttonLookFromSkin(skin));
    if (copyButton_)
        copyButton_->setLook(tss::buttonLookFromSkin(skin));
    if (pasteButton_)
        pasteButton_->setLook(tss::buttonLookFromSkin(skin));
}

void ModuleHeaderPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    resized();
    repaint();
}
