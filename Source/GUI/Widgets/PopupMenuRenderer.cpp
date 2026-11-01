#include "PopupMenuRenderer.h"
#include "ComboBox.h"

#include "GUI/Helpers/TextFitHelpers.h"
#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    PopupMenuRenderer::PopupMenuRenderer(float uiScale)
        : uiScale_(uiScale)
    {
    }

    void PopupMenuRenderer::setLook(const PopupMenuLook& look)
    {
        look_ = look;
    }

    void PopupMenuRenderer::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds) const
    {
        g.setColour(look_.background);
        g.fillRect(bounds);
    }

    void PopupMenuRenderer::drawBorder(juce::Graphics& g,
                                       const juce::Rectangle<float>& bounds,
                                       float systemDisplayScale,
                                       ScaledDrawing::StrokeSnapPolicy borderSnapPolicy) const
    {
        const auto& popupLayout = ComboBox::getPopupLayoutDimensions();
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(popupLayout.borderThickness),
            uiScale_,
            systemDisplayScale,
            borderSnapPolicy);
        g.setColour(look_.border);
        g.drawRect(bounds, borderThickness);
    }

    float PopupMenuRenderer::getHighlightGap(float /* systemDisplayScale */) const
    {
        // HeaderLogoPopupMenu pattern: design * uiScale, min 1 logical px (integer).
        constexpr float kHighlightGapDesign = 1.0f;
        return static_cast<float>(juce::jmax(1, juce::roundToInt(kHighlightGapDesign * uiScale_)));
    }

    void PopupMenuRenderer::drawLabelItem(juce::Graphics& g, const DrawLabelArgs& args) const
    {
        jassert(args.font != nullptr);
        const auto& font = *args.font;
        const bool showHover = args.isHighlighted && args.isEnabled;

        if (showHover)
        {
            const float gap = getHighlightGap(args.systemDisplayScale);
            auto hooverBoundsFloat = args.itemBounds.reduced(gap);
            if (args.highlightBottomExtraTrim > 0.0f)
                hooverBoundsFloat.setBottom(hooverBoundsFloat.getBottom() - args.highlightBottomExtraTrim);
            g.setColour(look_.backgroundHover);
            g.fillRect(hooverBoundsFloat);
        }

        auto textColour = showHover ? look_.textHover : look_.text;
        if (! args.isEnabled)
            textColour = look_.textDisabled;

        g.setColour(textColour);
        g.setFont(font);

        const float textPadding = static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * uiScale_;
        const auto textBounds = args.itemBounds.withTrimmedLeft(textPadding);
        const auto fitted = TextFitHelpers::fitWithAsciiEllipsis(
            args.text, font, textBounds.getWidth());
        g.drawText(fitted, textBounds, juce::Justification::centredLeft, false);
    }

    void PopupMenuRenderer::drawSubMenuChevron(juce::Graphics& g,
                                               const juce::Rectangle<float>& itemBounds,
                                               const juce::Font& font) const
    {
        const float arrowHeight = font.getHeight() * 0.6f;
        const float rightPad = static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * uiScale_;
        auto chevronBounds = itemBounds.withTrimmedRight(rightPad);
        const float arrowX = chevronBounds.removeFromRight(arrowHeight).getX();
        const float centreY = itemBounds.getCentreY();

        juce::Path chevron;
        chevron.addTriangle(arrowX,
                            centreY - arrowHeight * 0.5f,
                            arrowX,
                            centreY + arrowHeight * 0.5f,
                            arrowX + arrowHeight * 0.6f,
                            centreY);
        g.fillPath(chevron);
    }

    void PopupMenuRenderer::drawItem(juce::Graphics& g, const DrawItemArgs& args) const
    {
        const auto itemId = args.comboBox.getItemId(args.itemIndex);
        const auto isSelectable = itemId != 0 && args.comboBox.isItemEnabled(itemId);
        const auto isHighlighted = (args.highlightedItemIndex == args.itemIndex) && isSelectable;
        drawLabelItem(g, {
            .text = args.comboBox.popupListLabelForItemIndex(args.itemIndex),
            .itemBounds = args.itemBounds,
            .isHighlighted = isHighlighted,
            .isEnabled = isSelectable,
            .font = &args.font,
            .systemDisplayScale = args.systemDisplayScale,
            .highlightBottomExtraTrim = args.highlightBottomExtraTrim});
    }

    void PopupMenuRenderer::drawVerticalSeparators(juce::Graphics& g, const DrawVerticalSeparatorsArgs& args) const
    {
        if (args.columnCount <= 1)
            return;

        g.setColour(look_.separator);

        const float contentX = args.contentBounds.getX();
        const float contentY = args.contentBounds.getY();
        const float contentHeight = args.contentBounds.getHeight();

        for (int i = 1; i < args.columnCount; ++i)
        {
            const float separatorX = contentX
                + static_cast<float>(i) * args.actualColumnWidth
                + static_cast<float>(i - 1) * args.separatorWidth;
            g.fillRect(juce::Rectangle<float>(separatorX, contentY, args.separatorWidth, contentHeight));
        }
    }

    void PopupMenuRenderer::drawSentinelBottomRule(juce::Graphics& g,
                                                   const juce::Rectangle<float>& itemBounds,
                                                   float systemDisplayScale) const
    {
        const auto& popupLayout = ComboBox::getPopupLayoutDimensions();
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(popupLayout.borderThickness),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kFloor);
        g.setColour(look_.border);
        g.fillRect(itemBounds.getX(),
                   itemBounds.getBottom() - borderThickness,
                   itemBounds.getWidth(),
                   borderThickness);
    }
}
