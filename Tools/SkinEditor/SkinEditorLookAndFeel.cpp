#include "Tools/SkinEditor/SkinEditorLookAndFeel.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"

namespace SkinEditorChrome
{
    LookAndFeel::LookAndFeel()
    {
        setColourScheme(ColourScheme {
            surfaceApp(),          // windowBackground
            surfacePanel(),        // widgetBackground
            controlFill(),         // menuBackground
            controlOutline(),      // outline
            textPrimary(),         // defaultText
            controlFillHover(),    // defaultFill
            textPrimary(),         // highlightedText
            accent(),              // highlightedFill
            textPrimary()          // menuText
        });

        setColour(juce::TextButton::buttonColourId, controlFill());
        setColour(juce::TextButton::buttonOnColourId, controlFillDown());
        setColour(juce::TextButton::textColourOffId, textPrimary());
        setColour(juce::TextButton::textColourOnId, textPrimary());
        setColour(juce::ComboBox::backgroundColourId, controlFill());
        setColour(juce::ComboBox::outlineColourId, controlOutline());
        setColour(juce::ComboBox::arrowColourId, textMuted());
        setColour(juce::ComboBox::textColourId, textPrimary());
        setColour(juce::ComboBox::buttonColourId, controlFill());
        setColour(juce::PopupMenu::backgroundColourId, surfacePanel());
        setColour(juce::PopupMenu::textColourId, textPrimary());
        setColour(juce::PopupMenu::highlightedBackgroundColourId, controlFillHover());
        setColour(juce::PopupMenu::highlightedTextColourId, textPrimary());
        setColour(juce::TextEditor::backgroundColourId, surfaceApp());
        setColour(juce::TextEditor::textColourId, textPrimary());
        setColour(juce::TextEditor::outlineColourId, controlOutline());
        setColour(juce::TextEditor::focusedOutlineColourId, textMuted());
        setColour(juce::ListBox::backgroundColourId, surfacePanel());
        setColour(juce::ScrollBar::thumbColourId, controlOutline());
        setColour(juce::TooltipWindow::backgroundColourId, surfaceApp());
        setColour(juce::TooltipWindow::textColourId, textMuted());
        setColour(juce::TooltipWindow::outlineColourId, controlOutline());
    }

    void LookAndFeel::drawScrollbar(juce::Graphics& g,
                                    juce::ScrollBar& scrollbar,
                                    int x,
                                    int y,
                                    int width,
                                    int height,
                                    bool isScrollbarVertical,
                                    int thumbStartPosition,
                                    int thumbSize,
                                    bool isMouseOver,
                                    bool)
    {
        juce::Rectangle<int> thumbBounds;
        if (isScrollbarVertical)
        {
            const int thumbW = juce::jmin(kScrollThumbThickness, width);
            thumbBounds = { x + (width - thumbW) / 2, thumbStartPosition, thumbW, thumbSize };
        }
        else
        {
            const int thumbH = juce::jmin(kScrollThumbThickness, height);
            thumbBounds = { thumbStartPosition, y + (height - thumbH) / 2, thumbSize, thumbH };
        }

        auto c = scrollbar.findColour(juce::ScrollBar::ColourIds::thumbColourId);
        g.setColour(isMouseOver ? c.brighter(0.25f) : c);
        g.fillRoundedRectangle(thumbBounds.toFloat(), 4.0f);
    }

    juce::Font LookAndFeel::getTextButtonFont(juce::TextButton&, int)
    {
        return fontRegular();
    }

    juce::Font LookAndFeel::getComboBoxFont(juce::ComboBox&)
    {
        return fontRegular();
    }

    juce::Font LookAndFeel::getLabelFont(juce::Label& label)
    {
        // Honour Label::setFont — titles use Orbitron / Inter Medium via setFont.
        return label.getFont();
    }

    juce::Font LookAndFeel::getPopupMenuFont()
    {
        return fontRegular();
    }

    void LookAndFeel::drawButtonBackground(juce::Graphics& g,
                                          juce::Button& button,
                                          const juce::Colour& backgroundColour,
                                          bool shouldDrawButtonAsHighlighted,
                                          bool shouldDrawButtonAsDown)
    {
        auto bounds = button.getLocalBounds().toFloat().reduced(0.5f);
        auto base = backgroundColour;
        if (shouldDrawButtonAsDown)
            base = controlFillDown();
        else if (shouldDrawButtonAsHighlighted)
            base = controlFillHover();

        g.setColour(base);
        g.fillRoundedRectangle(bounds, kControlCorner);
        g.setColour(controlOutline());
        g.drawRoundedRectangle(bounds, kControlCorner, 1.0f);
    }

