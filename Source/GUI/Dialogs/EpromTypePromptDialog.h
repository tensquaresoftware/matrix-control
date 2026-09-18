#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/ComboBox.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"

namespace TSS
{
    class ISkin;
}

class EpromTypePromptDialog : public juce::Component
{
public:
    static constexpr int kDesignWidth = 420;
    static constexpr int kDesignHeight = 168;

    EpromTypePromptDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested);
    ~EpromTypePromptDialog() override;

    void prepareForShow(MatrixDeviceTypes::Type deviceType,
                        int preferredSelectedId,
                        std::function<void(int selectedId)> onConfirm,
                        std::function<void()> onLater);

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    void populateComboItems(MatrixDeviceTypes::Type deviceType, int preferredSelectedId);
    void dismissAsLater();
    void confirm();

    std::function<void()> onDismissRequested_;
    std::function<void(int)> onConfirm_;
    std::function<void()> onLater_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;

    std::unique_ptr<TSS::ComboBox> epromTypeCombo_;
    juce::TextButton okButton_;
    juce::TextButton laterButton_;

    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;
    inline constexpr static int kComboWidth_ = 160;
    inline constexpr static int kComboHeight_ = 20;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EpromTypePromptDialog)
};
