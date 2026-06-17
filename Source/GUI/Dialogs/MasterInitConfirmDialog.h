#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

class MasterInitConfirmDialog : public juce::Component
{
public:
    static constexpr int kDesignWidth = 420;
    static constexpr int kDesignHeight = 120;

    MasterInitConfirmDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested);
    ~MasterInitConfirmDialog() override;

    void prepareForShow(const juce::String& moduleDisplayName, std::function<void()> onConfirm);

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    juce::String formatBodyText() const;
    void dismiss();
    void confirm();

    std::function<void()> onDismissRequested_;
    std::function<void()> onConfirm_;
    TSS::ISkin* skin_;
    juce::String moduleDisplayName_;
    float uiScale_ = 1.0f;

    juce::TextButton resetButton_;
    juce::TextButton cancelButton_;

    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterInitConfirmDialog)
};
