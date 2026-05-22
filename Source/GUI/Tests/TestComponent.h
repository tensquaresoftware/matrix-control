#pragma once

#include <memory>

#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Label.h"

namespace tss
{
    class ISkin;
}

class TestButtons;
class TestSliders;

class TestComponent : public juce::Component
{
public:
    TestComponent(tss::ISkin& skin, juce::ValueTree& settingsState, int width, int height);
    ~TestComponent() override;

    void paint(juce::Graphics& g) override;
    void resized() override;

private:
    enum class TestWidgetType
    {
        Button = 1,
        Toggle,
        Slider,
        ComboBox,
        NumberBox,
        Label,
        GroupLabel,
        ModuleHeader,
        SectionHeader,
        EnvelopeDisplay,
        TrackGeneratorDisplay,
        PatchNameDisplay
    };

    static constexpr juce::uint32 kBackgroundColour_ = 0xFF151515;

    inline constexpr static int kPadding_ = 12;
    inline constexpr static int kGap_ = 12;
    inline constexpr static int kWidgetLabelHeight_ = 20;
    inline constexpr static int kWidgetSelectorWidth_ = 176;
    inline constexpr static int kWidgetSelectorHeight_ = 24;

    tss::ISkin& skin_;
    juce::ValueTree& settingsState_;
    std::unique_ptr<tss::Label> widgetLabel_;
    juce::ComboBox widgetSelector_;
    std::unique_ptr<TestButtons> testButtons_;
    std::unique_ptr<TestSliders> testSliders_;
    int widgetLabelWidth_ = 0;

    void createHeaderControls();
    void populateWidgetSelector();
    void restorePersistedWidgetSelection();
    void persistWidgetSelection(int selectedId);
    void updateVisibleTests();

    static int alignToMultipleOf4(int value);
    static int getDefaultWidgetSelectionId();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestComponent)
};
