#include "Tools/SkinEditor/SkinEditorColourPicker.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

SkinEditorColourPicker::SkinEditorColourPicker()
{
    for (auto* label : { &hexLabel_, &rLabel_, &gLabel_, &bLabel_ })
    {
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        addAndMakeVisible(*label);
    }

    auto styleEditor = [this](juce::TextEditor& ed)
    {
        ed.setColour(juce::TextEditor::backgroundColourId, SkinEditorChrome::surfaceApp());
        ed.setColour(juce::TextEditor::textColourId, SkinEditorChrome::textPrimary());
        ed.setColour(juce::TextEditor::outlineColourId, SkinEditorChrome::textMuted());
        ed.addListener(this);
        addAndMakeVisible(ed);
    };
    hexEditor_.setInputRestrictions(6, "0123456789ABCDEFabcdef");
    styleEditor(hexEditor_);
    rEditor_.setInputRestrictions(3, "0123456789");
    gEditor_.setInputRestrictions(3, "0123456789");
    bEditor_.setInputRestrictions(3, "0123456789");
    styleEditor(rEditor_);
    styleEditor(gEditor_);
    styleEditor(bEditor_);

    syncEditorsFromColour(false);
}

void SkinEditorColourPicker::setColour(juce::Colour colour, juce::NotificationType notification)
{
    colour_ = colour.withAlpha(1.0f);
    colour_.getHSB(hue_, sat_, val_);
    syncEditorsFromColour(false);
    repaint();
    if (notification != juce::dontSendNotification)
        notifyChanged();
}

void SkinEditorColourPicker::setEnabledEditing(bool canEdit)
{
    editingEnabled_ = canEdit;
    hexEditor_.setEnabled(canEdit);
    rEditor_.setEnabled(canEdit);
    gEditor_.setEnabled(canEdit);
    bEditor_.setEnabled(canEdit);
    repaint();
}

void SkinEditorColourPicker::paint(juce::Graphics& g)
{
    g.fillAll(SkinEditorChrome::surfacePanel());
    g.setColour(SkinEditorChrome::textPrimary());
    g.drawText("Color", getLocalBounds().removeFromTop(22).reduced(SkinEditorChrome::kPanelPad, 0),
               juce::Justification::centredLeft, false);

    // SV square
    if (! svBounds_.isEmpty())
    {
        juce::ColourGradient satGrad(juce::Colours::white, (float) svBounds_.getX(), (float) svBounds_.getY(),
                                     juce::Colour::fromHSV(hue_, 1.0f, 1.0f, 1.0f),
                                     (float) svBounds_.getRight(), (float) svBounds_.getY(), false);
        g.setGradientFill(satGrad);
        g.fillRect(svBounds_);

        juce::ColourGradient valGrad(juce::Colours::transparentBlack, (float) svBounds_.getX(), (float) svBounds_.getY(),
                                     juce::Colours::black, (float) svBounds_.getX(), (float) svBounds_.getBottom(), false);
        g.setGradientFill(valGrad);
        g.fillRect(svBounds_);

        const int cx = svBounds_.getX() + juce::roundToInt(sat_ * (float) (svBounds_.getWidth() - 1));
        const int cy = svBounds_.getY() + juce::roundToInt((1.0f - val_) * (float) (svBounds_.getHeight() - 1));
        g.setColour(juce::Colours::white);
        g.drawEllipse((float) cx - 5.0f, (float) cy - 5.0f, 10.0f, 10.0f, 1.5f);
        g.setColour(juce::Colours::black);
        g.drawEllipse((float) cx - 4.0f, (float) cy - 4.0f, 8.0f, 8.0f, 1.0f);
    }

    // Hue strip
    if (! hueBounds_.isEmpty())
    {
        for (int y = 0; y < hueBounds_.getHeight(); ++y)
        {
            const float h = (float) y / (float) juce::jmax(1, hueBounds_.getHeight() - 1);
            g.setColour(juce::Colour::fromHSV(h, 1.0f, 1.0f, 1.0f));
            g.drawHorizontalLine(hueBounds_.getY() + y, (float) hueBounds_.getX(), (float) hueBounds_.getRight());
        }
        const int hy = hueBounds_.getY() + juce::roundToInt(hue_ * (float) (hueBounds_.getHeight() - 1));
        g.setColour(juce::Colours::white);
        g.drawRect(hueBounds_.getX() - 1, hy - 2, hueBounds_.getWidth() + 2, 5, 1);
    }
}

