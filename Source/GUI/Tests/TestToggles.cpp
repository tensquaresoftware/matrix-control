#include "TestToggles.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Toggle.h"

class TestToggles::ToggleScalePanel : public juce::Component
{
public:
    ToggleScalePanel(float scale,
                     const juce::String& scaleLabelText,
                     const TSS::ToggleLook& toggleLook,
                     const TSS::LabelLook& labelLook,
                     const ToggleDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
        , toggleLook_(toggleLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            TestScaleColumns::kScaleLabelColumnDesignWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        toggleOff_ = createToggle("OFF", false);
        toggleOn_ = createToggle("ON", true);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledToggleHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledToggleHeight
             + rowGap
             + scaledToggleHeight;
    }

    void resized() override
    {
        const int scaledColumnWidth = TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
        const int scaledToggleWidth = TestScaleColumns::scaledSize(dimensions_.patchMutatorWidth, scale_);
        const int scaledToggleHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstToggleY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondToggleY = firstToggleY + scaledToggleHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledColumnWidth, TestScaleColumns::kScaleLabelHeight);
        toggleOff_->setBounds(0, firstToggleY, scaledToggleWidth, scaledToggleHeight);
        toggleOn_->setBounds(0, secondToggleY, scaledToggleWidth, scaledToggleHeight);
    }

private:
    std::unique_ptr<TSS::Toggle> createToggle(const juce::String& text, bool isOn)
    {
        auto toggle = std::make_unique<TSS::Toggle>(
            dimensions_.patchMutatorWidth,
            dimensions_.height,
            toggleLook_,
            text);
        toggle->setUiScale(scale_);
        toggle->setToggleState(isOn, juce::dontSendNotification);
        addAndMakeVisible(*toggle);
        return toggle;
    }

    float scale_ { 1.0f };
    ToggleDimensions dimensions_ {};
    TSS::ToggleLook toggleLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::Toggle> toggleOff_;
    std::unique_ptr<TSS::Toggle> toggleOn_;
};

TestToggles::TestToggles(TSS::ISkin& skin, const ToggleDimensions& dimensions)
    : dimensions_(dimensions)
{
    setSkin(skin);
}

TestToggles::~TestToggles() = default;

void TestToggles::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestToggles::resized()
{
    layoutColumnPanels();
}

int TestToggles::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestToggles::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestToggles::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto toggleLook = TSS::toggleLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ToggleScalePanel>(
            spec.scale,
            spec.label,
            toggleLook,
            labelLook,
            dimensions_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestToggles::layoutColumnPanels()
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
