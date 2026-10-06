#include "Logo.h"

#include "GUI/Looks/LookBuilders.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/ISkin.h"
#include "GUI/Skins/SkinValues.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    namespace
    {
        /** Per-UI-Scale optical Y nudge in whole component pixels (negative = up). */
        int logoOpticalNudgeYPx(float uiScale) noexcept
        {
            namespace Scale = PluginIDs::Settings::ScaleLevels;
            // 150%/175% half-pixel asks dropped (prefer crisp integer baselines).
            // 200%: -1 — prior -2.5 overshot (15px above / 17.5px below with that nudge).
            if (juce::approximatelyEqual(uiScale, Scale::kUiScales[Scale::k200]))
                return -1;
            return 0;
        }
    }

    Logo::Logo(ISkin& skin, int width, int height)
        : Label(width,
                height,
                brandLabelLookFromSkin(skin),
                PluginDisplayNames::kPluginName)
        , skin_(&skin)
    {
        setInterceptsMouseClicks(true, false);
    }

    void Logo::setSkin(ISkin& skin)
    {
        skin_ = &skin;
        setLook(brandLabelLookFromSkin(skin));
        applyTextColour();
    }

    void Logo::setHighlighted(bool highlighted)
    {
        if (isHighlighted_ == highlighted)
            return;

        isHighlighted_ = highlighted;
        applyTextColour();
    }

    void Logo::applyTextColour()
    {
        if (skin_ == nullptr)
            return;

        auto look = brandLabelLookFromSkin(*skin_);
        look.text = isHighlighted_
                        ? juce::Colour(ColourChart::kWhite)
                        : skin_->getColour(SkinColourId::kDarkPanelText);
        setLook(look);
    }

    void Logo::paint(juce::Graphics& g)
    {
        if (labelText_.isEmpty())
            return;

        // Centre glyph ink, then apply a per-scale integer Y nudge (crisp pixel baseline).
        const auto font = look_.font.withHeight(look_.font.getHeight() * uiScale_);
        juce::GlyphArrangement measure;
        measure.addLineOfText(font, labelText_, 0.0f, 0.0f);
        const auto ink = measure.getBoundingBox(0, measure.getNumGlyphs(), true);
        if (ink.isEmpty())
            return;

        const auto area = getLocalBounds().toFloat();
        const float baselineY = static_cast<float>(
            juce::roundToInt(area.getCentreY() - ink.getCentreY()) + logoOpticalNudgeYPx(uiScale_));

        juce::GlyphArrangement glyphs;
        glyphs.addLineOfText(font, labelText_, area.getX(), baselineY);
        g.setColour(look_.text);
        glyphs.draw(g);
    }

    void Logo::mouseUp(const juce::MouseEvent& e)
    {
        if (e.getNumberOfClicks() > 1)
            return;

        // Shift+Ctrl: Debug toggles the UI test harness; Release is a hard no-op
        // (must not fall through to Shift → Settings).
        if (e.mods.isShiftDown() && e.mods.isCtrlDown())
        {
            stopTimer();
#if JUCE_DEBUG
            if (onUiTestsToggleRequested)
                onUiTestsToggleRequested();
#endif
            return;
        }

        if (e.mods.isShiftDown())
        {
            stopTimer();

            if (onSettingsRequested)
                onSettingsRequested();
            return;
        }

        // Alt/Option intentionally does nothing (Settings is the only prefs door).
        if (e.mods.isAltDown())
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
