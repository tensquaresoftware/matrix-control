#include "TestPopupMenus.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"

class TestPopupMenus::PopupMenuScalePanel : public juce::Component
{
public:
    PopupMenuScalePanel(float scale,
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
            comboWidth_,
            TestScaleColumns::kScaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        combo_ = std::make_unique<TSS::ComboBox>(comboWidth_, comboHeight_, comboLook_);
        combo_->setPopupMenuLook(popupLook_);
        combo_->setUiScale(scale_);
        for (int itemIndex = 1; itemIndex <= 8; ++itemIndex)
            combo_->addItem("Item " + juce::String(itemIndex), itemIndex);
        combo_->setSelectedId(1, juce::dontSendNotification);
        addAndMakeVisible(*combo_);
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(comboWidth_, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledComboHeight = TestScaleColumns::scaledSize(comboHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;

        return TestScaleColumns::kScaleLabelHeight + rowGap + scaledComboHeight;
    }

    void resized() override
    {
        const int scaledWidth = TestScaleColumns::scaledSize(comboWidth_, scale_);
        const int scaledHeight = TestScaleColumns::scaledSize(comboHeight_, scale_);
        const int rowGap = TestScaleColumns::kGap;
        const int comboY = TestScaleColumns::kScaleLabelHeight + rowGap;

        scaleLabel_->setBounds(0, 0, scaledWidth, TestScaleColumns::kScaleLabelHeight);
        combo_->setBounds(0, comboY, scaledWidth, scaledHeight);
    }

private:
    float scale_ { 1.0f };
    int comboWidth_ = 0;
    int comboHeight_ = 0;
    TSS::ComboBoxLook comboLook_ {};
    TSS::PopupMenuLook popupLook_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::ComboBox> combo_;
};

TestPopupMenus::TestPopupMenus(TSS::ISkin& skin, int comboWidth, int comboHeight)
    : comboWidth_(comboWidth)
    , comboHeight_(comboHeight)
{
    setSkin(skin);
}

TestPopupMenus::~TestPopupMenus() = default;

void TestPopupMenus::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    rebuildPanels();
}

void TestPopupMenus::resized()
{
    layoutColumnPanels();
}

int TestPopupMenus::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const auto& panel) { return panel->getScaledColumnWidth(); });
}

int TestPopupMenus::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestPopupMenus::rebuildPanels()
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
        auto panel = std::make_unique<PopupMenuScalePanel>(
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

void TestPopupMenus::layoutColumnPanels()
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
