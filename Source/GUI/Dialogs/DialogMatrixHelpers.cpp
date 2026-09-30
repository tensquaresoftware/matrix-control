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

        std::vector<int> widths;
        widths.reserve(buttons.size());
        for (const auto& button : buttons)
            widths.push_back(button.width);

        const auto pack = measureCentredButtonPack(row.getWidth(), uiScale, widths);
        int x = row.getX() + pack.leftInset;

        // Recompute per-button widths with the same shrink factor as measureCentredButtonPack.
        const int count = static_cast<int>(buttons.size());
        const int sideMargin = scaled(kButtonSideMargin, uiScale);
        const int maxPackWidth = juce::jmax(1, row.getWidth() - sideMargin * 2);
        int widthsSum = 0;
        for (int width : widths)
            widthsSum += width;

        int gap = scaled(kButtonGap, uiScale);
        float widthFactor = 1.0f;
        if (widthsSum + gap * (count - 1) > maxPackWidth)
        {
            gap = count > 1 ? juce::jmax(0, (maxPackWidth - widthsSum) / (count - 1)) : 0;
            if (widthsSum > maxPackWidth)
                widthFactor = static_cast<float>(maxPackWidth) / static_cast<float>(juce::jmax(1, widthsSum));
        }

        for (const auto& button : buttons)
        {
            const int width = juce::roundToInt(static_cast<float>(button.width) * widthFactor);
            if (button.component != nullptr)
                button.component->setBounds(x, row.getY(), width, row.getHeight());
            x += width + gap;
        }
    }

    CentredButtonPackMetrics measureCentredButtonPack(int rowWidth,
                                                      float uiScale,
                                                      const std::vector<int>& buttonWidths)
    {
        CentredButtonPackMetrics metrics;
        if (buttonWidths.empty())
            return metrics;

        const int count = static_cast<int>(buttonWidths.size());
        const int sideMargin = scaled(kButtonSideMargin, uiScale);
        const int maxPackWidth = juce::jmax(1, rowWidth - sideMargin * 2);

        int widthsSum = 0;
        for (int width : buttonWidths)
            widthsSum += width;

        int gap = scaled(kButtonGap, uiScale);
        float widthFactor = 1.0f;
        if (widthsSum + gap * (count - 1) > maxPackWidth)
        {
            gap = count > 1 ? juce::jmax(0, (maxPackWidth - widthsSum) / (count - 1)) : 0;
            if (widthsSum > maxPackWidth)
                widthFactor = static_cast<float>(maxPackWidth) / static_cast<float>(juce::jmax(1, widthsSum));
        }

        metrics.packWidth = gap * (count - 1);
        for (int width : buttonWidths)
            metrics.packWidth += juce::roundToInt(static_cast<float>(width) * widthFactor);

        metrics.leftInset = juce::jmax(sideMargin, (rowWidth - metrics.packWidth) / 2);
        return metrics;
    }

    int contentWidthFor(int designWidth, float uiScale)
    {
        return scaled(designWidth, uiScale);
    }

    int bodyTextWidthFor(int contentWidth)
    {
        const int sideInset = juce::roundToInt(static_cast<float>(contentWidth) * kBodySideInsetFraction);
        return bodyTextWidthFor(contentWidth, sideInset, sideInset);
    }

    int bodyTextWidthFor(int contentWidth, int leftInset, int rightInset)
    {
        return juce::jmax(1, contentWidth - leftInset - rightInset);
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
        const int defaultSideInset = juce::roundToInt(
            static_cast<float>(contentWidth) * kBodySideInsetFraction);
        const int leftInset = args.bodyLeftInset >= 0 ? args.bodyLeftInset : defaultSideInset;
        const int rightInset = args.bodyRightInset >= 0 ? args.bodyRightInset : defaultSideInset;
        const int gapUnderTitle = scaled(kGapAfterTitle, args.uiScale);
        const int buttonHeight = scaled(kDefaultButtonHeight, args.uiScale);
        const int bottomMargin = scaled(kButtonBottomMargin, args.uiScale);
        const int gapBeforeButtons = scaled(kGapBeforeButtons, args.uiScale);
        // Plain confirms: 24 px last content → buttons. With extra controls (Don't ask again,
        // DEVICE SETUP rows): keep 24 px above and below those controls.
        const int bandHeight = args.extraBandHeight > 0
            ? gapBeforeButtons + args.extraBandHeight + gapBeforeButtons
            : gapBeforeButtons;

        const int contentHeight = gapUnderTitle + args.bodyHeight + bandHeight + buttonHeight + bottomMargin;
        geometry.dialogBounds = args.hostBounds.withSizeKeepingCentre(
            contentWidth + geometry.border * 2,
            contentHeight + geometry.titleBarHeight + geometry.border * 2);

        auto content = geometry.dialogBounds.reduced(geometry.border);
        content.removeFromTop(geometry.titleBarHeight);

        geometry.textArea = { content.getX() + leftInset,
                              content.getY() + gapUnderTitle,
                              content.getWidth() - leftInset - rightInset,
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

        const int contentWidth = contentWidthFor(args.designWidth, args.uiScale);
        const int defaultSideInset = juce::roundToInt(
            static_cast<float>(contentWidth) * kBodySideInsetFraction);
        const int leftInset = args.bodyLeftInset >= 0 ? args.bodyLeftInset : defaultSideInset;
        const int rightInset = args.bodyRightInset >= 0 ? args.bodyRightInset : defaultSideInset;
        const int textWidth = bodyTextWidthFor(contentWidth, leftInset, rightInset);
        const int bodyHeight = measureBodyHeight(layout.bodyFont, args.bodyText, textWidth);

        layout.geometry = computeModalGeometry({ .hostBounds = args.hostBounds,
                                                 .designWidth = args.designWidth,
                                                 .uiScale = args.uiScale,
                                                 .bodyHeight = bodyHeight,
                                                 .extraBandHeight = args.extraBandHeight,
                                                 .bodyLeftInset = args.bodyLeftInset,
                                                 .bodyRightInset = args.bodyRightInset });
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

        // Body panel under the title band (skin header grey).
        args.g.setColour(args.skin.getColour(SkinColourId::kHeaderPanelBackground));
        args.g.fillRect(content);

        // Black title band spans the full inner width (stops at the grey border).
        args.g.setColour(juce::Colour(kModalTitleBandColour));
        args.g.fillRect(titleBar);

        // Same light grey as idle Matrix button labels (not pure white).
        args.g.setColour(args.skin.getColour(SkinColourId::kButtonTextOff));
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
