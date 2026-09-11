#include "NumberBox.h"

#include <cmath>
#include <cstdlib>
#include <memory>

#include "GUI/Layout/ScaledDrawing.h"
#include "GUI/Skins/ColourChart.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace TSS
{
    namespace
    {
        // Digits-only field: no mouse/keyboard text selection during edit.
        class NumberBoxEditField final : public juce::TextEditor
        {
        public:
            std::function<void()> onCaretOrTextChanged;

            NumberBoxEditField()
            {
                setSelectAllWhenFocused(false);
                setPopupMenuEnabled(false);
                setCaretVisible(false);
            }

            void mouseDown(const juce::MouseEvent& e) override
            {
                juce::TextEditor::mouseDown(e);
                clearSelectionKeepCaret();
            }

            void mouseDrag(const juce::MouseEvent&) override
            {
                clearSelectionKeepCaret();
            }

            void mouseDoubleClick(const juce::MouseEvent&) override {}

            bool keyPressed(const juce::KeyPress& key) override
            {
                if (key.getModifiers().isShiftDown()
                    && (key.isKeyCode(juce::KeyPress::leftKey)
                        || key.isKeyCode(juce::KeyPress::rightKey)
                        || key.isKeyCode(juce::KeyPress::homeKey)
                        || key.isKeyCode(juce::KeyPress::endKey)
                        || key.isKeyCode(juce::KeyPress::upKey)
                        || key.isKeyCode(juce::KeyPress::downKey)))
                {
                    const juce::KeyPress withoutShift(
                        key.getKeyCode(),
                        key.getModifiers().withoutFlags(juce::ModifierKeys::shiftModifier),
                        key.getTextCharacter());
                    const bool handled = juce::TextEditor::keyPressed(withoutShift);
                    clearSelectionKeepCaret();
                    return handled;
                }

                const bool handled = juce::TextEditor::keyPressed(key);
                clearSelectionKeepCaret();
                return handled;
            }

        private:
            void clearSelectionKeepCaret()
            {
                const int caret = getCaretPosition();
                setHighlightedRegion({});
                setCaretPosition(caret);

                if (onCaretOrTextChanged)
                    onCaretOrTextChanged();
            }
        };
    }

    NumberBox::NumberBox(const NumberBoxLook& look, const Config& config)
        : look_(look)
        , height_(config.height)
        , minValue_(config.minValue)
        , maxValue_(config.maxValue)
        , editable_(config.editable)
    {
        setOpaque(true);
        setSize(config.width, height_);
        updateValueText();
    }

    void NumberBox::setLook(const NumberBoxLook& look)
    {
        look_ = look;
        applyEditorAppearance();
        repaint();
    }

    void NumberBox::setUiScale(float uiScale)
    {
        if (juce::approximatelyEqual(uiScale_, uiScale))
            return;

        uiScale_ = uiScale;
        applyEditorAppearance();
        layoutEditor();
        repaint();
    }

    void NumberBox::setRange(int minValue, int maxValue)
    {
        minValue_ = minValue;
        maxValue_ = maxValue;
        setValue(currentValue_);
        updateValueText();
        repaint();
    }

    void NumberBox::setValue(int newValue)
    {
        const int clampedValue = juce::jlimit(minValue_, maxValue_, newValue);

        if (currentValue_ != clampedValue)
        {
            currentValue_ = clampedValue;
            updateValueText();
            repaint();

            if (onValueChanged_)
                onValueChanged_(clampedValue);
        }
    }

    void NumberBox::setOnValueChanged(ValueChangedCallback callback)
    {
        onValueChanged_ = std::move(callback);
    }

    void NumberBox::setDisplayState(DisplayState state)
    {
        if (displayState_ == state)
            return;

        displayState_ = state;
        updateValueText();
        repaint();
    }

    void NumberBox::setFocusHighlight(bool focused)
    {
        if (focusHighlight_ == focused)
            return;

        focusHighlight_ = focused;
        repaint();
    }

    void NumberBox::paint(juce::Graphics& g)
    {
        const auto bounds = getLocalBounds().toFloat();

        // Parent paints the red edit plate so the grey border sits flush (no black ring).
        const auto fill = editor_ != nullptr
                              ? look_.textFocus
                              : (isEnabled() ? look_.background : look_.backgroundDisabled);
        g.setColour(fill);
        g.fillRect(bounds);

        g.setColour(getBorderColour());
        g.drawRect(bounds, borderStrokeThickness());

        if (editor_ != nullptr || cachedValueText_.isEmpty())
            return;

        g.setColour(getTextColour());
        g.setFont(scaledDisplayFont());
        g.drawText(cachedValueText_, bounds, juce::Justification::centred, false);
    }

    void NumberBox::paintOverChildren(juce::Graphics& g)
    {
        if (editor_ == nullptr || ! editCaretOn_)
            return;

        const float thickness = borderStrokeThickness();
        // Live scaled bounds — not getDesignHeight() (that is the unscaled config height).
        const float height = static_cast<float>(getLocalBounds().getHeight());
        const float caretHeight = height - 4.0f * thickness;

        if (caretHeight <= 0.0f)
            return;

        g.setColour(look_.editorText);
        g.fillRect(editCaretX(thickness), 2.0f * thickness, thickness, caretHeight);
    }

    void NumberBox::resized()
    {
        layoutEditor();
        repaint();
    }

    void NumberBox::enablementChanged()
    {
        if (! isEnabled() && editor_ != nullptr)
            hideEditor();

        repaint();
    }

    int NumberBox::digitCount() const
    {
        int digits = 1;
        for (int remaining = std::abs(maxValue_) / 10; remaining > 0; remaining /= 10)
            ++digits;

        return digits;
    }

    void NumberBox::updateValueText()
    {
        switch (displayState_)
        {
            case DisplayState::kUnavailable:
                cachedValueText_.clear();
                return;

            case DisplayState::kUndefined:
                cachedValueText_ = juce::String::repeatedString(
                    juce::String::charToString(PluginDisplayNames::Widgets::NumberBox::kUndefinedValueDigit),
                    digitCount());
                return;

            case DisplayState::kValue:
            default:
                cachedValueText_ = juce::String(currentValue_).paddedLeft('0', digitCount());
                return;
        }
    }

    void NumberBox::mouseDoubleClick(const juce::MouseEvent&)
    {
        if (! editable_ || ! isEnabled()
            || displayState_ == DisplayState::kUnavailable
            || displayState_ == DisplayState::kUndefined)
            return;

        showEditor();
    }

    juce::Colour NumberBox::getBorderColour() const
    {
        return isEnabled() ? look_.borderOn : look_.borderOff;
    }

    juce::Colour NumberBox::getTextColour() const
    {
        // Red marks navigation focus only; an undefined coordinate stays neutral.
        const bool useFocusColour = focusHighlight_ && displayState_ == DisplayState::kValue;
        return useFocusColour ? look_.textFocus : look_.text;
    }

    juce::Font NumberBox::scaledDisplayFont() const
    {
        return look_.font.withHeight(look_.font.getHeight() * uiScale_);
    }

    juce::Font NumberBox::scaledEditFont() const
    {
        return scaledDisplayFont().boldened();
    }

    float NumberBox::borderStrokeThickness() const
    {
        return ScaledDrawing::snappedControlBorderThickness(*this, uiScale_);
    }

    int NumberBox::editorBorderInset() const
    {
        // Ceil so a fractional stroke (e.g. UI Scale 125%) is never covered by the editor.
        return juce::jmax(1, static_cast<int>(std::ceil(static_cast<double>(borderStrokeThickness()))));
    }

    void NumberBox::layoutEditor()
    {
        if (editor_ == nullptr)
            return;

        editor_->setBounds(getLocalBounds().reduced(editorBorderInset()));
    }

    void NumberBox::applyEditorAppearance()
    {
        if (editor_ == nullptr)
            return;

        const auto editorFont = scaledEditFont();

        editor_->setFont(editorFont);
        editor_->applyFontToAllText(editorFont);
        editor_->setColour(juce::TextEditor::backgroundColourId, juce::Colour(ColourChart::kTransparent));
        editor_->setColour(juce::TextEditor::textColourId, look_.editorText);
        editor_->setColour(juce::TextEditor::highlightColourId, look_.editorSelectionBackground);
        editor_->setColour(juce::TextEditor::highlightedTextColourId, look_.editorText);
        editor_->setColour(juce::TextEditor::outlineColourId, juce::Colour(ColourChart::kTransparent));
        editor_->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(ColourChart::kTransparent));
    }

    void NumberBox::restartEditCaretBlink()
    {
        editCaretOn_ = true;
        startTimer(kCaretBlinkIntervalMs_);
        repaint();
    }

    void NumberBox::notifyEditCaretChanged()
    {
        restartEditCaretBlink();
    }

    float NumberBox::editCaretX(float thickness) const
    {
        if (editor_ == nullptr)
            return thickness;

        // Follow TextEditor insertion X (layout-accurate); centre our width-T bar on it.
        // Keep a full T of red void from the grey border on both sides (same token as vertical).
        const auto caretInEditor = editor_->getCaretRectangle().toFloat();
        const float insertionCentreX = static_cast<float>(editor_->getX()) + caretInEditor.getCentreX();
        const float minX = thickness;
        const float maxX = static_cast<float>(getLocalBounds().getWidth()) - 2.0f * thickness;
        return juce::jlimit(minX, maxX, insertionCentreX - 0.5f * thickness);
    }

    void NumberBox::timerCallback()
    {
        if (editor_ == nullptr)
        {
            stopTimer();
            return;
        }

        editCaretOn_ = ! editCaretOn_;
        repaint();
    }

    void NumberBox::showEditor()
    {
        if (editor_ != nullptr)
            return;

        auto field = std::make_unique<NumberBoxEditField>();
        field->onCaretOrTextChanged = [this] { notifyEditCaretChanged(); };
        editor_ = std::move(field);
        layoutEditor();
        // Empty field: user retypes the full value; Escape / focus-lost keeps currentValue_.
        editor_->setText({}, false);
        editor_->setJustification(juce::Justification::centred);
        applyEditorAppearance();

        editor_->setBorder(juce::BorderSize<int>(0));
        editor_->setIndents(0, 0);
        editor_->setInputRestrictions(digitCount(), "0123456789");

        editor_->onReturnKey = [this] { handleEditorReturn(); };
        editor_->onEscapeKey = [this] { hideEditor(); };
        editor_->onFocusLost = [this] { hideEditor(); };
        editor_->onTextChange = [this] { notifyEditCaretChanged(); };

        addAndMakeVisible(*editor_);
        editor_->grabKeyboardFocus();
        restartEditCaretBlink();
    }

    void NumberBox::hideEditor()
    {
        if (editor_ == nullptr)
            return;

        stopTimer();
        editCaretOn_ = true;
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
