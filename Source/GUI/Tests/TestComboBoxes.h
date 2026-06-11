#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestComboBoxes : public juce::Component
{
public:
    explicit TestComboBoxes(TSS::ISkin& skin);
    ~TestComboBoxes() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class ComboBoxScalePanel;

    TSS::ISkin* skin_ = nullptr;
    std::vector<std::unique_ptr<ComboBoxScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestComboBoxes)
};
