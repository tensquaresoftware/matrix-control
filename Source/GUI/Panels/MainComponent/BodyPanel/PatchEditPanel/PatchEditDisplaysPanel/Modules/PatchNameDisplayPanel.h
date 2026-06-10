#pragma once

#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
    class ModuleHeader;
    class PatchNameDisplay;
}

class PatchNameDisplayPanel : public juce::Component
{
public:
    PatchNameDisplayPanel(TSS::ISkin& skin,
                          int width,
                          int height,
                          const PatchNameDisplayDimensions& patchNameDims,
                          const ModuleHeaderDimensions& moduleHeaderDims);
    ~PatchNameDisplayPanel() override;

    void resized() override;
    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    TSS::PatchNameDisplay& getPatchNameDisplay();

private:
    int width_;
    int height_;
    PatchNameDisplayDimensions patchNameDims_;
    ModuleHeaderDimensions moduleHeaderDims_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::ModuleHeader> moduleHeader_;
    std::unique_ptr<TSS::PatchNameDisplay> patchNameDisplay_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(PatchNameDisplayPanel)
};
