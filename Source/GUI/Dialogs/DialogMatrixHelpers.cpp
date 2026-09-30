#include "DialogMatrixHelpers.h"

#include <cmath>

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/Skin.h"

using TSS::SkinColourId;

namespace
{
    constexpr int kMeasureMaxHeight = 4000;
    constexpr int kMaxBodyLines = 40;
    constexpr int kBodyHeightSlack = 2;
    constexpr int kButtonLabelPaddingDesign = 12;
    constexpr int kButtonWidthMax = 280;
    // Breathing room between body and buttons; extra controls get one em above and below.
    constexpr float kBandEmPlain = 1.5f;
    constexpr float kBandEmAroundExtra = 2.0f;
    constexpr float kToggleTickFactor = 1.1f;
    constexpr float kToggleTickMaxHeightFraction = 0.75f;
    constexpr int kToggleTextLeadDesign = 10;
    constexpr int kToggleTextTrailDesign = 2;

    int scaled(int designValue, float uiScale)
    {
        return juce::roundToInt(static_cast<float>(designValue) * uiScale);
    }
}

namespace DialogMatrixHelpers
{
    std::unique_ptr<TSS::Button> makeButton(TSS::ISkin& skin,
                                            int designWidth,
                                            const juce::String& text,
                                            int designHeight)
    {
        auto button = std::make_unique<TSS::Button>(
            designWidth, designHeight, TSS::buttonLookFromSkin(skin), text);
        button->setMouseClickGrabsKeyboardFocus(false);
        return button;
    }

    void applyButtonSkin(TSS::Button& button, TSS::ISkin& skin)
    {
        button.setLook(TSS::buttonLookFromSkin(skin));
    }

    void applyButtonUiScale(TSS::Button& button, float uiScale)
    {
        button.setUiScale(uiScale);
    }

    juce::Font scaledModalBodyFont(const TSS::ISkin& skin, float uiScale)
    {
        return skin.getModalBodyFont().withHeight(skin.getModalBodyFont().getHeight() * uiScale);
    }

    juce::Font scaledTitleFont(const TSS::ISkin& skin, float uiScale)
    {
        return skin.getBaseFontBold().withHeight(skin.getBaseFontBold().getHeight() * uiScale);
    }

    int estimateButtonWidth(TSS::ISkin& skin, const juce::String& text, float uiScale)
    {
        const auto labelFont = TSS::buttonLookFromSkin(skin).font;
        const auto font = labelFont.withHeight(labelFont.getHeight() * uiScale);
        const int textWidth = juce::GlyphArrangement::getStringWidthInt(font, text);
        const int padded = textWidth + scaled(kButtonLabelPaddingDesign, uiScale) * 2;
        return juce::jlimit(scaled(kDefaultButtonWidth, uiScale), scaled(kButtonWidthMax, uiScale), padded);
    }

    void layoutCentredButtonRow(juce::Rectangle<int> row,
                                float uiScale,
                                const std::vector<ButtonPlacement>& buttons)
    {
        if (buttons.empty())
            return;

        const int count = static_cast<int>(buttons.size());
        int widthsSum = 0;
        for (const auto& button : buttons)
            widthsSum += button.width;

        // Shrink gaps first, then widths, so the pack never leaves the row.
        int gap = scaled(kButtonGap, uiScale);
        float widthFactor = 1.0f;
        if (widthsSum + gap * (count - 1) > row.getWidth())
        {
            gap = count > 1 ? juce::jmax(0, (row.getWidth() - widthsSum) / (count - 1)) : 0;
            if (widthsSum > row.getWidth())
                widthFactor = static_cast<float>(row.getWidth()) / static_cast<float>(juce::jmax(1, widthsSum));
        }

        int packWidth = gap * (count - 1);
        for (const auto& button : buttons)
            packWidth += juce::roundToInt(static_cast<float>(button.width) * widthFactor);

        int x = row.getX() + juce::jmax(0, (row.getWidth() - packWidth) / 2);
        for (const auto& button : buttons)
        {
            const int width = juce::roundToInt(static_cast<float>(button.width) * widthFactor);
            if (button.component != nullptr)
                button.component->setBounds(x, row.getY(), width, row.getHeight());
            x += width + gap;
        }
    }

    int contentWidthFor(int designWidth, float uiScale)
    {
        return scaled(designWidth, uiScale);
    }

    int bodyTextWidthFor(int contentWidth)
    {
        const int sideInset = juce::roundToInt(static_cast<float>(contentWidth) * kBodySideInsetFraction);
        return juce::jmax(1, contentWidth - sideInset * 2);
    }

    int measureBodyHeight(const juce::Font& font, const juce::String& text, int textWidth)
    {
        juce::GlyphArrangement glyphs;
        glyphs.addFittedText(font,
                             text,
                             0.0f,
                             0.0f,
                             static_cast<float>(textWidth),
                             static_cast<float>(kMeasureMaxHeight),
                             juce::Justification::topLeft,
                             kMaxBodyLines,
                             1.0f);

        if (glyphs.getNumGlyphs() == 0)
            return 0;

        const auto box = glyphs.getBoundingBox(0, glyphs.getNumGlyphs(), true);
        return static_cast<int>(std::ceil(box.getBottom())) + kBodyHeightSlack;
    }

    int measureLineStep(const juce::Font& font, int textWidth)
    {
        return measureBodyHeight(font, "A\nA", textWidth) - measureBodyHeight(font, "A", textWidth);
    }

    void paintBodyText(juce::Graphics& g,
                       const juce::Font& font,
                       const juce::String& text,
                       juce::Rectangle<int> textArea)
    {
        g.setFont(font);
        g.drawFittedText(text, textArea, juce::Justification::topLeft, kMaxBodyLines, 1.0f);
    }

