#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace SkinEditorChrome
{
    /** Component IDs for Palette action buttons that paint Path icons instead of text. */
    inline constexpr const char* kPaletteIconNewId = "paletteIconNew";
    inline constexpr const char* kPaletteIconDuplicateId = "paletteIconDuplicate";
    inline constexpr const char* kPaletteIconDeleteId = "paletteIconDelete";

    inline bool isPaletteIconButton(const juce::Button& button) noexcept
    {
        const auto id = button.getComponentID();
        return id == kPaletteIconNewId
               || id == kPaletteIconDuplicateId
               || id == kPaletteIconDeleteId;
    }

    inline void drawPlusIcon(juce::Graphics& g, juce::Rectangle<float> area, float stroke)
    {
        const float cx = area.getCentreX();
        const float cy = area.getCentreY();
        const float arm = juce::jmin(area.getWidth(), area.getHeight()) * 0.38f;
        const juce::PathStrokeType strokeType(stroke, juce::PathStrokeType::curved,
                                              juce::PathStrokeType::rounded);
        juce::Path h;
        h.startNewSubPath(cx - arm, cy);
        h.lineTo(cx + arm, cy);
        juce::Path v;
        v.startNewSubPath(cx, cy - arm);
        v.lineTo(cx, cy + arm);
        g.strokePath(h, strokeType);
        g.strokePath(v, strokeType);
    }

    inline void drawDuplicateIcon(juce::Graphics& g, juce::Rectangle<float> area, float stroke)
    {
        const float s = juce::jmin(area.getWidth(), area.getHeight());
        const float w = s * 0.52f;
        const float h = s * 0.62f;
        const float r = 1.6f;
        const float ox = s * 0.14f;
        const float oy = s * 0.14f;
        const float x0 = area.getCentreX() - w * 0.5f - ox * 0.35f;
        const float y0 = area.getCentreY() - h * 0.5f + oy * 0.35f;

        g.drawRoundedRectangle({ x0 + ox, y0 - oy, w, h }, r, stroke);
        g.drawRoundedRectangle({ x0, y0, w, h }, r, stroke);
    }

    inline void drawTrashIcon(juce::Graphics& g, juce::Rectangle<float> area, float stroke)
    {
        const float s = juce::jmin(area.getWidth(), area.getHeight());
        const float cx = area.getCentreX();
        const float cy = area.getCentreY();
        const float bodyW = s * 0.46f;
        const float bodyH = s * 0.42f;
        const float lidW = s * 0.58f;
        const float left = cx - bodyW * 0.5f;
        const float top = cy - bodyH * 0.35f;
        const float bottom = top + bodyH;
        const float lidY = top - s * 0.12f;
        const float handleW = s * 0.18f;
        const float handleH = s * 0.08f;

        g.drawLine(cx - lidW * 0.5f, lidY, cx + lidW * 0.5f, lidY, stroke);
        g.drawLine(cx - handleW * 0.5f, lidY - handleH, cx + handleW * 0.5f, lidY - handleH, stroke);
        g.drawLine(cx - handleW * 0.5f, lidY - handleH, cx - handleW * 0.5f, lidY, stroke);
        g.drawLine(cx + handleW * 0.5f, lidY - handleH, cx + handleW * 0.5f, lidY, stroke);

        juce::Path body;
        body.startNewSubPath(left, top);
        body.lineTo(left + bodyW * 0.12f, bottom);
        body.lineTo(left + bodyW * 0.88f, bottom);
        body.lineTo(left + bodyW, top);
        g.strokePath(body, juce::PathStrokeType(stroke, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));
    }

    inline void drawPaletteActionIcon(juce::Graphics& g, juce::TextButton& button)
    {
        auto colour = button.findColour(button.getToggleState() ? juce::TextButton::textColourOnId
                                                                : juce::TextButton::textColourOffId);
        if (! button.isEnabled())
            colour = colour.withMultipliedAlpha(0.4f);

        constexpr float kStroke = 1.4f;
        auto iconArea = button.getLocalBounds().toFloat().reduced(5.0f);
        g.setColour(colour);

        const auto id = button.getComponentID();
        if (id == kPaletteIconNewId)
            drawPlusIcon(g, iconArea, kStroke);
        else if (id == kPaletteIconDuplicateId)
            drawDuplicateIcon(g, iconArea, kStroke);
        else if (id == kPaletteIconDeleteId)
            drawTrashIcon(g, iconArea, kStroke);
    }
}
