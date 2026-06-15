#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

class TestColourPickerButton : public juce::Component
{
public:
    explicit TestColourPickerButton(juce::Colour initialColour = juce::Colours::white);

    void setCurrentColour(juce::Colour colour, juce::NotificationType notification = juce::sendNotification);
    juce::Colour getCurrentColour() const noexcept { return currentColour_; }

    std::function<void(juce::Colour)> onColourChanged;

    void paint(juce::Graphics& g) override;
    void mouseDown(const juce::MouseEvent& event) override;

private:
    class ColourSelectorPopup;

    void showColourPicker();

    juce::Colour currentColour_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestColourPickerButton)
};
