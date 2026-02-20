#include "PatchNameDisplay.h"

#include "GUI/Themes/Skin.h"
#include "Shared/PluginDisplayNames.h"

namespace tss
{
    PatchNameDisplay::PatchNameDisplay(tss::Skin& skin, int width, int height)
        : skin_(&skin)
        , width_(width)
        , height_(height)
        , patchName_(PluginDescriptors::StandaloneWidgetDisplayNames::kDefaultPatchName)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void PatchNameDisplay::setSkin(tss::Skin& skin)
    {
        skin_ = &skin;
    }

    void PatchNameDisplay::setPatchName(const juce::String& patchName)
    {
        patchName_ = patchName;
        repaint();
    }

    void PatchNameDisplay::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        const auto bounds = getLocalBounds().toFloat();
        const auto contentBounds = bounds;

        drawBackground(g, contentBounds);
        drawBorder(g, contentBounds);
        drawText(g, contentBounds);
    }


    void PatchNameDisplay::drawBackground(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto backgroundColour = skin_->getPatchNameDisplayBackgroundColour();
        g.setColour(backgroundColour);
        g.fillRect(bounds);
    }

    void PatchNameDisplay::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto borderColour = skin_->getPatchNameDisplayBorderColour();
        g.setColour(borderColour);
        g.drawRect(bounds, static_cast<float>(kBorderThickness_));
    }

    void PatchNameDisplay::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto textColour = skin_->getPatchNameDisplayTextColour();
        const auto font = skin_->getBaseFont().withHeight(kFontHeight_);

        g.setColour(textColour);
        g.setFont(font);
        g.drawText(patchName_, bounds, juce::Justification::centred, false);
    }
}
