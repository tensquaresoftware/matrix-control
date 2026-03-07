#include "ParameterPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "Shared/Definitions/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"


ParameterPanel::~ParameterPanel() = default;

ParameterPanel::ParameterPanelDimensions ParameterPanel::getDimensionsForModuleType(ModuleType moduleType) const
{
    if (moduleType == ModuleType::PatchEdit)
    {
        return {
            PluginDimensions::Widgets::Widths::Label::kPatchEditModule,
            PluginDimensions::Widgets::Widths::ComboBox::kPatchEditModule,
            PluginDimensions::Widgets::Widths::HorizontalSeparator::kPatchEditModule
        };
    }
    
    return {
        PluginDimensions::Widgets::Widths::Label::kMasterEditModule,
        PluginDimensions::Widgets::Widths::ComboBox::kMasterEditModule,
        PluginDimensions::Widgets::Widths::HorizontalSeparator::kMasterEditModule
    };
}

ParameterPanel::ParameterPanel(tss::ISkin& skin,
                                WidgetFactory& factory,
                                const juce::String& parameterId,
                                ParameterType type,
                                ModuleType moduleType,
                                juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , parameterType_(type)
    , moduleType_(moduleType)
{
    setOpaque(false);
    if (type == ParameterType::None)
    {
        createSeparator(skin);
    }
    else
    {
        createParameterLabel(skin, factory, parameterId);
        createParameterWidget(skin, factory, parameterId, apvts);
        createSeparator(skin);
    }
}

void ParameterPanel::createParameterLabel(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);

    label_ = std::make_unique<tss::Label>(
        dimensions.labelWidth,
        PluginDimensions::Widgets::Heights::kLabel,
        factory.getParameterDisplayName(parameterId).value_or(""));
    label_->setLook(tss::labelLookFromSkin(skin));
    addAndMakeVisible(*label_);
}

void ParameterPanel::createParameterWidget(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts)
{
    if (parameterType_ == ParameterType::Slider)
        createSliderWidget(skin, factory, parameterId, apvts);
    else
        createComboBoxWidget(skin, factory, parameterId, apvts);
}

void ParameterPanel::createSliderWidget(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts)
{
    slider_ = factory.createIntParameterSlider(parameterId, skin);
    sliderAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts,
        parameterId,
        *slider_);
    addAndMakeVisible(*slider_);
}

void ParameterPanel::createComboBoxWidget(tss::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);

    comboBox_ = factory.createChoiceParameterComboBox(
        parameterId,
        skin,
        dimensions.comboBoxWidth,
        PluginDimensions::Widgets::Heights::kComboBox);
    comboBoxAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts,
        parameterId,
        *comboBox_);
    addAndMakeVisible(*comboBox_);
}

void ParameterPanel::createSeparator(tss::ISkin& skin)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);

    separator_ = std::make_unique<tss::HorizontalSeparator>(
        dimensions.separatorWidth,
        PluginDimensions::Widgets::Heights::kHorizontalSeparator);
    separator_->setLook(tss::horizontalSeparatorLookFromSkin(skin));
    addAndMakeVisible(*separator_);
}

void ParameterPanel::resized()
{
    const auto labelHeight = PluginDimensions::Widgets::Heights::kLabel;

    if (parameterType_ == ParameterType::None)
    {
        layoutSeparator(labelHeight);
    }
    else
    {
        int y = 0;

        layoutParameterLabel(y);
        layoutParameterWidget(y);
        y += labelHeight;
        layoutSeparator(y);
    }
    
    if (label_)
        label_->setScalingFactor(scalingFactor_);
    if (slider_)
        slider_->setScalingFactor(scalingFactor_);
    if (comboBox_)
        comboBox_->setScalingFactor(scalingFactor_);
    if (separator_)
        separator_->setScalingFactor(scalingFactor_);
}

void ParameterPanel::layoutParameterLabel(int y)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const auto labelHeight = PluginDimensions::Widgets::Heights::kLabel;

    if (auto* label = label_.get())
        label->setBounds(0, y, dimensions.labelWidth, labelHeight);
}

void ParameterPanel::layoutParameterWidget(int y)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const auto sliderWidth = PluginDimensions::Widgets::Widths::Slider::kStandard;
    const auto sliderHeight = PluginDimensions::Widgets::Heights::kSlider;
    const auto comboBoxHeight = PluginDimensions::Widgets::Heights::kComboBox;

    if (parameterType_ == ParameterType::Slider)
    {
        if (auto* slider = slider_.get())
            slider->setBounds(dimensions.labelWidth, y, sliderWidth, sliderHeight);
    }
    else
    {
        if (auto* comboBox = comboBox_.get())
            comboBox->setBounds(dimensions.labelWidth, y, dimensions.comboBoxWidth, comboBoxHeight);
    }
}

void ParameterPanel::layoutSeparator(int y)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const auto separatorHeight = PluginDimensions::Widgets::Heights::kHorizontalSeparator;

    if (auto* separator = separator_.get())
        separator->setBounds(0, y, dimensions.separatorWidth, separatorHeight);
}

void ParameterPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    if (label_)
        label_->setLook(tss::labelLookFromSkin(skin));
    if (slider_)
        slider_->setLook(tss::sliderLookFromSkin(skin));
    if (comboBox_)
    {
        comboBox_->setLook(tss::comboBoxLookFromSkin(skin));
        comboBox_->setPopupMenuLook(tss::popupMenuLookFromSkin(skin));
    }
    if (separator_)
        separator_->setLook(tss::horizontalSeparatorLookFromSkin(skin));
}

void ParameterPanel::setScalingFactor(float scalingFactor)
{
    if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
        return;
    
    scalingFactor_ = scalingFactor;
    resized();
    repaint();
}

int ParameterPanel::getTotalHeight() const
{
    return PluginDimensions::Widgets::Heights::kLabel + PluginDimensions::Widgets::Heights::kHorizontalSeparator;
}
