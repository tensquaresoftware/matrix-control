#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestLabels : public juce::Component
{
public:
    explicit TestLabels(TSS::ISkin& skin);
    ~TestLabels() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class LabelScalePanel;

    TSS::ISkin* skin_ = nullptr;
    std::vector<std::unique_ptr<LabelScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestLabels)
};
