#include "TrackGeneratorDisplay.h"


#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/ColourChart.h"

using tss::SkinColourId;

namespace tss
{
    TrackGeneratorDisplay::TrackGeneratorDisplay(tss::ISkin& skin, int width, int height)
        : skin_(&skin)
        , width_(width)
        , height_(height)
    {
        setOpaque(false);
        setSize(width_, height_);
        updateSkinCache();
        cachedPointValues_ = pointValues_;
    }

    void TrackGeneratorDisplay::setSkin(tss::ISkin& skin)
    {
        skin_ = &skin;
        updateSkinCache();
        invalidateCache();
    }

    void TrackGeneratorDisplay::paint(juce::Graphics& g)
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

        if (cachedImage_.isValid())
        {
            const auto destRect = contentBounds.toFloat();
            g.drawImage(cachedImage_, destRect, juce::RectanglePlacement::fillDestination);
        }
    }

    void TrackGeneratorDisplay::resized()
    {
        invalidateCache();
    }


    void TrackGeneratorDisplay::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto backgroundColour = skin_->getColour(SkinColourId::kTrackGeneratorDisplayBackground);
        g.setColour(backgroundColour);
        g.fillRect(bounds);
    }

    void TrackGeneratorDisplay::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto borderColour = skin_->getColour(SkinColourId::kTrackGeneratorDisplayBorder);
        g.setColour(borderColour);
        g.drawRect(bounds, static_cast<float>(kWidgetBorderThickness_));
    }

    void TrackGeneratorDisplay::drawTriangle(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto triangleColour = skin_->getColour(SkinColourId::kTrackGeneratorDisplayBorder);
        const auto triangleHeight = kWidgetTriangleBase_ * std::sqrt(3.0f) * 0.5f;
        const auto centreX = std::round(bounds.getCentreX());
        const auto baseY = bounds.getY();

        juce::Path triangle;
        triangle.addTriangle(std::round(centreX - kWidgetTriangleBase_ * 0.5f), baseY,
                             std::round(centreX + kWidgetTriangleBase_ * 0.5f), baseY,
                             centreX, std::round(baseY - triangleHeight));

        g.setColour(triangleColour);
        g.fillPath(triangle);
    }
    
    void TrackGeneratorDisplay::setOnValueChanged(ValueChangedCallback callback)
    {
        onValueChanged_ = std::move(callback);
    }
    
    void TrackGeneratorDisplay::setTrackPoint1(int value)
    {
        setTrackPoint1(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint1(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[0] != clampedValue)
        {
            pointValues_[0] = clampedValue;
            invalidateCache();
            
            if (notify && onValueChanged_)
                onValueChanged_(0, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint2(int value)
    {
        setTrackPoint2(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint2(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[1] != clampedValue)
        {
            pointValues_[1] = clampedValue;
            invalidateCache();
            
            if (notify && onValueChanged_)
                onValueChanged_(1, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint3(int value)
    {
        setTrackPoint3(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint3(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[2] != clampedValue)
        {
            pointValues_[2] = clampedValue;
            invalidateCache();
            
            if (notify && onValueChanged_)
                onValueChanged_(2, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint4(int value)
    {
        setTrackPoint4(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint4(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[3] != clampedValue)
        {
            pointValues_[3] = clampedValue;
            invalidateCache();
            
            if (notify && onValueChanged_)
                onValueChanged_(3, clampedValue);
        }
    }
    
    void TrackGeneratorDisplay::setTrackPoint5(int value)
    {
        setTrackPoint5(value, true);
    }
    
    void TrackGeneratorDisplay::setTrackPoint5(int value, bool notify)
    {
        const int clampedValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, value);
        
        if (pointValues_[4] != clampedValue)
        {
            pointValues_[4] = clampedValue;
            invalidateCache();
            
            if (notify && onValueChanged_)
                onValueChanged_(4, clampedValue);
        }
    }
    
    float TrackGeneratorDisplay::getPixelScale() const
    {
        if (auto* display = juce::Desktop::getInstance().getDisplays().getPrimaryDisplay())
            return static_cast<float>(display->scale);
        return 1.0f;
    }
    
    void TrackGeneratorDisplay::regenerateCache()
    {
        if (skin_ == nullptr)
            return;

        const auto localBounds = getLocalBounds();
        const auto contentRect = localBounds.withTrimmedTop(kWidgetPaddingTop_)
            .withTrimmedBottom(kWidgetPaddingBottom_);
        const int contentWidth = contentRect.getWidth();
        const int contentHeight = contentRect.getHeight();

        if (contentWidth <= 0 || contentHeight <= 0)
            return;

        bool valuesChanged = false;
        for (size_t i = 0; i < kCurvePointCount_; ++i)
        {
            if (cachedPointValues_[i] != pointValues_[i])
            {
                valuesChanged = true;
                break;
            }
        }

        if (!valuesChanged && cacheValid_)
            return;

        const float scale = getPixelScale();
        const int cacheWidth = juce::roundToInt(static_cast<float>(contentWidth) * scale);
        const int cacheHeight = juce::roundToInt(static_cast<float>(contentHeight) * scale);

        cachedImage_ = juce::Image(juce::Image::ARGB, cacheWidth, cacheHeight, true);

        juce::Graphics g(cachedImage_);
        g.addTransform(juce::AffineTransform::scale(scale));

        const auto innerBounds = juce::Rectangle<float>(0.0f, 0.0f,
                                                        static_cast<float>(contentWidth),
                                                        static_cast<float>(contentHeight));

        drawCurve(g, innerBounds);

        cachedPointValues_ = pointValues_;
        cacheValid_ = true;
    }
    
    void TrackGeneratorDisplay::invalidateCache()
    {
        cacheValid_ = false;
        repaint();
    }
    
    void TrackGeneratorDisplay::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;
        
        cachedCurveColour_ = juce::Colour(ColourChart::kGreen4);
    }
    
    void TrackGeneratorDisplay::drawCurve(juce::Graphics& g, const juce::Rectangle<float>& innerBounds)
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);

        if (centerBounds.getWidth() <= 0.0f || centerBounds.getHeight() <= 0.0f)
            return;

        g.setColour(cachedCurveColour_);

        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto p = calculatePointPosition(i, centerBounds);
            
            if (i < kCurvePointCount_ - 1)
            {
                const auto p2 = calculatePointPosition(i + 1, centerBounds);
                g.drawLine(p.x, p.y, p2.x, p2.y, kCurveLineThickness_);
            }
        }
        
        const auto hollowPointFillColour = juce::Colour(ColourChart::kGreen1);
        
        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto p = calculatePointPosition(i, centerBounds);
            
            g.setColour(hollowPointFillColour);
            g.fillEllipse(p.x - kCurvePointRadius_, p.y - kCurvePointRadius_,
                         kCurvePointRadius_ * 2.0f, kCurvePointRadius_ * 2.0f);
            
            g.setColour(cachedCurveColour_);
            g.drawEllipse(p.x - kCurvePointRadius_, p.y - kCurvePointRadius_,
                         kCurvePointRadius_ * 2.0f, kCurvePointRadius_ * 2.0f,
                         kCurveLineThickness_);
        }
    }
    
    juce::Rectangle<float> TrackGeneratorDisplay::getCurveCenterBounds(const juce::Rectangle<float>& innerBounds) const
    {
        const float totalPadding = kCurvePadding_ + static_cast<float>(kWidgetBorderThickness_);
        return innerBounds.reduced(totalPadding);
    }

    juce::Point<float> TrackGeneratorDisplay::calculatePointPosition(int pointIndex,
                                                                      const juce::Rectangle<float>& centerBounds) const
    {
        const float xPosition = computePointX(pointIndex, centerBounds);
        const float normalizedValue = static_cast<float>(pointValues_[static_cast<size_t>(pointIndex)]) / kPointMaxValue_;
        const float yPosition = centerBounds.getBottom() - (centerBounds.getHeight() * normalizedValue);

        return juce::Point<float>(xPosition, yPosition);
    }

    float TrackGeneratorDisplay::computePointX(int pointIndex, const juce::Rectangle<float>& centerBounds) const
    {
        if (pointIndex == 0)
            return centerBounds.getX();
        if (pointIndex == kCurvePointCount_ - 1)
            return centerBounds.getRight();
        return centerBounds.getX() + (centerBounds.getWidth() * pointIndex) / (kCurvePointCount_ - 1);
    }
    
    int TrackGeneratorDisplay::findPointAtPosition(const juce::Point<float>& position,
                                                    const juce::Rectangle<float>& innerBounds) const
    {
        const auto centerBounds = getCurveCenterBounds(innerBounds);

        for (int i = 0; i < kCurvePointCount_; ++i)
        {
            const auto pointPos = calculatePointPosition(i, centerBounds);
            const float distance = position.getDistanceFrom(pointPos);
            
            if (distance <= kPointHitZoneRadius_)
                return i;
        }
        
        return -1;
    }
    
    void TrackGeneratorDisplay::mouseDown(const juce::MouseEvent& e)
    {
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_))
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_));

        draggedPointIndex_ = findPointAtPosition(e.position, innerBounds);
    }
    
    void TrackGeneratorDisplay::mouseDrag(const juce::MouseEvent& e)
    {
        if (draggedPointIndex_ < 0)
            return;
        
        const auto bounds = getLocalBounds().toFloat();
        const auto innerBounds = bounds.withTrimmedTop(static_cast<float>(kWidgetPaddingTop_))
            .withTrimmedBottom(static_cast<float>(kWidgetPaddingBottom_));
        const auto centerBounds = getCurveCenterBounds(innerBounds);

        const float relativeY = e.position.y - centerBounds.getY();
        const float normalizedValue = 1.0f - (relativeY / centerBounds.getHeight());
        
        const int newValue = juce::jlimit(kPointMinValue_, kPointMaxValue_, 
                                          static_cast<int>(normalizedValue * kPointMaxValue_ + 0.5f));
        
        const auto index = static_cast<size_t>(draggedPointIndex_);
        if (pointValues_[index] != newValue)
        {
            pointValues_[index] = newValue;
            invalidateCache();
            
            if (onValueChanged_)
                onValueChanged_(draggedPointIndex_, newValue);
        }
    }
    
    void TrackGeneratorDisplay::mouseUp(const juce::MouseEvent&)
    {
        draggedPointIndex_ = -1;
    }
}
