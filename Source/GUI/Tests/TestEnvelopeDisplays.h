#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/WidgetDimensions.h"

namespace TSS
{
    class ISkin;
}

class TestEnvelopeDisplays : public juce::Component
{
public:
    explicit TestEnvelopeDisplays(TSS::ISkin& skin, const DisplayBandDimensions& dimensions);
    ~TestEnvelopeDisplays() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class EnvelopeDisplayScalePanel;

    TSS::ISkin* skin_ = nullptr;
    DisplayBandDimensions dimensions_;
    std::vector<std::unique_ptr<EnvelopeDisplayScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestEnvelopeDisplays)
};
