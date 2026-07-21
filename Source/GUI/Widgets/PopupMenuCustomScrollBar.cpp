#include "PopupMenuCustomScrollBar.h"

#include "GUI/Skins/ColourChart.h"

namespace TSS
{
    ViewportPopupMenuScrollModel::ViewportPopupMenuScrollModel(juce::Viewport& viewport)
        : viewport_(viewport)
    {
    }

    int ViewportPopupMenuScrollModel::getContentHeight() const
    {
        if (const auto* content = viewport_.getViewedComponent())
            return content->getHeight();
        return 0;
    }

    int ViewportPopupMenuScrollModel::getViewportHeight() const
    {
        return viewport_.getViewHeight();
    }

    int ViewportPopupMenuScrollModel::getViewY() const
    {
        return viewport_.getViewPositionY();
    }

    void ViewportPopupMenuScrollModel::setViewY(int y)
    {
        viewport_.setViewPosition(0, y);
    }

    CallbackPopupMenuScrollModel::CallbackPopupMenuScrollModel(std::function<int()> getContentHeight,
                                                               std::function<int()> getViewportHeight,
                                                               std::function<int()> getViewY,
                                                               std::function<void(int)> setViewY)
        : getContentHeight_(std::move(getContentHeight))
        , getViewportHeight_(std::move(getViewportHeight))
        , getViewY_(std::move(getViewY))
        , setViewY_(std::move(setViewY))
    {
    }

    int CallbackPopupMenuScrollModel::getContentHeight() const
    {
        return getContentHeight_ != nullptr ? getContentHeight_() : 0;
    }

    int CallbackPopupMenuScrollModel::getViewportHeight() const
    {
        return getViewportHeight_ != nullptr ? getViewportHeight_() : 0;
    }

    int CallbackPopupMenuScrollModel::getViewY() const
    {
        return getViewY_ != nullptr ? getViewY_() : 0;
    }

    void CallbackPopupMenuScrollModel::setViewY(int y)
    {
        if (setViewY_ != nullptr)
            setViewY_(y);
    }

    PopupMenuCustomScrollBar::PopupMenuCustomScrollBar(IPopupMenuScrollModel& model,
                                                       const juce::Colour& scrollbarColour,
                                                       float thumbInset,
                                                       int minThumbHeightDesign,
                                                       float uiScale)
        : model_(model)
        , scrollbarColour_(scrollbarColour)
        , thumbInset_(thumbInset)
        , minThumbHeightDesign_(minThumbHeightDesign)
        , uiScale_(uiScale)
    {
        setRepaintsOnMouseActivity(true);
    }

    int PopupMenuCustomScrollBar::computeThumbHeight(int trackHeight, int contentHeight, int viewportHeight) const
    {
        const int minThumbHeight = juce::roundToInt(static_cast<float>(minThumbHeightDesign_) * uiScale_);
        return juce::jmax(minThumbHeight,
                          juce::roundToInt(static_cast<float>(trackHeight)
                                           * static_cast<float>(viewportHeight)
                                           / static_cast<float>(contentHeight)));
    }

    void PopupMenuCustomScrollBar::paint(juce::Graphics& g)
    {
        g.fillAll(juce::Colour(ColourChart::kTransparent));

        const int trackHeight = getHeight();
        const int trackWidth = getWidth();
        const int contentHeight = model_.getContentHeight();
        const int viewportHeight = model_.getViewportHeight();
        if (contentHeight <= viewportHeight || trackHeight <= 0)
            return;

        const int viewY = model_.getViewY();
        const int thumbHeight = computeThumbHeight(trackHeight, contentHeight, viewportHeight);
        const int range = contentHeight - viewportHeight;
        const int thumbY = (range > 0)
            ? juce::roundToInt(static_cast<float>(viewY)
                               * static_cast<float>(trackHeight - thumbHeight)
                               / static_cast<float>(range))
            : 0;

        juce::Rectangle<float> thumbBounds(0.0f,
                                           static_cast<float>(thumbY),
                                           static_cast<float>(trackWidth),
                                           static_cast<float>(thumbHeight));
        const auto colour = (isMouseOver() || isMouseButtonDown())
            ? scrollbarColour_.brighter(kThumbHighlightBrighter_)
            : scrollbarColour_;
        g.setColour(colour);
        g.fillRect(thumbBounds.reduced(thumbInset_));
    }

    void PopupMenuCustomScrollBar::mouseDown(const juce::MouseEvent& e)
    {
        if (! isEnabled())
            return;
        scrollToMousePosition(e.getPosition().y);
    }

    void PopupMenuCustomScrollBar::mouseDrag(const juce::MouseEvent& e)
    {
        if (! isEnabled())
            return;
        scrollToMousePosition(e.getPosition().y);
    }

    void PopupMenuCustomScrollBar::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
    {
        if (! isEnabled())
            return;

        const int viewportHeight = model_.getViewportHeight();
        const int delta = juce::roundToInt(-wheel.deltaY * static_cast<float>(viewportHeight) * kWheelScrollFactorScrollbar_);
        if (delta == 0)
            return;

        const int contentHeight = model_.getContentHeight();
        if (contentHeight <= viewportHeight)
            return;

        const int range = contentHeight - viewportHeight;
        const int newY = juce::jlimit(0, range, model_.getViewY() + delta);
        model_.setViewY(newY);
        repaint();
    }

    void PopupMenuCustomScrollBar::scrollToMousePosition(int mouseY)
    {
        const int trackHeight = getHeight();
        const int contentHeight = model_.getContentHeight();
        const int viewportHeight = model_.getViewportHeight();
        if (contentHeight <= viewportHeight || trackHeight <= 0)
            return;

        const int thumbHeight = computeThumbHeight(trackHeight, contentHeight, viewportHeight);
        const int range = contentHeight - viewportHeight;
        const int thumbRange = trackHeight - thumbHeight;
        if (thumbRange <= 0)
            return;

        const int viewY = juce::jlimit(
            0,
            range,
            juce::roundToInt(static_cast<float>(mouseY - thumbHeight / 2)
                             * static_cast<float>(range)
                             / static_cast<float>(thumbRange)));
        model_.setViewY(viewY);
        repaint();
    }
}
