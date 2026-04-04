#include "PopupMenuRenderer.h"
#include "ComboBox.h"

namespace tss
{
    PopupMenuRenderer::PopupMenuRenderer(bool isButtonLike, float displayScale)
        : isButtonLike_(isButtonLike)
        , displayScale_(displayScale)
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

    void PopupMenuRenderer::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds) const
    {
        const float kBorderThickness = std::max(1.0f, 1.0f * displayScale_);
        const auto borderColour = isButtonLike_ 
            ? look_.borderButtonLike
            : look_.border;
        g.setColour(borderColour);
        g.drawRect(bounds, kBorderThickness);
    }

    void PopupMenuRenderer::drawItem(juce::Graphics& g, const ComboBox& comboBox, int itemIndex,
                                    const juce::Rectangle<float>& itemBounds, int highlightedItemIndex,
                                    const juce::Font& font) const
    {
        const auto isHighlighted = (highlightedItemIndex == itemIndex);
        const auto isActive = comboBox.getItemId(itemIndex) != 0;
        
        if (isHighlighted && isActive)
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
            
            const float textPadding = static_cast<float>(kTextLeftPadding_) * displayScale_;
            const auto textBounds = itemBounds.withTrimmedLeft(textPadding);
            g.drawText(comboBox.getItemText(itemIndex), textBounds, juce::Justification::centredLeft, false);
        }
        else
        {
            auto textColour = isButtonLike_ 
                ? look_.textButtonLike
                : look_.text;
            if (! isActive)
            {
                textColour = textColour.withAlpha(0.5f);
            }
            
            g.setColour(textColour);
            g.setFont(font);
            
            const float textPadding = static_cast<float>(kTextLeftPadding_) * displayScale_;
            const auto textBounds = itemBounds.withTrimmedLeft(textPadding);
            g.drawText(comboBox.getItemText(itemIndex), textBounds, juce::Justification::centredLeft, false);
        }
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
