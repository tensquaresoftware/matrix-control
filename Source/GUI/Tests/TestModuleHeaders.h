#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestModuleHeaders : public juce::Component
{
public:
    explicit TestModuleHeaders(TSS::ISkin& skin, const ModuleHeaderDimensions& dimensions);
    ~TestModuleHeaders() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ModuleHeaderScalePanel;

    TSS::ISkin* skin_ = nullptr;
    ModuleHeaderDimensions dimensions_;
    std::vector<std::unique_ptr<ModuleHeaderScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestModuleHeaders)
};
