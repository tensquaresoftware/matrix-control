#include "PopupMenuRenderer.h"
#include "ComboBox.h"

#include "GUI/Skins/ISkin.h"

namespace tss
{
    PopupMenuRenderer::PopupMenuRenderer(tss::ISkin& skin, bool isButtonLike)
        : skin_(skin)
        , isButtonLike_(isButtonLike)
    {
    }

    void PopupMenuRenderer::drawBackground(juce::Graphics& g, const juce::Rectangle<int>& bounds) const
    {
        const auto backgroundColour = skin_.getPopupMenuBackgroundColour(isButtonLike_);
        g.setColour(backgroundColour);
        g.fillRect(bounds);
    }

    void PopupMenuRenderer::drawBorder(juce::Graphics& g, const juce::Rectangle<int>& bounds) const
    {
        constexpr float kBorderThickness = 1.0f;
        const auto borderColour = skin_.getPopupMenuBorderColour(isButtonLike_);
        g.setColour(borderColour);
        g.drawRect(bounds.toFloat(), kBorderThickness);
    }

    void PopupMenuRenderer::drawItem(juce::Graphics& g, const ComboBox& comboBox, int itemIndex,
                                    const juce::Rectangle<int>& itemBounds, int highlightedItemIndex,
                                    const juce::Font& font) const
    {
        const auto isHighlighted = (highlightedItemIndex == itemIndex);
        const auto isActive = comboBox.getItemId(itemIndex) != 0;
        
        if (isHighlighted && isActive)
        {
            const auto hooverBackgroundColour = skin_.getPopupMenuBackgroundHooverColour(isButtonLike_);
            const auto hooverBounds = itemBounds.reduced(1);
            g.setColour(hooverBackgroundColour);
            g.fillRect(hooverBounds);
            
            const auto hooverTextColour = skin_.getPopupMenuTextHooverColour(isButtonLike_);
            g.setColour(hooverTextColour);
            g.setFont(font);
            
            auto textBounds = itemBounds;
            textBounds.removeFromLeft(kTextLeftPadding_);
            g.drawText(comboBox.getItemText(itemIndex), textBounds, juce::Justification::centredLeft, false);
        }
        else
        {
            auto textColour = skin_.getPopupMenuTextColour(isButtonLike_);
            if (! isActive)
            {
                textColour = textColour.withAlpha(0.5f);
            }
            
            g.setColour(textColour);
            g.setFont(font);
            
            auto textBounds = itemBounds;
            textBounds.removeFromLeft(kTextLeftPadding_);
            g.drawText(comboBox.getItemText(itemIndex), textBounds, juce::Justification::centredLeft, false);
        }
    }

    void PopupMenuRenderer::drawVerticalSeparators(juce::Graphics& g, const juce::Rectangle<int>& contentBounds,
                                                  int columnCount, int columnWidth, int separatorWidth) const
    {
        if (columnCount <= 1)
        {
            return;
        }
        
        const auto separatorColour = skin_.getPopupMenuSeparatorColour(isButtonLike_);
        g.setColour(separatorColour);
        
        for (int i = 1; i < columnCount; ++i)
        {
            const auto separatorX = contentBounds.getX() + i * columnWidth + (i - 1) * separatorWidth;
            g.fillRect(separatorX, contentBounds.getY(), separatorWidth, contentBounds.getHeight());
        }
    }
}
