#include "TestTrackGeneratorDisplays.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/TrackGeneratorDisplay.h"

class TestTrackGeneratorDisplays::TrackGeneratorDisplayScalePanel : public juce::Component
{
public:
    TrackGeneratorDisplayScalePanel(float scale,
                                    const juce::String& scaleLabelText,
                                    const TSS::TrackGeneratorDisplayLook& displayLook,
                                    const TSS::LabelLook& labelLook,
                                    const DisplayBandDimensions& dimensions)
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

        display_ = std::make_unique<TSS::TrackGeneratorDisplay>(dimensions_, displayLook_);
        display_->setUiScale(scale_);
        display_->setTrackPoint1(10, false);
        display_->setTrackPoint2(25, false);
        display_->setTrackPoint3(40, false);
        display_->setTrackPoint4(55, false);
        display_->setTrackPoint5(30, false);
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
    DisplayBandDimensions dimensions_ {};
    TSS::TrackGeneratorDisplayLook displayLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::TrackGeneratorDisplay> display_;
};

TestTrackGeneratorDisplays::TestTrackGeneratorDisplays(TSS::ISkin& skin, const DisplayBandDimensions& dimensions)
    : dimensions_(dimensions)
{
    setSkin(skin);
}

TestTrackGeneratorDisplays::~TestTrackGeneratorDisplays() = default;

void TestTrackGeneratorDisplays::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestTrackGeneratorDisplays::resized()
{
    layoutColumnPanels();
}

int TestTrackGeneratorDisplays::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestTrackGeneratorDisplays::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestTrackGeneratorDisplays::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto displayLook = TSS::trackGeneratorDisplayLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<TrackGeneratorDisplayScalePanel>(
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

void TestTrackGeneratorDisplays::layoutColumnPanels()
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
