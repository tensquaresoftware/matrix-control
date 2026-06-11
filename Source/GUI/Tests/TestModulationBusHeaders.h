#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestModulationBusHeaders : public juce::Component
{
public:
    explicit TestModulationBusHeaders(TSS::ISkin& skin,
                                      int headerWidth,
                                      int headerHeight,
                                      const ModulationBusHeaderDimensions& dimensions);
    ~TestModulationBusHeaders() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ModulationBusHeaderScalePanel;

    TSS::ISkin* skin_ = nullptr;
    int headerWidth_ = 0;
    int headerHeight_ = 0;
    ModulationBusHeaderDimensions dimensions_;
    std::vector<std::unique_ptr<ModulationBusHeaderScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestModulationBusHeaders)
};
