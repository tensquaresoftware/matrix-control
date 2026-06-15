#include "TestColourPickerButton.h"

#include <juce_gui_extra/juce_gui_extra.h>

namespace
{
    constexpr int kColourSelectorFlags_ = juce::ColourSelector::showAlphaChannel
                                        | juce::ColourSelector::showColourAtTop
                                        | juce::ColourSelector::showColourspace;
    constexpr int kColourSelectorWidth_ = 320;
    constexpr int kColourSelectorHeight_ = 420;
}

class TestColourPickerButton::ColourSelectorPopup final : public juce::Component,
                                                          private juce::ChangeListener
{
public:
    ColourSelectorPopup(juce::Colour initialColour, TestColourPickerButton& owner)
        : owner_(owner)
        , selector_(kColourSelectorFlags_)
    {
        selector_.setCurrentColour(initialColour, juce::dontSendNotification);
        selector_.addChangeListener(this);
        selector_.setSize(kColourSelectorWidth_, kColourSelectorHeight_);
        addAndMakeVisible(selector_);
        setSize(kColourSelectorWidth_, kColourSelectorHeight_);
    }

private:
    void resized() override
    {
        selector_.setBounds(getLocalBounds());
    }

    void changeListenerCallback(juce::ChangeBroadcaster* source) override
    {
        if (source != &selector_)
            return;

        owner_.setCurrentColour(selector_.getCurrentColour());
    }

    TestColourPickerButton& owner_;
    juce::ColourSelector selector_;
};

TestColourPickerButton::TestColourPickerButton(juce::Colour initialColour)
    : currentColour_(initialColour)
{
}

void TestColourPickerButton::setCurrentColour(juce::Colour colour, juce::NotificationType notification)
{
    if (currentColour_ == colour)
        return;

    currentColour_ = colour;
    repaint();

    if (notification == juce::sendNotification && onColourChanged != nullptr)
        onColourChanged(currentColour_);
}

void TestColourPickerButton::paint(juce::Graphics& g)
{
    const auto bounds = getLocalBounds();

    g.setColour(juce::Colours::white);
    g.fillCheckerBoard(bounds.toFloat(), 4.0f, 4.0f, juce::Colour(0xFF2A2A2A), juce::Colour(0xFF1E1E1E));

    g.setColour(currentColour_);
    g.fillRect(bounds.reduced(1));

    g.setColour(juce::Colours::white.withAlpha(0.35f));
    g.drawRect(bounds.toFloat().reduced(1.0f), 1.0f);
}

void TestColourPickerButton::mouseDown(const juce::MouseEvent& event)
{
    juce::ignoreUnused(event);
    showColourPicker();
}

void TestColourPickerButton::showColourPicker()
{
    auto popup = std::make_unique<ColourSelectorPopup>(currentColour_, *this);
    juce::CallOutBox::launchAsynchronously(std::move(popup), getScreenBounds(), getTopLevelComponent());
}
