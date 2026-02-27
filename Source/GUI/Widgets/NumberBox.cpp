#include "NumberBox.h"

#include <memory>

#include "GUI/Skins/Skin.h"

namespace tss
{
    NumberBox::NumberBox(tss::Skin& skin, int width, bool editable, int minValue, int maxValue)
        : skin_(&skin)
        , minValue_(minValue)
        , maxValue_(maxValue)
        , editable_(editable)
        , cachedFont_(juce::FontOptions())
    {
        setOpaque(true);
        setSize(width, kHeight_);
        updateSkinCache();
        updateTextWidthCache();
    }

    void NumberBox::setSkin(tss::Skin& skin)
    {
        skin_ = &skin;
        invalidateCache();
    }

    void NumberBox::setValue(int newValue)
    {
        const int clampedValue = juce::jlimit(minValue_, maxValue_, newValue);
        
        if (currentValue_ != clampedValue)
        {
            currentValue_ = clampedValue;
            updateTextWidthCache();
            invalidateCache();
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
            invalidateCache();
            repaint();
        }
    }

    void NumberBox::paint(juce::Graphics& g)
    {
        if (skin_ == nullptr)
            return;

        if (!cacheValid_)
            regenerateCache();

        if (cachedImage_.isValid())
        {
            g.drawImage(cachedImage_, getLocalBounds().toFloat(),
                       juce::RectanglePlacement::stretchToFit);
        }
    }

    void NumberBox::resized()
    {
        invalidateCache();
    }

    void NumberBox::regenerateCache()
    {
        const auto width = getWidth();
        const auto height = getHeight();

        if (width <= 0 || height <= 0)
            return;

        const float pixelScale = getPixelScale();
        const int imageWidth = juce::roundToInt(width * pixelScale);
        const int imageHeight = juce::roundToInt(height * pixelScale);

        // Create HiDPI image at physical resolution
        cachedImage_ = juce::Image(juce::Image::ARGB, imageWidth, imageHeight, true);
        juce::Graphics g(cachedImage_);
        
        // Scale graphics context to match physical resolution
        g.addTransform(juce::AffineTransform::scale(pixelScale));

        const auto bounds = juce::Rectangle<float>(0.0f, 0.0f, 
                                                    static_cast<float>(width), 
                                                    static_cast<float>(height));

        g.setColour(cachedBackgroundColour_);
        g.fillRect(bounds);

        g.setColour(cachedBorderColour_);
        g.drawRect(bounds, kBorderThickness_);

        g.setColour(cachedTextColour_);
        g.setFont(cachedFont_);
        g.drawText(cachedValueText_, bounds, juce::Justification::centred, false);

        if (showDot_)
        {
            const auto dotPosition = calculateDotPosition(bounds, cachedTextWidth_);

            g.setColour(cachedDotColour_);
            g.fillEllipse(dotPosition.x, dotPosition.y, kDotRadius_ * 2.0f, kDotRadius_ * 2.0f);
        }

        cacheValid_ = true;
    }

    void NumberBox::invalidateCache()
    {
        cacheValid_ = false;
    }

    void NumberBox::updateSkinCache()
    {
        if (skin_ == nullptr)
            return;

        cachedBackgroundColour_ = skin_->getButtonBackgroundColourOn();
        cachedBorderColour_ = getBorderColour();
        cachedTextColour_ = skin_->getNumberBoxTextColour();
        cachedDotColour_ = skin_->getNumberBoxDotColour();
        cachedFont_ = skin_->getBaseFont();
    }

    void NumberBox::updateTextWidthCache()
    {
        cachedValueText_ = juce::String(currentValue_);
        
        if (skin_ == nullptr)
        {
            cachedTextWidth_ = 0.0f;
            return;
        }

        juce::GlyphArrangement glyphArrangement;
        glyphArrangement.addLineOfText(cachedFont_, cachedValueText_, 0.0f, 0.0f);
        cachedTextWidth_ = glyphArrangement.getBoundingBox(0, -1, true).getWidth();
    }

    float NumberBox::getPixelScale() const
    {
        const auto* display = juce::Desktop::getInstance()
                                  .getDisplays()
                                  .getDisplayForRect(getScreenBounds());
        const float displayScale = display != nullptr ? static_cast<float>(display->scale) : 1.0f;
        return displayScale;
    }

    void NumberBox::mouseDoubleClick(const juce::MouseEvent&)
    {
        if (!editable_ || !isEnabled())
            return;

        showEditor();
    }

    juce::Colour NumberBox::getBorderColour() const
    {
        if (!isEnabled())
            return skin_->getButtonBorderColourOff();

        return skin_->getButtonBorderColourOn();
    }

    juce::Point<float> NumberBox::calculateDotPosition(const juce::Rectangle<float>& bounds, float textWidth) const
    {
        const auto textRight = bounds.getCentreX() + textWidth * 0.5f;
        const auto baselineY = bounds.getCentreY() + cachedFont_.getHeight() * 0.5f - cachedFont_.getDescent();

        return { std::round(textRight + kDotXOffset_), std::round(baselineY - kDotRadius_) };
    }

    void NumberBox::showEditor()
    {
        if (editor_ != nullptr)
            return;

        const auto baseFont = skin_->getBaseFont();
        const auto editorFont = skin_->getBaseFontBold()
                                    .withHeight(baseFont.getHeight() + kEditorFontSizeIncrease_);

        editor_ = std::make_unique<juce::TextEditor>();
        editor_->setBounds(getLocalBounds());
        editor_->setText(juce::String(currentValue_), false);
        editor_->setFont(editorFont);
        editor_->setJustification(juce::Justification::centred);
        
        editor_->setColour(juce::TextEditor::backgroundColourId, skin_->getNumberBoxEditorBackgroundColour());
        editor_->setColour(juce::TextEditor::textColourId, skin_->getNumberBoxEditorTextColour());
        editor_->setColour(juce::TextEditor::highlightColourId, skin_->getNumberBoxEditorSelectionBackgroundColour());
        editor_->setColour(juce::TextEditor::highlightedTextColourId, skin_->getNumberBoxEditorTextColour());
        editor_->setColour(juce::TextEditor::outlineColourId, juce::Colours::transparentBlack);
        editor_->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colours::transparentBlack);
        
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
