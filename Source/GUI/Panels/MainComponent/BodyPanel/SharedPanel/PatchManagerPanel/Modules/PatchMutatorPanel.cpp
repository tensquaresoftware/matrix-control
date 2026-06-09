#include "PatchMutatorPanel.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinHelpers.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Slider.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Toggle.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "GUI/Layout/Design/Design.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Core/Factories/ApvtsFactory.h"

PatchMutatorPanel::PatchMutatorPanel(TSS::ISkin& skin, int width, int height, WidgetFactory& widgetFactory, juce::AudioProcessorValueTreeState& apvts)
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

void PatchMutatorPanel::setupModuleHeader(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    moduleHeader_ = std::make_unique<TSS::ModuleHeader>(
        TSS::Design::PanelWidgets::Widths::ModuleHeader::kPatchManagerModule,
        TSS::Design::Atoms::Heights::kModuleHeader,
        TSS::moduleHeaderLookFromSkin(skin),
        TSS::ModuleHeader::ColourVariant::Blue,
        widgetFactory.getGroupDisplayName(PluginIDs::PatchManagerSection::PatchMutatorModule::kGroupId));
    addAndMakeVisible(*moduleHeader_);
}

void PatchMutatorPanel::setupAmountLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    amountLabel_ = std::make_unique<TSS::Label>(
        TSS::Design::Atoms::Widths::Label::kPatchMutator,
        TSS::Design::Atoms::Heights::kLabel,
        TSS::labelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount);
    addAndMakeVisible(*amountLabel_);

    const auto allIntParams = ApvtsFactory::getAllIntParameters();
    const auto amountIt = std::find_if(allIntParams.begin(), allIntParams.end(),
        [](const PluginDescriptors::IntParameterDescriptor& desc) {
            return desc.parameterId == PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kAmount;
        });
    
    amountSlider_ = std::make_unique<TSS::Slider>(
        TSS::Design::Atoms::Widths::Slider::kPatchMutator,
        TSS::Design::Atoms::Heights::kSlider,
        TSS::sliderLookFromSkin(skin),
        TSS::SliderConfig{
            amountIt != allIntParams.end() ? static_cast<double>(amountIt->minValue) : 0.0,
            amountIt != allIntParams.end() ? static_cast<double>(amountIt->maxValue) : 100.0,
            amountIt != allIntParams.end() ? static_cast<double>(amountIt->defaultValue) : 0.0,
            1.0,
            PluginDisplayNames::Units::kPercent});
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
        TSS::Design::Atoms::Heights::kButton);
    connectButtonToApvts(mutateButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate);
    addAndMakeVisible(*mutateButton_);

    dco1Toggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco1);
    connectToggleToApvts(dco1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco1);
    addAndMakeVisible(*dco1Toggle_);

    dco2Toggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco2);
    connectToggleToApvts(dco2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableDco2);
    addAndMakeVisible(*dco2Toggle_);

    vcfVcaToggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableVcfVca);
    connectToggleToApvts(vcfVcaToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableVcfVca);
    addAndMakeVisible(*vcfVcaToggle_);

    fmTrackToggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableFmTrack);
    connectToggleToApvts(fmTrackToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableFmTrack);
    addAndMakeVisible(*fmTrackToggle_);

    rampPortamentoToggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableRampPortamento);
    connectToggleToApvts(rampPortamentoToggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableRampPortamento);
    addAndMakeVisible(*rampPortamentoToggle_);
}

void PatchMutatorPanel::setupRandomLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    randomLabel_ = std::make_unique<TSS::Label>(
        TSS::Design::Atoms::Widths::Label::kPatchMutator,
        TSS::Design::Atoms::Heights::kLabel,
        TSS::labelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom);
    addAndMakeVisible(*randomLabel_);

    const auto allIntParams = ApvtsFactory::getAllIntParameters();
    const auto randomIt = std::find_if(allIntParams.begin(), allIntParams.end(),
        [](const PluginDescriptors::IntParameterDescriptor& desc) {
            return desc.parameterId == PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRandom;
        });
    
    randomSlider_ = std::make_unique<TSS::Slider>(
        TSS::Design::Atoms::Widths::Slider::kPatchMutator,
        TSS::Design::Atoms::Heights::kSlider,
        TSS::sliderLookFromSkin(skin),
        TSS::SliderConfig{
            randomIt != allIntParams.end() ? static_cast<double>(randomIt->minValue) : 0.0,
            randomIt != allIntParams.end() ? static_cast<double>(randomIt->maxValue) : 100.0,
            randomIt != allIntParams.end() ? static_cast<double>(randomIt->defaultValue) : 0.0,
            1.0,
            PluginDisplayNames::Units::kPercent});
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
        TSS::Design::Atoms::Heights::kButton);
    connectButtonToApvts(retryButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry);
    addAndMakeVisible(*retryButton_);

    env1Toggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope1);
    connectToggleToApvts(env1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope1);
    addAndMakeVisible(*env1Toggle_);

    env2Toggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope2);
    connectToggleToApvts(env2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope2);
    addAndMakeVisible(*env2Toggle_);

    env3Toggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope3);
    connectToggleToApvts(env3Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableEnvelope3);
    addAndMakeVisible(*env3Toggle_);

    lfo1Toggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo1);
    connectToggleToApvts(lfo1Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo1);
    addAndMakeVisible(*lfo1Toggle_);

    lfo2Toggle_ = std::make_unique<TSS::Toggle>(
        TSS::Design::Atoms::Widths::Toggle::kPatchMutator,
        TSS::Design::Atoms::Heights::kToggle,
        TSS::toggleLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo2);
    connectToggleToApvts(lfo2Toggle_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEnableLfo2);
    addAndMakeVisible(*lfo2Toggle_);
}

