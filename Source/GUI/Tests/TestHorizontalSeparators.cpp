#include "TestHorizontalSeparators.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/HorizontalSeparator.h"
#include "GUI/Widgets/Label.h"

class TestHorizontalSeparators::HorizontalSeparatorScalePanel : public juce::Component
{
public:
    HorizontalSeparatorScalePanel(float scale,
                                  const juce::String& scaleLabelText,
                                  const TSS::HorizontalSeparatorLook& separatorLook,
                                  const TSS::LabelLook& labelLook,
                                  int separatorWidth,
                                  int separatorHeight)
        : scale_(scale)
        , separatorWidth_(separatorWidth)
        , separatorHeight_(separatorHeight)
        , separatorLook_(separatorLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            separatorWidth_,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        separator_ = std::make_unique<TSS::HorizontalSeparator>(separatorWidth_, separatorHeight_, separatorLook_);
        separator_->setUiScale(scale_);
        addAndMakeVisible(*separator_);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(separatorWidth_, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledSeparatorHeight = TestScaleColumns::scaledSize(separatorHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight + rowGap + scaledSeparatorHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(separatorWidth_, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(separatorHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int separatorY = TestScaleColumns::kScaleLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        separator_->setBounds(0, separatorY, scaledWidth, scaledHeight);
    }

private:
    float scale_ { 1.0f };
    int separatorWidth_ = 0;
    int separatorHeight_ = 0;
    TSS::HorizontalSeparatorLook separatorLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::HorizontalSeparator> separator_;
};

TestHorizontalSeparators::TestHorizontalSeparators(TSS::ISkin& skin, int separatorWidth, int separatorHeight)
    : separatorWidth_(separatorWidth)
    , separatorHeight_(separatorHeight)
{
    setSkin(skin);
}

TestHorizontalSeparators::~TestHorizontalSeparators() = default;

void TestHorizontalSeparators::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestHorizontalSeparators::resized()
{
    layoutColumnPanels();
}

int TestHorizontalSeparators::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestHorizontalSeparators::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestHorizontalSeparators::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto separatorLook = TSS::horizontalSeparatorLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<HorizontalSeparatorScalePanel>(
            spec.scale,
            spec.label,
            separatorLook,
            labelLook,
            separatorWidth_,
            separatorHeight_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestHorizontalSeparators::layoutColumnPanels()
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
