// Extracted from HeaderPanel.cpp for modular maintenance.
// Horizontal packet layout for header controls.

#include "HeaderPanel.h"

#include "GUI/Layout/Design/DesignPanels.h"
#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"

namespace
{
    /** Same X as the SharedPanel outer left (Matrix Modulation column), matching BodyPanel. */
    int matrixModulationPanelLeftX(float uiScale) noexcept
    {
        using namespace TSS::Design::Panels::Body;
        return TSS::ScaledLayout::scaledInt(static_cast<float>(PatchEditSection::kPanelWidth), uiScale)
             + TSS::ScaledLayout::scaledInt(static_cast<float>(kInterColumnGap), uiScale);
    }

    void scaleCartoucheStroke(int designThickness,
                              float scaleFactor,
                              int strokeBaselinePx,
                              int& strokePx,
                              int& outwardPx) noexcept
    {
        strokePx = juce::jmax(1, TSS::ScaledLayout::scaledInt(static_cast<float>(designThickness), scaleFactor));
        outwardPx = juce::jmax(0, strokePx - strokeBaselinePx);
    }

    struct HeaderLayoutMetrics
    {
        float gap = 0.0f;
        float packetExternalGap = 0.0f;
        float controlHeight = 0.0f;
        int controlHeightPx = 0;
        int controlY = 0;
        int ledY = 0;
        float ledSize = 0.0f;
        int ledSizePx = 0;
        float fromKeyboardLabelWidth = 0.0f;
        float fromSynthLabelWidth = 0.0f;
        float toSynthLabelWidth = 0.0f;
        float midiCartoucheWidth = 0.0f;
        float midiCartoucheInset = 0.0f;
        int midiCartoucheStrokePx = 1;
        int midiCartoucheStrokeOutwardPx = 0;
        float audioCartoucheWidth = 0.0f;
        float audioCartoucheInset = 0.0f;
        int audioCartoucheStrokePx = 1;
        int audioCartoucheStrokeOutwardPx = 0;
        float editCartoucheWidth = 0.0f;
        float editCartoucheInset = 0.0f;
        int editCartoucheStrokePx = 1;
        int editCartoucheStrokeOutwardPx = 0;
        float cartoucheBadgeContentGap = 0.0f;
        float cartoucheGap = 0.0f;
        float midiLabelToNextLedGap = 0.0f;
        float midiToPanicGap = 0.0f;
        float inputGainLabelWidth = 0.0f;
        float inputGainLabelToSliderGap = 0.0f;
        float inputGainSliderWidth = 0.0f;
        float peakIndicatorWidth = 0.0f;
        float panicButtonWidth = 0.0f;
        float undoButtonWidth = 0.0f;
        float redoButtonWidth = 0.0f;
        int logoX = 0;
        int logoY = 0;
        int logoWidth = 0;
        int logoHeight = 0;
        float contentStartX = 0.0f;

