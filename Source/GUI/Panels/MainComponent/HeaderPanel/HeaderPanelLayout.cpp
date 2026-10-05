// Extracted from HeaderPanel.cpp for modular maintenance.
// Horizontal packet layout for header controls.

#include "HeaderPanel.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/Label.h"
#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"

namespace
{
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
        float inputGainLabelWidth = 0.0f;
        float inputGainLabelToSliderGap = 0.0f;
        float inputGainSliderWidth = 0.0f;
        float peakIndicatorWidth = 0.0f;
        float undoButtonWidth = 0.0f;
        float redoButtonWidth = 0.0f;
        int logoX = 0;
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
            // Layout air was calibrated to a 1 px design stroke; extra thickness grows outward.
            constexpr float kStrokeBaselineDesign = 1.0f;
            const int strokeBaselinePx = juce::jmax(1, TSS::ScaledLayout::scaledInt(kStrokeBaselineDesign, sf));

            m.midiCartoucheStrokePx = juce::jmax(
                1, TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.midiCartoucheStrokeThickness), sf));
            m.midiCartoucheStrokeOutwardPx = juce::jmax(0, m.midiCartoucheStrokePx - strokeBaselinePx);
            m.audioCartoucheStrokePx = juce::jmax(
                1, TSS::ScaledLayout::scaledInt(static_cast<float>(dimensions.audioCartoucheStrokeThickness), sf));
            m.audioCartoucheStrokeOutwardPx = juce::jmax(0, m.audioCartoucheStrokePx - strokeBaselinePx);

            m.inputGainLabelWidth = static_cast<float>(dimensions.inputGainLabelWidth) * sf;
            m.inputGainLabelToSliderGap = static_cast<float>(dimensions.inputGainLabelToSliderGap) * sf;
            m.inputGainSliderWidth = static_cast<float>(dimensions.inputGainSliderWidth) * sf;
            m.peakIndicatorWidth = static_cast<float>(dimensions.peakIndicatorWidth) * sf;
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

        void placeSlider(TSS::Slider& slider, float sliderWidth)
        {
            slider.setBounds(juce::roundToInt(x_), y_, juce::roundToInt(sliderWidth), h_);
            slider.setUiScale(uiScale_);
            x_ += sliderWidth + gap_;
        }

        void placePeak(TSS::PeakIndicator& peak, float peakWidth)
        {
            peak.setBounds(juce::roundToInt(x_), y_, juce::roundToInt(peakWidth), h_);
            peak.setUiScale(uiScale_);
            x_ += peakWidth + gap_;
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

    struct HeaderActionButtonCluster
    {
        const HeaderPanelDimensions& dimensions;
        const HeaderLayoutMetrics& metrics;
        float uiScale;
        int boundsRight;
        TSS::Button& undo;
        TSS::Button& redo;
        TSS::Button& panic;
    };

    void placeActionButtonsFromRight(const HeaderActionButtonCluster& cluster)
    {
        const int panicW = juce::roundToInt(static_cast<float>(cluster.dimensions.panicButtonWidth) * cluster.uiScale);
        const int rightPad = juce::roundToInt(static_cast<float>(cluster.dimensions.rightPadding) * cluster.uiScale);
        const int redoToPanicGap = juce::roundToInt(static_cast<float>(cluster.dimensions.redoToPanicGap) * cluster.uiScale);
        const int undoRedoGap = juce::roundToInt(cluster.metrics.gap);

        const int redoW = juce::roundToInt(cluster.metrics.redoButtonWidth);
        const int undoW = juce::roundToInt(cluster.metrics.undoButtonWidth);

        const int panicX = cluster.boundsRight - rightPad - panicW;
        const int redoX = panicX - redoToPanicGap - redoW;
        const int undoX = redoX - undoRedoGap - undoW;

        cluster.panic.setBounds(panicX, cluster.metrics.controlY, panicW, cluster.metrics.controlHeightPx);
        cluster.redo.setBounds(redoX, cluster.metrics.controlY, redoW, cluster.metrics.controlHeightPx);
        cluster.undo.setBounds(undoX, cluster.metrics.controlY, undoW, cluster.metrics.controlHeightPx);

        cluster.panic.setUiScale(cluster.uiScale);
        cluster.redo.setUiScale(cluster.uiScale);
        cluster.undo.setUiScale(cluster.uiScale);
    }

    struct CartoucheBounds
    {
        juce::Rectangle<int> badge;
        juce::Rectangle<int> frame;
        int strokePx = 1;
        float nextX = 0.0f;
    };

    struct MidiMonitoringWidgets
    {
        TSS::Led* keyboardLed = nullptr;
        TSS::Label* keyboardLabel = nullptr;
        TSS::Led* synthFromLed = nullptr;
        TSS::Label* synthFromLabel = nullptr;
        TSS::Led* synthToLed = nullptr;
        TSS::Label* synthToLabel = nullptr;
    };

    CartoucheBounds layoutMidiCartouche(const HeaderLayoutMetrics& metrics,
                                        float uiScale,
                                        const MidiMonitoringWidgets& widgets)
    {
        const int badgeX = juce::roundToInt(metrics.contentStartX);
        const int badgeW = juce::roundToInt(metrics.midiCartoucheWidth);
        const int inset = juce::roundToInt(metrics.midiCartoucheInset);
        const int outward = metrics.midiCartoucheStrokeOutwardPx;
        const int frameY = metrics.controlY - inset - outward;
        const int frameH = metrics.controlHeightPx + 2 * inset + 2 * outward;

        PacketPlacer placer(static_cast<float>(badgeX + badgeW + inset), metrics, uiScale);
        placer.placeLed(*widgets.keyboardLed);
        placer.placeLabel(*widgets.keyboardLabel, metrics.fromKeyboardLabelWidth);
        placer.endPacket();
        placer.placeLed(*widgets.synthFromLed);
        placer.placeLabel(*widgets.synthFromLabel, metrics.fromSynthLabelWidth);
        placer.endPacket();
        placer.placeLed(*widgets.synthToLed);
        placer.placeLabel(*widgets.synthToLabel, metrics.toSynthLabelWidth);

        const int frameRight = juce::roundToInt(placer.x()) + outward;
        return {
            .badge = { badgeX, frameY, badgeW, frameH },
            .frame = { badgeX, frameY, juce::jmax(badgeW, frameRight - badgeX), frameH },
            .strokePx = metrics.midiCartoucheStrokePx,
            .nextX = placer.x()
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
        const int inset = juce::roundToInt(metrics.audioCartoucheInset);
        const int outward = metrics.audioCartoucheStrokeOutwardPx;
        const int frameY = metrics.controlY - inset - outward;
        const int frameH = metrics.controlHeightPx + 2 * inset + 2 * outward;

        PacketPlacer placer(static_cast<float>(badgeX + badgeW + inset), metrics, uiScale);
        placer.placeLabel(*widgets.gainLabel, metrics.inputGainLabelWidth, metrics.inputGainLabelToSliderGap);
        placer.placeSlider(*widgets.gainSlider, metrics.inputGainSliderWidth);
        placer.placePeak(*widgets.peak, metrics.peakIndicatorWidth);

        const int frameRight = juce::roundToInt(placer.x()) + outward;
        return {
            .badge = { badgeX, frameY, badgeW, frameH },
            .frame = { badgeX, frameY, juce::jmax(badgeW, frameRight - badgeX), frameH },
            .strokePx = metrics.audioCartoucheStrokePx,
            .nextX = placer.x()
        };
    }
}

void HeaderPanel::resized()
{
    const auto metrics = HeaderLayoutMetrics::make(dimensions_, uiScale_, getLocalBounds());

    logo_.setBounds(metrics.logoX,
                    metrics.controlY + dimensions_.logoVerticalOffset,
                    metrics.logoWidth,
                    metrics.logoHeight);
    logo_.setUiScale(uiScale_);

    const auto midi = layoutMidiCartouche(
        metrics,
        uiScale_,
        MidiMonitoringWidgets{ &instrumentActivityLed_,
                               &keyboardFromLabel_,
                               &editorActivityLed_,
                               &midiFromLabel_,
                               &midiToActivityLed_,
                               &midiToLabel_ });
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
            midi.nextX,
            AudioCartoucheWidgets{ &inputGainLabel_, &inputGainSlider_, &peakIndicator_ });
        audioCartoucheBadgeBounds_ = audio.badge;
        audioCartoucheFrameBounds_ = audio.frame;
        audioCartoucheStrokePx_ = audio.strokePx;
    }

    placeActionButtonsFromRight({ dimensions_,
                                  metrics,
                                  uiScale_,
                                  getLocalBounds().getRight(),
                                  undoButton_,
                                  redoButton_,
                                  panicButton_ });
}
