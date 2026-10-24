#include "Tools/SkinEditor/SkinEditorLookAndFeel.h"

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
}
