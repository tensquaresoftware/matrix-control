#include "PatchNameDisplay.h"

#include "Shared/Definitions/PluginDisplayNames.h"

namespace tss
{
    PatchNameDisplay::PatchNameDisplay(int width, int height)
        : width_(width)
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

    void PatchNameDisplay::setScalingFactor(float scalingFactor)
    {
        if (juce::approximatelyEqual(scalingFactor_, scalingFactor))
            return;
        
        scalingFactor_ = scalingFactor;
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
        g.drawRect(bounds, std::max(1.0f, static_cast<float>(kBorderThickness_) * scalingFactor_));
    }

    void PatchNameDisplay::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        g.setColour(look_.text);
        g.setFont(look_.font.withHeight(kFontHeight_ * scalingFactor_));
        g.drawText(patchName_, bounds, juce::Justification::centred, false);
    }
}
