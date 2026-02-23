#include "PatchMutatorPanel.h"

#include "GUI/Themes/Skin.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/PluginDescriptors.h"
#include "Shared/PluginDimensions.h"
#include "GUI/Factories/WidgetFactory.h"

namespace
{
    constexpr int kLabelWidth = 45;
    constexpr int kLabelHeight = 20;
    constexpr int kSliderWidth = 45;
    constexpr int kSliderHeight = 20;
    constexpr int kComboBoxWidth = 45;
    constexpr int kComboBoxHeight = 20;
    constexpr int kToggleWidth = 20;
    constexpr int kToggleHeight = 20;
}

PatchMutatorPanel::PatchMutatorPanel(tss::Skin& skin, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory);
    setupAmountLine(skin, widgetFactory);
    setupRandomLine(skin, widgetFactory);
    setupHistoryLine(skin, widgetFactory);
    setSize(getWidth(), getHeight());
}

PatchMutatorPanel::~PatchMutatorPanel() = default;

void PatchMutatorPanel::setupModuleHeader(tss::Skin& skin, WidgetFactory& widgetFactory)
{
    moduleHeader_ = std::make_unique<tss::ModuleHeader>(
        skin,
        widgetFactory.getGroupDisplayName(PluginDescriptors::ModuleIds::kPatchMutator),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*moduleHeader_);
}

void PatchMutatorPanel::setupAmountLine(tss::Skin& skin, WidgetFactory& widgetFactory)
{
    amountLabel_ = std::make_unique<tss::Label>(
        skin,
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kAmount);
    addAndMakeVisible(*amountLabel_);

    amountSlider_ = std::make_unique<tss::Slider>(skin, kSliderWidth, kSliderHeight, 0.0);
    amountSlider_->setRange(0.0, 100.0, 1.0);
    amountSlider_->setValue(0.0);
    amountSlider_->setUnit(PluginDescriptors::DisplayNames::ChoiceLists::Units::kPercent);
    amountSlider_->onValueChange = [this]
    {
        apvts_.state.setProperty(
            PluginDescriptors::StandaloneWidgetIds::kPatchMutatorAmount,
            static_cast<int>(amountSlider_->getValue()),
            nullptr);
    };
    addAndMakeVisible(*amountSlider_);

    mutateButton_ = widgetFactory.createStandaloneButton(
        PluginDescriptors::StandaloneWidgetIds::kPatchMutatorMutate,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(mutateButton_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorMutate);
    addAndMakeVisible(*mutateButton_);

    dco1Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kDco1);
    connectToggleToApvts(dco1Toggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorDco1);
    addAndMakeVisible(*dco1Toggle_);

    dco2Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kDco2);
    connectToggleToApvts(dco2Toggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorDco2);
    addAndMakeVisible(*dco2Toggle_);

    vcfVcaToggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kVcfVca);
    connectToggleToApvts(vcfVcaToggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorVcfVca);
    addAndMakeVisible(*vcfVcaToggle_);

    fmTrackToggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kFmTrack);
    connectToggleToApvts(fmTrackToggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorFmTrack);
    addAndMakeVisible(*fmTrackToggle_);

    rampPortamentoToggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kRampPortamento);
    connectToggleToApvts(rampPortamentoToggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorRampPortamento);
    addAndMakeVisible(*rampPortamentoToggle_);
}

void PatchMutatorPanel::setupRandomLine(tss::Skin& skin, WidgetFactory& widgetFactory)
{
    randomLabel_ = std::make_unique<tss::Label>(
        skin,
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kRandom);
    addAndMakeVisible(*randomLabel_);

    randomSlider_ = std::make_unique<tss::Slider>(skin, kSliderWidth, kSliderHeight, 0.0);
    randomSlider_->setRange(0.0, 100.0, 1.0);
    randomSlider_->setValue(0.0);
    randomSlider_->setUnit(PluginDescriptors::DisplayNames::ChoiceLists::Units::kPercent);
    randomSlider_->onValueChange = [this]
    {
        apvts_.state.setProperty(
            PluginDescriptors::StandaloneWidgetIds::kPatchMutatorRandom,
            static_cast<int>(randomSlider_->getValue()),
            nullptr);
    };
    addAndMakeVisible(*randomSlider_);

    retryButton_ = widgetFactory.createStandaloneButton(
        PluginDescriptors::StandaloneWidgetIds::kPatchMutatorRetry,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(retryButton_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorRetry);
    addAndMakeVisible(*retryButton_);

    env1Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kEnvelope1);
    connectToggleToApvts(env1Toggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorEnv1);
    addAndMakeVisible(*env1Toggle_);

    env2Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kEnvelope2);
    connectToggleToApvts(env2Toggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorEnv2);
    addAndMakeVisible(*env2Toggle_);

    env3Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kEnvelope3);
    connectToggleToApvts(env3Toggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorEnv3);
    addAndMakeVisible(*env3Toggle_);

    lfo1Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kLfo1);
    connectToggleToApvts(lfo1Toggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorLfo1);
    addAndMakeVisible(*lfo1Toggle_);

    lfo2Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kLfo2);
    connectToggleToApvts(lfo2Toggle_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorLfo2);
    addAndMakeVisible(*lfo2Toggle_);
}

