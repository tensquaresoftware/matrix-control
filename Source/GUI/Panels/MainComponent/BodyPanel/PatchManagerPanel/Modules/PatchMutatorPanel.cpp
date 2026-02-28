#include "PatchMutatorPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDimensions.h"
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

PatchMutatorPanel::PatchMutatorPanel(tss::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
    : width_(width)
    , height_(height)
    , skin_(&skin)
    , apvts_(apvts)
{
    setOpaque(false);
    setupModuleHeader(skin, widgetFactory);
    setupAmountLine(skin, widgetFactory);
    setupRandomLine(skin, widgetFactory);
    setupHistoryLine(skin, widgetFactory);
    setSize(width_, height_);
}

PatchMutatorPanel::~PatchMutatorPanel() = default;

void PatchMutatorPanel::setupModuleHeader(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    moduleHeader_ = std::make_unique<tss::ModuleHeader>(
        skin,
        widgetFactory.getGroupDisplayName(PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId),
        PluginDimensions::Widgets::Widths::ModuleHeader::kPatchManagerModule,
        PluginDimensions::Widgets::Heights::kModuleHeader,
        tss::ModuleHeader::ColourVariant::Blue);
    addAndMakeVisible(*moduleHeader_);
}

void PatchMutatorPanel::setupAmountLine(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    amountLabel_ = std::make_unique<tss::Label>(
        skin,
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount);
    addAndMakeVisible(*amountLabel_);

    amountSlider_ = std::make_unique<tss::Slider>(skin, kSliderWidth, kSliderHeight, 0.0);
    amountSlider_->setRange(0.0, 100.0, 1.0);
    amountSlider_->setValue(0.0);
    amountSlider_->setUnit(PluginDisplayNames::Units::kPercent);
    amountSlider_->onValueChange = [this]
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount,
            static_cast<int>(amountSlider_->getValue()),
            nullptr);
    };
    addAndMakeVisible(*amountSlider_);

    mutateButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(mutateButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate);
    addAndMakeVisible(*mutateButton_);

    dco1Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1);
    connectToggleToApvts(dco1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco1);
    addAndMakeVisible(*dco1Toggle_);

    dco2Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2);
    connectToggleToApvts(dco2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDco2);
    addAndMakeVisible(*dco2Toggle_);

    vcfVcaToggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca);
    connectToggleToApvts(vcfVcaToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kVcfVca);
    addAndMakeVisible(*vcfVcaToggle_);

    fmTrackToggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack);
    connectToggleToApvts(fmTrackToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kFmTrack);
    addAndMakeVisible(*fmTrackToggle_);

    rampPortamentoToggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento);
    connectToggleToApvts(rampPortamentoToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRampPortamento);
    addAndMakeVisible(*rampPortamentoToggle_);
}

void PatchMutatorPanel::setupRandomLine(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    randomLabel_ = std::make_unique<tss::Label>(
        skin,
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom);
    addAndMakeVisible(*randomLabel_);

    randomSlider_ = std::make_unique<tss::Slider>(skin, kSliderWidth, kSliderHeight, 0.0);
    randomSlider_->setRange(0.0, 100.0, 1.0);
    randomSlider_->setValue(0.0);
    randomSlider_->setUnit(PluginDisplayNames::Units::kPercent);
    randomSlider_->onValueChange = [this]
    {
        apvts_.state.setProperty(
            PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom,
            static_cast<int>(randomSlider_->getValue()),
            nullptr);
    };
    addAndMakeVisible(*randomSlider_);

    retryButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(retryButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry);
    addAndMakeVisible(*retryButton_);

    env1Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1);
    connectToggleToApvts(env1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope1);
    addAndMakeVisible(*env1Toggle_);

    env2Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2);
    connectToggleToApvts(env2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope2);
    addAndMakeVisible(*env2Toggle_);

    env3Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3);
    connectToggleToApvts(env3Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnvelope3);
    addAndMakeVisible(*env3Toggle_);

    lfo1Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1);
    connectToggleToApvts(lfo1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo1);
    addAndMakeVisible(*lfo1Toggle_);

    lfo2Toggle_ = std::make_unique<tss::Toggle>(
        skin,
        PluginDimensions::Widgets::Widths::Toggle::kPatchMutator,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2);
    connectToggleToApvts(lfo2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kLfo2);
    addAndMakeVisible(*lfo2Toggle_);
}

void PatchMutatorPanel::setupHistoryLine(tss::ISkin& skin, WidgetFactory& widgetFactory)
{
    historyLabel_ = std::make_unique<tss::Label>(
        skin,
        PluginDimensions::Widgets::Widths::Label::kPatchMutator,
        PluginDimensions::Widgets::Heights::kLabel,
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory);
    addAndMakeVisible(*historyLabel_);

    historyComboBox_ = std::make_unique<tss::ComboBox>(
        skin,
        PluginDimensions::Widgets::Widths::ComboBox::kPatchMutatorHistory,
        kComboBoxHeight,
        tss::ComboBox::Style::Standard);
    historyComboBox_->addItem(PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEmptyHistory, 1);
    historyComboBox_->setSelectedId(1);
    historyComboBox_->onChange = [this]
    {
        if (auto* comboBox = historyComboBox_.get())
        {
            apvts_.state.setProperty(
                PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory,
                comboBox->getSelectedId(),
                nullptr);
        }
    };
    addAndMakeVisible(*historyComboBox_);

    compareButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(compareButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare);
    addAndMakeVisible(*compareButton_);

    deleteButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(deleteButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete);
    addAndMakeVisible(*deleteButton_);

    clearButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(clearButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear);
    addAndMakeVisible(*clearButton_);

    exportButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
        skin,
        PluginDimensions::Widgets::Heights::kButton);
    connectButtonToApvts(exportButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport);
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

void PatchMutatorPanel::setSkin(tss::ISkin& skin)
{
    skin_ = &skin;
    propagateSkinsToControlWidgets(skin);
    propagateSkinsToToggleWidgets(skin);
}

void PatchMutatorPanel::propagateSkinsToControlWidgets(tss::ISkin& skin)
{
    tss::propagateSkin(skin,
        moduleHeader_.get(),
        amountLabel_.get(), amountSlider_.get(),
        randomLabel_.get(), randomSlider_.get(),
        historyLabel_.get(), historyComboBox_.get(),
        mutateButton_.get(), retryButton_.get(),
        compareButton_.get(), deleteButton_.get(),
        clearButton_.get(), exportButton_.get());
}

void PatchMutatorPanel::propagateSkinsToToggleWidgets(tss::ISkin& skin)
{
    tss::propagateSkin(skin,
        dco1Toggle_.get(), dco2Toggle_.get(),
        vcfVcaToggle_.get(), fmTrackToggle_.get(),
        rampPortamentoToggle_.get(),
        env1Toggle_.get(), env2Toggle_.get(),
        env3Toggle_.get(), lfo1Toggle_.get(),
        lfo2Toggle_.get());
}
