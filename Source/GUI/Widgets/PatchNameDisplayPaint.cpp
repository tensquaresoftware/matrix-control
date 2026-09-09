#include "PatchNameDisplay.h"

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ColourChart.h"

namespace TSS
{
    void PatchNameDisplay::showDragOverlay(const juce::String& primaryText,
                                           const juce::String& secondaryText)
    {
        // Same overlay already running — do not reset blink phase (fileDragMove spam).
        if (dragOverlayActive_
            && dragOverlayPrimary_ == primaryText
            && dragOverlaySecondary_ == secondaryText)
        {
            return;
        }

        if (editing_)
        {
            if (nameRequiredArmed_)
                suspendEditForDragOverlay();
            else
                cancelEdit();
        }

        dragOverlayActive_ = true;
        dragOverlayPrimary_ = primaryText;
        dragOverlaySecondary_ = secondaryText;
        blinkSecondaryVisible_ = true;
        hoveredPrimary_ = false;
        startTimerHz(kDragSecondaryBlinkHz_);
        repaint();
    }

    void PatchNameDisplay::clearDragOverlay()
    {
        if (! dragOverlayActive_)
            return;

        stopTimer();
        dragOverlayActive_ = false;
        dragOverlayPrimary_.clear();
        dragOverlaySecondary_.clear();
        blinkSecondaryVisible_ = true;

        if (nameRequiredArmed_)
            enterNameRequiredEditSession();
        else
            repaint();
    }

    juce::Font PatchNameDisplay::scaledPrimaryFont() const
    {
        return look_.font.withHeight(look_.font.getHeight() * uiScale_);
    }

    juce::Font PatchNameDisplay::scaledSecondaryFont() const
    {
        return look_.secondaryFont.withHeight(look_.secondaryFont.getHeight() * uiScale_);
    }

    juce::Colour PatchNameDisplay::primaryTextColour() const
    {
        const float alpha = (editable_ && hoveredPrimary_ && ! editing_)
            ? kPrimaryHoverAlpha_
            : kPrimaryIdleAlpha_;
        return look_.text.withAlpha(alpha);
    }

    PatchNameDisplay::TextBlockLayout PatchNameDisplay::computeTextBlockLayout(
        juce::Rectangle<float> bounds) const
    {
        TextBlockLayout layout;
        const auto primaryFont = scaledPrimaryFont();
        const float primaryHeight = primaryFont.getHeight();
        const bool showSecondary = dragOverlayActive_
            ? dragOverlaySecondary_.isNotEmpty()
            : activeSecondaryText().isNotEmpty();

        if (! showSecondary)
        {
            layout.primaryRow = bounds.withSizeKeepingCentre(bounds.getWidth(), primaryHeight);
            return layout;
        }

        const auto secondaryFont = scaledSecondaryFont();
        const float secondaryHeight = secondaryFont.getHeight();
        const float gap = kDualLineGap_ * uiScale_;
        const float contentHeight = primaryHeight + gap + secondaryHeight;
        const float top = bounds.getY() + 0.5f * (bounds.getHeight() - contentHeight);

        layout.hasSecondary = true;
        layout.primaryRow = { bounds.getX(), top, bounds.getWidth(), primaryHeight };
        layout.secondaryRow = {
            bounds.getX(),
            top + primaryHeight + gap,
            bounds.getWidth(),
            secondaryHeight
        };
        return layout;
    }

    void PatchNameDisplay::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();
        drawBackground(g, bounds);
        drawBorder(g, bounds);

        const auto layout = computeTextBlockLayout(bounds);

        if (dragOverlayActive_)
        {
            g.setColour(look_.text);
            g.setFont(scaledPrimaryFont());
            g.drawText(dragOverlayPrimary_, layout.primaryRow, juce::Justification::centred, false);

            if (layout.hasSecondary && blinkSecondaryVisible_)
            {
                g.setColour(look_.secondaryText);
                g.setFont(scaledSecondaryFont());
                g.drawText(dragOverlaySecondary_, layout.secondaryRow, juce::Justification::centred, false);
            }

            return;
        }

