#include "ModulationBusHeader.h"
#include "ModulationBusRowLayout.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Layout/ScaledLayout.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace TSS
{
    ModulationBusHeader::ModulationBusHeader(int width, int height, const ModulationBusHeaderDimensions& dimensions,
                                             const ModulationBusHeaderLook& look, ColourVariant variant)
        : dimensions_(dimensions)
        , look_(look)
        , busNumberText_(PluginDisplayNames::MatrixModulationSection::Header::kBusNumber)
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

    void ModulationBusHeader::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
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
        // Destination label width stays 68 (not cell combo 104); X origins match cell columns.
        const auto cols = computeModulationBusColumnStrip(
            uiScale_,
            dimensions_.busNumberTextWidth,
            dimensions_.busSourceTextWidth,
            dimensions_.busAmountTextWidth,
            dimensions_.busDestinationTextWidth,
            0,
            dimensions_.interControlGap);

        const float textAreaHeight = static_cast<float>(
            ScaledLayout::scaledInt(static_cast<float>(kTextAreaHeight), uiScale_));
        const float textLeftPadding = static_cast<float>(
            ScaledLayout::scaledInt(static_cast<float>(kTextLeftPadding), uiScale_));
        const float y = bounds.getY();
        const float x0 = bounds.getX() + textLeftPadding;

        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));

        drawBusNumberText(g, x0 + static_cast<float>(cols.busNumberX), y,
                          static_cast<float>(cols.busNumberW), textAreaHeight);
        drawBusSourceText(g, x0 + static_cast<float>(cols.sourceX), y,
                          static_cast<float>(cols.sourceW), textAreaHeight);
        drawBusAmountText(g, x0 + static_cast<float>(cols.amountX), y,
                          static_cast<float>(cols.amountW), textAreaHeight);
        drawBusDestinationText(g, x0 + static_cast<float>(cols.destinationX), y,
                               static_cast<float>(cols.destinationW), textAreaHeight);
    }

    void ModulationBusHeader::drawBusNumberText(juce::Graphics& g, float x, float y, float width, float height)
    {
        g.drawText(busNumberText_, juce::Rectangle<float>(x, y, width, height),
                   juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusSourceText(juce::Graphics& g, float x, float y, float width, float height)
    {
        g.drawText(busSourceText_, juce::Rectangle<float>(x, y, width, height),
                   juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusAmountText(juce::Graphics& g, float x, float y, float width, float height)
    {
        g.drawText(busAmountText_, juce::Rectangle<float>(x, y, width, height),
                   juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawBusDestinationText(juce::Graphics& g, float x, float y, float width, float height)
    {
        g.drawText(busDestinationText_, juce::Rectangle<float>(x, y, width, height),
                   juce::Justification::centredLeft, false);
    }

    void ModulationBusHeader::drawLine(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float lineThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(kLineThickness),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        const float textAreaHeight = static_cast<float>(kTextAreaHeight) * uiScale_;
        const auto lineAreaHeight = bounds.getHeight() - textAreaHeight;
        const auto verticalOffset = textAreaHeight + (lineAreaHeight - lineThickness) * 0.5f;
        
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
