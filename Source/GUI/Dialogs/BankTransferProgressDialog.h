#pragma once

#include <functional>
#include <memory>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Widgets/Button.h"

namespace TSS
{
    class ISkin;
}

// Blocking modal overlay for Bank Utility EXPORT / IMPORT / COPY / PASTE.
// Export and COPY use a single progress lane; Import and PASTE use dual lanes.
// Detail placement: EXPORT / PASTE keep detail above progress; COPY / IMPORT put
// detail below progress so top-to-bottom reading matches the transfer flow.
class BankTransferProgressDialog : public juce::Component
{
public:
    enum class ContentLayout
    {
        SingleLane, // EXPORT / COPY
        DualLane    // IMPORT / PASTE
    };

    static constexpr int kDesignWidth = 420;
    // Content height includes action-footer separator (24 + 1px rule + 24) above Cancel.
    static constexpr int kDesignHeightSingle = 209;
    // Dual = single + section gap (1em) + phase lane (label + 0.5em + bar).
    static constexpr int kDesignHeightDual = 261;

    explicit BankTransferProgressDialog(TSS::ISkin& skin);
    ~BankTransferProgressDialog() override;

    // detail is typically an absolute folder path (may be empty).
    struct PrepareForShowArgs
    {
        juce::String title;
        juce::String message;
        juce::String detail;
        int totalSteps = 1;
        std::function<void()> onCancelRequested;
        ContentLayout layout = ContentLayout::DualLane;
    };

    void prepareForShow(PrepareForShowArgs args);

    // Activates the secondary lane (import/paste write / restore) and starts its progress at 0.
    void beginSecondaryPhase(const juce::String& message, int totalSteps);

    // Updates the active lane (primary until secondary is active, then secondary).
    void setProgress(int completedSteps);
    void setMessage(const juce::String& message);
    void setDetail(const juce::String& detail);
    void setCancelEnabled(bool enabled);

    void setSkin(TSS::ISkin& skin);
    void setUiScale(float uiScale);

    void paint(juce::Graphics& g) override;
    void resized() override;
    bool keyPressed(const juce::KeyPress& key) override;

private:
    int getBorderThickness() const;
    int getDesignContentHeight() const noexcept;
    juce::Rectangle<int> getDialogBounds() const;
    void paintProgressBar(juce::Graphics& g,
                          juce::Rectangle<int> bounds,
                          float fraction,
                          bool enabled) const;
    // Shared detail block: folder ops keep label + path on two lines; COPY/PASTE use one line.
    // When above progress: trailing 1em gap. When below: leading 1em gap.
    juce::Rectangle<int> paintDetailHeader(juce::Graphics& g,
                                           juce::Rectangle<int> body,
                                           const juce::Font& bodyFont,
                                           bool belowProgress) const;
    struct PhaseLanePaintArgs
    {
        juce::Rectangle<int>& body;
        const juce::Font& bodyFont;
        const juce::String& message;
        int completedSteps = 0;
        int totalSteps = 1;
        bool enabled = true;
    };

    void paintPhaseLane(juce::Graphics& g, const PhaseLanePaintArgs& args) const;
    void paintSingleLaneBody(juce::Graphics& g, juce::Rectangle<int> body, const juce::Font& bodyFont) const;
    void paintDualLaneBody(juce::Graphics& g, juce::Rectangle<int> body, const juce::Font& bodyFont) const;
    void applyOperationPresentationFromTitle();

    std::function<void()> onCancelRequested_;
    TSS::ISkin* skin_;
    ContentLayout contentLayout_ = ContentLayout::DualLane;
    // COPY / IMPORT: progress first, then detail (read then destination/source).
    // EXPORT / PASTE: detail first, then progress (matches their flow).
    bool detailBelowProgress_ = false;
    // COPY / PASTE: "Source: Clipboard" on one line. EXPORT / IMPORT: label then path.
    bool detailInline_ = false;
    juce::String title_;
    juce::String headerLabel_;
    juce::String detail_;

    juce::String primaryMessage_;
    int primaryTotalSteps_ = 1;
    int primaryCompletedSteps_ = 0;

    juce::String secondaryMessage_;
    int secondaryTotalSteps_ = 1;
    int secondaryCompletedSteps_ = 0;
    bool secondaryLaneActive_ = false;

    float uiScale_ = 1.0f;
    std::unique_ptr<TSS::Button> cancelButton_;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(BankTransferProgressDialog)
};
