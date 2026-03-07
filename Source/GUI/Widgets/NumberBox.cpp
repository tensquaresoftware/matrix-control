#include "NumberBox.h"

#include <memory>

#include "GUI/Skins/ColourChart.h"

namespace tss
{
    NumberBox::NumberBox(int width, bool editable, int minValue, int maxValue)
        : minValue_(minValue)
        , maxValue_(maxValue)
        , editable_(editable)
    {
        setOpaque(true);
        setSize(width, kHeight_);
        updateTextWidthCache();
    }

    void NumberBox::setLook(const NumberBoxLook& look)
    {
        look_ = look;
        updateTextWidthCache();
        repaint();
    }

    void NumberBox::setValue(int newValue)
    {
        const int clampedValue = juce::jlimit(minValue_, maxValue_, newValue);
        
        if (currentValue_ != clampedValue)
        {
            currentValue_ = clampedValue;
            updateTextWidthCache();
            repaint();
            
            if (onValueChanged_)
                onValueChanged_(clampedValue);
        }
    }
    
    void NumberBox::setOnValueChanged(ValueChangedCallback callback)
    {
        onValueChanged_ = std::move(callback);
    }

    void NumberBox::setShowDot(bool show)
    {
        if (showDot_ != show)
        {
            showDot_ = show;
            repaint();
        }
    }

    void NumberBox::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();

        g.setColour(look_.background);
        g.fillRect(bounds);

        g.setColour(getBorderColour());
        g.drawRect(bounds, static_cast<float>(kBorderThickness_));

        g.setColour(look_.text);
        g.setFont(look_.font);
        g.drawText(cachedValueText_, bounds, juce::Justification::centred, false);

        if (showDot_)
        {
            const auto dotPosition = calculateDotPosition(bounds, cachedTextWidth_);
            g.setColour(look_.dot);
            g.fillEllipse(dotPosition.x, dotPosition.y, kDotRadius_ * 2.0f, kDotRadius_ * 2.0f);
        }
    }

    void NumberBox::resized()
    {
        repaint();
    }

    void NumberBox::updateTextWidthCache()
    {
        cachedValueText_ = juce::String(currentValue_);

        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(look_.font, cachedValueText_, 0.0f, 0.0f);
        cachedTextWidth_ = glyphArrangement.getBoundingBox(0, -1, true).getWidth();
    }

    void NumberBox::mouseDoubleClick(const juce::MouseEvent&)
    {
        if (!editable_ || !isEnabled())
            return;

        showEditor();
    }

    juce::Colour NumberBox::getBorderColour() const
    {
        return isEnabled() ? look_.borderOn : look_.borderOff;
    }

    juce::Point<float> NumberBox::calculateDotPosition(const juce::Rectangle<float>& bounds, float textWidth) const
    {
        const float textRight = bounds.getCentreX() + textWidth * 0.5f;
        const float baselineY = bounds.getCentreY() + look_.font.getHeight() * 0.5f - look_.font.getDescent();

        return { textRight + kDotXOffset_, baselineY - kDotRadius_ };
    }

    void NumberBox::showEditor()
    {
        if (editor_ != nullptr)
            return;

        const auto editorFont = look_.font.withHeight(look_.font.getHeight() + kEditorFontSizeIncrease_);

        editor_ = std::make_unique<juce::TextEditor>();
        editor_->setBounds(getLocalBounds());
        editor_->setText(juce::String(currentValue_), false);
        editor_->setFont(editorFont);
        editor_->setJustification(juce::Justification::centred);

        editor_->setColour(juce::TextEditor::backgroundColourId, look_.editorBackground);
        editor_->setColour(juce::TextEditor::textColourId, look_.editorText);
        editor_->setColour(juce::TextEditor::highlightColourId, look_.editorSelectionBackground);
        editor_->setColour(juce::TextEditor::highlightedTextColourId, look_.editorText);
        editor_->setColour(juce::TextEditor::outlineColourId, juce::Colour(ColourChart::kTransparent));
        editor_->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(ColourChart::kTransparent));
        
        editor_->setBorder(juce::BorderSize<int>(0));
        editor_->setIndents(0, 0);
        editor_->setInputRestrictions(0, "0123456789");

        editor_->onReturnKey = [this] { handleEditorReturn(); };
        editor_->onEscapeKey = [this] { hideEditor(); };
        editor_->onFocusLost = [this] { hideEditor(); };

        addAndMakeVisible(*editor_);
        editor_->grabKeyboardFocus();
        editor_->selectAll();
        editor_->applyFontToAllText(editorFont);
    }

    void NumberBox::hideEditor()
    {
        if (editor_ == nullptr)
            return;

        removeChildComponent(editor_.get());
        editor_.reset();
        repaint();
    }

    void NumberBox::handleEditorReturn()
    {
        if (editor_ == nullptr)
            return;

        const auto text = editor_->getText();
        
        if (text.isEmpty())
        {
            hideEditor();
            return;
        }
        
        const int rawValue = text.getIntValue();
        const int clampedValue = juce::jlimit(minValue_, maxValue_, rawValue);

        setValue(clampedValue);
        hideEditor();
    }
}
