#pragma once

#include <functional>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Dialogs/DialogMatrixHelpers.h"
#include "GUI/PluginEditorInternal.h"
#include "GUI/Widgets/Button.h"

namespace TSS
{
    class ISkin;
}

/** Sync Matrix-chrome confirm used by product modal gates (LTR Cancel / [middle] / primary). */
class MatrixOrderedConfirmDialog : public juce::Component
{
public:
    MatrixOrderedConfirmDialog(TSS::ISkin& skin,
                               float uiScale,
                               const PluginEditorInternal::OrderedConfirmAlertOptions& options);
    ~MatrixOrderedConfirmDialog() override = default;

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    void finish(int code);

    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;
    juce::String title_;
    juce::String message_;
    bool hasMiddle_ = false;

    std::unique_ptr<TSS::Button> cancelButton_;
    std::unique_ptr<TSS::Button> middleButton_;
    std::unique_ptr<TSS::Button> primaryButton_;

    inline constexpr static int kDesignWidth_ = 460;
    inline constexpr static int kDesignHeight_ = 160;
    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixOrderedConfirmDialog)
};

/** Sync Matrix-chrome Mutator Delete confirm with optional don't-ask-again toggle. */
class MatrixMutatorDeleteConfirmDialog : public juce::Component
{
public:
    MatrixMutatorDeleteConfirmDialog(TSS::ISkin& skin, float uiScale);
    ~MatrixMutatorDeleteConfirmDialog() override = default;

    PluginEditorInternal::MutatorDeleteConfirmResult getResult() const noexcept { return result_; }

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& e) override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    juce::Rectangle<int> getDialogBounds() const;
    void finish(bool confirmed);

    TSS::ISkin* skin_ = nullptr;
    float uiScale_ = 1.0f;
    PluginEditorInternal::MutatorDeleteConfirmResult result_{};

    juce::ToggleButton dontAskAgain_;
    std::unique_ptr<TSS::Button> cancelButton_;
    std::unique_ptr<TSS::Button> deleteButton_;

    inline constexpr static int kDesignWidth_ = 460;
    inline constexpr static int kDesignHeight_ = 188;
    inline constexpr static int kTitleBarHeight_ = 28;
    inline constexpr static int kBorderThickness_ = 4;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MatrixMutatorDeleteConfirmDialog)
};
