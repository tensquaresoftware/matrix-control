#include "TestVerticalSeparators.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/VerticalSeparator.h"

class TestVerticalSeparators::VerticalSeparatorScalePanel : public juce::Component
{
public:
    VerticalSeparatorScalePanel(float scale,
                                const juce::String& scaleLabelText,
                                const TSS::VerticalSeparatorLook& separatorLook,
                                const TSS::LabelLook& labelLook,
                                const SeparatorDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
        , separatorLook_(separatorLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            TestScaleColumns::kScaleLabelColumnDesignWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        separator_ = std::make_unique<TSS::VerticalSeparator>(
            dimensions_.verticalStandardWidth,
            dimensions_.verticalStandardHeight,
            separatorLook_,
            dimensions_);
        separator_->setUiScale(scale_);
        addAndMakeVisible(*separator_);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledSeparatorHeight = TestScaleColumns::scaledSize(dimensions_.verticalStandardHeight, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight + rowGap + scaledSeparatorHeight;
    }

    void resized() override
    {
        const int scaledColumnWidth = TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
        const int scaledSeparatorWidth = TestScaleColumns::scaledSize(dimensions_.verticalStandardWidth, scale_);
        const int scaledSeparatorHeight = TestScaleColumns::scaledSize(dimensions_.verticalStandardHeight, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int separatorY = TestScaleColumns::kScaleLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledColumnWidth, TestScaleColumns::kScaleLabelHeight);
        separator_->setBounds(0, separatorY, scaledSeparatorWidth, scaledSeparatorHeight);
    }

private:
    float scale_ { 1.0f };
    SeparatorDimensions dimensions_ {};
    TSS::VerticalSeparatorLook separatorLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::VerticalSeparator> separator_;
};

TestVerticalSeparators::TestVerticalSeparators(TSS::ISkin& skin, const SeparatorDimensions& dimensions)
    : dimensions_(dimensions)
{
    setSkin(skin);
}

TestVerticalSeparators::~TestVerticalSeparators() = default;

void TestVerticalSeparators::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestVerticalSeparators::resized()
{
    layoutColumnPanels();
}

int TestVerticalSeparators::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestVerticalSeparators::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestVerticalSeparators::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto separatorLook = TSS::verticalSeparatorLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<VerticalSeparatorScalePanel>(
            spec.scale,
            spec.label,
            separatorLook,
            labelLook,
            dimensions_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestVerticalSeparators::layoutColumnPanels()
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
