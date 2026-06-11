#include "TestModuleHeaders.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/ModuleHeader.h"

class TestModuleHeaders::ModuleHeaderScalePanel : public juce::Component
{
public:
    ModuleHeaderScalePanel(float scale,
                           const juce::String& scaleLabelText,
                           const TSS::ModuleHeaderLook& headerLook,
                           const TSS::LabelLook& labelLook,
                           const ModuleHeaderDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
        , headerLook_(headerLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            dimensions_.patchEditTitleBandWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        blueHeader_ = createHeader(TSS::ModuleHeader::ColourVariant::Blue);
        orangeHeader_ = createHeader(TSS::ModuleHeader::ColourVariant::Orange);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(dimensions_.patchEditTitleBandWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledHeaderHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledHeaderHeight
             + rowGap
             + scaledHeaderHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(dimensions_.patchEditTitleBandWidth, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstHeaderY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondHeaderY = firstHeaderY + scaledHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        blueHeader_->setBounds(0, firstHeaderY, scaledWidth, scaledHeight);
        orangeHeader_->setBounds(0, secondHeaderY, scaledWidth, scaledHeight);
    }

private:
    std::unique_ptr<TSS::ModuleHeader> createHeader(TSS::ModuleHeader::ColourVariant variant)
    {
        auto header = std::make_unique<TSS::ModuleHeader>(
            dimensions_.patchEditTitleBandWidth,
            dimensions_.height,
            headerLook_,
            variant,
            "MODULE",
            dimensions_);
        header->setUiScale(scale_);
        addAndMakeVisible(*header);
        return header;
    }

    float scale_ { 1.0f };
    ModuleHeaderDimensions dimensions_ {};
    TSS::ModuleHeaderLook headerLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::ModuleHeader> blueHeader_;
    std::unique_ptr<TSS::ModuleHeader> orangeHeader_;
};

TestModuleHeaders::TestModuleHeaders(TSS::ISkin& skin, const ModuleHeaderDimensions& dimensions)
    : dimensions_(dimensions)
{
    setSkin(skin);
}

TestModuleHeaders::~TestModuleHeaders() = default;

void TestModuleHeaders::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestModuleHeaders::resized()
{
    layoutColumnPanels();
}

int TestModuleHeaders::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestModuleHeaders::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestModuleHeaders::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto headerLook = TSS::moduleHeaderLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ModuleHeaderScalePanel>(
            spec.scale,
            spec.label,
            headerLook,
            labelLook,
            dimensions_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestModuleHeaders::layoutColumnPanels()
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
