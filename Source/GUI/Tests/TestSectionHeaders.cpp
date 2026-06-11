#include "TestSectionHeaders.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/SectionHeader.h"

class TestSectionHeaders::SectionHeaderScalePanel : public juce::Component
{
public:
    SectionHeaderScalePanel(float scale,
                            const juce::String& scaleLabelText,
                            const TSS::SectionHeaderLook& headerLook,
                            const TSS::LabelLook& labelLook,
                            int sectionHeaderWidth,
                            int sectionHeaderHeight)
        : scale_(scale)
        , sectionHeaderWidth_(sectionHeaderWidth)
        , sectionHeaderHeight_(sectionHeaderHeight)
        , headerLook_(headerLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            sectionHeaderWidth_,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        blueHeader_ = createHeader(TSS::SectionHeader::ColourVariant::Blue);
        orangeHeader_ = createHeader(TSS::SectionHeader::ColourVariant::Orange);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(sectionHeaderWidth_, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledHeaderHeight = TestScaleColumns::scaledSize(sectionHeaderHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledHeaderHeight
             + rowGap
             + scaledHeaderHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(sectionHeaderWidth_, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(sectionHeaderHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstHeaderY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondHeaderY = firstHeaderY + scaledHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        blueHeader_->setBounds(0, firstHeaderY, scaledWidth, scaledHeight);
        orangeHeader_->setBounds(0, secondHeaderY, scaledWidth, scaledHeight);
    }

private:
    std::unique_ptr<TSS::SectionHeader> createHeader(TSS::SectionHeader::ColourVariant variant)
    {
        auto header = std::make_unique<TSS::SectionHeader>(
            sectionHeaderWidth_,
            sectionHeaderHeight_,
            headerLook_,
            "SECTION",
            variant);
        header->setUiScale(scale_);
        addAndMakeVisible(*header);
        return header;
    }

    float scale_ { 1.0f };
    int sectionHeaderWidth_ = 0;
    int sectionHeaderHeight_ = 0;
    TSS::SectionHeaderLook headerLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::SectionHeader> blueHeader_;
    std::unique_ptr<TSS::SectionHeader> orangeHeader_;
};

TestSectionHeaders::TestSectionHeaders(TSS::ISkin& skin, int sectionHeaderWidth, int sectionHeaderHeight)
    : sectionHeaderWidth_(sectionHeaderWidth)
    , sectionHeaderHeight_(sectionHeaderHeight)
{
    setSkin(skin);
}

TestSectionHeaders::~TestSectionHeaders() = default;

void TestSectionHeaders::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestSectionHeaders::resized()
{
    layoutColumnPanels();
}

int TestSectionHeaders::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestSectionHeaders::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestSectionHeaders::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto headerLook = TSS::sectionHeaderLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<SectionHeaderScalePanel>(
            spec.scale,
            spec.label,
            headerLook,
            labelLook,
            sectionHeaderWidth_,
            sectionHeaderHeight_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestSectionHeaders::layoutColumnPanels()
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