void SkinEditorColourPicker::resized()
{
    auto bounds = getLocalBounds().reduced(SkinEditorChrome::kPanelPad);
    bounds.removeFromTop(24);

    auto bottom = bounds.removeFromBottom(72);
    auto hexRow = bottom.removeFromTop(24);
    hexLabel_.setBounds(hexRow.removeFromLeft(16));
    hexEditor_.setBounds(hexRow.removeFromLeft(72));
    hexRow.removeFromLeft(8);
    auto placeRgb = [&hexRow](juce::Label& lab, juce::TextEditor& ed)
    {
        lab.setBounds(hexRow.removeFromLeft(14));
        ed.setBounds(hexRow.removeFromLeft(40));
        hexRow.removeFromLeft(4);
    };
    placeRgb(rLabel_, rEditor_);
    placeRgb(gLabel_, gEditor_);
    placeRgb(bLabel_, bEditor_);

    bounds.removeFromBottom(8);
    const int hueW = 18;
    hueBounds_ = bounds.removeFromRight(hueW);
    bounds.removeFromRight(8);
    svBounds_ = bounds;
}

void SkinEditorColourPicker::mouseDown(const juce::MouseEvent& e)
{
    mouseDrag(e);
}

void SkinEditorColourPicker::mouseDrag(const juce::MouseEvent& e)
{
    if (! editingEnabled_)
        return;

    if (svBounds_.contains(e.getPosition()))
        updateFromSvOrHue(e.position, false);
    else if (hueBounds_.contains(e.getPosition()))
        updateFromSvOrHue(e.position, true);
}

void SkinEditorColourPicker::textEditorTextChanged(juce::TextEditor&)
{
    if (suppressEditorCallback_ || ! editingEnabled_)
        return;
}

void SkinEditorColourPicker::textEditorReturnKeyPressed(juce::TextEditor& ed)
{
    if (! editingEnabled_)
        return;
    if (&ed == &hexEditor_)
        applyHexFromEditor();
    else
        applyRgbFromEditors();
}

void SkinEditorColourPicker::textEditorFocusLost(juce::TextEditor& ed)
{
    if (! editingEnabled_ || suppressEditorCallback_)
        return;
    if (&ed == &hexEditor_)
        applyHexFromEditor();
    else
        applyRgbFromEditors();
}

void SkinEditorColourPicker::syncEditorsFromColour(bool)
{
    const juce::ScopedValueSetter<bool> scope(suppressEditorCallback_, true);
    hexEditor_.setText(juce::String::formatted("%02X%02X%02X",
                                               colour_.getRed(),
                                               colour_.getGreen(),
                                               colour_.getBlue()),
                       false);
    rEditor_.setText(juce::String(colour_.getRed()), false);
    gEditor_.setText(juce::String(colour_.getGreen()), false);
    bEditor_.setText(juce::String(colour_.getBlue()), false);
}

void SkinEditorColourPicker::applyHexFromEditor()
{
    auto text = hexEditor_.getText().trim().trimCharactersAtStart("#");
    if (text.length() != 6)
    {
        syncEditorsFromColour(false);
        return;
    }
    for (int i = 0; i < text.length(); ++i)
        if (juce::CharacterFunctions::getHexDigitValue(text[i]) < 0)
        {
            syncEditorsFromColour(false);
            return;
        }

    setColour(juce::Colour::fromString("FF" + text), juce::sendNotification);
}

void SkinEditorColourPicker::applyRgbFromEditors()
{
    const int r = juce::jlimit(0, 255, rEditor_.getText().getIntValue());
    const int g = juce::jlimit(0, 255, gEditor_.getText().getIntValue());
    const int b = juce::jlimit(0, 255, bEditor_.getText().getIntValue());
    setColour(juce::Colour((juce::uint8) r, (juce::uint8) g, (juce::uint8) b), juce::sendNotification);
}

void SkinEditorColourPicker::updateFromSvOrHue(juce::Point<float> pos, bool isHue)
{
    if (isHue)
    {
        hue_ = juce::jlimit(0.0f, 1.0f,
                            (pos.y - (float) hueBounds_.getY())
                                / (float) juce::jmax(1, hueBounds_.getHeight() - 1));
    }
    else
    {
        sat_ = juce::jlimit(0.0f, 1.0f,
                            (pos.x - (float) svBounds_.getX())
                                / (float) juce::jmax(1, svBounds_.getWidth() - 1));
        val_ = juce::jlimit(0.0f, 1.0f,
                            1.0f - (pos.y - (float) svBounds_.getY())
                                       / (float) juce::jmax(1, svBounds_.getHeight() - 1));
    }
    colour_ = juce::Colour::fromHSV(hue_, sat_, val_, 1.0f);
    syncEditorsFromColour(false);
    repaint();
    notifyChanged();
}

void SkinEditorColourPicker::notifyChanged()
{
    if (onColourChanged)
        onColourChanged(colour_);
}
