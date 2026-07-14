#include "PopupMenuRenderer.h"
#include "ComboBox.h"

#include "GUI/Layout/ScaledDrawing.h"

namespace TSS
{
    PopupMenuRenderer::PopupMenuRenderer(bool isButtonLike, float uiScale)
        : isButtonLike_(isButtonLike)
        , uiScale_(uiScale)
    {
    }

    void PopupMenuRenderer::setLook(const PopupMenuLook& look)
    {
        look_ = look;
    }

    void PopupMenuRenderer::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds) const
    {
        const auto backgroundColour = isButtonLike_ 
            ? look_.backgroundButtonLike
            : look_.background;
        g.setColour(backgroundColour);
        g.fillRect(bounds);
    }

    void PopupMenuRenderer::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds, float systemDisplayScale) const
    {
        const auto& popupLayout = ComboBox::getPopupLayoutDimensions();
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(popupLayout.borderThickness),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const auto borderColour = isButtonLike_
            ? look_.borderButtonLike
            : look_.border;
        g.setColour(borderColour);
        g.drawRect(bounds, borderThickness);
    }

    void PopupMenuRenderer::drawLabelItem(juce::Graphics& g,
                                          const juce::String& text,
                                          const juce::Rectangle<float>& itemBounds,
                                          bool isHighlighted,
                                          bool isEnabled,
                                          const juce::Font& font) const
    {
        if (isHighlighted && isEnabled)
        {
            const auto hooverBackgroundColour = isButtonLike_
                ? look_.backgroundHoverButtonLike
                : look_.backgroundHover;
            const float gap = getHighlightGap();
            const auto hooverBoundsFloat = itemBounds.reduced(gap);
            g.setColour(hooverBackgroundColour);
            g.fillRect(hooverBoundsFloat);

            const auto hooverTextColour = isButtonLike_
                ? look_.textHoverButtonLike
                : look_.textHover;
            g.setColour(hooverTextColour);
            g.setFont(font);

            const float textPadding = static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * uiScale_;
            const auto textBounds = itemBounds.withTrimmedLeft(textPadding);
            g.drawText(text, textBounds, juce::Justification::centredLeft, false);
            return;
        }

        auto textColour = isButtonLike_
            ? look_.textButtonLike
            : look_.text;
        if (! isEnabled)
            textColour = textColour.withAlpha(0.5f);

        g.setColour(textColour);
        g.setFont(font);

        const float textPadding = static_cast<float>(ComboBox::getPopupLayoutDimensions().textLeftPadding) * uiScale_;
        const auto textBounds = itemBounds.withTrimmedLeft(textPadding);
        g.drawText(text, textBounds, juce::Justification::centredLeft, false);
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

    void PopupMenuRenderer::drawItem(juce::Graphics& g, const ComboBox& comboBox, int itemIndex,
                                    const juce::Rectangle<float>& itemBounds, int highlightedItemIndex,
                                    const juce::Font& font) const
    {
        const auto isHighlighted = (highlightedItemIndex == itemIndex);
        const auto isActive = comboBox.getItemId(itemIndex) != 0;
        drawLabelItem(g, comboBox.getItemText(itemIndex), itemBounds, isHighlighted && isActive, isActive, font);
    }

    void PopupMenuRenderer::drawVerticalSeparators(juce::Graphics& g, const juce::Rectangle<float>& contentBounds,
                                                  int columnCount, float actualColumnWidth, float separatorWidth) const
    {
        if (columnCount <= 1)
            return;
        
        const auto separatorColour = isButtonLike_ 
            ? look_.separatorButtonLike
            : look_.separator;
        g.setColour(separatorColour);
        
        const float contentX = contentBounds.getX();
        const float contentY = contentBounds.getY();
        const float contentHeight = contentBounds.getHeight();
        
        for (int i = 1; i < columnCount; ++i)
        {
            const float separatorX = contentX + static_cast<float>(i) * actualColumnWidth + static_cast<float>(i - 1) * separatorWidth;
            g.fillRect(juce::Rectangle<float>(separatorX, contentY, separatorWidth, contentHeight));
        }
    }
}
