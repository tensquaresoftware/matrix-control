#include "TestPatchNameDisplays.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/PatchNameDisplay.h"

class TestPatchNameDisplays::PatchNameDisplayScalePanel : public juce::Component
{
public:
    PatchNameDisplayScalePanel(float scale,
                               const juce::String& scaleLabelText,
                               const TSS::PatchNameDisplayLook& displayLook,
                               const TSS::LabelLook& labelLook,
                               const PatchNameDisplayDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
        , displayLook_(displayLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            dimensions_.width,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        display_ = std::make_unique<TSS::PatchNameDisplay>(dimensions_.width, dimensions_.height, displayLook_);
        display_->setUiScale(scale_);
        display_->setPatchName("INIT PATCH");
        addAndMakeVisible(*display_);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(dimensions_.width, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledDisplayHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight + rowGap + scaledDisplayHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(dimensions_.width, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int displayY = TestScaleColumns::kScaleLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        display_->setBounds(0, displayY, scaledWidth, scaledHeight);
    }

private:
    float scale_ { 1.0f };
    PatchNameDisplayDimensions dimensions_ {};
    TSS::PatchNameDisplayLook displayLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::PatchNameDisplay> display_;
};

TestPatchNameDisplays::TestPatchNameDisplays(TSS::ISkin& skin, const PatchNameDisplayDimensions& dimensions)
    : dimensions_(dimensions)
{
    setSkin(skin);
}

TestPatchNameDisplays::~TestPatchNameDisplays() = default;

void TestPatchNameDisplays::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestPatchNameDisplays::resized()
{
    layoutColumnPanels();
}

int TestPatchNameDisplays::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestPatchNameDisplays::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestPatchNameDisplays::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto displayLook = TSS::patchNameDisplayLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<PatchNameDisplayScalePanel>(
            spec.scale,
            spec.label,
            displayLook,
            labelLook,
            dimensions_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestPatchNameDisplays::layoutColumnPanels()
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