    ModalGeometry computeModalGeometry(const ModalGeometryArgs& args)
    {
        ModalGeometry geometry;
        geometry.border = scaled(kBorderThickness, args.uiScale);
        geometry.titleBarHeight = scaled(kTitleBarHeight, args.uiScale);

        const int contentWidth = contentWidthFor(args.designWidth, args.uiScale);
        const int sideInset = (contentWidth - bodyTextWidthFor(contentWidth)) / 2;
        const int gapUnderTitle = juce::roundToInt(args.bodyEm);
        const int buttonHeight = scaled(kDefaultButtonHeight, args.uiScale);
        // Frozen: bottom margin is never smaller than the side inset.
        const int bottomMargin = juce::jmax(scaled(kButtonBottomMargin, args.uiScale), sideInset);
        const float bandEm = args.extraBandHeight > 0 ? kBandEmAroundExtra : kBandEmPlain;
        const int bandHeight = args.extraBandHeight + juce::roundToInt(args.bodyEm * bandEm);

        const int contentHeight = gapUnderTitle + args.bodyHeight + bandHeight + buttonHeight + bottomMargin;
        geometry.dialogBounds = args.hostBounds.withSizeKeepingCentre(
            contentWidth + geometry.border * 2,
            contentHeight + geometry.titleBarHeight + geometry.border * 2);

        auto content = geometry.dialogBounds.reduced(geometry.border);
        content.removeFromTop(geometry.titleBarHeight);

        geometry.textArea = { content.getX() + sideInset,
                              content.getY() + gapUnderTitle,
                              content.getWidth() - sideInset * 2,
                              args.bodyHeight };
        geometry.buttonRow = { content.getX(),
                               content.getBottom() - bottomMargin - buttonHeight,
                               content.getWidth(),
                               buttonHeight };
        geometry.band = { content.getX(),
                          geometry.textArea.getBottom(),
                          content.getWidth(),
                          geometry.buttonRow.getY() - geometry.textArea.getBottom() };
        return geometry;
    }

    TextModalLayout computeTextModalLayout(const TextModalLayoutArgs& args)
    {
        TextModalLayout layout;
        layout.bodyFont = scaledModalBodyFont(args.skin, args.uiScale);

        const int textWidth = bodyTextWidthFor(contentWidthFor(args.designWidth, args.uiScale));
        const int bodyHeight = measureBodyHeight(layout.bodyFont, args.bodyText, textWidth);

        layout.geometry = computeModalGeometry({ .hostBounds = args.hostBounds,
                                                 .designWidth = args.designWidth,
                                                 .uiScale = args.uiScale,
                                                 .bodyEm = layout.bodyFont.getHeight(),
                                                 .bodyHeight = bodyHeight,
                                                 .extraBandHeight = args.extraBandHeight });
        return layout;
    }

    void paintMatrixOverlayChrome(const OverlayChromePaintArgs& args)
    {
        args.g.fillAll(args.skin.getColour(SkinColourId::kBodyPanelBackground).withAlpha(0.85f));

        args.g.setColour(juce::Colour(kDialogBorderColour));
        args.g.fillRect(args.dialogBounds);

        auto inner = args.dialogBounds.reduced(args.borderThickness);
        auto titleBar = inner.removeFromTop(args.titleBarHeight);
        auto content = inner;

        args.g.setColour(args.skin.getColour(SkinColourId::kHeaderPanelBackground));
        args.g.fillRect(titleBar);
        args.g.fillRect(content);

        args.g.setColour(args.skin.getColour(SkinColourId::kDarkPanelText));
        args.g.setFont(scaledTitleFont(args.skin, args.uiScale));
        args.g.drawText(args.title.toUpperCase(), titleBar, juce::Justification::centred, false);
    }

    void ModalToggleLookAndFeel::setLabelFont(juce::Font font)
    {
        labelFont_ = std::move(font);
    }

    void ModalToggleLookAndFeel::setUiScale(float uiScale)
    {
        uiScale_ = uiScale;
    }

    float ModalToggleLookAndFeel::getTickWidth(int buttonHeight) const
    {
        const float tickBase = juce::jmin(labelFont_.getHeight(),
                                          static_cast<float>(buttonHeight) * kToggleTickMaxHeightFraction);
        return tickBase * kToggleTickFactor;
    }

    int ModalToggleLookAndFeel::getPreferredWidth(const juce::String& text, int buttonHeight) const
    {
        const int textWidth = juce::GlyphArrangement::getStringWidthInt(labelFont_, text);
        return textWidth + juce::roundToInt(getTickWidth(buttonHeight))
               + scaled(kToggleTextLeadDesign, uiScale_) + scaled(kToggleTextTrailDesign, uiScale_);
    }

    void ModalToggleLookAndFeel::drawToggleButton(juce::Graphics& g,
                                                  juce::ToggleButton& button,
                                                  bool shouldDrawButtonAsHighlighted,
                                                  bool shouldDrawButtonAsDown)
    {
        const float tickWidth = getTickWidth(button.getHeight());

        drawTickBox(g,
                    button,
                    0.0f,
                    (static_cast<float>(button.getHeight()) - tickWidth) * 0.5f,
                    tickWidth,
                    tickWidth,
                    button.getToggleState(),
                    button.isEnabled(),
                    shouldDrawButtonAsHighlighted,
                    shouldDrawButtonAsDown);

        g.setColour(button.findColour(juce::ToggleButton::textColourId));
        g.setFont(labelFont_);
        g.drawFittedText(button.getButtonText(),
                         button.getLocalBounds().withTrimmedLeft(juce::roundToInt(tickWidth) + scaled(kToggleTextLeadDesign, uiScale_)),
                         juce::Justification::centredLeft,
                         1,
                         1.0f);
    }
}
