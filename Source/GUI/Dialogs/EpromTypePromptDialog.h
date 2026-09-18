#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/ComboBox.h"
#include "GUI/Widgets/Label.h"
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

    struct PrepareForShowArgs
    {
        MatrixDeviceTypes::Type deviceType = MatrixDeviceTypes::Type::kUnknown;
        int preferredSelectedId = 0;
        bool includeFirmwareSuggestionHint = false;
        std::function<void(int selectedId)> onConfirm;
        std::function<void()> onLater;
    };

    EpromTypePromptDialog(TSS::ISkin& skin, std::function<void()> onDismissRequested);
    ~EpromTypePromptDialog() override;

    void prepareForShow(PrepareForShowArgs args);

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    struct ContentLayout
    {
        juce::Rectangle<int> bodyTextArea;
        juce::Rectangle<int> controlBand; // between body and buttons
        juce::Rectangle<int> buttonRow;
    };

    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    ContentLayout computeContentLayout() const;
    juce::String bodyText() const;
    void populateComboItems(MatrixDeviceTypes::Type deviceType, int preferredSelectedId);
    void dismissAsLater();
    void confirm();

    std::function<void()> onDismissRequested_;
    std::function<void(int)> onConfirm_;
    std::function<void()> onLater_;
    TSS::ISkin* skin_;
    float uiScale_ = 1.0f;
    bool includeFirmwareSuggestionHint_ = false;

    std::unique_ptr<TSS::Label> epromTypeLabel_;
    std::unique_ptr<TSS::ComboBox> epromTypeCombo_;
    juce::TextButton confirmButton_;
    juce::TextButton specifyLaterButton_;

    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;
    inline constexpr static int kLabelWidth_ = 120;
    inline constexpr static int kComboWidth_ = 140;
    inline constexpr static int kControlHeight_ = 20;
    inline constexpr static int kSpecifyLaterButtonWidth_ = 120;
    inline constexpr static int kConfirmButtonWidth_ = 80;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(EpromTypePromptDialog)
};