        static HeaderLayoutMetrics make(const HeaderPanelDimensions& dimensions,
                                        float uiScale,
                                        juce::Rectangle<int> bounds)
        {
            HeaderLayoutMetrics m;
            const float sf = uiScale;
            m.gap = static_cast<float>(dimensions.gap) * sf;
            m.packetExternalGap = static_cast<float>(dimensions.packetExternalGap) * sf;
            m.controlHeight = static_cast<float>(dimensions.controlHeight) * sf;
            m.controlHeightPx = juce::roundToInt(m.controlHeight);

            const int contentYOffset = TSS::ScaledLayout::scaledInt(
                static_cast<float>(dimensions.contentVerticalOffset), sf);
            m.controlY = bounds.getY() + (bounds.getHeight() - m.controlHeightPx) / 2 + contentYOffset;

            m.fromKeyboardLabelWidth = static_cast<float>(dimensions.fromKeyboardLabelWidth) * sf;
            m.fromSynthLabelWidth = static_cast<float>(dimensions.fromSynthLabelWidth) * sf;
            m.toSynthLabelWidth = static_cast<float>(dimensions.toSynthLabelWidth) * sf;
            m.midiCartoucheWidth = static_cast<float>(dimensions.midiCartoucheWidth) * sf;
            m.midiCartoucheInset = static_cast<float>(dimensions.midiCartoucheInset) * sf;
            m.audioCartoucheWidth = static_cast<float>(dimensions.audioCartoucheWidth) * sf;
            m.audioCartoucheInset = static_cast<float>(dimensions.audioCartoucheInset) * sf;
            m.editCartoucheWidth = static_cast<float>(dimensions.editCartoucheWidth) * sf;
            m.editCartoucheInset = static_cast<float>(dimensions.editCartoucheInset) * sf;
            m.cartoucheBadgeContentGap = static_cast<float>(dimensions.cartoucheBadgeContentGap) * sf;
            m.cartoucheGap = static_cast<float>(dimensions.cartoucheGap) * sf;
            m.midiLabelToNextLedGap = static_cast<float>(dimensions.midiLabelToNextLedGap) * sf;
            m.midiToPanicGap = static_cast<float>(dimensions.midiToPanicGap) * sf;

            // Layout air was calibrated to a 1 px design stroke; extra thickness grows outward.
            constexpr float kStrokeBaselineDesign = 1.0f;
            const int strokeBaselinePx = juce::jmax(1, TSS::ScaledLayout::scaledInt(kStrokeBaselineDesign, sf));
            scaleCartoucheStroke(dimensions.midiCartoucheStrokeThickness, sf, strokeBaselinePx,
                                 m.midiCartoucheStrokePx, m.midiCartoucheStrokeOutwardPx);
            scaleCartoucheStroke(dimensions.audioCartoucheStrokeThickness, sf, strokeBaselinePx,
                                 m.audioCartoucheStrokePx, m.audioCartoucheStrokeOutwardPx);
            scaleCartoucheStroke(dimensions.editCartoucheStrokeThickness, sf, strokeBaselinePx,
                                 m.editCartoucheStrokePx, m.editCartoucheStrokeOutwardPx);

            m.inputGainLabelWidth = static_cast<float>(dimensions.inputGainLabelWidth) * sf;
            m.inputGainLabelToSliderGap = static_cast<float>(dimensions.inputGainLabelToSliderGap) * sf;
            m.inputGainSliderWidth = static_cast<float>(dimensions.inputGainSliderWidth) * sf;
            m.peakIndicatorWidth = static_cast<float>(dimensions.peakIndicatorWidth) * sf;
            m.panicButtonWidth = static_cast<float>(dimensions.panicButtonWidth) * sf;
            m.undoButtonWidth = static_cast<float>(dimensions.undoButtonWidth) * sf;
            m.redoButtonWidth = static_cast<float>(dimensions.redoButtonWidth) * sf;
            m.ledSize = static_cast<float>(dimensions.ledSize) * sf;
            m.ledSizePx = juce::roundToInt(m.ledSize);
            m.ledY = bounds.getY() + (bounds.getHeight() - m.ledSizePx) / 2 + contentYOffset;

            const float leftPadding = static_cast<float>(dimensions.leftPadding) * sf;
            const float logoGapAfter = static_cast<float>(dimensions.logoGapAfter) * sf;
            m.logoWidth = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.logoWidth), sf);
            m.logoHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.logoHeight), sf);
            m.logoX = juce::roundToInt(static_cast<float>(bounds.getX()) + leftPadding);
            // Centre the logo box in the header (not relative to the control row / unscaled nudge).
            m.logoY = bounds.getY() + (bounds.getHeight() - m.logoHeight) / 2;
            m.contentStartX = static_cast<float>(m.logoX + m.logoWidth) + logoGapAfter;
            return m;
        }
    };

    class PacketPlacer
    {
    public:
        PacketPlacer(float startX, const HeaderLayoutMetrics& metrics, float uiScale)
            : x_(startX)
            , y_(metrics.controlY)
            , h_(metrics.controlHeightPx)
            , ledY_(metrics.ledY)
            , ledH_(metrics.ledSizePx)
            , gap_(metrics.gap)
            , packetExternalGap_(metrics.packetExternalGap)
            , ledSize_(metrics.ledSize)
            , uiScale_(uiScale)
        {
        }

        void placeLabel(TSS::Label& label, float labelWidth, float followingGap = -1.0f)
        {
            label.setBounds(juce::roundToInt(x_), y_, juce::roundToInt(labelWidth), h_);
            label.setUiScale(uiScale_);
            const float gapAfter = juce::approximatelyEqual(followingGap, -1.0f) ? gap_ : followingGap;
            x_ += labelWidth + gapAfter;
        }

        void placeLed(TSS::Led& led)
        {
            led.setBounds(juce::roundToInt(x_), ledY_, juce::roundToInt(ledSize_), ledH_);
            led.setUiScale(uiScale_);
            x_ += ledSize_ + gap_;
        }

        void placeSlider(TSS::Slider& slider, float sliderWidth, float followingGap = -1.0f)
        {
            slider.setBounds(juce::roundToInt(x_), y_, juce::roundToInt(sliderWidth), h_);
            slider.setUiScale(uiScale_);
            const float gapAfter = juce::approximatelyEqual(followingGap, -1.0f) ? gap_ : followingGap;
            x_ += sliderWidth + gapAfter;
        }

        void placePeak(TSS::PeakIndicator& peak, float peakWidth, float followingGap = -1.0f)
        {
            peak.setBounds(juce::roundToInt(x_), y_, juce::roundToInt(peakWidth), h_);
            peak.setUiScale(uiScale_);
            const float gapAfter = juce::approximatelyEqual(followingGap, -1.0f) ? gap_ : followingGap;
            x_ += peakWidth + gapAfter;
        }

        void placeButton(TSS::Button& button, float buttonWidth, float followingGap = -1.0f)
        {
            button.setBounds(juce::roundToInt(x_), y_, juce::roundToInt(buttonWidth), h_);
            button.setUiScale(uiScale_);
            const float gapAfter = juce::approximatelyEqual(followingGap, -1.0f) ? gap_ : followingGap;
            x_ += buttonWidth + gapAfter;
        }

        [[nodiscard]] float x() const noexcept { return x_; }
        [[nodiscard]] int y() const noexcept { return y_; }
        [[nodiscard]] int h() const noexcept { return h_; }

        void endPacket()
        {
            x_ += packetExternalGap_ - gap_;
        }

    private:
        float x_;
        int y_;
        int h_;
        int ledY_;
        int ledH_;
        float gap_;
        float packetExternalGap_;
        float ledSize_;
        float uiScale_;
    };

    struct CartoucheBounds
    {
        juce::Rectangle<int> badge;
        juce::Rectangle<int> frame;
        int strokePx = 1;
        float nextX = 0.0f;
    };

    struct MidiCartoucheWidgets
    {
        TSS::Led* keyboardLed = nullptr;
        TSS::Label* keyboardLabel = nullptr;
        TSS::Led* synthFromLed = nullptr;
        TSS::Label* synthFromLabel = nullptr;
        TSS::Led* synthToLed = nullptr;
        TSS::Label* synthToLabel = nullptr;
        TSS::Button* panic = nullptr;
    };

    CartoucheBounds layoutMidiCartouche(const HeaderLayoutMetrics& metrics,
                                        float uiScale,
                                        float startX,
                                        const MidiCartoucheWidgets& widgets)
    {
        const int badgeX = juce::roundToInt(startX);
        const int badgeW = juce::roundToInt(metrics.midiCartoucheWidth);
        const int verticalInset = juce::roundToInt(metrics.midiCartoucheInset);
        const int badgeContentGap = juce::roundToInt(metrics.cartoucheBadgeContentGap);
        const int outward = metrics.midiCartoucheStrokeOutwardPx;
        const int frameY = metrics.controlY - verticalInset - outward;
        const int frameH = metrics.controlHeightPx + 2 * verticalInset + 2 * outward;

        PacketPlacer placer(static_cast<float>(badgeX + badgeW + badgeContentGap), metrics, uiScale);
        placer.placeLed(*widgets.keyboardLed);
        placer.placeLabel(*widgets.keyboardLabel,
                          metrics.fromKeyboardLabelWidth,
                          metrics.midiLabelToNextLedGap);
        placer.placeLed(*widgets.synthFromLed);
        placer.placeLabel(*widgets.synthFromLabel,
                          metrics.fromSynthLabelWidth,
                          metrics.midiLabelToNextLedGap);
        placer.placeLed(*widgets.synthToLed);
        placer.placeLabel(*widgets.synthToLabel, metrics.toSynthLabelWidth, metrics.midiToPanicGap);
        placer.placeButton(*widgets.panic, metrics.panicButtonWidth, 0.0f);

        const int frameRight = juce::roundToInt(placer.x()) + badgeContentGap
                               + metrics.midiCartoucheStrokePx;
        return {
            .badge = { badgeX, frameY, badgeW, frameH },
            .frame = { badgeX, frameY, juce::jmax(badgeW, frameRight - badgeX), frameH },
            .strokePx = metrics.midiCartoucheStrokePx,
            .nextX = static_cast<float>(frameRight)
        };
    }

    struct AudioCartoucheWidgets
    {
        TSS::Label* gainLabel = nullptr;
        TSS::Slider* gainSlider = nullptr;
        TSS::PeakIndicator* peak = nullptr;
    };

    CartoucheBounds layoutAudioCartouche(const HeaderLayoutMetrics& metrics,
                                         float uiScale,
                                         float startX,
                                         const AudioCartoucheWidgets& widgets)
    {
        const int badgeX = juce::roundToInt(startX);
        const int badgeW = juce::roundToInt(metrics.audioCartoucheWidth);
        const int verticalInset = juce::roundToInt(metrics.audioCartoucheInset);
        const int badgeContentGap = juce::roundToInt(metrics.cartoucheBadgeContentGap);
        const int outward = metrics.audioCartoucheStrokeOutwardPx;
        const int frameY = metrics.controlY - verticalInset - outward;
        const int frameH = metrics.controlHeightPx + 2 * verticalInset + 2 * outward;

        PacketPlacer placer(static_cast<float>(badgeX + badgeW + badgeContentGap), metrics, uiScale);
        placer.placeLabel(*widgets.gainLabel, metrics.inputGainLabelWidth, metrics.inputGainLabelToSliderGap);
        placer.placeSlider(*widgets.gainSlider, metrics.inputGainSliderWidth, metrics.cartoucheBadgeContentGap);
        placer.placePeak(*widgets.peak, metrics.peakIndicatorWidth, 0.0f);

        const int frameRight = juce::roundToInt(placer.x()) + badgeContentGap
                               + metrics.audioCartoucheStrokePx;
        return {
            .badge = { badgeX, frameY, badgeW, frameH },
            .frame = { badgeX, frameY, juce::jmax(badgeW, frameRight - badgeX), frameH },
            .strokePx = metrics.audioCartoucheStrokePx,
            .nextX = static_cast<float>(frameRight)
        };
    }

    struct EditCartoucheWidgets
    {
        TSS::Button* undo = nullptr;
        TSS::Button* redo = nullptr;
    };

    CartoucheBounds layoutEditCartouche(const HeaderLayoutMetrics& metrics,
                                        float uiScale,
                                        float startX,
                                        const EditCartoucheWidgets& widgets)
    {
        const int badgeX = juce::roundToInt(startX);
        const int badgeW = juce::roundToInt(metrics.editCartoucheWidth);
        const int verticalInset = juce::roundToInt(metrics.editCartoucheInset);
        const int badgeContentGap = juce::roundToInt(metrics.cartoucheBadgeContentGap);
        const int outward = metrics.editCartoucheStrokeOutwardPx;
        const int frameY = metrics.controlY - verticalInset - outward;
        const int frameH = metrics.controlHeightPx + 2 * verticalInset + 2 * outward;

        PacketPlacer placer(static_cast<float>(badgeX + badgeW + badgeContentGap), metrics, uiScale);
        placer.placeButton(*widgets.undo, metrics.undoButtonWidth);
        placer.placeButton(*widgets.redo, metrics.redoButtonWidth, 0.0f);

        const int frameRight = juce::roundToInt(placer.x()) + badgeContentGap
                               + metrics.editCartoucheStrokePx;
        return {
            .badge = { badgeX, frameY, badgeW, frameH },
            .frame = { badgeX, frameY, juce::jmax(badgeW, frameRight - badgeX), frameH },
            .strokePx = metrics.editCartoucheStrokePx,
            .nextX = static_cast<float>(frameRight)
        };
    }
}

