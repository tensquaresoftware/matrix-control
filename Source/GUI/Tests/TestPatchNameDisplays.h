#pragma once

#include <functional>
#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestPatchNameDisplays : public juce::Component
{
public:
    explicit TestPatchNameDisplays(TSS::ISkin& skin, const PatchNameDisplayDimensions& dimensions);
    ~TestPatchNameDisplays() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class PatchNameDisplayScalePanel;

    enum class DemoMode
    {
        kIdle = 1,
        kInlineEdit,
        kMutatorSecondary,
        kDragOverlay,
        kNameRequired,
        kNameRequiredDragRestore
    };

    TSS::ISkin* skin_ = nullptr;
    PatchNameDisplayDimensions dimensions_;
    DemoMode selectedMode_ = DemoMode::kIdle;
    std::unique_ptr<juce::ComboBox> modeSelector_;
    std::unique_ptr<juce::Label> modeLabel_;
    std::unique_ptr<juce::Label> outcomeLabel_;
    std::vector<std::unique_ptr<PatchNameDisplayScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();
    void applySelectedModeToPanels();
    void setOutcomeReadout(const juce::String& outcome);
    static int modeControlsHeight();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestPatchNameDisplays)
};
