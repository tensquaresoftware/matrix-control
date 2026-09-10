#include "Slider.h"

#include <memory>

#include "GUI/Skins/ColourChart.h"

namespace TSS
{
    namespace
    {
        // Digits-only field: no mouse/keyboard text selection during edit.
        class SliderEditField final : public juce::TextEditor
        {
        public:
            std::function<bool(const juce::MouseEvent&)> onCommandOrCtrlClick;

            SliderEditField()
            {
                setSelectAllWhenFocused(false);
                setPopupMenuEnabled(false);
                setCaretVisible(true);
            }

            void mouseDown(const juce::MouseEvent& e) override
            {
                if (e.mods.isCommandDown() && onCommandOrCtrlClick && onCommandOrCtrlClick(e))
                    return;

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
            }
        };
    }

    juce::Font Slider::scaledEditFont() const
    {
        return scaledValueFont().boldened();
    }

    juce::String Slider::editorAllowedCharacters() const
    {
        juce::String allowed("0123456789");

        if (getMinimum() < 0.0)
            allowed += "-";

        if (valueDecimalPlaces_ > 0)
            allowed += ".";

        return allowed;
    }

    void Slider::layoutEditor()
    {
        if (editor_ == nullptr)
            return;

        editor_->setBounds(getLocalBounds());
    }

    void Slider::applyEditorAppearance()
    {
        if (editor_ == nullptr)
            return;

        const auto editorFont = scaledEditFont();

        editor_->setFont(editorFont);
        editor_->applyFontToAllText(editorFont);
        editor_->setColour(juce::TextEditor::backgroundColourId, juce::Colour(ColourChart::kTransparent));
        editor_->setColour(juce::TextEditor::textColourId, look_.editorText);
        editor_->setColour(juce::TextEditor::highlightColourId, juce::Colour(ColourChart::kTransparent));
        editor_->setColour(juce::TextEditor::highlightedTextColourId, look_.editorText);
        editor_->setColour(juce::TextEditor::outlineColourId, juce::Colour(ColourChart::kTransparent));
        editor_->setColour(juce::TextEditor::focusedOutlineColourId, juce::Colour(ColourChart::kTransparent));
        editor_->setColour(juce::CaretComponent::caretColourId, look_.editorCaret);
    }

    void Slider::showValueEditor()
    {
        if (editor_ != nullptr)
            return;

        cancelActiveDragSession();

        auto field = std::make_unique<SliderEditField>();
        field->onCommandOrCtrlClick = [this](const juce::MouseEvent&)
        {
            hideValueEditor();
            resetToDefaultValue();
            return true;
        };

        editor_ = std::move(field);
        layoutEditor();
        editor_->setText({}, false);
        editor_->setJustification(juce::Justification::centred);
        applyEditorAppearance();

        editor_->setBorder(juce::BorderSize<int>(0));
        editor_->setIndents(0, 0);
        editor_->setInputRestrictions(kMaxEditCharacters_, editorAllowedCharacters());

        editor_->onReturnKey = [this] { handleEditorReturn(); };
        editor_->onEscapeKey = [this] { hideValueEditor(); };
        editor_->onFocusLost = [this] { hideValueEditor(); };

        addAndMakeVisible(*editor_);
        editor_->grabKeyboardFocus();
        repaint();
    }

    void Slider::hideValueEditor()
    {
        if (editor_ == nullptr)
            return;

        removeChildComponent(editor_.get());
        editor_.reset();
        repaint();
    }

    bool Slider::tryParseEditText(const juce::String& text, double& outValue)
    {
        const auto trimmed = text.trim();

        if (trimmed.isEmpty())
            return false;

        int index = 0;

        if (trimmed[0] == '-')
        {
            if (trimmed.length() == 1)
                return false;

            index = 1;
        }

        bool sawDigit = false;
        bool sawDot = false;

        for (; index < trimmed.length(); ++index)
        {
            const auto character = trimmed[index];

            if (character >= '0' && character <= '9')
            {
                sawDigit = true;
            }
            else if (character == '.' && ! sawDot)
            {
                sawDot = true;
            }
            else
            {
                return false;
            }
        }

        if (! sawDigit)
            return false;

        outValue = trimmed.getDoubleValue();
        return true;
    }

    void Slider::handleEditorReturn()
    {
        if (editor_ == nullptr)
            return;

        const auto text = editor_->getText();
        double parsedValue = 0.0;

        if (! tryParseEditText(text, parsedValue))
        {
            hideValueEditor();
            return;
        }

        const auto range = getNormalisableRange();
        auto newValue = juce::jlimit(range.start, range.end, parsedValue);
        newValue = range.snapToLegalValue(newValue);

        {
            const juce::Slider::ScopedDragNotification dragSession(*this);
            setValue(newValue, juce::sendNotificationSync);
        }

        hideValueEditor();
    }
}
