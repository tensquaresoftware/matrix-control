#include "Tools/SkinDesigner/SkinDesignerStepper.h"

#include "Tools/SkinDesigner/SkinDesignerChromeTokens.h"

void SkinDesignerStepper::setDecrementEnabled(bool enabled)
{
    if (decrementEnabled_ == enabled)
        return;
    decrementEnabled_ = enabled;
    if (! enabled && (hover_ == Half::decrement || pressed_ == Half::decrement))
    {
        hover_ = Half::none;
        pressed_ = Half::none;
    }
    repaint();
}

void SkinDesignerStepper::setIncrementEnabled(bool enabled)
{
    if (incrementEnabled_ == enabled)
        return;
    incrementEnabled_ = enabled;
    if (! enabled && (hover_ == Half::increment || pressed_ == Half::increment))
    {
        hover_ = Half::none;
        pressed_ = Half::none;
    }
    repaint();
}

void SkinDesignerStepper::setDecrementTooltip(const juce::String& tip)
{
    decrementTooltip_ = tip;
}

void SkinDesignerStepper::setIncrementTooltip(const juce::String& tip)
{
    incrementTooltip_ = tip;
}

SkinDesignerStepper::Half SkinDesignerStepper::halfAt(juce::Point<int> localPos) const noexcept
{
    if (! getLocalBounds().contains(localPos))
        return Half::none;
    return localPos.x < getWidth() / 2 ? Half::decrement : Half::increment;
}

void SkinDesignerStepper::setHover(Half half)
{
    if (hover_ == half)
        return;
    hover_ = half;
    refreshTooltip(half);
    repaint();
}

void SkinDesignerStepper::refreshTooltip(Half half)
{
    if (half == Half::decrement)
        setTooltip(decrementTooltip_);
    else if (half == Half::increment)
        setTooltip(incrementTooltip_);
    else
        setTooltip({});
}

juce::Colour SkinDesignerStepper::cellFill(Half half, bool enabled) const noexcept
{
    if (! enabled)
        return SkinDesignerChrome::controlFill();
    if (pressed_ == half)
        return SkinDesignerChrome::controlFillDown();
    if (hover_ == half)
        return SkinDesignerChrome::controlFillHover();
    return SkinDesignerChrome::controlFill();
}

void SkinDesignerStepper::fillCell(juce::Graphics& g,
                                   juce::Rectangle<float> bounds,
                                   Half half,
                                   bool enabled) const
{
    const float r = SkinDesignerChrome::kControlCorner;
    juce::Path path;
    if (half == Half::decrement)
        path.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                                 r, r, true, false, true, false);
    else
        path.addRoundedRectangle(bounds.getX(), bounds.getY(), bounds.getWidth(), bounds.getHeight(),
                                 r, r, false, true, false, true);

    g.setColour(cellFill(half, enabled));
    g.fillPath(path);
}

void SkinDesignerStepper::drawChevron(juce::Graphics& g,
                                      juce::Rectangle<float> cell,
                                      bool pointLeft,
                                      bool enabled) const
{
    constexpr float kW = 5.0f;
    constexpr float kH = 8.0f;
    constexpr float kCorner = 1.0f;

    const auto centre = cell.getCentre();
    juce::Path triangle;
    if (pointLeft)
    {
        triangle.startNewSubPath(centre.x - kW * 0.5f, centre.y);
        triangle.lineTo(centre.x + kW * 0.5f, centre.y - kH * 0.5f);
        triangle.lineTo(centre.x + kW * 0.5f, centre.y + kH * 0.5f);
    }
    else
    {
        triangle.startNewSubPath(centre.x + kW * 0.5f, centre.y);
        triangle.lineTo(centre.x - kW * 0.5f, centre.y - kH * 0.5f);
        triangle.lineTo(centre.x - kW * 0.5f, centre.y + kH * 0.5f);
    }
    triangle.closeSubPath();

    auto arrow = SkinDesignerChrome::textPrimary();
    if (! enabled)
        arrow = arrow.withMultipliedAlpha(0.5f);
    g.setColour(arrow);
    g.fillPath(triangle.createPathWithRoundedCorners(kCorner));
}

void SkinDesignerStepper::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat().reduced(0.5f);
    const float midX = bounds.getCentreX();
    auto left = bounds.withRight(midX);
    auto right = bounds.withLeft(midX);

    fillCell(g, left, Half::decrement, decrementEnabled_);
    fillCell(g, right, Half::increment, incrementEnabled_);

    g.setColour(SkinDesignerChrome::controlOutline());
    g.drawRoundedRectangle(bounds, SkinDesignerChrome::kControlCorner, 1.0f);

    // Single shared divider (not a second outer stroke).
    constexpr float kInset = 3.0f;
    g.drawLine(midX, bounds.getY() + kInset, midX, bounds.getBottom() - kInset, 1.0f);

    drawChevron(g, left, true, decrementEnabled_);
    drawChevron(g, right, false, incrementEnabled_);
}

void SkinDesignerStepper::mouseMove(const juce::MouseEvent& e)
{
    const auto half = halfAt(e.getPosition());
    const bool enabled = (half == Half::decrement && decrementEnabled_)
                         || (half == Half::increment && incrementEnabled_);
    setHover(enabled ? half : Half::none);
}

void SkinDesignerStepper::mouseEnter(const juce::MouseEvent& e)
{
    mouseMove(e);
}

void SkinDesignerStepper::mouseExit(const juce::MouseEvent&)
{
    pressed_ = Half::none;
    setHover(Half::none);
}

void SkinDesignerStepper::mouseDown(const juce::MouseEvent& e)
{
    const auto half = halfAt(e.getPosition());
    if (half == Half::decrement && decrementEnabled_)
        pressed_ = Half::decrement;
    else if (half == Half::increment && incrementEnabled_)
        pressed_ = Half::increment;
    else
        pressed_ = Half::none;
    repaint();
}

void SkinDesignerStepper::mouseDrag(const juce::MouseEvent& e)
{
    mouseMove(e);
    if (pressed_ != Half::none && halfAt(e.getPosition()) != pressed_)
    {
        pressed_ = Half::none;
        repaint();
    }
}

void SkinDesignerStepper::mouseUp(const juce::MouseEvent& e)
{
    const auto half = pressed_;
    pressed_ = Half::none;
    repaint();

    if (half == Half::none || halfAt(e.getPosition()) != half)
        return;

    if (half == Half::decrement && decrementEnabled_ && onDecrement != nullptr)
        onDecrement();
    else if (half == Half::increment && incrementEnabled_ && onIncrement != nullptr)
        onIncrement();
}
