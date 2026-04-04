#include "ModulationBusHeader.h"

#include "Shared/Definitions/PluginDescriptors.h"

namespace tss
{
    ModulationBusHeader::ModulationBusHeader(int width, int height, ColourVariant variant)
        : busNumberText_(PluginDisplayNames::MatrixModulationSection::Header::kBusNumber)
        , busSourceText_(PluginDisplayNames::MatrixModulationSection::Header::kSource)
        , busAmountText_(PluginDisplayNames::MatrixModulationSection::Header::kAmount)
        , busDestinationText_(PluginDisplayNames::MatrixModulationSection::Header::kDestination)
        , colourVariant_(variant)
        , height_(height)
    {
        setOpaque(false);
        setSize(width, height);
    }

    void ModulationBusHeader::setLook(const ModulationBusHeaderLook& look)
    {
        look_ = look;
        repaint();
    }

    void ModulationBusHeader::setDisplayScale(float displayScale)
    {
        if (juce::approximatelyEqual(displayScale_, displayScale))
            return;
        
        displayScale_ = displayScale;
        repaint();
    }

    void ModulationBusHeader::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        drawText(g, bounds);
        drawLine(g, bounds);
    }

    void ModulationBusHeader::resized()
    {
        repaint();
    }

    void ModulationBusHeader::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        const float textLeftPadding = static_cast<float>(kTextLeftPadding_) * displayScale_;
        
        auto textArea = bounds;
        textArea.setHeight(textAreaHeight);
        textArea.removeFromLeft(textLeftPadding);

        auto x = textArea.getX();
        auto y = textArea.getY();

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * displayScale_));

        drawBusNumberText(g, x, y);
        x += static_cast<float>(kBusNumberTextWidth_) * displayScale_;

        drawBusSourceText(g, x, y);
        x += static_cast<float>(kBusSourceTextWidth_) * displayScale_;

        drawBusAmountText(g, x, y);
        x += static_cast<float>(kBusAmountTextWidth_) * displayScale_;

        drawBusDestinationText(g, x, y);
    }

    void ModulationBusHeader::drawBusNumberText(juce::Graphics& g, float x, float y)
    {
        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        auto busNumberBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusNumberTextWidth_) * displayScale_, textAreaHeight);
        g.drawText(busNumberText_, busNumberBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusSourceText(juce::Graphics& g, float x, float y)
    {
        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        auto busSourceBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusSourceTextWidth_) * displayScale_, textAreaHeight);
        g.drawText(busSourceText_, busSourceBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusAmountText(juce::Graphics& g, float x, float y)
    {
        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        auto busAmountBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusAmountTextWidth_) * displayScale_, textAreaHeight);
        g.drawText(busAmountText_, busAmountBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusDestinationText(juce::Graphics& g, float x, float y)
    {
        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        auto busDestinationBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusDestinationTextWidth_) * displayScale_, textAreaHeight);
        g.drawText(busDestinationText_, busDestinationBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const float lineThickness = std::max(1.0f, static_cast<float>(kLineThickness_) * displayScale_);
        const float textAreaHeight = static_cast<float>(kTextAreaHeight_) * displayScale_;
        const auto lineAreaHeight = bounds.getHeight() - textAreaHeight;
        const auto verticalOffset = textAreaHeight + (lineAreaHeight - lineThickness) / 2.0f;
        
        auto lineBounds = bounds;
        lineBounds.setHeight(lineThickness);
        lineBounds.translate(0.0f, verticalOffset);

        g.setColour(getLineColour());
        g.fillRect(lineBounds);
    }

    juce::Colour ModulationBusHeader::getLineColour() const
    {
        return (colourVariant_ == ColourVariant::Blue) ? look_.lineBlue : look_.lineOrange;
    }
}
