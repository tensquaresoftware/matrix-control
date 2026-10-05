#pragma once

#include <juce_data_structures/juce_data_structures.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Layout/ScaledLayout.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace SettingsShellMetrics
{
    // Edge inset matches tab-label inset (body left → tab text) and content right gap.
    inline constexpr int kPadding = 8;
    inline constexpr int kRailWidth = 120;
    inline constexpr int kRuleGutter = 4;
    inline constexpr int kLabelWidth = 120;
    // Matches widest MASTER button pack (three 44 + gaps, or SAVE AS INIT + DELETE).
    inline constexpr int kControlColumnWidth = 140;
    inline constexpr int kContentWidth = kLabelWidth + kControlColumnWidth + kPadding * 2;
    inline constexpr int kDesignWidth = kRailWidth + kRuleGutter + kContentWidth;
    inline constexpr int kRowGap = 8;
    inline constexpr int kControlHeight = 20;
    inline constexpr int kTabRowHeight = 20;
    inline constexpr int kPeakWidth = 12;
    inline constexpr int kPeakGap = 8;
    inline constexpr int kSynthFromComboWidth = kControlColumnWidth - kPeakWidth - kPeakGap;
    // AUDIO: 9 control rows + 2 blanks + up to two three-row channel wraps.
    inline constexpr int kTallestPageRows = 16;
    inline constexpr int kRuleThicknessUntil200 = 1;
    inline constexpr int kRuleThicknessAt200 = 2;

    static_assert(kDesignWidth == 400);
    static_assert(kLabelWidth + kControlColumnWidth + kPadding * 2 == kContentWidth);
    static_assert(kSynthFromComboWidth + kPeakGap + kPeakWidth == kControlColumnWidth);
    static_assert(kPadding % 4 == 0);
    static_assert(kRailWidth % 4 == 0);
    static_assert(kLabelWidth % 4 == 0);
    static_assert(kControlColumnWidth % 4 == 0);
    static_assert(kSynthFromComboWidth % 4 == 0);

    inline int tabCount(bool isPluginMode) noexcept
    {
        return PluginIDs::Settings::LastTab::count(isPluginMode);
    }

    inline const char* tabLabel(int index, bool isPluginMode) noexcept
    {
        using namespace PluginDisplayNames::Settings;
        using namespace PluginIDs::Settings::LastTab;

        if (index < 0 || index >= count(isPluginMode))
            return "";

        switch (idAt(index, isPluginMode))
        {
            case kUserInterface: return kUserInterfaceTab;
            case kDevice: return kDeviceSection;
            case kAudio: return kAudioTab;
            case kPatch: return kPatchSection;
            case kPatchMutator: return kPatchMutatorSection;
            case kMaster: return kMasterSection;
            default: return "";
        }
    }

    inline int railLabelStackHeight(bool isPluginMode) noexcept
    {
        return tabCount(isPluginMode) * kTabRowHeight;
    }

    inline int pageContentHeight(int rowCount) noexcept
    {
        if (rowCount <= 0)
            return 0;

        return rowCount * kControlHeight + (rowCount - 1) * kRowGap;
    }

    inline bool deviceShowsHardwareLatency(bool isPluginMode) noexcept
    {
        return isPluginMode;
    }

    inline bool showsAudioTab(bool isPluginMode) noexcept
    {
        return ! isPluginMode;
    }

    inline int tallestPageContentHeight() noexcept
    {
        return pageContentHeight(kTallestPageRows);
    }

    inline int paddedBodyDesignHeight(bool isPluginMode) noexcept
    {
        return kPadding * 2 + juce::jmax(railLabelStackHeight(isPluginMode), tallestPageContentHeight());
    }

    inline int scaledRailWidth(float uiScale) noexcept
    {
        return TSS::ScaledLayout::scaledInt(static_cast<float>(kRailWidth), uiScale);
    }

    inline int scaledRuleGutter(float uiScale) noexcept
    {
        return TSS::ScaledLayout::scaledInt(static_cast<float>(kRuleGutter), uiScale);
    }

    inline int scaledContentWidth(float uiScale) noexcept
    {
        return TSS::ScaledLayout::scaledInt(static_cast<float>(kContentWidth), uiScale);
    }

    inline int scaledBodyWidth(float uiScale) noexcept
    {
        return scaledRailWidth(uiScale) + scaledRuleGutter(uiScale) + scaledContentWidth(uiScale);
    }

    inline int scaledBodyHeight(float uiScale, bool isPluginMode) noexcept
    {
        return TSS::ScaledLayout::scaledInt(static_cast<float>(paddedBodyDesignHeight(isPluginMode)), uiScale);
    }

    inline int scaledLabelWidth(float uiScale) noexcept
    {
        return TSS::ScaledLayout::scaledInt(static_cast<float>(kLabelWidth), uiScale);
    }

    inline int controlColumnX(int contentOriginX, float uiScale) noexcept
    {
        return contentOriginX + scaledLabelWidth(uiScale);
    }

    inline juce::Rectangle<int> tabRowBounds(int tabIndex, juce::Rectangle<int> railInner, float uiScale)
    {
        const int rowHeight = TSS::ScaledLayout::scaledInt(static_cast<float>(kTabRowHeight), uiScale);
        const int y = railInner.getY() + tabIndex * rowHeight;
        const juce::Rectangle<int> row { railInner.getX(), y, railInner.getWidth(), rowHeight };
        return row.getIntersection(railInner);
    }

    inline int ruleStrokeThickness(float uiScale) noexcept
    {
        using namespace PluginIDs::Settings::ScaleLevels;
        return uiScale >= kUiScales[k200] ? kRuleThicknessAt200 : kRuleThicknessUntil200;
    }

    inline int ruleFillX(int bodyX, int railWidth, int gutter, float uiScale) noexcept
    {
        const int thickness = ruleStrokeThickness(uiScale);
        const int inset = juce::jmax(0, (gutter - thickness) / 2);
        return bodyX + railWidth + inset;
    }

    inline int ruleFillX(int bodyX, float uiScale) noexcept
    {
        return ruleFillX(bodyX, scaledRailWidth(uiScale), scaledRuleGutter(uiScale), uiScale);
    }

    inline juce::Rectangle<int> centredClampedDialog(juce::Rectangle<int> editorBounds,
                                                     int dialogWidth,
                                                     int dialogHeight)
    {
        const int width = juce::jmin(dialogWidth, editorBounds.getWidth());
        const int height = juce::jmin(dialogHeight, editorBounds.getHeight());
        return editorBounds.withSizeKeepingCentre(width, height);
    }

    inline int readAndCoerceLastTab(juce::ValueTree& state, bool isPluginMode)
    {
        using namespace PluginIDs::Settings;

        if (! state.hasProperty(kLastSettingsTab))
            return LastTab::kDefault;

        const int raw = static_cast<int>(state.getProperty(kLastSettingsTab, LastTab::kDefault));
        const int normalized = LastTab::normalize(raw, isPluginMode);

        if (normalized != raw)
            state.setProperty(kLastSettingsTab, normalized, nullptr);

        return normalized;
    }

    inline void writeLastTab(juce::ValueTree& state, int tabId, bool isPluginMode)
    {
        using namespace PluginIDs::Settings;
        state.setProperty(kLastSettingsTab, LastTab::normalize(tabId, isPluginMode), nullptr);
    }
}
