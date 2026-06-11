#include "TestModulationBusCells.h"

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModulationBusCell.h"
#include "Shared/Definitions/PluginIDs.h"

class TestModulationBusCells::ModulationBusCellScalePanel : public juce::Component
{
public:
    ModulationBusCellScalePanel(float scale,
                                const juce::String& scaleLabelText,
                                TSS::ISkin& skin,
                                WidgetFactory& widgetFactory,
                                juce::AudioProcessorValueTreeState& apvts,
                                const ModulationBusCellDimensions& dimensions,
                                int panelWidth,
                                int panelHeight,
                                const TSS::LabelLook& labelLook)
        : scale_(scale)
        , dimensions_(dimensions)
        , panelWidth_(panelWidth)
        , panelHeight_(panelHeight)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            panelWidth_,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        busCell_ = std::make_unique<ModulationBusCell>(
            skin,
            panelWidth_,
            panelHeight_,
            dimensions_,
            0,
            widgetFactory,
            apvts,
            PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source,
            PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount,
            PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Destination,
            PluginIDs::MatrixModulationSection::ModulationBus::kBus0);
        busCell_->setUiScale(scale_);
        addAndMakeVisible(*busCell_);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(panelWidth_, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledPanelHeight = TestScaleColumns::scaledSize(panelHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight + rowGap + scaledPanelHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(panelWidth_, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(panelHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int cellY = TestScaleColumns::kScaleLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        busCell_->setBounds(0, cellY, scaledWidth, scaledHeight);
    }

    void setSkin(TSS::ISkin& skin)
    {
        busCell_->setSkin(skin);
    }

private:
    float scale_ { 1.0f };
    ModulationBusCellDimensions dimensions_ {};
    int panelWidth_ = 0;
    int panelHeight_ = 0;
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<ModulationBusCell> busCell_;
};

TestModulationBusCells::TestModulationBusCells(TSS::ISkin& skin,
                                               WidgetFactory& widgetFactory,
                                               juce::AudioProcessorValueTreeState& apvts,
                                               const ModulationBusCellDimensions& dimensions,
                                               int panelWidth,
                                               int panelHeight)
    : dimensions_(dimensions)
    , panelWidth_(panelWidth)
    , panelHeight_(panelHeight)
{
    widgetFactory_ = &widgetFactory;
    apvts_ = &apvts;
    setSkin(skin);
}

TestModulationBusCells::~TestModulationBusCells() = default;

void TestModulationBusCells::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestModulationBusCells::resized()
{
    layoutColumnPanels();
}

int TestModulationBusCells::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestModulationBusCells::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestModulationBusCells::rebuildPanels()
{
    if (skin_ == nullptr || widgetFactory_ == nullptr || apvts_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ModulationBusCellScalePanel>(
            spec.scale,
            spec.label,
            *skin_,
            *widgetFactory_,
            *apvts_,
            dimensions_,
            panelWidth_,
            panelHeight_,
            labelLook);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestModulationBusCells::layoutColumnPanels()
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
