#include "EnvelopeDisplay.h"


#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/ColourChart.h"

using tss::SkinColourId;

namespace tss
{
    EnvelopeDisplay::EnvelopeDisplay(tss::ISkin& skin, int width, int height)
        : skin_(&skin)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
        updateSkinCache();
    }

    void EnvelopeDisplay::setSkin(tss::ISkin& skin)
    {
        skin_ = &skin;
        updateSkinCache();
        invalidateCache();
    }

    void EnvelopeDisplay::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        const auto bounds = getLocalBounds().toFloat();
        const auto contentBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_))
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_));

        drawBackground(g, contentBounds);
        drawBorder(g, contentBounds);
        drawTriangle(g, contentBounds);
        
        if (!cacheValid_)
            regenerateCache();
        
        const auto destRect = contentBounds.toFloat();
        g.drawImage(cachedImage_, destRect, juce::RectanglePlacement::fillDestination);
    }


    void EnvelopeDisplay::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto backgroundColour = skin_->getColour(SkinColourId::kEnvelopeDisplayBackground);
        g.setColour(backgroundColour);
        g.fillRect(bounds);
    }

    void EnvelopeDisplay::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto borderColour = skin_->getColour(SkinColourId::kEnvelopeDisplayBorder);
        g.setColour(borderColour);
        g.drawRect(bounds, static_cast<float>(kWidgetBorderThickness_));
    }

    void EnvelopeDisplay::drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto triangleColour = skin_->getColour(SkinColourId::kEnvelopeDisplayBorder);
        const auto triangleHeight = kWidgetTriangleBase_ * std::sqrt(3.0f) * 0.5f;
        const auto centreX = std::round(bounds.getCentreX());
        const auto baseY = bounds.getBottom();

        juce::Path triangle;
        triangle.addTriangle(std::round(centreX - kWidgetTriangleBase_ * 0.5f), baseY,
                             std::round(centreX + kWidgetTriangleBase_ * 0.5f), baseY,
                             centreX, std::round(baseY + triangleHeight));

        g.setColour(triangleColour);
        g.fillPath(triangle);
    }
    
    void EnvelopeDisplay::setOnValueChanged(ValueChangedCallback callback)
    {
        onValueChanged_ = std::move(callback);
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
            invalidateCache();
            
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
            invalidateCache();
            
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
            invalidateCache();
            
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
            invalidateCache();
            
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
            invalidateCache();
            
            if (notify && onValueChanged_)
                onValueChanged_(4, clampedValue);
        }
    }
    
    void EnvelopeDisplay::regenerateCache()
    {
        const auto scale = getPixelScale();
        const auto innerBounds = getLocalBounds()
            .withTrimmedTop(kWidgetPaddingTop_)
            .withTrimmedBottom(kWidgetPaddingBottom_)
            .toFloat();
        const int imageWidth = juce::roundToInt(innerBounds.getWidth() * scale);
        const int imageHeight = juce::roundToInt(innerBounds.getHeight() * scale);
        
        if (imageWidth <= 0 || imageHeight <= 0)
        {
            cacheValid_ = false;
            return;
        }
        
        cachedImage_ = juce::Image(juce::Image::ARGB, imageWidth, imageHeight, true);
        juce::Graphics g(cachedImage_);
        g.addTransform(juce::AffineTransform::scale(scale));
        
        const juce::Rectangle<float> drawBounds(0.0f, 0.0f, innerBounds.getWidth(), innerBounds.getHeight());
        drawEnvelope(g, drawBounds);
        
        cachedDelay_ = delay_;
        cachedAttack_ = attack_;
        cachedDecay_ = decay_;
        cachedSustain_ = sustain_;
        cachedRelease_ = release_;
        cachedCurveColour_ = skin_->getColour(SkinColourId::kEnvelopeDisplayEnvelope);
        cacheValid_ = true;
    }
    
    void EnvelopeDisplay::invalidateCache()
    {
        cacheValid_ = false;
        repaint();
    }
    
    void EnvelopeDisplay::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;
        
        cachedCurveColour_ = skin_->getColour(SkinColourId::kEnvelopeDisplayEnvelope);
    }
    
    float EnvelopeDisplay::getPixelScale() const
    {
        return static_cast<float>(juce::Desktop::getInstance().getDisplays().getPrimaryDisplay()->scale);
    }
    
    juce::Rectangle<float> EnvelopeDisplay::getCurveCenterBounds(const juce::Rectangle<float>& innerBounds) const
    {
        const float totalPadding = kCurvePadding_ + static_cast<float>(kWidgetBorderThickness_);
        return innerBounds.reduced(totalPadding);
    }
    
    void EnvelopeDisplay::drawEnvelope(juce::Graphics& g, const juce::Rectangle<float>& innerBounds)
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);
        
        if (centerBounds.getWidth() <= 0.0f || centerBounds.getHeight() <= 0.0f)
            return;
        
        const auto envelopePoints = calculateEnvelopePoints(centerBounds);
        
        g.setColour(cachedCurveColour_);
        
        for (int i = 0; i < kCurvePointCount_ - 1; ++i)
        {
            g.drawLine(envelopePoints.points[i].x, envelopePoints.points[i].y,
                      envelopePoints.points[i + 1].x, envelopePoints.points[i + 1].y,
                      kCurveLineThickness_);
        }
        
        const auto hollowPointFillColour = juce::Colour(ColourChart::kGreen1);
        
        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto circleX = envelopePoints.points[i].x - kCurvePointRadius_;
            const auto circleY = envelopePoints.points[i].y - kCurvePointRadius_;
            const auto circleDiameter = kCurvePointRadius_ * 2.0f;
            
            if (i == 1 || i == 2 || i == 3 || i == 5)
            {
                g.setColour(hollowPointFillColour);
                g.fillEllipse(circleX, circleY, circleDiameter, circleDiameter);
                
                g.setColour(cachedCurveColour_);
                g.drawEllipse(circleX, circleY, circleDiameter, circleDiameter, kCurveLineThickness_);
            }
            else
            {
                g.setColour(cachedCurveColour_);
                g.fillEllipse(circleX, circleY, circleDiameter, circleDiameter);
                g.drawEllipse(circleX, circleY, circleDiameter, circleDiameter, kCurveLineThickness_);
            }
        }
    }
    
    EnvelopeDisplay::EnvelopePoints EnvelopeDisplay::calculateEnvelopePoints(const juce::Rectangle<float>& centerBounds) const
    {
        EnvelopePoints result;
        
        const float maxWidth = centerBounds.getWidth();
        result.segmentMaxWidth = maxWidth * 0.20f;
        
        const float delayWidth = (static_cast<float>(delay_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        
        float attackWidth = (static_cast<float>(attack_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        if (attack_ == 0)
            attackWidth = kMinCurveSegmentWidth_;
        
        float decayWidth = (static_cast<float>(decay_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        if (decay_ == 0)
            decayWidth = kMinCurveSegmentWidth_;
        
        const float sustainWidth = result.segmentMaxWidth;
        
        float releaseWidth = (static_cast<float>(release_) / static_cast<float>(kPointMaxValue_)) * result.segmentMaxWidth;
        if (release_ == 0)
            releaseWidth = kMinCurveSegmentWidth_;
        
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
        
        const int editablePoints[] = {1, 2, 3, 5};
        
        for (int editableIndex : editablePoints)
        {
            const auto pointPos = envelopePoints.points[editableIndex];
            const float distance = position.getDistanceFrom(pointPos);
            
            if (distance <= kPointHitZoneRadius_)
                return editableIndex;
        }
        
        return -1;
    }
    
    bool EnvelopeDisplay::findSustainSegmentAtPosition(const juce::Point<float>& position, const juce::Rectangle<float>& innerBounds) const
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);
        const auto envelopePoints = calculateEnvelopePoints(centerBounds);
        
        const auto& point3 = envelopePoints.points[3];
        const auto& point4 = envelopePoints.points[4];
        
        if (position.x >= point3.x && position.x <= point4.x)
        {
            const float distanceFromSegment = std::abs(position.y - point3.y);
            if (distanceFromSegment <= kSustainSegmentHitZone_)
                return true;
        }
        
        return false;
    }
    
    void EnvelopeDisplay::mouseDown(const juce::MouseEvent& e)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_))
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_));
        
        draggedPointIndex_ = findPointAtPosition(e.position, innerBounds);
        
        if (draggedPointIndex_ < 0)
            draggingSustainSegment_ = findSustainSegmentAtPosition(e.position, innerBounds);
    }
    
    void EnvelopeDisplay::mouseDrag(const juce::MouseEvent& e)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_))
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_));
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
                invalidateCache();
                
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
                invalidateCache();
                
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
                invalidateCache();
                
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
                invalidateCache();
                
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
                invalidateCache();
                
                if (onValueChanged_)
                    onValueChanged_(4, newValue);
            }
        }
    }
    
    void EnvelopeDisplay::mouseUp(const juce::MouseEvent&)
    {
        draggedPointIndex_ = -1;
        draggingSustainSegment_ = false;
    }
}
