#include "ModuleHeader.h"

#include "GUI/Widgets/ModuleHeaderTitleColour.h"

namespace TSS
{
    void ModuleHeader::setTitleClickHandler(std::function<void()> handler)
    {
        titleClickHandler_ = std::move(handler);

        if (! isTitleClickEnabled())
        {
            clearTitleHover();
            return;
        }

        // Title-only headers keep clicks; action buttons (if any) still receive theirs.
        setInterceptsMouseClicks(true, true);
        // Added early in panels — stay above siblings so hover is not stolen by overlap.
        toFront(false);

        if (isMouseOver(false))
            syncTitleHoverFromPosition(getMouseXYRelative().toFloat());
        else
            clearTitleHover();
    }

    juce::Colour ModuleHeader::resolveTitleTextColour() const
    {
        return resolveModuleHeaderTitleTextColour(isTitleClickEnabled(), titleHovered_, look_.text, look_.textHover);
    }

    juce::Rectangle<float> ModuleHeader::getTitleInteractionBounds() const
    {
        const auto paintBounds = getTitlePaintBounds();
        if (text_.isEmpty())
            return {};

        const float textLeftPadding = static_cast<float>(dimensions_.textLeftPadding) * uiScale_;
        const auto font = look_.font.withHeight(look_.font.getHeight() * uiScale_);
        const float textWidth = juce::GlyphArrangement::getStringWidth(font, text_);
        const float maxWidth = juce::jmax(0.0f, paintBounds.getWidth() - textLeftPadding);

        auto hit = paintBounds;
        hit.setX(paintBounds.getX() + textLeftPadding);
        hit.setWidth(juce::jmin(textWidth, maxWidth));
        return hit;
    }

    bool ModuleHeader::isPointInTitleBand(juce::Point<float> localPoint) const
    {
        return getTitleInteractionBounds().contains(localPoint);
    }

    void ModuleHeader::clearTitleHover()
    {
        if (! titleHovered_ && getMouseCursor() == juce::MouseCursor::NormalCursor)
            return;

        titleHovered_ = false;
        setMouseCursor(juce::MouseCursor::NormalCursor);
        repaint();
    }

    void ModuleHeader::syncTitleHoverFromPosition(juce::Point<float> localPoint)
    {
        if (! isTitleClickEnabled())
        {
            clearTitleHover();
            return;
        }

        const bool overTitle = isPointInTitleBand(localPoint);
        if (overTitle == titleHovered_)
        {
            if (overTitle)
                setMouseCursor(juce::MouseCursor::PointingHandCursor);
            return;
        }

        titleHovered_ = overTitle;
        setMouseCursor(overTitle ? juce::MouseCursor::PointingHandCursor
                                 : juce::MouseCursor::NormalCursor);
        repaint();
    }

    void ModuleHeader::mouseEnter(const juce::MouseEvent& event)
    {
        syncTitleHoverFromPosition(event.position);
    }

    void ModuleHeader::mouseMove(const juce::MouseEvent& event)
    {
        syncTitleHoverFromPosition(event.position);
    }

    void ModuleHeader::mouseExit(const juce::MouseEvent&)
    {
        clearTitleHover();
    }

    void ModuleHeader::mouseDown(const juce::MouseEvent& event)
    {
        if (! event.mods.isLeftButtonDown())
            return;

        syncTitleHoverFromPosition(event.position);

        if (! isTitleClickEnabled() || ! isPointInTitleBand(event.position))
            return;

        if (titleClickHandler_)
            titleClickHandler_();
    }
}