void PatchMutatorPanel::setupHistoryLine(TSS::ISkin& skin, WidgetFactory& widgetFactory)
{
    historyLabel_ = std::make_unique<TSS::Label>(
        TSS::Design::Atoms::Widths::Label::kPatchMutator,
        TSS::Design::Atoms::Heights::kLabel,
        TSS::labelLookFromSkin(skin),
        PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistory);
    addAndMakeVisible(*historyLabel_);

    historyComboBox_ = std::make_unique<TSS::ComboBox>(
        TSS::Design::Atoms::Widths::ComboBox::kPatchMutatorHistory,
        TSS::Design::Atoms::Heights::kComboBox,
        TSS::comboBoxLookFromSkin(skin),
        TSS::ComboBox::Style::Standard);
    historyComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
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
        TSS::Design::Atoms::Heights::kButton);
    connectButtonToApvts(compareButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare);
    addAndMakeVisible(*compareButton_);

    deleteButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
        skin,
        TSS::Design::Atoms::Heights::kButton);
    connectButtonToApvts(deleteButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete);
    addAndMakeVisible(*deleteButton_);

    clearButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
        skin,
        TSS::Design::Atoms::Heights::kButton);
    connectButtonToApvts(clearButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear);
    addAndMakeVisible(*clearButton_);

    exportButton_ = widgetFactory.createStandaloneButton(
        PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
        skin,
        TSS::Design::Atoms::Heights::kButton);
    connectButtonToApvts(exportButton_.get(), PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport);
    addAndMakeVisible(*exportButton_);
}

void PatchMutatorPanel::connectButtonToApvts(TSS::Button* button, const char* widgetId)
{
    if (button != nullptr)
    {
        button->onClick = [this, widgetId]
        {
            apvts_.state.setProperty(widgetId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
        };
    }
}

void PatchMutatorPanel::connectToggleToApvts(TSS::Toggle* toggle, const char* widgetId)
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
    const float sf = uiScale_;

    const int moduleHeaderW = juce::roundToInt(static_cast<float>(TSS::Design::PanelWidgets::Widths::ModuleHeader::kPatchManagerModule) * sf);
    const int moduleHeaderH = juce::roundToInt(static_cast<float>(TSS::Design::Atoms::Heights::kModuleHeader) * sf);

    if (auto* header = moduleHeader_.get())
        header->setBounds(0, 0, moduleHeaderW, moduleHeaderH);

    // Y positions for the 3 lines, computed independently from float origin
    const int row0Y = juce::roundToInt(static_cast<float>(TSS::Design::Atoms::Heights::kModuleHeader + kGap_) * sf);
    const int row1Y = juce::roundToInt(static_cast<float>(TSS::Design::Atoms::Heights::kModuleHeader + kGap_ + TSS::Design::Atoms::Heights::kLabel + kGap_) * sf);
    const int row2Y = juce::roundToInt(static_cast<float>(TSS::Design::Atoms::Heights::kModuleHeader + kGap_ + (TSS::Design::Atoms::Heights::kLabel + kGap_) * 2) * sf);

    layoutSliderLine(0, row0Y, amountLabel_.get(), amountSlider_.get(), mutateButton_.get(),
        {dco1Toggle_.get(), dco2Toggle_.get(), vcfVcaToggle_.get(), fmTrackToggle_.get(), rampPortamentoToggle_.get()});
    layoutSliderLine(0, row1Y, randomLabel_.get(), randomSlider_.get(), retryButton_.get(),
        {env1Toggle_.get(), env2Toggle_.get(), env3Toggle_.get(), lfo1Toggle_.get(), lfo2Toggle_.get()});
    layoutHistoryLine(0, row2Y);

    if (moduleHeader_)            moduleHeader_->setUiScale(sf);
    if (amountLabel_)             amountLabel_->setUiScale(sf);
    if (amountSlider_)            amountSlider_->setUiScale(sf);
    if (mutateButton_)            mutateButton_->setUiScale(sf);
    if (randomLabel_)             randomLabel_->setUiScale(sf);
    if (randomSlider_)            randomSlider_->setUiScale(sf);
    if (retryButton_)             retryButton_->setUiScale(sf);
    if (historyLabel_)            historyLabel_->setUiScale(sf);
    if (historyComboBox_)         historyComboBox_->setUiScale(sf);
    if (compareButton_)           compareButton_->setUiScale(sf);
    if (deleteButton_)            deleteButton_->setUiScale(sf);
    if (clearButton_)             clearButton_->setUiScale(sf);
    if (exportButton_)            exportButton_->setUiScale(sf);
    if (dco1Toggle_)              dco1Toggle_->setUiScale(sf);
    if (dco2Toggle_)              dco2Toggle_->setUiScale(sf);
    if (vcfVcaToggle_)            vcfVcaToggle_->setUiScale(sf);
    if (fmTrackToggle_)           fmTrackToggle_->setUiScale(sf);
    if (rampPortamentoToggle_)    rampPortamentoToggle_->setUiScale(sf);
    if (env1Toggle_)              env1Toggle_->setUiScale(sf);
    if (env2Toggle_)              env2Toggle_->setUiScale(sf);
    if (env3Toggle_)              env3Toggle_->setUiScale(sf);
    if (lfo1Toggle_)              lfo1Toggle_->setUiScale(sf);
    if (lfo2Toggle_)              lfo2Toggle_->setUiScale(sf);
}

