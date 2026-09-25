#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    class ISkin;
}

// Blocking choice after a successful .m1km decode: Master settings only, Full Master, or Cancel.
class MasterM1kmLoadChoiceDialog : public juce::Component
{
public:
    static constexpr int kDesignWidth = 560;
    static constexpr int kDesignHeight = 168;

    MasterM1kmLoadChoiceDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested);
    ~MasterM1kmLoadChoiceDialog() override;

    void prepareForShow(std::function<void()> onMasterSettingsOnly,
                        std::function<void()> onFullMaster);

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    void dismiss();
    void chooseMasterSettingsOnly();
    void chooseFullMaster();

    std::function<void()> onDismissRequested_;
    std::function<void()> onMasterSettingsOnly_;
    std::function<void()> onFullMaster_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    juce::TextButton masterSettingsOnlyButton_;
    juce::TextButton fullMasterButton_;
    juce::TextButton cancelButton_;

    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterM1kmLoadChoiceDialog)
};