void PatchMutatorPanel::setupHistoryLine(tss::Skin& skin, WidgetFactory& widgetFactory)
{
    historyLabel_ = std::make_unique<tss::Label>(
        skin,
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kHistory);
    addAndMakeVisible(*historyLabel_);

    historyComboBox_ = std::make_unique<tss::ComboBox>(
        skin,
        PluginDimensions::Widgets::Widths::ComboBox::kPatchMutatorHistory,
        kComboBoxHeight,
        tss::ComboBox::Style::Standard);
    historyComboBox_->addItem(PluginDescriptors::DisplayNames::PatchManager::PatchMutator::StandaloneWidgets::kEmptyHistory, 1);
    historyComboBox_->setSelectedId(1);
    historyComboBox_->onChange = [this]
    {
        if (auto* comboBox = historyComboBox_.get())
        {
            apvts_.state.setProperty(
                PluginDescriptors::StandaloneWidgetIds::kPatchMutatorHistory,
                comboBox->getSelectedId(),
                nullptr);
        }
    };
    addAndMakeVisible(*historyComboBox_);

    compareButton_ = widgetFactory.createStandaloneButton(
        PluginDescriptors::StandaloneWidgetIds::kPatchMutatorCompare,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(compareButton_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorCompare);
    addAndMakeVisible(*compareButton_);

    deleteButton_ = widgetFactory.createStandaloneButton(
        PluginDescriptors::StandaloneWidgetIds::kPatchMutatorDelete,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(deleteButton_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorDelete);
    addAndMakeVisible(*deleteButton_);

    clearButton_ = widgetFactory.createStandaloneButton(
        PluginDescriptors::StandaloneWidgetIds::kPatchMutatorClear,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(clearButton_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorClear);
    addAndMakeVisible(*clearButton_);

    exportButton_ = widgetFactory.createStandaloneButton(
        PluginDescriptors::StandaloneWidgetIds::kPatchMutatorExport,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(exportButton_.get(), PluginDescriptors::StandaloneWidgetIds::kPatchMutatorExport);
    addAndMakeVisible(*exportButton_);
}

void PatchMutatorPanel::connectButtonToApvts(tss::Button* button, const char* widgetId)
{
    if (button != nullptr)
    {
        button->onClick = [this, widgetId]
        {
            apvts_.state.setProperty(widgetId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
        };
    }
}

void PatchMutatorPanel::connectToggleToApvts(tss::Toggle* toggle, const char* widgetId)
{
    if (toggle != nullptr)
    {
        toggle->onStateChange = [this, toggle, widgetId]
        {
            apvts_.state.setProperty(widgetId, toggle->getToggleState(), nullptr);
        };
    }
}

void PatchMutatorPanel::resized()
{
    int x = 0;
    int y = 0;

    layoutModuleHeader(x, y);
    y += PluginDimensions::Widgets::Heights::kModuleHeader + kSpacing_;

    layoutAmountLine(x, y);
    layoutRandomLine(x, y);
    layoutHistoryLine(x, y);
}

void PatchMutatorPanel::layoutModuleHeader(int x, int y)
{
    const auto width = PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule;
    const auto height = PluginDimensions::Widgets::Heights::kModuleHeader;

    if (auto* header = moduleHeader_.get())
        header->setBounds(x, y, width, height);
}

void PatchMutatorPanel::layoutAmountLine(int x, int& y)
{
    const int widgetY = y;

    if (auto* label = amountLabel_.get())
        label->setBounds(x, widgetY, kLabelWidth, kLabelHeight);
    x += kLabelWidth + kSpacing_;

    if (auto* slider = amountSlider_.get())
        slider->setBounds(x, widgetY, kSliderWidth, kSliderHeight);
    x += kSliderWidth + kSpacing_;

    if (auto* button = mutateButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), kLabelHeight);
    x += mutateButton_->getWidth() + kSpacing_;

    if (auto* toggle = dco1Toggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = dco2Toggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = vcfVcaToggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = fmTrackToggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = rampPortamentoToggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);

    y += kLabelHeight + kSpacing_;
}

void PatchMutatorPanel::layoutRandomLine(int x, int& y)
{
    const int widgetY = y;

    if (auto* label = randomLabel_.get())
        label->setBounds(x, widgetY, kLabelWidth, kLabelHeight);
    x += kLabelWidth + kSpacing_;

    if (auto* slider = randomSlider_.get())
        slider->setBounds(x, widgetY, kSliderWidth, kSliderHeight);
    x += kSliderWidth + kSpacing_;

    if (auto* button = retryButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), kLabelHeight);
    x += retryButton_->getWidth() + kSpacing_;

    if (auto* toggle = env1Toggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = env2Toggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = env3Toggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = lfo1Toggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);
    x += kToggleWidth + kSpacing_;
    if (auto* toggle = lfo2Toggle_.get())
        toggle->setBounds(x, widgetY, kToggleWidth, kToggleHeight);

    y += kLabelHeight + kSpacing_;
}

void PatchMutatorPanel::layoutHistoryLine(int x, int& y)
{
    const int widgetY = y;

    if (auto* label = historyLabel_.get())
        label->setBounds(x, widgetY, kLabelWidth, kLabelHeight);
    x += kLabelWidth + kSpacing_;

    if (auto* comboBox = historyComboBox_.get())
        comboBox->setBounds(x, widgetY, kComboBoxWidth, kComboBoxHeight);
    x += kComboBoxWidth + kSpacing_;

    if (auto* button = compareButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), kLabelHeight);
    x += compareButton_->getWidth() + kSpacing_;

    if (auto* button = deleteButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), kLabelHeight);
    x += deleteButton_->getWidth() + kSpacing_;

    if (auto* button = clearButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), kLabelHeight);
    x += clearButton_->getWidth() + kSpacing_;

    if (auto* button = exportButton_.get())
        button->setBounds(x, widgetY, button->getWidth(), kLabelHeight);

    y += kLabelHeight;
}

