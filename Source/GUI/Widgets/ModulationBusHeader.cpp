#include "ModulationBusHeader.h"

#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace tss
{
    ModulationBusHeader::ModulationBusHeader(tss::Skin& skin, int width, int height, ColourVariant variant)
        : skin_(&skin)
        , busNumberText_(PluginDisplayNames::MatrixModulationSection::Header::kBusNumber)
        , busSourceText_(PluginDisplayNames::MatrixModulationSection::Header::kSource)
        , busAmountText_(PluginDisplayNames::MatrixModulationSection::Header::kAmount)
        , busDestinationText_(PluginDisplayNames::MatrixModulationSection::Header::kDestination)
        , colourVariant_(variant)
        , cachedFont_(juce::FontOptions())
    {
        setOpaque(false);
        setSize(width, height);
        cachedTextColour_ = skin_->getModuleHeaderTextColour();
        cachedLineColour_ = getLineColour();
        cachedFont_ = skin_->getBaseFontBold().withHeight(kModuleHeaderFontHeight);
    }

    void ModulationBusHeader::setSkin(tss::Skin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void ModulationBusHeader::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        if (!cacheValid_)
            regenerateCache();

        if (cachedImage_.isValid())
        {
            g.drawImage(cachedImage_, getLocalBounds().toFloat(),
                       juce::RectanglePlacement::stretchToFit);
        }
    }

    void ModulationBusHeader::resized()
    {
        invalidateCache();
    }

    void ModulationBusHeader::regenerateCache()
    {
        const auto width = getWidth();
        const auto height = getHeight();

        if (width <= 0 || height <= 0)
            return;

        const float pixelScale = getPixelScale();
        const int imageWidth = juce::roundToInt(width * pixelScale);
        const int imageHeight = juce::roundToInt(height * pixelScale);

        cachedImage_ = juce::Image(juce::Image::ARGB, imageWidth, imageHeight, true);
        juce::Graphics g(cachedImage_);
        g.addTransform(juce::AffineTransform::scale(pixelScale));

        auto bounds = juce::Rectangle<float>(0.0f, 0.0f, 
                                              static_cast<float>(width), 
                                              static_cast<float>(height));

        drawText(g, bounds);
        drawLine(g, bounds);

        cacheValid_ = true;
    }

    void ModulationBusHeader::invalidateCache()
    {
        cacheValid_ = false;
    }

    float ModulationBusHeader::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        return display != nullptr ? static_cast<float>(display->scale) : 1.0f;
    }


    void ModulationBusHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        auto textArea = bounds;
        textArea.setHeight(kTextAreaHeight_);
        textArea.removeFromLeft(kTextLeftPadding_);

        auto x = textArea.getX();
        auto y = textArea.getY();

        g.setColour(cachedTextColour_);
        g.setFont(cachedFont_);

        drawBusNumberText(g, x, y);
        x += kBusNumberTextWidth_;

        drawBusSourceText(g, x, y);
        x += kBusSourceTextWidth_;

        drawBusAmountText(g, x, y);
        x += kBusAmountTextWidth_;

        drawBusDestinationText(g, x, y);
    }

    void ModulationBusHeader::drawBusNumberText(juce::Graphics& g, float x, float y)
    {
        auto busNumberBounds = juce::Rectangle<float>(x, y, kBusNumberTextWidth_, kTextAreaHeight_);
        g.drawText(busNumberText_, busNumberBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusSourceText(juce::Graphics& g, float x, float y)
    {
        auto busSourceBounds = juce::Rectangle<float>(x, y, kBusSourceTextWidth_, kTextAreaHeight_);
        g.drawText(busSourceText_, busSourceBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusAmountText(juce::Graphics& g, float x, float y)
    {
        auto busAmountBounds = juce::Rectangle<float>(x, y, kBusAmountTextWidth_, kTextAreaHeight_);
        g.drawText(busAmountText_, busAmountBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusDestinationText(juce::Graphics& g, float x, float y)
    {
        auto busDestinationBounds = juce::Rectangle<float>(x, y, kBusDestinationTextWidth_, kTextAreaHeight_);
        g.drawText(busDestinationText_, busDestinationBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        auto lineThickness = kLineThickness_;
        auto lineAreaHeight = bounds.getHeight() - kTextAreaHeight_;
        auto verticalOffset = kTextAreaHeight_ + (lineAreaHeight - lineThickness) / 2.0f;
        
        auto lineBounds = bounds;
        lineBounds.setHeight(lineThickness);
        lineBounds.translate(0.0f, verticalOffset);

        g.setColour(cachedLineColour_);
        g.fillRect(lineBounds);
    }

    juce::Colour ModulationBusHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) 
            ? skin_->getModuleHeaderLineColourBlue() 
            : skin_->getModuleHeaderLineColourOrange();
    }
}

