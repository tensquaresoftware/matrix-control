#include "ParameterCell.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "GUI/Layout/Design/Design.h"
#include "GUI/Factories/WidgetFactory.h"


ParameterCell::~ParameterCell() = default;

ParameterCell::ParameterCellDimensions ParameterCell::getDimensionsForModuleType(ModuleType moduleType) const
{
    if (moduleType == ModuleType::PatchEdit)
    {
        return {
            TSS::Design::Recipes::Label::kPatchEditModule,
            TSS::Design::Recipes::ComboBox::kPatchEditModule,
            TSS::Design::PanelWidgets::Widths::HorizontalSeparator::kPatchEditModule
        };
    }

    return {
        TSS::Design::Recipes::Label::kMasterEditModule,
        TSS::Design::Recipes::ComboBox::kMasterEditModule,
        TSS::Design::PanelWidgets::Widths::HorizontalSeparator::kMasterEditModule
    };
}

ParameterCell::ParameterCell(TSS::ISkin& skin,
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

void ParameterCell::createParameterLabel(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);

    label_ = std::make_unique<TSS::Label>(
        dimensions.labelWidth,
        TSS::Design::Atoms::Heights::kLabel,
        TSS::labelLookFromSkin(skin),
        factory.getParameterDisplayName(parameterId).value_or(""));
    addAndMakeVisible(*label_);
}

void ParameterCell::createParameterWidget(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts)
{
    if (parameterType_ == ParameterType::Slider)
        createSliderWidget(skin, factory, parameterId, apvts);
    else
        createComboBoxWidget(skin, factory, parameterId, apvts);
}

void ParameterCell::createSliderWidget(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts)
{
    slider_ = factory.createIntParameterSlider(parameterId, skin);
    sliderAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        apvts,
        parameterId,
        *slider_);
    addAndMakeVisible(*slider_);
}

void ParameterCell::createComboBoxWidget(TSS::ISkin& skin, WidgetFactory& factory, const juce::String& parameterId, juce::AudioProcessorValueTreeState& apvts)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);

    comboBox_ = factory.createChoiceParameterComboBox(
        parameterId,
        skin,
        dimensions.comboBoxWidth,
        TSS::Design::Atoms::Heights::kComboBox);
    comboBoxAttachment_ = std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        apvts,
        parameterId,
        *comboBox_);
    addAndMakeVisible(*comboBox_);
}

void ParameterCell::createSeparator(TSS::ISkin& skin)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);

    separator_ = std::make_unique<TSS::HorizontalSeparator>(
        dimensions.separatorWidth,
        TSS::Design::Atoms::Heights::kHorizontalSeparator,
        TSS::horizontalSeparatorLookFromSkin(skin));
    addAndMakeVisible(*separator_);
}

void ParameterCell::resized()
{
    const float sf = uiScale_;
    const int h = getHeight();
    const int labelH = TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Atoms::Heights::kLabel), sf);
    const int sepH = juce::jmax(1, TSS::ScaledLayout::scaledInt(
        static_cast<float>(TSS::Design::Atoms::Heights::kHorizontalSeparator), sf));

    if (parameterType_ == ParameterType::None)
    {
        const int rowContentH = labelH;
        const int ySep = juce::jmin(rowContentH, juce::jmax(0, h - sepH));
        layoutSeparator(ySep, sepH);
        applyChildUiScales();
        return;
    }

    const int widgetH = (parameterType_ == ParameterType::Slider)
        ? TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Atoms::Heights::kSlider), sf)
        : TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Atoms::Heights::kComboBox), sf);
    const int rowContentH = juce::jmax(labelH, widgetH);
    const int ySep = juce::jmin(rowContentH, juce::jmax(0, h - sepH));

    layoutParameterLabel(0);
    layoutParameterWidget(0);
    layoutSeparator(ySep, sepH);
    applyChildUiScales();
}

void ParameterCell::layoutParameterLabel(int y)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const int labelWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.labelWidth), uiScale_);
    const int labelHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Atoms::Heights::kLabel), uiScale_);

    if (auto* label = label_.get())
        label->setBounds(0, y, labelWidth, labelHeight);
}

void ParameterCell::layoutParameterWidget(int y)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const int labelWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.labelWidth), uiScale_);
    const int sliderWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Recipes::Slider::kStandard), uiScale_);
    const int sliderHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Atoms::Heights::kSlider), uiScale_);
    const int comboBoxWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.comboBoxWidth), uiScale_);
    const int comboBoxHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(TSS::Design::Atoms::Heights::kComboBox), uiScale_);

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

void ParameterCell::layoutSeparator(int yTop, int separatorHeight)
{
    const auto dimensions = getDimensionsForModuleType(moduleType_);
    const int separatorWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.separatorWidth), uiScale_);

    if (auto* separator = separator_.get())
        separator->setBounds(0, yTop, separatorWidth, separatorHeight);
}

void ParameterCell::applyChildUiScales()
{
    if (label_)
        label_->setUiScale(uiScale_);
    if (slider_)
        slider_->setUiScale(uiScale_);
    if (comboBox_)
        comboBox_->setUiScale(uiScale_);
    if (separator_)
        separator_->setUiScale(uiScale_);
}

void ParameterCell::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    if (label_)
        label_->setLook(TSS::labelLookFromSkin(skin));
    if (slider_)
        slider_->setLook(TSS::sliderLookFromSkin(skin));
    if (comboBox_)
    {
        comboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        comboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    }
    if (separator_)
        separator_->setLook(TSS::horizontalSeparatorLookFromSkin(skin));
}

void ParameterCell::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;

    uiScale_ = uiScale;
    resized();
    repaint();
}

int ParameterCell::getTotalHeight() const
{
    return TSS::Design::Recipes::ParameterCell::kHeight;
}