        drawNameSlots(g, layout.primaryRow);

        if (layout.hasSecondary)
            drawSecondaryText(g, layout.secondaryRow);
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

    void PatchNameDisplay::drawNameSlots(juce::Graphics& g, const juce::Rectangle<float>& rowBounds)
    {
        const auto scaledFont = scaledPrimaryFont();
        g.setFont(scaledFont);

        if (! editing_)
        {
            g.setColour(primaryTextColour());
            g.drawText(patchName_, rowBounds, juce::Justification::centred, false);
            return;
        }

        drawEditingNameSlots(g, rowBounds, scaledFont);
    }

    void PatchNameDisplay::paintEditSlotCharacter(juce::Graphics& g,
                                                  const juce::Rectangle<float>& bounds,
                                                  const juce::String& character,
                                                  bool caretHere)
    {
        const bool isSpace = character == " ";

        if (caretHere)
        {
            g.setColour(look_.text);
            g.fillRect(bounds);
            g.setColour(juce::Colour(ColourChart::kBlack));
            if (! isSpace)
                g.drawText(character, bounds, juce::Justification::centred, false);
            return;
        }

        if (! isSpace)
        {
            g.setColour(look_.text.withAlpha(kPrimaryIdleAlpha_));
            g.drawText(character, bounds, juce::Justification::centred, false);
        }
    }

    void PatchNameDisplay::drawEditingNameSlots(juce::Graphics& g,
                                                const juce::Rectangle<float>& rowBounds,
                                                const juce::Font& scaledFont)
    {
        // Characters use natural pitch. Past-the-end caret exists only while length < 8.
        const float glyphWidth = juce::jmax(
            1.0f,
            juce::GlyphArrangement::getStringWidth(scaledFont, "M"));
        const float caretPadX = kCaretPadX_ * uiScale_;
        const float caretPadY = kCaretPadY_ * uiScale_;
        const float caretBottomTrim = kCaretBottomTrim_ * uiScale_;
        const float slotWidth = glyphWidth + 2.0f * caretPadX;
        const float slotGap = kSlotGap_ * uiScale_;
        const bool showEndCaretSlot = editBuffer_.length() < kNameLength_;
        const int drawnSlots = juce::jmax(1, editBuffer_.length() + (showEndCaretSlot ? 1 : 0));
        const float blockWidth = slotWidth * static_cast<float>(drawnSlots)
            + slotGap * static_cast<float>(juce::jmax(0, drawnSlots - 1));
        const float blockX = rowBounds.getCentreX() - 0.5f * blockWidth;
        const float fullCaretHeight = scaledFont.getHeight() + 2.0f * caretPadY;
        const float caretHeight = juce::jmax(1.0f, fullCaretHeight - caretBottomTrim);
        const float caretY = rowBounds.getCentreY() - 0.5f * fullCaretHeight;

        auto slotBoundsAt = [&](int index) -> juce::Rectangle<float>
        {
            const float slotX = blockX + static_cast<float>(index) * (slotWidth + slotGap);
            return { slotX, caretY, slotWidth, caretHeight };
        };

        for (int i = 0; i < editBuffer_.length(); ++i)
        {
            paintEditSlotCharacter(g,
                                   slotBoundsAt(i),
                                   editBuffer_.substring(i, i + 1),
                                   (i == caretIndex_) && caretOn_);
        }

        // Empty insertion caret after the last character — only while there is room to type.
        if (showEndCaretSlot && caretIndex_ == editBuffer_.length() && caretOn_)
        {
            g.setColour(look_.text);
            g.fillRect(slotBoundsAt(editBuffer_.length()));
        }
    }

    void PatchNameDisplay::drawSecondaryText(juce::Graphics& g, const juce::Rectangle<float>& rowBounds)
    {
        if (showsNameRequiredSecondary() && ! blinkSecondaryVisible_)
            return;

        g.setColour(look_.secondaryText);
        g.setFont(scaledSecondaryFont());
        g.drawText(activeSecondaryText(), rowBounds, juce::Justification::centred, false);
    }
}
