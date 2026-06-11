#pragma once

#include <memory>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class TestPopupMenus : public juce::Component
{
public:
    explicit TestPopupMenus(TSS::ISkin& skin, int comboWidth, int comboHeight);
    ~TestPopupMenus() override;

    void setSkin(TSS::ISkin& skin);
    void resized() override;
    int getPreferredWidth() const;
    int getPreferredHeight() const;

private:
    class PopupMenuScalePanel;

    TSS::ISkin* skin_ = nullptr;
    int comboWidth_ = 0;
    int comboHeight_ = 0;
    std::vector<std::unique_ptr<PopupMenuScalePanel>> columnPanels_;

    void rebuildPanels();
    void layoutColumnPanels();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(TestPopupMenus)
};
