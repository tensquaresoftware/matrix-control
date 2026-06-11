#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Widgets/Label.h"

namespace TSS
{
    class ISkin;
}

class WidgetFactory;
class TestActivityLeds;
class TestButtons;
class TestComboBoxes;
class TestEnvelopeDisplays;
class TestGroupLabels;
class TestHorizontalSeparators;
class TestLabels;
class TestModulationBusCells;
class TestModulationBusHeaders;
class TestModuleHeaders;
class TestNumberBoxes;
class TestParameterCells;
class TestPatchNameDisplays;
class TestPeakIndicators;
class TestPopupMenus;
class TestSectionHeaders;
class TestSliders;
class TestToggles;
class TestTrackGeneratorDisplays;
class TestVerticalSeparators;

class TestComponent : public juce::Component
{
public:
    TestComponent(TSS::ISkin& skin,
                  juce::AudioProcessorValueTreeState& apvts,
                  juce::ValueTree& settingsState,
                  int width,
                  int height);
    ~TestComponent() override;

    void setSkin(TSS::ISkin& skin);
    void paint(juce::Graphics& g) override;
    void resized() override;

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
        PatchNameDisplay,
        HorizontalSeparator,
        VerticalSeparator,
        ActivityLed,
        PeakIndicator,
        ParameterCell,
        ModulationBusCell,
        ModulationBusHeader,
        PopupMenu
    };

private:
    static constexpr juce::uint32 kBackgroundColour_ = 0xFF151515;

    inline constexpr static int kPadding_ = 12;
    inline constexpr static int kGap_ = 12;
    inline constexpr static int kWidgetLabelHeight_ = 20;
    inline constexpr static int kWidgetSelectorWidth_ = 176;
    inline constexpr static int kWidgetSelectorHeight_ = 24;

    TSS::ISkin* skin_ = nullptr;
    juce::ValueTree& settingsState_;
    GuiLayoutDimensions layoutDimensions_;
    std::unique_ptr<WidgetFactory> widgetFactory_;
    std::unique_ptr<TSS::Label> widgetLabel_;
    juce::ComboBox widgetSelector_;
    juce::Viewport testViewport_;
    std::unique_ptr<juce::Component> testContentHost_;
    std::unique_ptr<TestButtons> testButtons_;
    std::unique_ptr<TestToggles> testToggles_;
    std::unique_ptr<TestSliders> testSliders_;
    std::unique_ptr<TestComboBoxes> testComboBoxes_;
    std::unique_ptr<TestNumberBoxes> testNumberBoxes_;
    std::unique_ptr<TestLabels> testLabels_;
    std::unique_ptr<TestGroupLabels> testGroupLabels_;
    std::unique_ptr<TestModuleHeaders> testModuleHeaders_;
    std::unique_ptr<TestSectionHeaders> testSectionHeaders_;
    std::unique_ptr<TestEnvelopeDisplays> testEnvelopeDisplays_;
    std::unique_ptr<TestTrackGeneratorDisplays> testTrackGeneratorDisplays_;
    std::unique_ptr<TestPatchNameDisplays> testPatchNameDisplays_;
    std::unique_ptr<TestHorizontalSeparators> testHorizontalSeparators_;
    std::unique_ptr<TestVerticalSeparators> testVerticalSeparators_;
    std::unique_ptr<TestActivityLeds> testActivityLeds_;
    std::unique_ptr<TestPeakIndicators> testPeakIndicators_;
    std::unique_ptr<TestParameterCells> testParameterCells_;
    std::unique_ptr<TestModulationBusCells> testModulationBusCells_;
    std::unique_ptr<TestModulationBusHeaders> testModulationBusHeaders_;
    std::unique_ptr<TestPopupMenus> testPopupMenus_;
    int widgetLabelWidth_ = 0;

    void createHeaderControls();
    void createTestPages(juce::AudioProcessorValueTreeState& apvts);
    void populateWidgetSelector();
    void restorePersistedWidgetSelection();
    void persistWidgetSelection(int selectedId);
    void updateVisibleTests();
    void layoutTestContentHost();

    static int alignToMultipleOf4(int value);
    static int getDefaultWidgetSelectionId();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestComponent)
};
