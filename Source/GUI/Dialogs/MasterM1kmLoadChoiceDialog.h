#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/Widgets/Button.h"

namespace TSS
{
    class ISkin;
}

// Blocking choice after a successful .m1km decode: Master settings only, Full Master, or Cancel.
class MasterM1kmLoadChoiceDialog : public juce::Component
{
public:
    static constexpr int kDesignWidth = 560;

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
    DialogMatrixHelpers::TextModalLayout computeLayout() const;
    void dismiss();
    void chooseMasterSettingsOnly();
    void chooseFullMaster();

    std::function<void()> onDismissRequested_;
    std::function<void()> onMasterSettingsOnly_;
    std::function<void()> onFullMaster_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::Button> masterSettingsOnlyButton_;
    std::unique_ptr<TSS::Button> fullMasterButton_;
    std::unique_ptr<TSS::Button> cancelButton_;

    inline constexpr static int kSettingsOnlyButtonWidth_ = 148;
    inline constexpr static int kFullMasterButtonWidth_ = 268;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MasterM1kmLoadChoiceDialog)
};
