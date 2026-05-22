#include "TestButtons.h"

#include <array>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ISkin.h"
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

    struct ColumnSpec
    {
        float scale;
        const char* label;
    };

    int scaledSize(int value, float scale)
    {
        return juce::roundToInt(static_cast<float>(value) * scale);
    }

    constexpr auto kColumnSpecs_ = std::array<ColumnSpec, 7>
    {{
        { 0.5f, "50%" },
        { 0.75f, "75%" },
        { 1.0f, "100%" },
        { 1.25f, "125%" },
        { 1.5f, "150%" },
        { 1.75f, "175%" },
        { 2.0f, "200%" }
    }};
}

class TestButtons::ButtonScalePanel : public juce::Component
{
public:
    ButtonScalePanel(float scale,
                     const juce::String& scaleLabelText,
                     const tss::ButtonLook& buttonLook,
                     const tss::LabelLook& labelLook,
                     const ButtonSetDimensions& dimensions)
        : scale_(scale)
        , dimensions_(dimensions)
    {
        auto largeTextButtonLook = buttonLook;
        largeTextButtonLook.font = largeTextButtonLook.font.withHeight(dimensions_.largeTextFontHeight);

        scaleLabel_ = std::make_unique<tss::Label>(
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
        return scaledSize(dimensions_.bigButtonWidth, scale_);
    }

    int getPreferredHeight() const
    {
        const int scaledGap = scaledSize(dimensions_.gap, scale_);
        const int scaledMainButtonHeight = scaledSize(dimensions_.mainButtonHeight, scale_);
        const int scaledBigButtonHeight = scaledSize(dimensions_.bigButtonHeight, scale_);
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
        const int scaledGap = scaledSize(dimensions_.gap, scale_);
        const int scaledMainButtonWidth = scaledSize(dimensions_.mainButtonWidth, scale_);
        const int scaledMainButtonHeight = scaledSize(dimensions_.mainButtonHeight, scale_);
        const int scaledSmallButtonSize = scaledSize(dimensions_.smallButtonSize, scale_);
        const int scaledBigButtonWidth = scaledSize(dimensions_.bigButtonWidth, scale_);
        const int scaledBigButtonHeight = scaledSize(dimensions_.bigButtonHeight, scale_);

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
    std::unique_ptr<tss::Button> createButton(int width,
                                              int height,
                                              const tss::ButtonLook& look,
                                              const juce::String& text)
    {
        auto button = std::make_unique<tss::Button>(width, height, look, text);
        button->setUiScale(scale_);
        addAndMakeVisible(*button);
        return button;
    }

    float scale_ { 1.0f };
    ButtonSetDimensions dimensions_ {};
    std::unique_ptr<tss::Label> scaleLabel_;
    std::unique_ptr<tss::Button> mainButton_;
    std::unique_ptr<tss::Button> initButton_;
    std::unique_ptr<tss::Button> copyButton_;
    std::unique_ptr<tss::Button> pasteButton_;
    std::unique_ptr<tss::Button> bigButton_;
    std::unique_ptr<tss::Button> bigTextButton_;
};

TestButtons::TestButtons(tss::ISkin& skin)
{
    createColumnPanels(skin);
    layoutColumnPanels();
}

TestButtons::~TestButtons() = default;

void TestButtons::resized()
{
    layoutColumnPanels();
}

void TestButtons::createColumnPanels(tss::ISkin& skin)
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

    const auto buttonLook = tss::buttonLookFromSkin(skin);
    const auto labelLook = tss::labelLookFromSkin(skin);

    columnPanels_.clear();
    columnPanels_.reserve(kColumnSpecs_.size());

    for (const auto& spec : kColumnSpecs_)
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
    const int panelGap = kGap_ * kPanelGapMultiplier_;
    int nextColumnX = 0;

    for (auto& panel : columnPanels_)
    {
        const int columnWidth = panel->getScaledColumnWidth();
        panel->setBounds(nextColumnX, 0, columnWidth, panel->getPreferredHeight());
        nextColumnX += columnWidth + panelGap;
    }
}
