#include "TestActivityLeds.h"

#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/ActivityLed.h"
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

private:
    std::unique_ptr<TSS::ActivityLed> createLed(TSS::ISkin& skin, float level)
    {
        auto led = std::make_unique<TSS::ActivityLed>(ledSize_, ledSize_);
        led->setSkin(skin);
        led->setUiScale(scale_);
        led->setLevel(level);
        addAndMakeVisible(*led);
        return led;
    }

    float scale_ { 1.0f };
    int ledSize_ = 0;
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::ActivityLed> ledOff_;
    std::unique_ptr<TSS::ActivityLed> ledOn_;
};

TestActivityLeds::TestActivityLeds(TSS::ISkin& skin, int ledSize)
    : ledSize_(ledSize)
{
    setSkin(skin);
}

TestActivityLeds::~TestActivityLeds() = default;

void TestActivityLeds::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestActivityLeds::resized()
{
    layoutColumnPanels();
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
