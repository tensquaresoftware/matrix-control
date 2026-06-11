#include "TestPeakIndicators.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/PeakIndicator.h"

class TestPeakIndicators::PeakIndicatorScalePanel : public juce::Component
{
public:
    PeakIndicatorScalePanel(float scale,
                            const juce::String& scaleLabelText,
                            TSS::ISkin& skin,
                            const TSS::LabelLook& labelLook,
                            int indicatorWidth,
                            int indicatorHeight)
        : scale_(scale)
        , indicatorWidth_(indicatorWidth)
        , indicatorHeight_(indicatorHeight)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            TestScaleColumns::kScaleLabelColumnDesignWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        levelLow_ = createIndicator(skin, 0.25f);
        levelMid_ = createIndicator(skin, 0.5f);
        levelHigh_ = createIndicator(skin, 1.0f);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledIndicatorHeight = TestScaleColumns::scaledSize(indicatorHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledIndicatorHeight
             + rowGap
             + scaledIndicatorHeight
             + rowGap
             + scaledIndicatorHeight;
    }

    void resized() override
    {
        const int scaledColumnWidth = TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
        const int scaledIndicatorWidth = TestScaleColumns::scaledSize(indicatorWidth_, scale_);
        const int scaledIndicatorHeight = TestScaleColumns::scaledSize(indicatorHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstIndicatorY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondIndicatorY = firstIndicatorY + scaledIndicatorHeight + rowGap;
        const int thirdIndicatorY = secondIndicatorY + scaledIndicatorHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledColumnWidth, TestScaleColumns::kScaleLabelHeight);
        levelLow_->setBounds(0, firstIndicatorY, scaledIndicatorWidth, scaledIndicatorHeight);
        levelMid_->setBounds(0, secondIndicatorY, scaledIndicatorWidth, scaledIndicatorHeight);
        levelHigh_->setBounds(0, thirdIndicatorY, scaledIndicatorWidth, scaledIndicatorHeight);
    }

    void setSkin(TSS::ISkin& skin)
    {
        levelLow_->setSkin(skin);
        levelMid_->setSkin(skin);
        levelHigh_->setSkin(skin);
    }

    void triggerPeak()
    {
        levelLow_->setLevel(1.0f);
        levelMid_->setLevel(1.0f);
        levelHigh_->setLevel(1.0f);
    }

    void restoreDemoLevels()
    {
        levelLow_->setLevel(0.25f);
        levelMid_->setLevel(0.5f);
        levelHigh_->setLevel(1.0f);
    }

private:
    std::unique_ptr<TSS::PeakIndicator> createIndicator(TSS::ISkin& skin, float level)
    {
        auto indicator = std::make_unique<TSS::PeakIndicator>(indicatorWidth_, indicatorHeight_);
        indicator->setSkin(skin);
        indicator->setUiScale(scale_);
        indicator->setLevel(level);
        addAndMakeVisible(*indicator);
        return indicator;
    }

    float scale_ { 1.0f };
    int indicatorWidth_ = 0;
    int indicatorHeight_ = 0;
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::PeakIndicator> levelLow_;
    std::unique_ptr<TSS::PeakIndicator> levelMid_;
    std::unique_ptr<TSS::PeakIndicator> levelHigh_;
};

namespace
{
    constexpr int kAnimationTimerHz_ = 30;
    constexpr int kPulseIntervalMs_ = 1000;
    constexpr int kPeakHoldMs_ = 150;
    constexpr int kTimerIntervalMs_ = 1000 / kAnimationTimerHz_;
}

TestPeakIndicators::TestPeakIndicators(TSS::ISkin& skin, int indicatorWidth, int indicatorHeight)
    : indicatorWidth_(indicatorWidth)
    , indicatorHeight_(indicatorHeight)
    , msUntilNextPulse_(kPulseIntervalMs_)
{
    setSkin(skin);
}

TestPeakIndicators::~TestPeakIndicators()
{
    stopTimer();
}

void TestPeakIndicators::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestPeakIndicators::resized()
{
    layoutColumnPanels();
}

void TestPeakIndicators::visibilityChanged()
{
    if (isVisible())
        startTimerHz(kAnimationTimerHz_);
    else
        stopTimer();
}

int TestPeakIndicators::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestPeakIndicators::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestPeakIndicators::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<PeakIndicatorScalePanel>(
            spec.scale,
            spec.label,
            *skin_,
            labelLook,
            indicatorWidth_,
            indicatorHeight_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestPeakIndicators::layoutColumnPanels()
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

void TestPeakIndicators::triggerPeakOnAllIndicators()
{
    for (auto& panel : columnPanels_)
        panel->triggerPeak();
}

void TestPeakIndicators::restoreAllIndicatorLevels()
{
    for (auto& panel : columnPanels_)
        panel->restoreDemoLevels();
}

void TestPeakIndicators::timerCallback()
{
    if (peakHoldRemainingMs_ > 0)
    {
        peakHoldRemainingMs_ -= kTimerIntervalMs_;

        if (peakHoldRemainingMs_ <= 0)
            restoreAllIndicatorLevels();

        return;
    }

    msUntilNextPulse_ -= kTimerIntervalMs_;

    if (msUntilNextPulse_ <= 0)
    {
        msUntilNextPulse_ = kPulseIntervalMs_;
        triggerPeakOnAllIndicators();
        peakHoldRemainingMs_ = kPeakHoldMs_;
    }
}
