#pragma once

#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/PanelDimensions.h"
#include "GUI/Tests/TestColourPickerButton.h"

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
    bool keyPressed(const juce::KeyPress& key) override;

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
    static constexpr float kMinContentZoom_ = 0.25f;
    static constexpr float kMaxContentZoom_ = 8.0f;
    static constexpr float kContentZoomStepFactor_ = 1.25f;

    inline constexpr static int kPadding_ = 12;
    inline constexpr static int kGap_ = 12;
    inline constexpr static int kWidgetLabelHeight_ = 20;
    inline constexpr static int kWidgetSelectorWidth_ = 176;
    inline constexpr static int kWidgetSelectorHeight_ = 24;
    inline constexpr static int kHeaderSectionGap_ = 16;
    inline constexpr static int kDebugSectionLeadingGap_ = kHeaderSectionGap_ * 2;
    inline constexpr static int kDebugControlGap_ = 8;
    inline constexpr static int kDebugControlHeight_ = 20;
    inline constexpr static int kGridToggleWidth_ = 60;
    inline constexpr static int kBoundsToggleWidth_ = 84;
    inline constexpr static int kGridSizeSelectorWidth_ = 84;
    inline constexpr static int kColourPickerSize_ = 20;
    inline constexpr static int kWidgetLabelWidthReduction_ = 0;
    inline constexpr static int kWidgetLabelExtraWidth_ = 8;

    TSS::ISkin* skin_ = nullptr;
    juce::ValueTree& settingsState_;
    GuiLayoutDimensions layoutDimensions_;
    std::unique_ptr<WidgetFactory> widgetFactory_;
    juce::Label widgetLabel_;
    juce::ComboBox widgetSelector_;
    juce::ToggleButton gridToggle_;
    juce::ComboBox gridSizeSelector_;
    TestColourPickerButton gridColourPicker_;
    juce::ToggleButton boundsToggle_;
    TestColourPickerButton boundsColourPicker_;
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
    int baseContentWidth_ = 0;
    int baseContentHeight_ = 0;
    float contentZoom_ = 1.0f;

    void createHeaderControls();
    void createVisualDebugControls();
    void layoutHeaderControls(int topRowHeight);
    void repaintVisualDebugLayers();
    void createTestPages(juce::AudioProcessorValueTreeState& apvts);
    void populateWidgetSelector();
    void restorePersistedWidgetSelection();
    void persistWidgetSelection(int selectedId);
    void updateVisibleTests();
    void layoutTestContentHost();
    void applyContentZoom();
    void setContentZoom(float zoom);
    void zoomContentIn();
    void zoomContentOut();
    void resetContentZoom();
    static bool isZoomResetKey(const juce::KeyPress& key);

    static int alignToMultipleOf4(int value);
    static int getDefaultWidgetSelectionId();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestComponent)
};