void HeaderPanel::resized()
{
    const auto metrics = HeaderLayoutMetrics::make(dimensions_, uiScale_, getLocalBounds());

    logo_.setBounds(metrics.logoX,
                    metrics.logoY,
                    metrics.logoWidth,
                    metrics.logoHeight);
    logo_.setUiScale(uiScale_);

    const EditCartoucheWidgets editWidgets{ &undoButton_, &redoButton_ };
    const MidiCartoucheWidgets midiWidgets{ &instrumentActivityLed_,
                                            &keyboardFromLabel_,
                                            &editorActivityLed_,
                                            &midiFromLabel_,
                                            &midiToActivityLed_,
                                            &midiToLabel_,
                                            &panicButton_ };

    // EDIT → MIDI → AUDIO: shift the cluster so AUDIO's left edge matches Matrix Modulation
    // (same scale tokens as BodyPanel + SharedPanel). Probe widths at x=0, then place for real.
    const auto editProbe = layoutEditCartouche(metrics, uiScale_, 0.0f, editWidgets);
    const auto midiProbe = layoutMidiCartouche(
        metrics, uiScale_, editProbe.nextX + metrics.cartoucheGap, midiWidgets);
    const float audioStartIfClusterAtZero = midiProbe.nextX + metrics.cartoucheGap;
    const float alignedStartX = static_cast<float>(matrixModulationPanelLeftX(uiScale_))
                                - audioStartIfClusterAtZero
                                + static_cast<float>(TSS::ScaledLayout::scaledInt(
                                      static_cast<float>(TSS::Design::Panels::Header::kCartoucheClusterNudgeX),
                                      uiScale_));
    const float clusterStartX = juce::jmax(metrics.contentStartX, alignedStartX);

    const auto edit = layoutEditCartouche(metrics, uiScale_, clusterStartX, editWidgets);
    editCartoucheBadgeBounds_ = edit.badge;
    editCartoucheFrameBounds_ = edit.frame;
    editCartoucheStrokePx_ = edit.strokePx;

    const auto midi = layoutMidiCartouche(
        metrics, uiScale_, edit.nextX + metrics.cartoucheGap, midiWidgets);
    midiCartoucheBadgeBounds_ = midi.badge;
    midiCartoucheFrameBounds_ = midi.frame;
    midiCartoucheStrokePx_ = midi.strokePx;

    audioCartoucheBadgeBounds_ = {};
    audioCartoucheFrameBounds_ = {};
    audioCartoucheStrokePx_ = 1;
    if (! isPluginMode_)
    {
        const auto audio = layoutAudioCartouche(
            metrics,
            uiScale_,
            midi.nextX + metrics.cartoucheGap,
            AudioCartoucheWidgets{ &inputGainLabel_, &inputGainSlider_, &peakIndicator_ });
        audioCartoucheBadgeBounds_ = audio.badge;
        audioCartoucheFrameBounds_ = audio.frame;
        audioCartoucheStrokePx_ = audio.strokePx;
    }
}
