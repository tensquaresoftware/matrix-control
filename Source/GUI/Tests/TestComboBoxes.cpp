#include "TestComboBoxes.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"

class TestComboBoxes::ComboBoxScalePanel : public juce::Component
{
public:
    ComboBoxScalePanel(float scale,
                       const juce::String& scaleLabelText,
                       const TSS::ComboBoxLook& comboLook,
                       const TSS::PopupMenuLook& popupLook,
                       const TSS::LabelLook& labelLook,
                       int comboWidth,
                       int comboHeight)
        : scale_(scale)
        , comboWidth_(comboWidth)
        , comboHeight_(comboHeight)
        , comboLook_(comboLook)
        , popupLook_(popupLook)
    {
        scaleLabel_ = std::make_unique<TSS::Label>(
            TestScaleColumns::kScaleLabelColumnDesignWidth,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        standardCombo_ = createComboBox(TSS::ComboBox::Style::Standard);
        buttonLikeCombo_ = createComboBox(TSS::ComboBox::Style::ButtonLike);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledComboHeight = TestScaleColumns::scaledSize(comboHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight
             + rowGap
             + scaledComboHeight
             + rowGap
             + scaledComboHeight;
    }

    void resized() override
    {
        const int scaledColumnWidth = TestScaleColumns::scaledSize(TestScaleColumns::kScaleLabelColumnDesignWidth, scale_);
        const int scaledComboWidth = TestScaleColumns::scaledSize(comboWidth_, scale_);
        const int scaledComboHeight = TestScaleColumns::scaledSize(comboHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int firstComboY = TestScaleColumns::kScaleLabelHeight + rowGap;
        const int secondComboY = firstComboY + scaledComboHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledColumnWidth, TestScaleColumns::kScaleLabelHeight);
        standardCombo_->setBounds(0, firstComboY, scaledComboWidth, scaledComboHeight);
        buttonLikeCombo_->setBounds(0, secondComboY, scaledComboWidth, scaledComboHeight);
    }

private:
    std::unique_ptr<TSS::ComboBox> createComboBox(TSS::ComboBox::Style style)
    {
        auto combo = std::make_unique<TSS::ComboBox>(comboWidth_, comboHeight_, comboLook_, style);
        combo->setPopupMenuLook(popupLook_);
        combo->setUiScale(scale_);
        combo->addItem("Option A", 1);
        combo->addItem("Option B", 2);
        combo->addItem("Option C", 3);
        combo->setSelectedId(1, juce::dontSendNotification);
        addAndMakeVisible(*combo);
        return combo;
    }

    float scale_ { 1.0f };
    int comboWidth_ = 0;
    int comboHeight_ = 0;
    TSS::ComboBoxLook comboLook_ {};
    TSS::PopupMenuLook popupLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::ComboBox> standardCombo_;
    std::unique_ptr<TSS::ComboBox> buttonLikeCombo_;
};

TestComboBoxes::TestComboBoxes(TSS::ISkin& skin)
{
    setSkin(skin);
}

TestComboBoxes::~TestComboBoxes() = default;

void TestComboBoxes::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestComboBoxes::resized()
{
    layoutColumnPanels();
}

int TestComboBoxes::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestComboBoxes::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestComboBoxes::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto comboLook = TSS::comboBoxLookFromSkin(*skin_);
    const auto popupLook = TSS::popupMenuLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);
    const int comboWidth = TSS::ComboBox::getBaseWidth();
    const int comboHeight = TSS::ComboBox::getBaseHeight();

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ComboBoxScalePanel>(
            spec.scale,
            spec.label,
            comboLook,
            popupLook,
            labelLook,
            comboWidth,
            comboHeight);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestComboBoxes::layoutColumnPanels()
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
