#include "Tools/SkinDesigner/SkinDesignerColourPicker.h"

#include "Tools/SkinDesigner/SkinDesignerChrome.h"

namespace
{
    constexpr int kFieldRowH = 24;
    constexpr int kHueBarW = 18;
    constexpr int kSvHueGap = 8;
    constexpr int kLabelToField = 4;
}

void SkinDesignerColourPicker::styleAxisSlider(juce::Slider& slider)
{
    slider.setRange(0.0, 1.0, 0.0);
    slider.setSliderStyle(juce::Slider::LinearHorizontal);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    slider.setColour(juce::Slider::backgroundColourId, SkinDesignerChrome::surfaceApp());
    slider.setColour(juce::Slider::trackColourId, SkinDesignerChrome::controlOutline());
    slider.setColour(juce::Slider::thumbColourId, SkinDesignerChrome::textMuted());
    slider.addListener(this);
    addAndMakeVisible(slider);
}

SkinDesignerColourPicker::SkinDesignerColourPicker()
{
    SkinDesignerChrome::stylePanelTitle(titleLabel_, "COLOR");
    addAndMakeVisible(titleLabel_);

    for (auto* label : { &hexLabel_, &rLabel_, &gLabel_, &bLabel_,
                         &saturationLabel_, &brightnessLabel_ })
    {
        label->setFont(SkinDesignerChrome::fontRegular());
        label->setColour(juce::Label::textColourId, SkinDesignerChrome::textMuted());
        SkinDesignerChrome::preventTextSquish(*label);
        addAndMakeVisible(*label);
    }
    for (auto* label : { &rLabel_, &gLabel_, &bLabel_ })
        label->setJustificationType(juce::Justification::centredRight);
    // Match "#" — left edge of the COLOR content column.
    saturationLabel_.setJustificationType(juce::Justification::centredLeft);
    brightnessLabel_.setJustificationType(juce::Justification::centredLeft);

    escapeForwarder_.handler = &onEscapeRequested;
    setWantsKeyboardFocus(true);
    addKeyListener(&escapeForwarder_);

    auto styleEditor = [this](juce::TextEditor& ed)
    {
        ed.setFont(SkinDesignerChrome::fontMono());
        ed.setColour(juce::TextEditor::backgroundColourId, SkinDesignerChrome::surfaceApp());
        ed.setColour(juce::TextEditor::textColourId, SkinDesignerChrome::textPrimary());
        ed.setColour(juce::TextEditor::outlineColourId, SkinDesignerChrome::controlOutline());
        ed.addListener(this);
        ed.addKeyListener(&escapeForwarder_);
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

    styleAxisSlider(saturationSlider_);
    styleAxisSlider(brightnessSlider_);

    clearEditors();
    setEnabledEditing(false);
}

SkinDesignerColourPicker::~SkinDesignerColourPicker()
{
    saturationSlider_.removeListener(this);
    brightnessSlider_.removeListener(this);
    removeKeyListener(&escapeForwarder_);
    hexEditor_.removeKeyListener(&escapeForwarder_);
    rEditor_.removeKeyListener(&escapeForwarder_);
    gEditor_.removeKeyListener(&escapeForwarder_);
    bEditor_.removeKeyListener(&escapeForwarder_);
}

bool SkinDesignerColourPicker::keyPressed(const juce::KeyPress& key)
{
    if (key == juce::KeyPress::escapeKey && onEscapeRequested != nullptr)
        return onEscapeRequested();
    return false;
}

void SkinDesignerColourPicker::setColour(juce::Colour colour, juce::NotificationType notification)
{
    hasActiveColour_ = true;
    colour_ = colour; // keep alpha so the damier preview can show transparency
    auto opaque = colour_.withAlpha(1.0f);
    opaque.getHSB(hue_, sat_, val_);
    syncEditorsFromColour();
    repaint();
    if (notification != juce::dontSendNotification)
        notifyChanged();
}

void SkinDesignerColourPicker::clearActiveColour()
{
    endWellDragIfActive();
    endSliderGestureIfActive();
    hasActiveColour_ = false;
    colour_ = juce::Colours::black;
    hue_ = sat_ = val_ = 0.0f;
    clearEditors();
    repaint();
}

void SkinDesignerColourPicker::setEnabledEditing(bool canEdit)
{
    const bool next = canEdit && hasActiveColour_;
    if (editingEnabled_ && ! next)
    {
        endWellDragIfActive();
        endSliderGestureIfActive();
    }
    editingEnabled_ = next;
    hexEditor_.setEnabled(editingEnabled_);
    rEditor_.setEnabled(editingEnabled_);
    gEditor_.setEnabled(editingEnabled_);
    bEditor_.setEnabled(editingEnabled_);
    saturationSlider_.setEnabled(editingEnabled_);
    brightnessSlider_.setEnabled(editingEnabled_);
    repaint();
}

void SkinDesignerColourPicker::paint(juce::Graphics& g)
{
    SkinDesignerChrome::paintPanelFrame(g, getLocalBounds(), titleTabBounds_, 0);
    if (! svBounds_.isEmpty())
        paintSvWell(g);
    if (! hueBounds_.isEmpty())
        paintHueBar(g);
}

void SkinDesignerColourPicker::paintSvWell(juce::Graphics& g) const
{
    if (! hasActiveColour_)
    {
        g.setColour(juce::Colour(0xff151515));
        g.fillRect(svBounds_);
        return;
    }

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

void SkinDesignerColourPicker::paintHueBar(juce::Graphics& g) const
{
    if (! hasActiveColour_)
    {
        g.setColour(juce::Colour(0xff151515));
        g.fillRect(hueBounds_);
        return;
    }

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

void SkinDesignerColourPicker::layoutHexRgbRow(int rowY)
{
    constexpr int kRgbGap = 9;
    const auto labelFont = SkinDesignerChrome::fontRegular();
    const auto monoFont = SkinDesignerChrome::fontMono();
    const int hashW = SkinDesignerChrome::measureLabelWidth("#", labelFont);
    const int rgbLabW = SkinDesignerChrome::measureTextWidth(labelFont, "R") + 2;
    const int hexW0 = SkinDesignerChrome::measureTextWidth(monoFont, "FFFFFF") + 8;
    const int hexW = juce::jmax(24, juce::roundToInt((float) hexW0 * 1.2f) - 8 + 4 - 3);
    const int edW = juce::jmax(28, SkinDesignerChrome::measureTextWidth(monoFont, "255") + 12 - 2);
    const int rgbGroupW = 3 * (rgbLabW + kLabelToField + edW) + 2 * kRgbGap;

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

void SkinDesignerColourPicker::layoutAxisRows(int rowY)
{
    const auto labelFont = SkinDesignerChrome::fontRegular();
    const int axisLabW = juce::jmax(SkinDesignerChrome::measureTextWidth(labelFont, "Saturation:"),
                                    SkinDesignerChrome::measureTextWidth(labelFont, "Brightness:"));
    constexpr int tipPad = juce::jmin(12, kFieldRowH / 2), trimRight = 3, sliderYNudge = 1;
    constexpr int leftInset = 2; // past label column (shortened 8 px from prior smoke)
    const int labelLeft = hexLabel_.getX();
    const int sliderLeft = labelLeft + axisLabW + leftInset;
    const int sliderRight = juce::jmin(hueBounds_.getRight(), bEditor_.getRight() + tipPad - trimRight);
    const int sliderW = juce::jmax(0, sliderRight - sliderLeft);
    // 5px tighter than kPanelContentPad (matches preferredHeightForWidth smoke pad).
    const int satY = rowY + kFieldRowH + (SkinDesignerChrome::kPanelContentPad - 5);
    saturationLabel_.setBounds(labelLeft, satY, axisLabW, kFieldRowH);
    saturationSlider_.setBounds(sliderLeft, satY + sliderYNudge, sliderW, kFieldRowH);
    brightnessLabel_.setBounds(labelLeft, satY + kFieldRowH, axisLabW, kFieldRowH);
    brightnessSlider_.setBounds(sliderLeft, satY + kFieldRowH + sliderYNudge, sliderW, kFieldRowH);
}

void SkinDesignerColourPicker::resized()
{
    auto chrome = SkinDesignerChrome::layoutPanel(getLocalBounds(),
                                                SkinDesignerChrome::titleTabWidth("COLOR"),
                                                0);
    // layoutPanel insets bottom by kPanelContentPad; COLOR uses 5px less (preferredHeight).
    chrome.content.setHeight(chrome.content.getHeight() + 5);
    titleTabBounds_ = chrome.tab;
    titleLabel_.setBounds(chrome.tab);
    auto bounds = chrome.content;

    const int side = juce::jmax(0, bounds.getWidth() - kHueBarW - kSvHueGap);
    svBounds_ = { bounds.getX(), bounds.getY(), side, side };
    hueBounds_ = { svBounds_.getRight() + kSvHueGap, svBounds_.getY(), kHueBarW, side };

    const int rowY = svBounds_.getBottom() + SkinDesignerChrome::kPanelContentPad;
    layoutHexRgbRow(rowY);
    layoutAxisRows(rowY);
}

void SkinDesignerColourPicker::mouseDown(const juce::MouseEvent& e)
{
    grabKeyboardFocus();
    dragTarget_ = DragTarget::kNone;
    if (! editingEnabled_ || ! hasActiveColour_)
        return;

    if (svBounds_.contains(e.getPosition()))
        dragTarget_ = DragTarget::kSv;
    else if (hueBounds_.contains(e.getPosition()))
        dragTarget_ = DragTarget::kHue;

    if (dragTarget_ == DragTarget::kNone)
        return;

    if (onColourGestureBegan != nullptr)
        onColourGestureBegan();
    mouseDrag(e);
}

void SkinDesignerColourPicker::mouseDrag(const juce::MouseEvent& e)
{
    if (! editingEnabled_ || ! hasActiveColour_)
        return;

    if (dragTarget_ == DragTarget::kSv)
        updateFromSvDrag(e.position);
    else if (dragTarget_ == DragTarget::kHue)
        updateFromHueDrag(e.position);
}

void SkinDesignerColourPicker::mouseUp(const juce::MouseEvent&)
{
    endWellDragIfActive();
}

void SkinDesignerColourPicker::endWellDragIfActive()
{
    if (dragTarget_ == DragTarget::kNone)
        return;
    dragTarget_ = DragTarget::kNone;
    if (onColourGestureEnded != nullptr)
        onColourGestureEnded();
}

void SkinDesignerColourPicker::endSliderGestureIfActive()
{
    if (! sliderGestureActive_)
        return;
    sliderGestureActive_ = false;
    if (onColourGestureEnded != nullptr)
        onColourGestureEnded();
}

void SkinDesignerColourPicker::textEditorReturnKeyPressed(juce::TextEditor& ed)
{
    if (! editingEnabled_)
        return;
    if (&ed == &hexEditor_)
        applyHexFromEditor();
    else
        applyRgbFromEditors();
}

void SkinDesignerColourPicker::textEditorFocusLost(juce::TextEditor& ed)
{
    if (! editingEnabled_ || isSyncingEditors_)
        return;
    if (&ed == &hexEditor_)
        applyHexFromEditor();
    else
        applyRgbFromEditors();
}

void SkinDesignerColourPicker::sliderValueChanged(juce::Slider* slider)
{
    if (! editingEnabled_ || ! hasActiveColour_ || isSyncingEditors_ || slider == nullptr)
        return;

    if (slider == &saturationSlider_)
        sat_ = (float) saturationSlider_.getValue();
    else if (slider == &brightnessSlider_)
        val_ = (float) brightnessSlider_.getValue();
    else
        return;

    // Wheel / programmatic nudges skip DragStarted/Ended — wrap as a one-shot gesture.
    const bool atomicNudge = ! sliderGestureActive_;
    if (atomicNudge && onColourGestureBegan != nullptr)
        onColourGestureBegan();
    applyHsvaFromDrag();
    if (atomicNudge && onColourGestureEnded != nullptr)
        onColourGestureEnded();
}

void SkinDesignerColourPicker::sliderDragStarted(juce::Slider*)
{
    if (! editingEnabled_ || ! hasActiveColour_)
        return;
    sliderGestureActive_ = true;
    if (onColourGestureBegan != nullptr)
        onColourGestureBegan();
}

void SkinDesignerColourPicker::sliderDragEnded(juce::Slider*)
{
    endSliderGestureIfActive();
}

void SkinDesignerColourPicker::syncEditorsFromColour()
{
    const juce::ScopedValueSetter<bool> scope(isSyncingEditors_, true);
    hexEditor_.setText(juce::String::formatted("%02X%02X%02X",
                                               colour_.getRed(),
                                               colour_.getGreen(),
                                               colour_.getBlue()),
                       false);
    rEditor_.setText(juce::String(colour_.getRed()), false);
    gEditor_.setText(juce::String(colour_.getGreen()), false);
    bEditor_.setText(juce::String(colour_.getBlue()), false);
    saturationSlider_.setValue((double) sat_, juce::dontSendNotification);
    brightnessSlider_.setValue((double) val_, juce::dontSendNotification);
}

void SkinDesignerColourPicker::clearEditors()
{
    const juce::ScopedValueSetter<bool> scope(isSyncingEditors_, true);
    hexEditor_.clear();
    rEditor_.clear();
    gEditor_.clear();
    bEditor_.clear();
    saturationSlider_.setValue(0.0, juce::dontSendNotification);
    brightnessSlider_.setValue(0.0, juce::dontSendNotification);
}

void SkinDesignerColourPicker::applyHexFromEditor()
{
    auto text = hexEditor_.getText().trim().trimCharactersAtStart("#");
    if (text.length() != 6)
    {
        syncEditorsFromColour();
        return;
    }
    for (int i = 0; i < text.length(); ++i)
        if (juce::CharacterFunctions::getHexDigitValue(text[i]) < 0)
        {
            syncEditorsFromColour();
            return;
        }

    const auto alpha = colour_.getFloatAlpha();
    setColour(juce::Colour::fromString("FF" + text).withAlpha(alpha), juce::sendNotification);
}

void SkinDesignerColourPicker::applyRgbFromEditors()
{
    const int r = juce::jlimit(0, 255, rEditor_.getText().getIntValue());
    const int g = juce::jlimit(0, 255, gEditor_.getText().getIntValue());
    const int b = juce::jlimit(0, 255, bEditor_.getText().getIntValue());
    const auto alpha = colour_.getFloatAlpha();
    setColour(juce::Colour((juce::uint8) r, (juce::uint8) g, (juce::uint8) b).withAlpha(alpha),
              juce::sendNotification);
}

void SkinDesignerColourPicker::updateFromSvDrag(juce::Point<float> pos)
{
    sat_ = juce::jlimit(0.0f, 1.0f,
                        (pos.x - (float) svBounds_.getX())
                            / (float) juce::jmax(1, svBounds_.getWidth() - 1));
    val_ = juce::jlimit(0.0f, 1.0f,
                        1.0f - (pos.y - (float) svBounds_.getY())
                                   / (float) juce::jmax(1, svBounds_.getHeight() - 1));
    applyHsvaFromDrag();
}

void SkinDesignerColourPicker::updateFromHueDrag(juce::Point<float> pos)
{
    hue_ = juce::jlimit(0.0f, 1.0f,
                        (pos.y - (float) hueBounds_.getY())
                            / (float) juce::jmax(1, hueBounds_.getHeight() - 1));
    applyHsvaFromDrag();
}

void SkinDesignerColourPicker::applyHsvaFromDrag()
{
    colour_ = juce::Colour::fromHSV(hue_, sat_, val_, colour_.getFloatAlpha());
    syncEditorsFromColour();
    repaint();
    notifyChanged();
}

void SkinDesignerColourPicker::notifyChanged()
{
    if (onColourChanged)
        onColourChanged(colour_);
}