void PatchMutatorPanel::layoutSliderLine(int x, int y, TSS::Label* label, TSS::Slider* slider, TSS::Button* button, const std::vector<TSS::Toggle*>& toggles)
{
    using namespace TSS::Design::Atoms;
    const float sf = uiScale_;

    const int labelW      = juce::roundToInt(static_cast<float>(Widths::Label::kPatchMutator) * sf);
    const int labelH      = juce::roundToInt(static_cast<float>(Heights::kLabel) * sf);
    const int sliderW     = juce::roundToInt(static_cast<float>(Widths::Slider::kPatchMutator) * sf);
    const int sliderH     = juce::roundToInt(static_cast<float>(Heights::kSlider) * sf);
    const int buttonW     = juce::roundToInt(static_cast<float>(Widths::Button::kPatchMutatorMutate) * sf);
    const int toggleW     = juce::roundToInt(static_cast<float>(Widths::Toggle::kPatchMutator) * sf);
    const int toggleH     = juce::roundToInt(static_cast<float>(Heights::kToggle) * sf);

    // Offsets computed in float to avoid accumulation
    const float labelStep  = static_cast<float>(Widths::Label::kPatchMutator + kGap_) * sf;
    const float sliderStep = static_cast<float>(Widths::Slider::kPatchMutator + kGap_) * sf;
    const float buttonStep = static_cast<float>(Widths::Button::kPatchMutatorMutate + kGap_) * sf;
    const float toggleStep = static_cast<float>(Widths::Toggle::kPatchMutator + kGap_) * sf;

    const float originX = static_cast<float>(x);

    if (label != nullptr)
        label->setBounds(x, y, labelW, labelH);

    if (slider != nullptr)
        slider->setBounds(juce::roundToInt(originX + labelStep), y, sliderW, sliderH);

    if (button != nullptr)
        button->setBounds(juce::roundToInt(originX + labelStep + sliderStep), y, buttonW, labelH);

    const float toggleOriginX = originX + labelStep + sliderStep + buttonStep;
    for (int i = 0; i < static_cast<int>(toggles.size()); ++i)
    {
        if (toggles[static_cast<size_t>(i)] != nullptr)
            toggles[static_cast<size_t>(i)]->setBounds(juce::roundToInt(toggleOriginX + static_cast<float>(i) * toggleStep), y, toggleW, toggleH);
    }
}

