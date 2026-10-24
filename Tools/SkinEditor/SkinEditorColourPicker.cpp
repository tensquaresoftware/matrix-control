#include "Tools/SkinEditor/SkinEditorColourPicker.h"

#include "Tools/SkinEditor/SkinEditorChrome.h"

SkinEditorColourPicker::SkinEditorColourPicker()
{
    SkinEditorChrome::stylePanelTitle(titleLabel_, "COLOR");
    addAndMakeVisible(titleLabel_);

    for (auto* label : { &hexLabel_, &rLabel_, &gLabel_, &bLabel_ })
    {
        label->setFont(SkinEditorChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinEditorChrome::textMuted());
        SkinEditorChrome::preventTextSquish(*label);
        addAndMakeVisible(*label);
    }
    // Sit the letter against its field (gap lives to the left of the label).
    for (auto* label : { &rLabel_, &gLabel_, &bLabel_ })
        label->setJustificationType(juce::Justification::centredRight);

    auto styleEditor = [this](juce::TextEditor& ed)
    {
        ed.setFont(SkinEditorChrome::fontMono());
        ed.setColour(juce::TextEditor::backgroundColourId, SkinEditorChrome::surfaceApp());
        ed.setColour(juce::TextEditor::textColourId, SkinEditorChrome::textPrimary());
        ed.setColour(juce::TextEditor::outlineColourId, SkinEditorChrome::controlOutline());
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
    colour_ = colour; // keep alpha so the damier preview can show transparency
    auto opaque = colour_.withAlpha(1.0f);
    opaque.getHSB(hue_, sat_, val_);
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
    SkinEditorChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_);

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
    const auto chrome = SkinEditorChrome::layoutPanel(getLocalBounds(),
                                                      SkinEditorChrome::titleTabWidth("COLOR"));
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    auto bounds = chrome.content;

    constexpr int kFieldRowH = 24;
    const int hueW = 18;
    const int side = juce::jmin(bounds.getWidth() - hueW - 8,
                                bounds.getHeight() - kFieldRowH - SkinEditorChrome::kPanelContentPad);
    svBounds_ = { bounds.getX(), bounds.getY(), side, side };
    hueBounds_ = { svBounds_.getRight() + 8, svBounds_.getY(), hueW, side };

    // Fixed control widths (font-based only) — do not shrink with the panel / window.
    const auto labelFont = SkinEditorChrome::fontRegular();
    const auto monoFont = SkinEditorChrome::fontMono();
    const int hashW = SkinEditorChrome::measureLabelWidth("#", labelFont);
    const int rgbLabW = SkinEditorChrome::measureTextWidth(labelFont, "R") + 2;
    constexpr int kLabelToField = 4;
    constexpr int kRgbGap = 9;
    const int hexW0 = SkinEditorChrome::measureTextWidth(monoFont, "FFFFFF") + 8;
    const int hexW = juce::jmax(24, juce::roundToInt((float) hexW0 * 1.2f) - 8 + 4 - 3);
    const int edW = juce::jmax(28, SkinEditorChrome::measureTextWidth(monoFont, "255") + 12 - 2);
    const int rgbGroupW = 3 * (rgbLabW + kLabelToField + edW) + 2 * kRgbGap;

    const int rowY = svBounds_.getBottom() + SkinEditorChrome::kPanelContentPad;
    // # / hex stay left under the SV; R/G/B pack to the right so B aligns with SV right.
    hexLabel_.setBounds(svBounds_.getX(), rowY, hashW, kFieldRowH);
    hexEditor_.setBounds(svBounds_.getX() + hashW, rowY, hexW, kFieldRowH);

    auto rgbRow = juce::Rectangle<int>(svBounds_.getRight() - rgbGroupW, rowY, rgbGroupW, kFieldRowH);
    auto placeRgb = [&rgbRow, rgbLabW, edW](juce::Label& lab, juce::TextEditor& ed, bool last)
    {
        lab.setBounds(rgbRow.removeFromLeft(rgbLabW));
        rgbRow.removeFromLeft(kLabelToField);
        ed.setBounds(rgbRow.removeFromLeft(edW));
        if (! last)
            rgbRow.removeFromLeft(kRgbGap);
    };
    placeRgb(rLabel_, rEditor_, false);
    placeRgb(gLabel_, gEditor_, false);
    placeRgb(bLabel_, bEditor_, true);
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
