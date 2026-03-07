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
    {
        setOpaque(false);
        setSize(width, height);
    }

    void ModulationBusHeader::setLook(const ModulationBusHeaderLook& look)
    {
        look_ = look;
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
        auto textArea = bounds;
        textArea.setHeight(kTextAreaHeight_);
        textArea.removeFromLeft(kTextLeftPadding_);

        auto x = textArea.getX();
        auto y = textArea.getY();

        g.setColour(look_.text);
        g.setFont(look_.font);

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
        auto busNumberBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusNumberTextWidth_), static_cast<float>(kTextAreaHeight_));
        g.drawText(busNumberText_, busNumberBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusSourceText(juce::Graphics& g, float x, float y)
    {
        auto busSourceBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusSourceTextWidth_), static_cast<float>(kTextAreaHeight_));
        g.drawText(busSourceText_, busSourceBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusAmountText(juce::Graphics& g, float x, float y)
    {
        auto busAmountBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusAmountTextWidth_), static_cast<float>(kTextAreaHeight_));
        g.drawText(busAmountText_, busAmountBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusDestinationText(juce::Graphics& g, float x, float y)
    {
        auto busDestinationBounds = juce::Rectangle<float>(x, y, static_cast<float>(kBusDestinationTextWidth_), static_cast<float>(kTextAreaHeight_));
        g.drawText(busDestinationText_, busDestinationBounds, juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto lineThickness = static_cast<float>(kLineThickness_);
        const auto lineAreaHeight = bounds.getHeight() - kTextAreaHeight_;
        const auto verticalOffset = kTextAreaHeight_ + (lineAreaHeight - lineThickness) / 2.0f;
        
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
