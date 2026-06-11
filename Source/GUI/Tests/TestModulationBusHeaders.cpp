#include "TestModulationBusHeaders.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModulationBusHeader.h"

class TestModulationBusHeaders::ModulationBusHeaderScalePanel : public juce::Component
{
public:
    ModulationBusHeaderScalePanel(float scale,
                                  const juce::String& scaleLabelText,
                                  const TSS::ModulationBusHeaderLook& headerLook,
                                  const TSS::LabelLook& labelLook,
                                  int headerWidth,
                                  int headerHeight,
                                  const ModulationBusHeaderDimensions& dimensions)
        : scale_(scale)
        , headerWidth_(headerWidth)
        , headerHeight_(headerHeight)
        , dimensions_(dimensions)
        , headerLook_(headerLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            headerWidth_,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        blueHeader_ = createHeader(TSS::ModulationBusHeader::ColourVariant::Blue);
        orangeHeader_ = createHeader(TSS::ModulationBusHeader::ColourVariant::Orange);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(headerWidth_, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledHeaderHeight = TestScaleColumns::scaledSize(headerHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledHeaderHeight
             + rowGap
             + scaledHeaderHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(headerWidth_, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(headerHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstHeaderY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondHeaderY = firstHeaderY + scaledHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        blueHeader_->setBounds(0, firstHeaderY, scaledWidth, scaledHeight);
        orangeHeader_->setBounds(0, secondHeaderY, scaledWidth, scaledHeight);
    }

private:
    std::unique_ptr<TSS::ModulationBusHeader> createHeader(TSS::ModulationBusHeader::ColourVariant variant)
    {
        auto header = std::make_unique<TSS::ModulationBusHeader>(
            headerWidth_,
            headerHeight_,
            dimensions_,
            headerLook_,
            variant);
        header->setUiScale(scale_);
        addAndMakeVisible(*header);
        return header;
    }

    float scale_ { 1.0f };
    int headerWidth_ = 0;
    int headerHeight_ = 0;
    ModulationBusHeaderDimensions dimensions_ {};
    TSS::ModulationBusHeaderLook headerLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::ModulationBusHeader> blueHeader_;
    std::unique_ptr<TSS::ModulationBusHeader> orangeHeader_;
};

TestModulationBusHeaders::TestModulationBusHeaders(TSS::ISkin& skin,
                                                   int headerWidth,
                                                   int headerHeight,
                                                   const ModulationBusHeaderDimensions& dimensions)
    : headerWidth_(headerWidth)
    , headerHeight_(headerHeight)
    , dimensions_(dimensions)
{
    setSkin(skin);
}

TestModulationBusHeaders::~TestModulationBusHeaders() = default;

void TestModulationBusHeaders::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestModulationBusHeaders::resized()
{
    layoutColumnPanels();
}

int TestModulationBusHeaders::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestModulationBusHeaders::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestModulationBusHeaders::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto headerLook = TSS::modulationBusHeaderLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ModulationBusHeaderScalePanel>(
            spec.scale,
            spec.label,
            headerLook,
            labelLook,
            headerWidth_,
            headerHeight_,
            dimensions_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestModulationBusHeaders::layoutColumnPanels()
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
