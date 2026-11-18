#pragma once

#include "ColourChartTones.h"
#include "SkinColoursCommon.h"

namespace TSS::SkinColours::Tones
{
    inline constexpr int kZoneCount = 197;

    /** Shared zone order for all dark TONES (Trials CHERRY mappings).
        Values are palette stop indices into ColourChartTones::kRamps[tone]. */
    inline constexpr int kSharedStepIndices[kZoneCount] = {
        1, 1, 15, 2, 1, 15, 17, 20, 16, 0, 10, 16, 19, 20, 17, 14,
        16, 16, 17, 20, 2, 14, 16, 17, 7, 19, 14, 17, 7, 0, 0, 0,
        0, 1, 19, 4, 4, 4, 17, 20, 20, 20, 7, 20, 0, 20, 3, 5,
        1, 4, 4, 8, 17, 7, 4, 4, 4, 4, 16, 16, 16, 17, 7, 7,
        7, 7, 3, 3, 3, 0, 2, 2, 2, 2, 8, 8, 8, 7, 6, 6,
        6, 6, 4, 4, 4, 4, 20, 20, 20, 20, 20, 20, 20, 20, 16, 7,
        18, 7, 3, 2, 8, 6, 3, 3, 0, 4, 1, 1, 4, 4, 4, 4,
        4, 4, 16, 16, 17, 7, 7, 7, 20, 20, 20, 4, 4, 4, 8, 8,
        7, 7, 7, 7, 3, 3, 0, 17, 17, 18, 17, 17, 7, 17, 17, 18,
        1, 1, 0, 8, 8, 12, 17, 17, 7, 4, 4, 4, 0, 1, 4, 4,
        17, 20, 4, 4, 20, 20, 0, 4, 17, 0, 4, 17, 0, 4, 17, 11,
        0, 9, 17, 18, 0, 13, 6, 17, 19, 0, 0, 17, 18, 4, 4, 0,
        0, 17, 18, 4, 4
    };

    struct StepOverride
    {
        int zoneIndex;
        int stepIndex;
    };

    /** PETROL differs on five intentional ranks vs the shared step table. */
    inline constexpr StepOverride kPetrolStepOverrides[] = {
        { 11, 13 },
        { 15, 13 },
        { 21, 13 },
        { 26, 13 },
        { 181, 14 }
    };

    inline constexpr SkinColourKey kZoneKeys[kZoneCount] = {
        skinColourKey(SkinObjectType::kWindowSurface, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kHeaderPanel, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kHeaderPanel, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kBodyPanel, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kFooterPanel, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kFooterPanel, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kChromeText, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kChromeText, SkinColourProperty::kText, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kChromeText, SkinColourProperty::kText, SkinColourState::kEmphasis),
        skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kBackground, SkinColourState::kError),
        skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kInfo),
        skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kWarning),
        skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kText, SkinColourState::kError),
        skinColourKey(SkinObjectType::kFooterMessage, SkinColourProperty::kGlyph, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kLineBlue),
        skinColourKey(SkinObjectType::kSectionHeader, SkinColourProperty::kLineOrange),
        skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kBackground, SkinColourState::kEmphasis),
        skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kText, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kText, SkinColourState::kEmphasis),
        skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kLineBlue),
        skinColourKey(SkinObjectType::kModuleHeader, SkinColourProperty::kLineOrange),
        skinColourKey(SkinObjectType::kGroupLabel, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kGroupLabel, SkinColourProperty::kLine),
        skinColourKey(SkinObjectType::kLabel, SkinColourProperty::kBackground, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kLabel, SkinColourProperty::kBackground, SkinColourState::kClicked),
        skinColourKey(SkinObjectType::kLabel, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kHorizontalSeparator, SkinColourProperty::kLine),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kClicked),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBackground, SkinColourState::kError),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kBorder, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kClicked),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kEmphasis),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kText, SkinColourState::kError),
        skinColourKey(SkinObjectType::kButton, SkinColourProperty::kSecondaryText, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBackground, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kBorder, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kToggle, SkinColourProperty::kText, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kBackground, SkinColourState::kDisabled, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDefault, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kText, SkinColourState::kDisabled, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDefault, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kTrack, SkinColourState::kDisabled, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDefault, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kValueBar, SkinColourState::kDisabled, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kFocusBorder, SkinColourState::kDefault, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kEditorText, SkinColourState::kDefault, ColourSet::k4),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kSlider, SkinColourProperty::kCaret, SkinColourState::kDefault, ColourSet::k4),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kText, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kSecondaryText),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kSecondaryText, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kTrack),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kTrack, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar),
        skinColourKey(SkinObjectType::kProgressBar, SkinColourProperty::kValueBar, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBackground, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kBorder, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kFocus),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kFocus, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kText, SkinColourState::kFocus, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kFocusBorder, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kComboBox, SkinColourProperty::kTriangle, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kHover, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBackground, SkinColourState::kHover, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kBorder, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kHover),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kHover, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kHover, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDisabled, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kText, SkinColourState::kDisabled, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kSeparator, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar, SkinColourState::kDefault, ColourSet::k2),
        skinColourKey(SkinObjectType::kPopupMenu, SkinColourProperty::kScrollbar, SkinColourState::kDefault, ColourSet::k3),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBackground, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kBorder, SkinColourState::kDisabled),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kText, SkinColourState::kFocus),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorBackground),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorSelectionBackground),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kEditorText),
        skinColourKey(SkinObjectType::kNumberBox, SkinColourProperty::kCaret),
        skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kEnvelopeDisplay, SkinColourProperty::kEnvelope),
        skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kTrackGeneratorDisplay, SkinColourProperty::kShaper),
        skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kSecondaryText),
        skinColourKey(SkinObjectType::kPatchNameDisplay, SkinColourProperty::kCaret),
        skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kBorder),
        skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kText, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kTitleBand),
        skinColourKey(SkinObjectType::kDialog, SkinColourProperty::kGlyph),
        skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kBackground),
        skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText),
        skinColourKey(SkinObjectType::kReadOnlyValueField, SkinColourProperty::kText, SkinColourState::kError),
        skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOff, ColourSet::k2),
        skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBackground, SkinColourState::kOn, ColourSet::k2),
        skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBorder, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kLed, SkinColourProperty::kBorder, SkinColourState::kOff, ColourSet::k2),
        skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBackground, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBackground, SkinColourState::kOff, ColourSet::k2),
        skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBackground, SkinColourState::kOn),
        skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBackground, SkinColourState::kOn, ColourSet::k2),
        skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBorder, SkinColourState::kOff),
        skinColourKey(SkinObjectType::kPeakIndicator, SkinColourProperty::kBorder, SkinColourState::kOff, ColourSet::k2)
    };

    inline int stepIndexForZone(ColourChartTones::ToneId tone, int zoneIndex) noexcept
    {
        int step = kSharedStepIndices[zoneIndex];
        if (tone == ColourChartTones::ToneId::kPetrol)
        {
            for (const auto& override : kPetrolStepOverrides)
                if (override.zoneIndex == zoneIndex)
                    return override.stepIndex;
        }
        return step;
    }

    template <typename SetColourFn>
    void applyToneColours(ColourChartTones::ToneId tone, SetColourFn&& setColour)
    {
        for (int i = 0; i < kZoneCount; ++i)
        {
            const int step = stepIndexForZone(tone, i);
            setColour(kZoneKeys[i], juce::Colour(ColourChartTones::stop(tone, step)));
        }
    }
}