void PatchMutatorPanel::setSkin(tss::Skin& skin)
{
    skin_ = &skin;

    if (auto* header = moduleHeader_.get())
        header->setSkin(skin);

    if (auto* label = amountLabel_.get())
        label->setSkin(skin);
    if (auto* slider = amountSlider_.get())
        slider->setSkin(skin);
    if (auto* label = randomLabel_.get())
        label->setSkin(skin);
    if (auto* slider = randomSlider_.get())
        slider->setSkin(skin);
    if (auto* label = historyLabel_.get())
        label->setSkin(skin);

    if (auto* comboBox = historyComboBox_.get())
        comboBox->setSkin(skin);

    if (mutateButton_)
        mutateButton_->setSkin(skin);
    if (retryButton_)
        retryButton_->setSkin(skin);
    if (compareButton_)
        compareButton_->setSkin(skin);
    if (deleteButton_)
        deleteButton_->setSkin(skin);
    if (clearButton_)
        clearButton_->setSkin(skin);
    if (exportButton_)
        exportButton_->setSkin(skin);

    if (dco1Toggle_)
        dco1Toggle_->setSkin(skin);
    if (dco2Toggle_)
        dco2Toggle_->setSkin(skin);
    if (vcfVcaToggle_)
        vcfVcaToggle_->setSkin(skin);
    if (fmTrackToggle_)
        fmTrackToggle_->setSkin(skin);
    if (rampPortamentoToggle_)
        rampPortamentoToggle_->setSkin(skin);
    if (env1Toggle_)
        env1Toggle_->setSkin(skin);
    if (env2Toggle_)
        env2Toggle_->setSkin(skin);
    if (env3Toggle_)
        env3Toggle_->setSkin(skin);
    if (lfo1Toggle_)
        lfo1Toggle_->setSkin(skin);
    if (lfo2Toggle_)
        lfo2Toggle_->setSkin(skin);
}
