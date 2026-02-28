#include "PatchNameDisplay.h"


#include "GUI/Skins/ISkin.h"
#include "Shared/Definitions/PluginDisplayNames.h"

using tss::SkinColourId;

namespace tss
{
    PatchNameDisplay::PatchNameDisplay(tss::ISkin& skin, int width, int height)
        : skin_(&skin)
        , width_(width)
        , height_(height)
        , patchName_(PluginDisplayNames::PatchEditSection::PatchNameModule::StandaloneWidgets::kDefaultPatchName)
    {
        setOpaque(false);
        setSize(width_, height_);
    }

    void PatchNameDisplay::setSkin(tss::ISkin& skin)
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
        const auto backgroundColour = skin_->getColour(SkinColourId::kPatchNameDisplayBackground);
        g.setColour(backgroundColour);
        g.fillRect(bounds);
    }

    void PatchNameDisplay::drawBorder(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto borderColour = skin_->getColour(SkinColourId::kPatchNameDisplayBorder);
        g.setColour(borderColour);
        g.drawRect(bounds, static_cast<float>(kBorderThickness_));
    }

    void PatchNameDisplay::drawText(juce::Graphics& g, const juce::Rectangle<float>& bounds)
    {
        const auto textColour = skin_->getColour(SkinColourId::kPatchNameDisplayText);
        const auto font = skin_->getBaseFont().withHeight(kFontHeight_);

        g.setColour(textColour);
        g.setFont(font);
        g.drawText(patchName_, bounds, juce::Justification::centred, false);
    }
}
