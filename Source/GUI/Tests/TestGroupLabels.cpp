#include "TestGroupLabels.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/GroupLabel.h"
#include "GUI/Widgets/Label.h"

class TestGroupLabels::GroupLabelScalePanel : public juce::Component
{
public:
    GroupLabelScalePanel(float scale,
                         const juce::String& scaleLabelText,
                         const TSS::GroupLabelLook& groupLabelLook,
                         const TSS::LabelLook& labelLook,
                         const GroupLabelDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
        , groupLabelLook_(groupLabelLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            dimensions_.internalPatchesBrowserWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        groupLabel_ = std::make_unique<TSS::GroupLabel>(
            dimensions_.internalPatchesBrowserWidth,
            dimensions_.height,
            groupLabelLook_,
            "GROUP");
        groupLabel_->setUiScale(scale_);
        addAndMakeVisible(*groupLabel_);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(dimensions_.internalPatchesBrowserWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledGroupHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight + rowGap + scaledGroupHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(dimensions_.internalPatchesBrowserWidth, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int groupY = TestScaleColumns::kScaleLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        groupLabel_->setBounds(0, groupY, scaledWidth, scaledHeight);
    }

private:
    float scale_ { 1.0f };
    GroupLabelDimensions dimensions_ {};
    TSS::GroupLabelLook groupLabelLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::GroupLabel> groupLabel_;
};

TestGroupLabels::TestGroupLabels(TSS::ISkin& skin, const GroupLabelDimensions& dimensions)
    : dimensions_(dimensions)
{
    setSkin(skin);
}

TestGroupLabels::~TestGroupLabels() = default;

void TestGroupLabels::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestGroupLabels::resized()
{
    layoutColumnPanels();
}

int TestGroupLabels::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestGroupLabels::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestGroupLabels::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto groupLabelLook = TSS::groupLabelLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<GroupLabelScalePanel>(
            spec.scale,
            spec.label,
            groupLabelLook,
            labelLook,
            dimensions_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestGroupLabels::layoutColumnPanels()
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
