#include "Tools/SkinEditor/SkinEditorLookAndFeel.h"

#include "Tools/SkinEditor/SkinEditorChromeFonts.h"

namespace
{
    juce::Colour toggleBoxFillColour(const juce::ToggleButton& button,
                                     bool shouldDrawButtonAsHighlighted,
                                     bool shouldDrawButtonAsDown)
    {
        // Checked: same chrome as active sort badges (yellow fill).
        auto fill = SkinEditorChrome::controlFill();
        if (button.getToggleState())
            fill = SkinEditorChrome::attention();
        if (! button.isEnabled())
            return fill.withMultipliedAlpha(0.55f);
        if (button.getToggleState())
            return fill;
        if (shouldDrawButtonAsDown)
            return SkinEditorChrome::controlFillDown();
        if (shouldDrawButtonAsHighlighted)
            return SkinEditorChrome::controlFillHover();
        return fill;
    }

    void drawToggleTick(juce::Graphics& g,
                        juce::Rectangle<float> box,
                        float boxSide,
                        bool enabled)
    {
        juce::Path tick;
        tick.startNewSubPath(box.getX() + boxSide * 0.22f, box.getCentreY());
        tick.lineTo(box.getX() + boxSide * 0.42f, box.getY() + boxSide * 0.72f);
        tick.lineTo(box.getX() + boxSide * 0.78f, box.getY() + boxSide * 0.28f);
        auto tickColour = SkinEditorChrome::surfacePanel();
        if (! enabled)
            tickColour = tickColour.withMultipliedAlpha(0.5f);
        g.setColour(tickColour);
        g.strokePath(tick, juce::PathStrokeType(1.4f, juce::PathStrokeType::curved,
                                                juce::PathStrokeType::rounded));
    }
}

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
        // Match ComboBox field / dropdown chrome (controlFill, not panel surface).
        setColour(juce::PopupMenu::backgroundColourId, controlFill());
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

        // AlertWindow: dark chrome (native macOS alerts cannot be themed this way).
        setColour(juce::AlertWindow::backgroundColourId, surfacePanel());
        setColour(juce::AlertWindow::textColourId, textPrimary());
        setColour(juce::AlertWindow::outlineColourId, controlOutline());
        setUsingNativeAlertWindows(false);
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

    juce::Font LookAndFeel::getTextButtonFont(juce::TextButton& button, int)
    {
        // Alert buttons match the message font (family + size), not V4's larger default.
        for (auto* c = button.getParentComponent(); c != nullptr; c = c->getParentComponent())
            if (dynamic_cast<juce::AlertWindow*>(c) != nullptr)
                return getAlertWindowMessageFont();
        return fontRegular();
    }

    std::unique_ptr<juce::AlertWindow> LookAndFeel::createAlertWindowForOptions(
        const juce::MessageBoxOptions& options)
    {
        // Skip LookAndFeel_V4's +50px pad and +40px button nudge — that left a
        // large empty band above the buttons in Skin Editor dialogs.
        return juce::LookAndFeel_V2::createAlertWindowForOptions(options);
    }

    int LookAndFeel::getAlertWindowButtonHeight()
    {
        return 28;
    }

    juce::Font LookAndFeel::getAlertWindowTitleFont()
    {
        return fontBold();
    }

    juce::Font LookAndFeel::getAlertWindowMessageFont()
    {
        return fontRegular();
    }

    juce::Font LookAndFeel::getAlertWindowFont()
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

        g.setColour(toggleBoxFillColour(button, shouldDrawButtonAsHighlighted,
                                        shouldDrawButtonAsDown));
        g.fillRoundedRectangle(box, kControlCorner);
        if (! button.getToggleState())
        {
            g.setColour(controlOutline());
            g.drawRoundedRectangle(box, kControlCorner, 1.0f);
        }
        else
        {
            drawToggleTick(g, box, boxSide, button.isEnabled());
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

    void LookAndFeel::positionComboBoxText(juce::ComboBox& box, juce::Label& label)
    {
        // V4 reserves 30px for the arrow; our drawComboBox arrow sits near width-16.
        label.setBounds(1, 1,
                        juce::jmax(1, box.getWidth() - kComboArrowReserve),
                        box.getHeight() - 2);
        label.setFont(getComboBoxFont(box));
        label.setMinimumHorizontalScale(1.0f);
    }

    void LookAndFeel::fillTextEditorBackground(juce::Graphics& g, int width, int height, juce::TextEditor& ed)
    {
        const bool inAlert = dynamic_cast<juce::AlertWindow*>(ed.getParentComponent()) != nullptr;
        g.setColour(inAlert ? surfaceApp() : ed.findColour(juce::TextEditor::backgroundColourId));
        g.fillRoundedRectangle(0.5f, 0.5f, (float) width - 1.0f, (float) height - 1.0f, kControlCorner);
    }

    void LookAndFeel::drawTextEditorOutline(juce::Graphics& g, int width, int height, juce::TextEditor& ed)
    {
        if (! ed.isEnabled())
            return;

        const bool inAlert = dynamic_cast<juce::AlertWindow*>(ed.getParentComponent()) != nullptr;
        const bool focused = ed.hasKeyboardFocus(true) && ! ed.isReadOnly();
        if (inAlert)
        {
            g.setColour(focused ? textMuted() : controlOutline());
            g.drawRoundedRectangle(0.5f, 0.5f, (float) width - 1.0f, (float) height - 1.0f,
                                   kControlCorner, focused ? 1.5f : 1.0f);
            return;
        }

        g.setColour(ed.findColour(focused ? juce::TextEditor::focusedOutlineColourId
                                          : juce::TextEditor::outlineColourId));
        g.drawRoundedRectangle(0.5f, 0.5f, (float) width - 1.0f, (float) height - 1.0f,
                               kControlCorner, focused ? 1.5f : 1.0f);
    }

    namespace
    {
        int drawAlertIconBubble(juce::Graphics& g,
                                juce::MessageBoxIconType type,
                                juce::Rectangle<int> iconRect)
        {
            juce::Path icon;
            char character = '?';

            if (type == juce::MessageBoxIconType::WarningIcon)
            {
                character = '!';
                icon.addTriangle((float) iconRect.getX() + (float) iconRect.getWidth() * 0.5f,
                                 (float) iconRect.getY(),
                                 (float) iconRect.getRight(),
                                 (float) iconRect.getBottom(),
                                 (float) iconRect.getX(),
                                 (float) iconRect.getBottom());
                icon = icon.createPathWithRoundedCorners(5.0f);
            }
            else
            {
                character = (type == juce::MessageBoxIconType::InfoIcon) ? 'i' : '?';
                icon.addEllipse(iconRect.toFloat());
            }

            juce::GlyphArrangement ga;
            ga.addFittedText(juce::Font(juce::FontOptions((float) iconRect.getHeight() * 0.9f,
                                                          juce::Font::bold)),
                             juce::String::charToString((juce::juce_wchar) (uint8_t) character),
                             (float) iconRect.getX(),
                             (float) iconRect.getY(),
                             (float) iconRect.getWidth(),
                             (float) iconRect.getHeight(),
                             juce::Justification::centred,
                             false);
            ga.createPath(icon);
            icon.setUsingNonZeroWinding(false);

            // Decorative icon: lighter grey bubble on dark alert chrome (no system blue).
            g.setColour(juce::Colour(0x66999999));
            g.fillPath(icon);
            return 80;
        }
    }

    void LookAndFeel::drawAlertBox(juce::Graphics& g,
                                   juce::AlertWindow& alert,
                                   const juce::Rectangle<int>& textArea,
                                   juce::TextLayout& textLayout)
    {
        g.fillAll(alert.findColour(juce::AlertWindow::backgroundColourId));

        int iconSpaceUsed = 0;
        if (alert.getAlertType() != juce::MessageBoxIconType::NoIcon)
        {
            // Fixed bubble size (V2): do not scale with alert height or the glyph balloons.
            constexpr int kIconWidth = 80;
            const int iconSize = juce::jmin(kIconWidth + 50, textArea.getHeight() + 50);
            const juce::Rectangle<int> iconRect(iconSize / -10, iconSize / -10, iconSize, iconSize);
            iconSpaceUsed = drawAlertIconBubble(g, alert.getAlertType(), iconRect);
        }

        g.setColour(alert.findColour(juce::AlertWindow::textColourId));
        textLayout.draw(g,
                        juce::Rectangle<int>(textArea.getX() + iconSpaceUsed,
                                             textArea.getY(),
                                             textArea.getWidth() - iconSpaceUsed,
                                             textArea.getHeight())
                            .toFloat());

        g.setColour(alert.findColour(juce::AlertWindow::outlineColourId));
        g.drawRect(0, 0, alert.getWidth(), alert.getHeight());
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
