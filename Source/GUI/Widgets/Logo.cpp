#include "Logo.h"

#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    Logo::Logo()
        : logoText_(PluginDisplayNames::kPluginName)
    {
        setOpaque(false);
        setInterceptsMouseClicks(true, false);
    }

    void Logo::setSkin(ISkin& skin)
    {
        skin_ = &skin;
        repaint();
    }

    void Logo::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        repaint();
    }

    void Logo::setHighlighted(bool highlighted)
    {
        if (isHighlighted_ == highlighted)
            return;

        isHighlighted_ = highlighted;
        repaint();
    }

    juce::Font Logo::getScaledFont() const
    {
        if (skin_ == nullptr)
            return juce::Font(juce::FontOptions().withHeight(Design::Panels::Header::kLogoFontHeight * uiScale_));

        return skin_->getBrandFontBold().withHeight(skin_->getBrandFontBold().getHeight() * uiScale_);
    }

    int Logo::getPreferredWidth() const
    {
        const auto font = getScaledFont();
        juce::GlyphArrangement glyphs;
        glyphs.addLineOfText(font, logoText_, 0.0f, 0.0f);
        return juce::roundToInt(glyphs.getBoundingBox(0, -1, true).getWidth())
            + juce::roundToInt(static_cast<float>(Design::Panels::Header::kLogoExtraWidth) * uiScale_);
    }

    void Logo::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr || logoText_.isEmpty())
            return;

        g.setColour(isHighlighted_
                        ? juce::Colour(ColourChart::kWhite)
                        : skin_->getColour(SkinColourId::kLabelText));
        g.setFont(getScaledFont());
        g.drawText(logoText_, getLocalBounds(), juce::Justification::centredLeft, false);
    }

    void Logo::mouseUp(const juce::MouseEvent& e)
    {
        if (e.getNumberOfClicks() > 1)
            return;

        startTimer(200);
    }

    void Logo::mouseDoubleClick(const juce::MouseEvent&)
    {
        stopTimer();

        if (onUiScaleReset)
            onUiScaleReset();
    }

    void Logo::timerCallback()
    {
        stopTimer();

        if (onPopupRequested)
            onPopupRequested();
    }

    void Logo::mouseEnter(const juce::MouseEvent&)
    {
        setHighlighted(true);
        setMouseCursor(juce::MouseCursor::PointingHandCursor);
    }

    void Logo::mouseExit(const juce::MouseEvent&)
    {
        setHighlighted(false);
        setMouseCursor(juce::MouseCursor::NormalCursor);
    }
}
