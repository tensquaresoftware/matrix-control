#include "SettingsAudioPage.h"

#include "GUI/Layout/ScaledLayout.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Widgets/ComboBox.h"

namespace
{
    struct AudioPageLayoutMetrics
    {
        int rowGap = 0;
        int labelWidth = 0;
        int controlHeight = 0;
        int comboWidth = 0;
        int synthComboWidth = 0;
        int peakWidth = 0;
        int peakGap = 0;
        int testWidth = 0;
        float uiScale = 1.0f;

        static AudioPageLayoutMetrics make(float uiScale)
        {
            return {
                .rowGap = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kRowGap), uiScale),
                .labelWidth = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kLabelWidth), uiScale),
                .controlHeight = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kControlHeight), uiScale),
                .comboWidth = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kControlColumnWidth), uiScale),
                .synthComboWidth = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kSynthFromComboWidth), uiScale),
                .peakWidth = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kPeakWidth), uiScale),
                .peakGap = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kPeakGap), uiScale),
                .testWidth = TSS::ScaledLayout::scaledInt(
                    static_cast<float>(SettingsShellMetrics::kControlColumnWidth), uiScale),
                .uiScale = uiScale,
            };
        }
    };

    struct LabeledControl
    {
        TSS::Label* label = nullptr;
        juce::Component* control = nullptr;
        int controlWidth = 0;
    };

    void placeLabeledRow(juce::Rectangle<int>& bounds,
                         const AudioPageLayoutMetrics& metrics,
                         const LabeledControl& rowControl)
    {
        auto row = bounds.removeFromTop(metrics.controlHeight);
        rowControl.label->setBounds(row.getX(), row.getY(), metrics.labelWidth, metrics.controlHeight);
        rowControl.label->setUiScale(metrics.uiScale);
        rowControl.control->setBounds(row.getX() + metrics.labelWidth,
                                      row.getY(),
                                      rowControl.controlWidth,
                                      metrics.controlHeight);
        if (auto* combo = dynamic_cast<TSS::ComboBox*>(rowControl.control))
            combo->setUiScale(metrics.uiScale);
        bounds.removeFromTop(metrics.rowGap);
    }

    void placeChannelGroupRow(juce::Rectangle<int>& bounds,
                              const AudioPageLayoutMetrics& metrics,
                              TSS::Label& label,
                              TSS::RadioButtonGroup& group)
    {
        const int groupHeight = juce::jmax(metrics.controlHeight,
                                           group.getPreferredHeight(metrics.comboWidth));
        auto row = bounds.removeFromTop(groupHeight);
        label.setBounds(row.getX(), row.getY(), metrics.labelWidth, metrics.controlHeight);
        label.setUiScale(metrics.uiScale);
        group.setBounds(row.getX() + metrics.labelWidth, row.getY(), metrics.comboWidth, groupHeight);
        group.setUiScale(metrics.uiScale);
        bounds.removeFromTop(metrics.rowGap);
    }
}

void SettingsAudioPage::resized()
{
    const auto metrics = AudioPageLayoutMetrics::make(uiScale_);
    auto bounds = getLocalBounds();

    placeLabeledRow(bounds, metrics, { driverTypeLabel_.get(), driverTypeCombo_.get(), metrics.comboWidth });
    placeLabeledRow(bounds, metrics, { inputDeviceLabel_.get(), inputDeviceCombo_.get(), metrics.comboWidth });
    placeLabeledRow(bounds, metrics, { outputDeviceLabel_.get(), outputDeviceCombo_.get(), metrics.comboWidth });
    placeLabeledRow(bounds, metrics, { sampleRateLabel_.get(), sampleRateCombo_.get(), metrics.comboWidth });
    placeLabeledRow(bounds, metrics, { bufferSizeLabel_.get(), bufferSizeCombo_.get(), metrics.comboWidth });
    bounds.removeFromTop(metrics.controlHeight + metrics.rowGap);

    placeChannelGroupRow(bounds, metrics, *inputChannelsLabel_, *inputChannelsGroup_);

    {
        auto row = bounds.removeFromTop(metrics.controlHeight);
        synthFromLabel_->setBounds(row.getX(), row.getY(), metrics.labelWidth, metrics.controlHeight);
        synthFromLabel_->setUiScale(uiScale_);
        const int controlX = row.getX() + metrics.labelWidth;
        synthFromCombo_->setBounds(controlX, row.getY(), metrics.synthComboWidth, metrics.controlHeight);
        synthFromCombo_->setUiScale(uiScale_);
        peakIndicator_->setBounds(controlX + metrics.synthComboWidth + metrics.peakGap,
                                  row.getY(),
                                  metrics.peakWidth,
                                  metrics.controlHeight);
        peakIndicator_->setUiScale(uiScale_);
        bounds.removeFromTop(metrics.rowGap);
    }

    bounds.removeFromTop(metrics.controlHeight + metrics.rowGap);
    placeChannelGroupRow(bounds, metrics, *outputChannelsLabel_, *outputChannelsGroup_);

    auto row = bounds.removeFromTop(metrics.controlHeight);
    playTestToneButton_->setBounds(row.getX() + metrics.labelWidth,
                                   row.getY(),
                                   metrics.testWidth,
                                   metrics.controlHeight);
    playTestToneButton_->setUiScale(uiScale_);
}
