#include "TestActivityLeds.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Looks/LookBuilders.h"

class TestActivityLeds::ActivityLedScalePanel : public juce::Component
{
public:
    ActivityLedScalePanel(float scale,
                          const juce::String& scaleLabelText,
                          TSS::ISkin& skin,
                          const TSS::LabelLook& labelLook,
                          int ledSize)
        : scale_(scale)
        , ledSize_(ledSize)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            TestScaleColumns::kScaleLabelColumnDesignWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        ledOff_ = createLed(skin, 0.0f);
        ledOn_ = createLed(skin, 1.0f);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledLedSize = TestScaleColumns::scaledSize(ledSize_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledLedSize
             + rowGap
             + scaledLedSize;
    }

    void resized() override
    {
        const int scaledColumnWidth = TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
        const int scaledLedSize = TestScaleColumns::scaledSize(ledSize_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstLedY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondLedY = firstLedY + scaledLedSize + rowGap;

        scaleLabel_->setBounds(0, 0, scaledColumnWidth, TestScaleColumns::kScaleLabelHeight);
        ledOff_->setBounds(0, firstLedY, scaledLedSize, scaledLedSize);
        ledOn_->setBounds(0, secondLedY, scaledLedSize, scaledLedSize);
    }

    void setSkin(TSS::ISkin& skin)
    {
        ledOff_->setSkin(skin);
        ledOn_->setSkin(skin);
    }

    void pulseLeds()
    {
        ledOff_->setLevel(1.0f);
    }

    void tickLedDecay()
    {
        ledOff_->setLevel(0.0f);
        ledOn_->setLevel(1.0f);
    }

private:
    std::unique_ptr<TSS::Led> createLed(TSS::ISkin& skin, float level)
    {
        auto led = std::make_unique<TSS::Led>(ledSize_, ledSize_);
        led->setSkin(skin);
        led->setUiScale(scale_);
        led->setLevel(level);
        addAndMakeVisible(*led);
        return led;
    }

    float scale_ { 1.0f };
    int ledSize_ = 0;
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::Led> ledOff_;
    std::unique_ptr<TSS::Led> ledOn_;
};

namespace
{
    constexpr int kAnimationTimerHz_ = 30;
    constexpr int kPulseIntervalMs_ = 1000;
    constexpr int kTimerIntervalMs_ = 1000 / kAnimationTimerHz_;
}

TestActivityLeds::TestActivityLeds(TSS::ISkin& skin, int ledSize)
    : ledSize_(ledSize)
    , msUntilNextPulse_(kPulseIntervalMs_)
{
    setSkin(skin);
}

TestActivityLeds::~TestActivityLeds()
{
    stopTimer();
}

void TestActivityLeds::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestActivityLeds::resized()
{
    layoutColumnPanels();
}

void TestActivityLeds::visibilityChanged()
{
    if (isVisible())
        startTimerHz(kAnimationTimerHz_);
    else
        stopTimer();
}

int TestActivityLeds::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestActivityLeds::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestActivityLeds::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ActivityLedScalePanel>(
            spec.scale,
            spec.label,
            *skin_,
            labelLook,
            ledSize_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestActivityLeds::layoutColumnPanels()
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

void TestActivityLeds::pulseAllLeds()
{
    for (auto& panel : columnPanels_)
        panel->pulseLeds();
}

void TestActivityLeds::tickAllLedsDecay()
{
    for (auto& panel : columnPanels_)
        panel->tickLedDecay();
}

void TestActivityLeds::timerCallback()
{
    msUntilNextPulse_ -= kTimerIntervalMs_;

    if (msUntilNextPulse_ <= 0)
    {
        msUntilNextPulse_ = kPulseIntervalMs_;
        pulseAllLeds();
        return;
    }

    tickAllLedsDecay();
}
