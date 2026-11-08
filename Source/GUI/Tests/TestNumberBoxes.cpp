#include "TestNumberBoxes.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/NumberBox.h"

class TestNumberBoxes::NumberBoxScalePanel : public juce::Component
{
public:
    struct Setup
    {
        float scale = 1.0f;
        juce::String scaleLabelText;
        TSS::NumberBoxLook numberBoxLook {};
        TSS::LabelLook labelLook {};
        NumberBoxDimensions dimensions {};
    };

    explicit NumberBoxScalePanel(const Setup& setup)
        : scale_(setup.scale)
        , dimensions_(setup.dimensions)
        , numberBoxLook_(setup.numberBoxLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            TestScaleColumns::kScaleLabelColumnDesignWidth,
            TestScaleColumns::kScaleLabelHeight,
            setup.labelLook,
            TSS::LabelConfig{ .text = setup.scaleLabelText });
        addAndMakeVisible(*scaleLabel_);

        valueBox_ = createNumberBox(false, false, 42);
        editableFocusBox_ = createNumberBox(true, true, 7);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledBoxHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledBoxHeight
             + rowGap
             + scaledBoxHeight;
    }

    void resized() override
    {
        const int scaledColumnWidth = TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
        const int scaledBoxWidth = TestScaleColumns::scaledSize(dimensions_.patchNumberWidth, scale_);
        const int scaledBoxHeight = TestScaleColumns::scaledSize(dimensions_.height, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstBoxY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondBoxY = firstBoxY + scaledBoxHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledColumnWidth, TestScaleColumns::kScaleLabelHeight);
        valueBox_->setBounds(0, firstBoxY, scaledBoxWidth, scaledBoxHeight);
        editableFocusBox_->setBounds(0, secondBoxY, scaledBoxWidth, scaledBoxHeight);
    }

private:
    std::unique_ptr<TSS::NumberBox> createNumberBox(bool editable, bool focusHighlight, int value)
    {
        auto box = std::make_unique<TSS::NumberBox>(
            numberBoxLook_,
            TSS::NumberBox::Config {
                .width = dimensions_.patchNumberWidth,
                .height = dimensions_.height,
                .editable = editable,
                .minValue = 0,
                .maxValue = 99 });
        box->setUiScale(scale_);
        box->setFocusHighlight(focusHighlight);
        box->setValue(value);
        addAndMakeVisible(*box);
        return box;
    }

    float scale_ { 1.0f };
    NumberBoxDimensions dimensions_ {};
    TSS::NumberBoxLook numberBoxLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::NumberBox> valueBox_;
    std::unique_ptr<TSS::NumberBox> editableFocusBox_;
};

TestNumberBoxes::TestNumberBoxes(TSS::ISkin& skin, const NumberBoxDimensions& dimensions)
    : dimensions_(dimensions)
{
    setSkin(skin);
}

TestNumberBoxes::~TestNumberBoxes() = default;

void TestNumberBoxes::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestNumberBoxes::resized()
{
    layoutColumnPanels();
}

int TestNumberBoxes::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestNumberBoxes::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestNumberBoxes::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto numberBoxLook = TSS::numberBoxLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<NumberBoxScalePanel>(NumberBoxScalePanel::Setup {
            .scale = spec.scale,
            .scaleLabelText = spec.label,
            .numberBoxLook = numberBoxLook,
            .labelLook = labelLook,
            .dimensions = dimensions_ });
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestNumberBoxes::layoutColumnPanels()
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
