#include "TestButtons.h"

#include <array>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Tests/TestScaleColumns.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/Label.h"

namespace
{
    struct ButtonSetDimensions
    {
        int gap;
        int mainButtonWidth;
        int mainButtonHeight;
        int smallButtonSize;
        int bigButtonWidth;
        int bigButtonHeight;
        int scaleLabelHeight;
        float largeTextFontHeight;
    };
}

class TestButtons::ButtonScalePanel : public juce::Component
{
public:
    ButtonScalePanel(float scale,
                     const juce::String& scaleLabelText,
                     const TSS::ButtonLook& buttonLook,
                     const TSS::LabelLook& labelLook,
                     const ButtonSetDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
    {
        auto largeTextButtonLook = buttonLook;
        largeTextButtonLook.font = largeTextButtonLook.font.withHeight(dimensions_.largeTextFontHeight);

        scaleLabel_ = std::make_unique<TSS::Label>(
            dimensions_.bigButtonWidth,
            dimensions_.scaleLabelHeight,
            labelLook,
            scaleLabelText);
        addAndMakeVisible(*scaleLabel_);

        mainButton_ = createButton(dimensions_.mainButtonWidth, dimensions_.mainButtonHeight, buttonLook, "BUTTON");
        initButton_ = createButton(dimensions_.smallButtonSize, dimensions_.smallButtonSize, buttonLook, "I");
        copyButton_ = createButton(dimensions_.smallButtonSize, dimensions_.smallButtonSize, buttonLook, "C");
        pasteButton_ = createButton(dimensions_.smallButtonSize, dimensions_.smallButtonSize, buttonLook, "P");
        bigButton_ = createButton(dimensions_.bigButtonWidth, dimensions_.bigButtonHeight, buttonLook, "BIG BUTTON");
        bigTextButton_ = createButton(dimensions_.bigButtonWidth, dimensions_.bigButtonHeight, largeTextButtonLook, "BIG BUTTON");
    }

    int getScaledColumnWidth() const
    {
        return TestScaleColumns::scaledSize(dimensions_.bigButtonWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledGap = TestScaleColumns::scaledSize(dimensions_.gap, scale_);
        const int scaledMainButtonHeight = TestScaleColumns::scaledSize(dimensions_.mainButtonHeight, scale_);
        const int scaledBigButtonHeight = TestScaleColumns::scaledSize(dimensions_.bigButtonHeight, scale_);
        const int labelToButtonsGap = dimensions_.gap;

        return dimensions_.scaleLabelHeight
         + labelToButtonsGap
         + scaledMainButtonHeight
         + scaledGap
         + scaledBigButtonHeight
         + scaledGap
         + scaledBigButtonHeight;
    }

    void resized() override
    {
        const int scaledGap = TestScaleColumns::scaledSize(dimensions_.gap, scale_);
        const int scaledMainButtonWidth = TestScaleColumns::scaledSize(dimensions_.mainButtonWidth, scale_);
        const int scaledMainButtonHeight = TestScaleColumns::scaledSize(dimensions_.mainButtonHeight, scale_);
        const int scaledSmallButtonSize = TestScaleColumns::scaledSize(dimensions_.smallButtonSize, scale_);
        const int scaledBigButtonWidth = TestScaleColumns::scaledSize(dimensions_.bigButtonWidth, scale_);
        const int scaledBigButtonHeight = TestScaleColumns::scaledSize(dimensions_.bigButtonHeight, scale_);

        const int labelY = 0;
        const int buttonsStartY = dimensions_.scaleLabelHeight + dimensions_.gap;
        const int initX = scaledMainButtonWidth + scaledGap;
        const int copyX = initX + scaledSmallButtonSize;
        const int pasteX = copyX + scaledSmallButtonSize;
        const int secondRowY = buttonsStartY + scaledMainButtonHeight + scaledGap;
        const int thirdRowY = secondRowY + scaledBigButtonHeight + scaledGap;

        scaleLabel_->setBounds(0, labelY, scaledBigButtonWidth, dimensions_.scaleLabelHeight);
        mainButton_->setBounds(0, buttonsStartY, scaledMainButtonWidth, scaledMainButtonHeight);
        initButton_->setBounds(initX, buttonsStartY, scaledSmallButtonSize, scaledSmallButtonSize);
        copyButton_->setBounds(copyX, buttonsStartY, scaledSmallButtonSize, scaledSmallButtonSize);
        pasteButton_->setBounds(pasteX, buttonsStartY, scaledSmallButtonSize, scaledSmallButtonSize);
        bigButton_->setBounds(0, secondRowY, scaledBigButtonWidth, scaledBigButtonHeight);
        bigTextButton_->setBounds(0, thirdRowY, scaledBigButtonWidth, scaledBigButtonHeight);
    }

private:
    std::unique_ptr<TSS::Button> createButton(int width,
                                              int height,
                                              const TSS::ButtonLook& look,
                                              const juce::String& text)
    {
        auto button = std::make_unique<TSS::Button>(width, height, look, text);
        button->setUiScale(scale_);
        addAndMakeVisible(*button);
        return button;
    }

    float scale_ { 1.0f };
    ButtonSetDimensions dimensions_ {};
    std::unique_ptr<TSS::Label> scaleLabel_;
    std::unique_ptr<TSS::Button> mainButton_;
    std::unique_ptr<TSS::Button> initButton_;
    std::unique_ptr<TSS::Button> copyButton_;
    std::unique_ptr<TSS::Button> pasteButton_;
    std::unique_ptr<TSS::Button> bigButton_;
    std::unique_ptr<TSS::Button> bigTextButton_;
};

TestButtons::TestButtons(TSS::ISkin& skin)
{
    setSkin(skin);
}

TestButtons::~TestButtons() = default;

void TestButtons::setSkin(TSS::ISkin& skin)
{
    skin_ = &skin;
    createColumnPanels(skin);
    layoutColumnPanels();
}

void TestButtons::resized()
{
    layoutColumnPanels();
}

int TestButtons::getPreferredWidth() const
{
    const int panelGap = TestScaleColumns::kGap * TestScaleColumns::kPanelGapMultiplier;
    return TestScaleColumns::sumPanelRowWidth(columnPanels_, panelGap,
        [](const std::unique_ptr<ButtonScalePanel>& panel) { return panel->getScaledColumnWidth(); });
}

int TestButtons::getPreferredHeight() const
{
    int maxHeight = 0;
    for (const auto& panel : columnPanels_)
        maxHeight = juce::jmax(maxHeight, panel->getPreferredHeight());

    return maxHeight;
}

void TestButtons::createColumnPanels(TSS::ISkin& skin)
{
    const ButtonSetDimensions dimensions
    {
        kGap_,
        kMainButtonWidth_,
        kMainButtonHeight_,
        kSmallButtonSize_,
        kBigButtonWidth_,
        kBigButtonHeight_,
        kScaleLabelHeight_,
        kLargeTextFontHeight_
    };

    const auto buttonLook = TSS::buttonLookFromSkin(skin);
    const auto labelLook = TSS::labelLookFromSkin(skin);

    columnPanels_.clear();
    removeAllChildren();
    columnPanels_.reserve(TestScaleColumns::kSpecs.size());

    for (const auto& spec : TestScaleColumns::kSpecs)
    {
        auto panel = std::make_unique<ButtonScalePanel>(
            spec.scale,
            spec.label,
            buttonLook,
            labelLook,
            dimensions);
        addAndMakeVisible(*panel);
        columnPanels_.push_back(std::move(panel));
    }
}

void TestButtons::layoutColumnPanels()
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