void PatchMutatorPanel::layoutHistoryLine(int x, int y)
{
    using namespace TSS::Design::Atoms;
    const float sf = uiScale_;

    const int labelW      = juce::roundToInt(static_cast<float>(Widths::Label::kPatchMutator) * sf);
    const int labelH      = juce::roundToInt(static_cast<float>(Heights::kLabel) * sf);
    const int comboBoxW   = juce::roundToInt(static_cast<float>(Widths::ComboBox::kPatchMutatorHistory) * sf);
    const int comboBoxH   = juce::roundToInt(static_cast<float>(Heights::kComboBox) * sf);
    const int compareW    = juce::roundToInt(static_cast<float>(Widths::Button::kPatchMutatorCompare) * sf);
    const int deleteW     = juce::roundToInt(static_cast<float>(Widths::Button::kPatchMutatorDelete) * sf);
    const int clearW      = juce::roundToInt(static_cast<float>(Widths::Button::kPatchMutatorClear) * sf);
    const int exportW     = juce::roundToInt(static_cast<float>(Widths::Button::kPatchMutatorExport) * sf);

    // X positions computed independently from float origin
    const float originX     = static_cast<float>(x);
    const float labelStep   = static_cast<float>(Widths::Label::kPatchMutator + kGap_) * sf;
    const float comboStep   = static_cast<float>(Widths::ComboBox::kPatchMutatorHistory + kGap_) * sf;
    const float compareStep = static_cast<float>(Widths::Button::kPatchMutatorCompare + kGap_) * sf;
    const float deleteStep  = static_cast<float>(Widths::Button::kPatchMutatorDelete + kGap_) * sf;
    const float clearStep   = static_cast<float>(Widths::Button::kPatchMutatorClear + kGap_) * sf;

    if (auto* label = historyLabel_.get())
        label->setBounds(x, y, labelW, labelH);
    if (auto* comboBox = historyComboBox_.get())
        comboBox->setBounds(juce::roundToInt(originX + labelStep), y, comboBoxW, comboBoxH);
    if (auto* button = compareButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboStep), y, compareW, labelH);
    if (auto* button = deleteButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboStep + compareStep), y, deleteW, labelH);
    if (auto* button = clearButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboStep + compareStep + deleteStep), y, clearW, labelH);
    if (auto* button = exportButton_.get())
        button->setBounds(juce::roundToInt(originX + labelStep + comboStep + compareStep + deleteStep + clearStep), y, exportW, labelH);
}

void PatchMutatorPanel::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    propagateSkinsToControlWidgets(skin);
    propagateSkinsToToggleWidgets(skin);
}

void PatchMutatorPanel::setUiScale(float uiScale)
{
    if (juce::approximatelyEqual(uiScale_, uiScale))
        return;
    
    uiScale_ = uiScale;
    repaint();
}

void PatchMutatorPanel::propagateSkinsToControlWidgets(TSS::ISkin& skin)
{
    if (moduleHeader_)
        moduleHeader_->setLook(TSS::moduleHeaderLookFromSkin(skin));

    if (amountLabel_)
        amountLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (amountSlider_)
        amountSlider_->setLook(TSS::sliderLookFromSkin(skin));
    if (randomLabel_)
        randomLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (randomSlider_)
        randomSlider_->setLook(TSS::sliderLookFromSkin(skin));
    if (historyLabel_)
        historyLabel_->setLook(TSS::labelLookFromSkin(skin));
    if (historyComboBox_)
    {
        historyComboBox_->setLook(TSS::comboBoxLookFromSkin(skin));
        historyComboBox_->setPopupMenuLook(TSS::popupMenuLookFromSkin(skin));
    }
    if (mutateButton_)
        mutateButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (retryButton_)
        retryButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (compareButton_)
        compareButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (deleteButton_)
        deleteButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (clearButton_)
        clearButton_->setLook(TSS::buttonLookFromSkin(skin));
    if (exportButton_)
        exportButton_->setLook(TSS::buttonLookFromSkin(skin));
}

void PatchMutatorPanel::propagateSkinsToToggleWidgets(TSS::ISkin& skin)
{
    const auto toggleLook = TSS::toggleLookFromSkin(skin);
    if (dco1Toggle_)
        dco1Toggle_->setLook(toggleLook);
    if (dco2Toggle_)
        dco2Toggle_->setLook(toggleLook);
    if (vcfVcaToggle_)
        vcfVcaToggle_->setLook(toggleLook);
    if (fmTrackToggle_)
        fmTrackToggle_->setLook(toggleLook);
    if (rampPortamentoToggle_)
        rampPortamentoToggle_->setLook(toggleLook);
    if (env1Toggle_)
        env1Toggle_->setLook(toggleLook);
    if (env2Toggle_)
        env2Toggle_->setLook(toggleLook);
    if (env3Toggle_)
        env3Toggle_->setLook(toggleLook);
    if (lfo1Toggle_)
        lfo1Toggle_->setLook(toggleLook);
    if (lfo2Toggle_)
        lfo2Toggle_->setLook(toggleLook);
}
