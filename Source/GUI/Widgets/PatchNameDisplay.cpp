#include "PatchNameDisplay.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    PatchNameDisplay::PatchNameDisplay(int width, int height, const PatchNameDisplayLook& look)
        : look_(look)
        , width_(width)
        , height_(height)
        , patchName_(PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void PatchNameDisplay::setLook(const PatchNameDisplayLook& look)
    {
        look_ = look;
        repaint();
    }

    void PatchNameDisplay::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;
        
        uiScale_ = uiScale;
        repaint();
    }

    void PatchNameDisplay::setPatchName(const juce::String& patchName)
    {
        patchName_ = patchName;
        repaint();
    }

    void PatchNameDisplay::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        drawBackground(g, bounds);
        drawBorder(g, bounds);
        drawText(g, bounds);
    }

    void PatchNameDisplay::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.background);
        g.fillRect(bounds);
    }

    void PatchNameDisplay::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.border);
        const float systemDisplayScale = ScaledDrawing::systemDisplayScaleForComponent(*this);
        const float borderThickness = ScaledDrawing::snappedStrokeThicknessFromDesign(
            static_cast<float>(kBorderThickness_),
            uiScale_,
            systemDisplayScale,
            ScaledDrawing::StrokeSnapPolicy::kRound);
        g.drawRect(bounds, borderThickness);
    }

    void PatchNameDisplay::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(look_.font.getHeight() * uiScale_));
        g.drawText(patchName_, bounds, juce::Justification::centred, false);
    }
}
