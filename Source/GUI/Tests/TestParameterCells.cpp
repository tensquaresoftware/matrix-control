#include "TestParameterCells.h"

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ParameterCell.h"
#include "Shared/Definitions/PluginIDs.h"

class TestParameterCells::ParameterCellScalePanel : public juce::Component
{
public:
    ParameterCellScalePanel(float scale,
                            const juce::String& scaleLabelText,
                            TSS::ISkin& skin,
                            WidgetFactory& widgetFactory,
                            juce::AudioProcessorValueTreeState& apvts,
                            const ParameterCellDimensions& dimensions,
                            const TSS::LabelLook& labelLook)
        : scale_(scale)
        , dimensions_(dimensions)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            dimensions_.labelWidth + dimensions_.controlWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        sliderCell_ = std::make_unique<ParameterCell>(
            skin,
            widgetFactory,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
            ParameterCell::ParameterType::Slider,
            ParameterCell::ModuleType::PatchEdit,
            apvts,
            dimensions_);
        sliderCell_->setUiScale(scale_);
        addAndMakeVisible(*sliderCell_);

        comboCell_ = std::make_unique<ParameterCell>(
            skin,
            widgetFactory,
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kWaveSelect,
            ParameterCell::ParameterType::ComboBox,
            ParameterCell::ModuleType::PatchEdit,
            apvts,
            dimensions_);
        comboCell_->setUiScale(scale_);
        addAndMakeVisible(*comboCell_);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(dimensions_.labelWidth + dimensions_.controlWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledRowHeight = TestScaleColumns::scaledSize(dimensions_.rowHeight, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledRowHeight
             + rowGap
             + scaledRowHeight;
    }

    void resized() override
    {
        const int scaledWidth = getScaledColumnWidth();
        const int scaledRowHeight = TestScaleColumns::scaledSize(dimensions_.rowHeight, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstCellY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondCellY = firstCellY + scaledRowHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        sliderCell_->setBounds(0, firstCellY, scaledWidth, scaledRowHeight);
        comboCell_->setBounds(0, secondCellY, scaledWidth, scaledRowHeight);
    }

    void setSkin(TSS::ISkin& skin)
    {
        sliderCell_->setSkin(skin);
        comboCell_->setSkin(skin);
    }

private:
    float scale_ { 1.0f };
    ParameterCellDimensions dimensions_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<ParameterCell> sliderCell_;
    std::unique_ptr<ParameterCell> comboCell_;
};

TestParameterCells::TestParameterCells(TSS::ISkin& skin,
                                       WidgetFactory& widgetFactory,
                                       juce::AudioProcessorValueTreeState& apvts,
                                       const ParameterCellDimensions& dimensions)
    : dimensions_(dimensions)
{
    widgetFactory_ = &widgetFactory;
    apvts_ = &apvts;
    setSkin(skin);
}

TestParameterCells::~TestParameterCells() = default;

void TestParameterCells::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestParameterCells::resized()
{
    layoutColumnPanels();
}

int TestParameterCells::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestParameterCells::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestParameterCells::rebuildPanels()
{
    if (skin_ == nullptr || widgetFactory_ == nullptr || apvts_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ParameterCellScalePanel>(
            spec.scale,
            spec.label,
            *skin_,
            *widgetFactory_,
            *apvts_,
            dimensions_,
            labelLook);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestParameterCells::layoutColumnPanels()
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    int nextColumnX = 0;

    for (auto& panel : columnPanels_)
    {
        const int columnWidth = panel->getScaledColumnWidth();
        panel->setBounds(nextColumnX, 0, columnWidth, panel->getPreferredHeight());
        nextColumnX += columnWidth + panelGap;
    }
}
