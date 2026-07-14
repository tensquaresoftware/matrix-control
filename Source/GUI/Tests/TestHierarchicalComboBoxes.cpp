#include "TestHierarchicalComboBoxes.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/HierarchicalComboBox.h"
#include "GUI/Widgets/Label.h"
#include "Shared/Definitions/PluginDisplayNames.h"

class TestHierarchicalComboBoxes::HierarchicalComboBoxScalePanel : public juce::Component
{
public:
    HierarchicalComboBoxScalePanel(float scale,
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

        historyCombo_ = std::make_unique<TSS::HierarchicalComboBox>(comboWidth_, comboHeight_, comboLook_);
        historyCombo_->setPopupMenuLook(popupLook_);
        historyCombo_->setUiScale(scale_);
        populateSampleHistory(*historyCombo_);
        historyCombo_->setSelectedIds(1, 1, juce::dontSendNotification);
        addAndMakeVisible(*historyCombo_);

        emptyCombo_ = std::make_unique<TSS::HierarchicalComboBox>(comboWidth_, comboHeight_, comboLook_);
        emptyCombo_->setPopupMenuLook(popupLook_);
        emptyCombo_->setUiScale(scale_);
        emptyCombo_->setTextWhenNothingSelected(PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kEmptyHistorySentinel);
        addAndMakeVisible(*emptyCombo_);
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
        historyCombo_->setBounds(0, firstComboY, scaledComboWidth, scaledComboHeight);
        emptyCombo_->setBounds(0, secondComboY, scaledComboWidth, scaledComboHeight);
    }

private:
    static void populateSampleHistory(TSS::HierarchicalComboBox& combo)
    {
        combo.clear();
        combo.addPrimaryItem(1, "M00");
        combo.addChildItem(1, 1, PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryRootSentinel);
        combo.addChildItem(1, 2, "R00");
        combo.addChildItem(1, 3, "R01");
        combo.addPrimaryItem(2, "M01");
        combo.addChildItem(2, 1, PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryRootSentinel);
        combo.addChildItem(2, 2, "R00");
        combo.addPrimaryItem(3, "M02");
        combo.addChildItem(3, 1, PluginDisplayNames::PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kHistoryRootSentinel);
        combo.addChildItem(3, 2, "R00");
        combo.addChildItem(3, 3, "R01");
        combo.addChildItem(3, 4, "R02");
        combo.addChildItem(3, 5, "R03");
    }

    float scale_ { 1.0f };
    int comboWidth_ = 0;
    int comboHeight_ = 0;
    TSS::ComboBoxLook comboLook_ {};
    TSS::PopupMenuLook popupLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::HierarchicalComboBox> historyCombo_;
    std::unique_ptr<TSS::HierarchicalComboBox> emptyCombo_;
};

TestHierarchicalComboBoxes::TestHierarchicalComboBoxes(TSS::ISkin& skin, int comboWidth, int comboHeight)
    : comboWidth_(comboWidth)
    , comboHeight_(comboHeight)
{
    setSkin(skin);
}

TestHierarchicalComboBoxes::~TestHierarchicalComboBoxes() = default;

void TestHierarchicalComboBoxes::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestHierarchicalComboBoxes::resized()
{
    layoutColumnPanels();
}

int TestHierarchicalComboBoxes::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestHierarchicalComboBoxes::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestHierarchicalComboBoxes::rebuildPanels()
{
    if (skin_ == nullptr)
        return;

    columnPanels_.clear();
    removeAllChildren();

    const auto comboLook = TSS::comboBoxLookFromSkin(*skin_);
    const auto popupLook = TSS::popupMenuLookFromSkin(*skin_);
    const auto labelLook = TSS::labelLookFromSkin(*skin_);

    columnPanels_.reserve(TestScaleColumns::kSpecs.size());
    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<HierarchicalComboBoxScalePanel>(
            spec.scale,
            spec.label,
            comboLook,
            popupLook,
            labelLook,
            comboWidth_,
            comboHeight_);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }

    layoutColumnPanels();
}

void TestHierarchicalComboBoxes::layoutColumnPanels()
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
