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

    if (moduleHeader_)
        moduleHeader_->setScalingFactor(scalingFactor_);
    if (initButton_)
        initButton_->setScalingFactor(scalingFactor_);
    if (copyButton_)
        copyButton_->setScalingFactor(scalingFactor_);
    if (pasteButton_)
        pasteButton_->setScalingFactor(scalingFactor_);
}

void ModuleHeaderPanel::layoutModuleHeader()
{
    const int baseWidth = (moduleType_ == ModuleType::PatchEdit)
        ? PluginDimensions::Widgets::Widths::ModuleHeader::kPatchEditModule
        : PluginDimensions::Widgets::Widths::ModuleHeader::kMasterEditModule;
    const int moduleHeaderWidth = juce::roundToInt(static_cast<float>(baseWidth) * scalingFactor_);
    const int moduleHeaderHeight = juce::roundToInt(
        static_cast<float>(PluginDimensions::Widgets::Heights::kModuleHeader) * scalingFactor_);

    if (auto* header = moduleHeader_.get())
        header->setBounds(0, 0, moduleHeaderWidth, moduleHeaderHeight);
}

void ModuleHeaderPanel::layoutInitOnlyButtons()
{
    const int initButtonWidth = juce::roundToInt(
        static_cast<float>(PluginDimensions::Widgets::Widths::Button::kInit) * scalingFactor_);
    const int buttonHeight = juce::roundToInt(
        static_cast<float>(PluginDimensions::Widgets::Heights::kButton) * scalingFactor_);
    const int panelWidth = getWidth();

    if (auto* button = initButton_.get())
        button->setBounds(panelWidth - initButtonWidth, 0, initButtonWidth, buttonHeight);
}

void ModuleHeaderPanel::layoutInitCopyPasteButtons()
{
    const float sf = scalingFactor_;
    const int buttonHeight = juce::roundToInt(static_cast<float>(PluginDimensions::Widgets::Heights::kButton) * sf);
    const int panelWidth = getWidth();

    // X positions computed from right edge independently to avoid rounding accumulation
    const float pasteW  = static_cast<float>(PluginDimensions::Widgets::Widths::Button::kPaste) * sf;
    const float copyW   = static_cast<float>(PluginDimensions::Widgets::Widths::Button::kCopy) * sf;
    const float initW   = static_cast<float>(PluginDimensions::Widgets::Widths::Button::kInit) * sf;

    const int pasteButtonWidth = juce::roundToInt(pasteW);
    const int copyButtonWidth  = juce::roundToInt(copyW);
    const int initButtonWidth  = juce::roundToInt(initW);

    const int pasteX = panelWidth - juce::roundToInt(pasteW);
    const int copyX  = panelWidth - juce::roundToInt(pasteW + copyW);
    const int initX  = panelWidth - juce::roundToInt(pasteW + copyW + initW);

    if (auto* button = pasteButton_.get())
        button->setBounds(pasteX, 0, pasteButtonWidth, buttonHeight);
    if (auto* button = copyButton_.get())
        button->setBounds(copyX, 0, copyButtonWidth, buttonHeight);
    if (auto* button = initButton_.get())
        button->setBounds(initX, 0, initButtonWidth, buttonHeight);
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
    repaint();
}
