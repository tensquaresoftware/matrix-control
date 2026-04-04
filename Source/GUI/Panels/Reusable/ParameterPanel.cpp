#include "ParameterPanel.h"

#include "GUI/Layout/ScaledLayout.h"
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
    const float sf = displayScale_;
    const int h = getHeight();
    const int labelH = tss::ScaledLayout::scaledInt(static_cast<float>(PluginDimensions::Widgets::Heights::kLabel), sf);
    const int sepH = juce::jmax(1, tss::ScaledLayout::scaledInt(
        static_cast<float>(PluginDimensions::Widgets::Heights::kHorizontalSeparator), sf));

    if (parameterType_ == ParameterType::None)
    {
        const int rowContentH = labelH;
        const int ySep = juce::jmin(rowContentH, juce::jmax(0, h - sepH));
        layoutSeparator(ySep, sepH);
        applyChildDisplayScales();
        return;
    }

    const int widgetH = (parameterType_ == ParameterType::Slider)
        ? tss::ScaledLayout::scaledInt(static_cast<float>(PluginDimensions::Widgets::Heights::kSlider), sf)
        : tss::ScaledLayout::scaledInt(static_cast<float>(PluginDimensions::Widgets::Heights::kComboBox), sf);
    const int rowContentH = juce::jmax(labelH, widgetH);
    const int ySep = juce::jmin(rowContentH, juce::jmax(0, h - sepH));

    layoutParameterLabel(0);
    layoutParameterWidget(0);
    layoutSeparator(ySep, sepH);
    applyChildDisplayScales();
}

void ParameterPanel::layoutParameterLabel(int y)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const int labelWidth = tss::ScaledLayout::scaledInt(static_cast<float>(dimensions.labelWidth), displayScale_);
    const int labelHeight = tss::ScaledLayout::scaledInt(static_cast<float>(PluginDimensions::Widgets::Heights::kLabel), displayScale_);

    if (auto* label = label_.get())
        label->setBounds(0, y, labelWidth, labelHeight);
}

void ParameterPanel::layoutParameterWidget(int y)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const int labelWidth = tss::ScaledLayout::scaledInt(static_cast<float>(dimensions.labelWidth), displayScale_);
    const int sliderWidth = tss::ScaledLayout::scaledInt(static_cast<float>(PluginDimensions::Widgets::Widths::Slider::kStandard), displayScale_);
    const int sliderHeight = tss::ScaledLayout::scaledInt(static_cast<float>(PluginDimensions::Widgets::Heights::kSlider), displayScale_);
    const int comboBoxWidth = tss::ScaledLayout::scaledInt(static_cast<float>(dimensions.comboBoxWidth), displayScale_);
    const int comboBoxHeight = tss::ScaledLayout::scaledInt(static_cast<float>(PluginDimensions::Widgets::Heights::kComboBox), displayScale_);

    if (parameterType_ == ParameterType::Slider)
    {
        if (auto* slider = slider_.get())
            slider->setBounds(labelWidth, y, sliderWidth, sliderHeight);
    }
    else
    {
        if (auto* comboBox = comboBox_.get())
            comboBox->setBounds(labelWidth, y, comboBoxWidth, comboBoxHeight);
    }
}

void ParameterPanel::layoutSeparator(int yTop, int separatorHeight)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const int separatorWidth = tss::ScaledLayout::scaledInt(static_cast<float>(dimensions.separatorWidth), displayScale_);

    if (auto* separator = separator_.get())
        separator->setBounds(0, yTop, separatorWidth, separatorHeight);
}

void ParameterPanel::applyChildDisplayScales()
{
    if (label_)
        label_->setDisplayScale(displayScale_);
    if (slider_)
        slider_->setDisplayScale(displayScale_);
    if (comboBox_)
        comboBox_->setDisplayScale(displayScale_);
    if (separator_)
        separator_->setDisplayScale(displayScale_);
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

void ParameterPanel::setDisplayScale(float displayScale)
{
    if (juce::approximatelyEqual(displayScale_, displayScale))
        return;
    
    displayScale_ = displayScale;
    resized();
    repaint();
}

int ParameterPanel::getTotalHeight() const
{
    return PluginDimensions::Widgets::Heights::kLabel + PluginDimensions::Widgets::Heights::kHorizontalSeparator;
}
