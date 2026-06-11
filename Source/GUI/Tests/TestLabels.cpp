#include "TestLabels.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"

class TestLabels::LabelScalePanel : public juce::Component
{
public:
    LabelScalePanel(float scale,
                    const juce::String& scaleLabelText,
                    const TSS::LabelLook& labelLook,
                    int labelWidth,
                    int labelHeight)
        : scale_(scale)
        , labelWidth_(labelWidth)
        , labelHeight_(labelHeight)
        , labelLook_(labelLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            TestScaleColumns::kScaleLabelColumnDesignWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        defaultLabel_ = createLabel("LABEL", TSS::LabelStyle::Default);
        centeredLabel_ = createLabel("CENTER", TSS::LabelStyle::Centered);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledLabelHeight = TestScaleColumns::scaledSize(labelHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledLabelHeight
             + rowGap
             + scaledLabelHeight;
    }

    void resized() override
    {
        const int scaledColumnWidth = TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
        const int scaledLabelWidth = TestScaleColumns::scaledSize(labelWidth_, scale_);
        const int scaledLabelHeight = TestScaleColumns::scaledSize(labelHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstLabelY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondLabelY = firstLabelY + scaledLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledColumnWidth, TestScaleColumns::kScaleLabelHeight);
        defaultLabel_->setBounds(0, firstLabelY, scaledLabelWidth, scaledLabelHeight);
        centeredLabel_->setBounds(0, secondLabelY, scaledLabelWidth, scaledLabelHeight);
    }

private:
    std::unique_ptr<TSS::Label> createLabel(const juce::String& text, TSS::LabelStyle style)
    {
        auto label = std::make_unique<TSS::Label>(labelWidth_, labelHeight_, labelLook_, text, style);
        label->setUiScale(scale_);
        addAndMakeVisible(*label);
        return label;
    }

    float scale_ { 1.0f };
    int labelWidth_ = 0;
    int labelHeight_ = 0;
    TSS::LabelLook labelLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::Label> defaultLabel_;
    std::unique_ptr<TSS::Label> centeredLabel_;
};

TestLabels::TestLabels(TSS::ISkin& skin)
{
    setSkin(skin);
}

TestLabels::~TestLabels() = default;

void TestLabels::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestLabels::resized()
{
    layoutColumnPanels();
}

int TestLabels::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestLabels::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestLabels::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto labelLook = TSS::labelLookFromSkin(*skin_);
    const int labelWidth = TSS::Label::getBaseWidth();
    const int labelHeight = TSS::Label::getBaseHeight();

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<LabelScalePanel>(
            spec.scale,
            spec.label,
            labelLook,
            labelWidth,
            labelHeight);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestLabels::layoutColumnPanels()
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