    void LookAndFeel::drawToggleButton(juce::Graphics& g,
                                       juce::ToggleButton& button,
                                       bool shouldDrawButtonAsHighlighted,
                                       bool shouldDrawButtonAsDown)
    {
        const auto font = fontRegular();
        const float boxSide = (float) juce::jmin(button.getHeight() - 2, 16);
        auto box = juce::Rectangle<float>(0.5f,
                                          ((float) button.getHeight() - boxSide) * 0.5f,
                                          boxSide,
                                          boxSide);

        auto fill = controlFill();
        if (! button.isEnabled())
            fill = controlFill().withMultipliedAlpha(0.55f);
        else if (shouldDrawButtonAsDown)
            fill = controlFillDown();
        else if (shouldDrawButtonAsHighlighted)
            fill = controlFillHover();

        g.setColour(fill);
        g.fillRoundedRectangle(box, kControlCorner);
        g.setColour(controlOutline());
        g.drawRoundedRectangle(box, kControlCorner, 1.0f);

        if (button.getToggleState())
        {
            juce::Path tick;
            tick.startNewSubPath(box.getX() + boxSide * 0.22f, box.getCentreY());
            tick.lineTo(box.getX() + boxSide * 0.42f, box.getY() + boxSide * 0.72f);
            tick.lineTo(box.getX() + boxSide * 0.78f, box.getY() + boxSide * 0.28f);
            g.setColour(button.isEnabled() ? textPrimary() : textMuted());
            g.strokePath(tick, juce::PathStrokeType(1.4f, juce::PathStrokeType::curved,
                                                    juce::PathStrokeType::rounded));
        }

        auto textColour = button.findColour(juce::ToggleButton::textColourId);
        if (! button.isEnabled())
            textColour = textColour.withMultipliedAlpha(0.5f);
        g.setColour(textColour);
        g.setFont(font);
        const int textX = juce::roundToInt(box.getRight()) + 6;
        g.drawText(button.getButtonText(),
                   juce::Rectangle<int>(textX, 0, juce::jmax(0, button.getWidth() - textX),
                                        button.getHeight()),
                   juce::Justification::centredLeft, false);
    }

    void LookAndFeel::drawComboBox(juce::Graphics& g,
                                   int width,
                                   int height,
                                   bool,
                                   int,
                                   int,
                                   int,
                                   int,
                                   juce::ComboBox& box)
    {
        auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height).reduced(0.5f);
        g.setColour(box.findColour(juce::ComboBox::backgroundColourId));
        g.fillRoundedRectangle(bounds, kControlCorner);
        g.setColour(box.findColour(juce::ComboBox::outlineColourId));
        g.drawRoundedRectangle(bounds, kControlCorner, 1.0f);

        const float arrowX = (float) width - 16.0f;
        const float arrowY = (float) height * 0.5f;
        juce::Path path;
        path.startNewSubPath(arrowX, arrowY - 2.0f);
        path.lineTo(arrowX + 6.0f, arrowY - 2.0f);
        path.lineTo(arrowX + 3.0f, arrowY + 2.5f);
        path.closeSubPath();
        g.setColour(box.findColour(juce::ComboBox::arrowColourId));
        g.fillPath(path);
    }

    void LookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& ed)
    {
        if (dynamic_cast<juce::AlertWindow*>(ed.getParentComponent()) != nullptr)
        {
            LookAndFeel_V4::fillTextEditorBackground(g, width, height, ed);
            return;
        }

        g.setColour(ed.findColour(juce::TextEditor::backgroundColourId));
        g.fillRoundedRectangle(0.5f, 0.5f, (float) width - 1.0f, (float) height - 1.0f, kControlCorner);
    }

    void LookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed)
    {
        if (dynamic_cast<juce::AlertWindow*>(ed.getParentComponent()) != nullptr)
            return;

        if (! ed.isEnabled())
            return;

        const bool focused = ed.hasKeyboardFocus(true) && ! ed.isReadOnly();
        g.setColour(ed.findColour(focused ? juce::TextEditor::focusedOutlineColourId
                                          : juce::TextEditor::outlineColourId));
        g.drawRoundedRectangle(0.5f, 0.5f, (float) width - 1.0f, (float) height - 1.0f,
                               kControlCorner, focused ? 1.5f : 1.0f);
    }

    namespace
    {
        constexpr int kTooltipPadX = 12;
        constexpr int kTooltipPadY = 6;
    }

    juce::Rectangle<int> LookAndFeel::getTooltipBounds(const juce::String& tipText,
                                                       juce::Point<int> screenPos,
                                                       juce::Rectangle<int> parentArea)
    {
        juce::ignoreUnused(screenPos);

        const auto font = fontRegular();
        const int maxW = juce::jmax(1, parentArea.getWidth() - 2 * kWindowEdgeGutter);
        const int textW = measureTextWidth(font, tipText);
        const int w = juce::jmin(textW + 2 * kTooltipPadX, maxW);
        const int h = juce::roundToInt(font.getHeight()) + 2 * kTooltipPadY;

        // Fixed Y: tooltip bottom sits mid-gutter between body panels and status bar,
        // then nudged +8 so the tip clears the panel silhouette optically.
        const int x = parentArea.getCentreX() - w / 2;
        const int statusTop = parentArea.getBottom() - kStatusBarHeight;
        const int y = statusTop - (kGutter / 2) - h + 8;

        return juce::Rectangle<int>(x, y, w, h).constrainedWithin(parentArea);
    }

    void LookAndFeel::drawTooltip(juce::Graphics& g, const juce::String& text, int width, int height)
    {
        auto bounds = juce::Rectangle<float>(0.0f, 0.0f, (float) width, (float) height).reduced(0.5f);
        g.setColour(surfaceApp());
        g.fillRoundedRectangle(bounds, kControlCorner);
        g.setColour(controlOutline());
        g.drawRoundedRectangle(bounds, kControlCorner, 1.0f);

        const auto font = fontRegular();
        const int textMaxW = juce::jmax(0, width - 2 * kTooltipPadX);
        const auto shown = elidePathToWidth(font, text, textMaxW);
        g.setFont(font);
        g.setColour(textMuted());
        g.drawText(shown,
                   juce::Rectangle<int>(0, 0, width, height).reduced(kTooltipPadX, 0),
                   juce::Justification::centred,
                   false);
    }
}
