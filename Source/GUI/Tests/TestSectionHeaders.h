#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestSectionHeaders : public juce::Component
{
public:
    explicit TestSectionHeaders(TSS::ISkin& skin, int sectionHeaderWidth, int sectionHeaderHeight);
    ~TestSectionHeaders() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class SectionHeaderScalePanel;

    TSS::ISkin* skin_ = nullptr;
    int sectionHeaderWidth_ = 0;
    int sectionHeaderHeight_ = 0;
    std::vector<std::unique_ptr<SectionHeaderScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestSectionHeaders)
};
