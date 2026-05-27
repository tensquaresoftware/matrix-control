#include "EnvelopeDisplay.h"

namespace tss
{
    EnvelopeDisplay::EnvelopeDisplay(int width, int height, const EnvelopeDisplayLook& look)
        : look_(look)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void EnvelopeDisplay::setLook(const EnvelopeDisplayLook& look)
    {
        look_ = look;
        repaint();
    }

    void EnvelopeDisplay::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
    }

    void EnvelopeDisplay::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto contentBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_) * uiScale_)
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_) * uiScale_);

        drawBackground(g, contentBounds);
        drawBorder(g, contentBounds);
        drawTriangle(g, contentBounds);
        drawEnvelope(g, contentBounds);
    }


    void EnvelopeDisplay::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.background);
        g.fillRect(bounds);
    }

    void EnvelopeDisplay::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.border);
        g.drawRect(bounds, std::max(1.0f, static_cast<float>(kWidgetBorderThickness_) * uiScale_));
    }

    void EnvelopeDisplay::drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto triangleColour = look_.border;
        const float triangleBase = kWidgetTriangleBase_ * uiScale_;
        const float triangleHeight = triangleBase * std::sqrt(3.0f) * 0.5f;
        const float centreX = bounds.getCentreX();
        const float baseY = bounds.getBottom();

        juce::Path triangle;
        triangle.addTriangle(centreX - triangleBase * 0.5f, baseY,
                             centreX + triangleBase * 0.5f, baseY,
                             centreX, baseY + triangleHeight);

        g.setColour(triangleColour);
        g.fillPath(triangle);
    }
    
    void EnvelopeDisplay::setOnValueChanged(ValueChangedCallback callback)
    {
        onValueChanged_ = std::move(callback);
    }

    void EnvelopeDisplay::setOnEditGestureBegin(EditGestureCallback callback)
    {
        onEditGestureBegin_ = std::move(callback);
    }

    void EnvelopeDisplay::setOnEditGestureEnd(std::function<void()> callback)
    {
        onEditGestureEnd_ = std::move(callback);
    }
    
    void EnvelopeDisplay::setDelay(int value)
    {
        setDelay(value, true);
    }
    
    void EnvelopeDisplay::setDelay(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (delay_ != clampedValue)
        {
            delay_ = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(0, clampedValue);
        }
    }
    
    void EnvelopeDisplay::setAttack(int value)
    {
        setAttack(value, true);
    }
    
    void EnvelopeDisplay::setAttack(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (attack_ != clampedValue)
        {
            attack_ = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(1, clampedValue);
        }
    }
    
    void EnvelopeDisplay::setDecay(int value)
    {
        setDecay(value, true);
    }
    
    void EnvelopeDisplay::setDecay(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (decay_ != clampedValue)
        {
            decay_ = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(2, clampedValue);
        }
    }
    
    void EnvelopeDisplay::setSustain(int value)
    {
        setSustain(value, true);
    }
    
    void EnvelopeDisplay::setSustain(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (sustain_ != clampedValue)
        {
            sustain_ = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(3, clampedValue);
        }
    }
    
    void EnvelopeDisplay::setRelease(int value)
    {
        setRelease(value, true);
    }
    
    void EnvelopeDisplay::setRelease(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (release_ != clampedValue)
        {
            release_ = clampedValue;
            repaint();

            if (notify && onValueChanged_)
                onValueChanged_(4, clampedValue);
        }
    }
    
    
    juce::Rectangle<float> EnvelopeDisplay::getCurveCenterBounds(const juce::Rectangle<float>& innerBounds) const
    {
        const float totalPadding = (kCurvePadding_ + static_cast<float>(kWidgetBorderThickness_)) * uiScale_;
        return innerBounds.reduced(totalPadding);
    }
    
    void EnvelopeDisplay::drawEnvelope(juce::Graphics& g, const juce::Rectangle<float>& innerBounds)
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);
        
        if (centerBounds.getWidth() <= 0.0f || centerBounds.getHeight() <= 0.0f)
            return;
        
        const auto envelopePoints = calculateEnvelopePoints(centerBounds);
        
        const float lineThickness = std::max(1.0f, kCurveLineThickness_ * uiScale_);
        
        g.setColour(look_.envelope);
        
        for (int i = 0; i < kCurvePointCount_ - 1; ++i)
        {
            g.drawLine(envelopePoints.points[i].x, envelopePoints.points[i].y,
                      envelopePoints.points[i + 1].x, envelopePoints.points[i + 1].y,
                      lineThickness);
        }
        
        const auto hollowPointFillColour = look_.envelope.withAlpha(0.4f);
        const float pointRadius = kCurvePointRadius_ * uiScale_;

        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto circleX = envelopePoints.points[i].x - pointRadius;
            const auto circleY = envelopePoints.points[i].y - pointRadius;
            const auto circleDiameter = pointRadius * 2.0f;
            
            if (i == 1 || i == 2 || i == 3 || i == 5)
            {
                g.setColour(hollowPointFillColour);
                g.fillEllipse(circleX, circleY, circleDiameter, circleDiameter);
                
                g.setColour(look_.envelope);
                g.drawEllipse(circleX, circleY, circleDiameter, circleDiameter, lineThickness);
            }
            else
            {
                g.setColour(look_.envelope);
                g.fillEllipse(circleX, circleY, circleDiameter, circleDiameter);
                g.drawEllipse(circleX, circleY, circleDiameter, circleDiameter, lineThickness);
            }
        }
    }
    
    EnvelopeDisplay::EnvelopePoints EnvelopeDisplay::calculateEnvelopePoints(const juce::Rectangle<float>& centerBounds) const
    {
        EnvelopePoints result;
        
        const float maxWidth = centerBounds.getWidth();
        result.segmentMaxWidth = maxWidth * 0.20f;
        
        const float delayWidth = (static_cast<float>(delay_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        
        const float minSegmentWidth = kMinCurveSegmentWidth_ * uiScale_;
        float attackWidth = (static_cast<float>(attack_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        if (attack_ == 0)
            attackWidth = minSegmentWidth;
        
        float decayWidth = (static_cast<float>(decay_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        if (decay_ == 0)
            decayWidth = minSegmentWidth;
        
        const float sustainWidth = result.segmentMaxWidth;
        
        float releaseWidth = (static_cast<float>(release_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        if (release_ == 0)
            releaseWidth = minSegmentWidth;
        
        const float sustainNormalized = static_cast<float>(sustain_) / static_cast<float>(kPointMaxValue_);
        const float ySustain = centerBounds.getBottom() - (centerBounds.getHeight() * sustainNormalized);
        
        const float x0 = centerBounds.getX();
        const float x1 = x0 + delayWidth;
        const float x2 = x1 + attackWidth;
        const float x3 = x2 + decayWidth;
        const float x4 = x3 + sustainWidth;
        const float x5 = x4 + releaseWidth;
        
        const float y0 = centerBounds.getBottom();
        const float y1 = y0;
        const float y2 = centerBounds.getY();
        const float y3 = ySustain;
        const float y4 = y3;
        const float y5 = centerBounds.getBottom();
        
        result.points[0] = {x0, y0};
        result.points[1] = {x1, y1};
        result.points[2] = {x2, y2};
        result.points[3] = {x3, y3};
        result.points[4] = {x4, y4};
        result.points[5] = {x5, y5};
        
        return result;
    }
    
    int EnvelopeDisplay::findPointAtPosition(const juce::Point<float>& position, const juce::Rectangle<float>& innerBounds) const
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);
        const auto envelopePoints = calculateEnvelopePoints(centerBounds);
        
        const float hitZoneRadius = kPointHitZoneRadius_ * uiScale_;
        const int editablePoints[] = {1, 2, 3, 5};
        
        for (int editableIndex : editablePoints)
        {
            const auto pointPos = envelopePoints.points[editableIndex];
            const float distance = position.getDistanceFrom(pointPos);
            
            if (distance <= hitZoneRadius)
                return editableIndex;
        }
        
        return -1;
    }
    
    bool EnvelopeDisplay::findSustainSegmentAtPosition(const juce::Point<float>& position, const juce::Rectangle<float>& innerBounds) const
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);
        const auto envelopePoints = calculateEnvelopePoints(centerBounds);
        
        const float sustainHitZone = kSustainSegmentHitZone_ * uiScale_;
        const auto& point3 = envelopePoints.points[3];
        const auto& point4 = envelopePoints.points[4];
        
        if (position.x >= point3.x && position.x <= point4.x)
        {
            const float distanceFromSegment = std::abs(position.y - point3.y);
            if (distanceFromSegment <= sustainHitZone)
                return true;
        }
        
        return false;
    }
    
    void EnvelopeDisplay::mouseDown(const juce::MouseEvent& e)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_) * uiScale_)
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_) * uiScale_);
        
        draggedPointIndex_ = findPointAtPosition(e.position, innerBounds);
        draggingSustainSegment_ = false;

        if (draggedPointIndex_ < 0)
            draggingSustainSegment_ = findSustainSegmentAtPosition(e.position, innerBounds);

        if (!onEditGestureBegin_)
            return;

        if (draggingSustainSegment_)
        {
            onEditGestureBegin_(3);
            editGestureActive_ = true;
            return;
        }

        if (draggedPointIndex_ < 0)
            return;

        const int paramIndexByPoint[] = { -1, 0, 1, 2, -1, 4 };
        const int paramIndex = paramIndexByPoint[draggedPointIndex_];

        if (paramIndex >= 0)
        {
            onEditGestureBegin_(paramIndex);
            editGestureActive_ = true;
        }
    }
    
    void EnvelopeDisplay::mouseDrag(const juce::MouseEvent& e)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_) * uiScale_)
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_) * uiScale_);
        const auto centerBounds = getCurveCenterBounds(innerBounds);
        
        if (draggingSustainSegment_)
        {
            const float relativeY = e.position.y - centerBounds.getY();
            const float normalizedValue = 1.0f - (relativeY / centerBounds.getHeight());
            
            const int newValue = juce::jlimit(kPointMinValue_, kPointMaxValue_,
                                             static_cast<int>(normalizedValue * kPointMaxValue_ + 0.5f));
            
            if (sustain_ != newValue)
            {
                sustain_ = newValue;
                repaint();
                
                if (onValueChanged_)
                    onValueChanged_(3, newValue);
            }
            return;
        }
        
        if (draggedPointIndex_ < 0)
            return;
        
        const auto envelopePoints = calculateEnvelopePoints(centerBounds);
        const float segmentMaxWidth = envelopePoints.segmentMaxWidth;
        
        if (draggedPointIndex_ == 1)
        {
            const float relativeX = e.position.x - centerBounds.getX();
            const float normalizedValue = relativeX / segmentMaxWidth;
            const int newValue = juce::jlimit(kPointMinValue_, kPointMaxValue_,
                                             static_cast<int>(normalizedValue * kPointMaxValue_ + 0.5f));
            
            if (delay_ != newValue)
            {
                delay_ = newValue;
                repaint();
                
                if (onValueChanged_)
                    onValueChanged_(0, newValue);
            }
        }
        else if (draggedPointIndex_ == 2)
        {
            const float relativeX = e.position.x - envelopePoints.points[1].x;
            const float normalizedValue = relativeX / segmentMaxWidth;
            const int newValue = juce::jlimit(kPointMinValue_, kPointMaxValue_,
                                             static_cast<int>(normalizedValue * kPointMaxValue_ + 0.5f));
            
            if (attack_ != newValue)
            {
                attack_ = newValue;
                repaint();
                
                if (onValueChanged_)
                    onValueChanged_(1, newValue);
            }
        }
        else if (draggedPointIndex_ == 3)
        {
            const float relativeX = e.position.x - envelopePoints.points[2].x;
            const float normalizedValue = relativeX / segmentMaxWidth;
            const int newValue = juce::jlimit(kPointMinValue_, kPointMaxValue_,
                                             static_cast<int>(normalizedValue * kPointMaxValue_ + 0.5f));
            
            if (decay_ != newValue)
            {
                decay_ = newValue;
                repaint();
                
                if (onValueChanged_)
                    onValueChanged_(2, newValue);
            }
        }
        else if (draggedPointIndex_ == 5)
        {
            const float relativeX = e.position.x - envelopePoints.points[4].x;
            const float normalizedValue = relativeX / segmentMaxWidth;
            const int newValue = juce::jlimit(kPointMinValue_, kPointMaxValue_,
                                             static_cast<int>(normalizedValue * kPointMaxValue_ + 0.5f));
            
            if (release_ != newValue)
            {
                release_ = newValue;
                repaint();
                
                if (onValueChanged_)
                    onValueChanged_(4, newValue);
            }
        }
    }
    
    void EnvelopeDisplay::mouseUp(const juce::MouseEvent&)
    {
        draggedPointIndex_ = -1;
        draggingSustainSegment_ = false;

        if (editGestureActive_ && onEditGestureEnd_)
            onEditGestureEnd_();

        editGestureActive_ = false;
    }
}
