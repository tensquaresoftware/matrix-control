#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "GUI/Settings/SettingsShellMetrics.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

class SettingsTabsShellTests : public juce::UnitTest
{
public:
    SettingsTabsShellTests()
        : juce::UnitTest("SettingsTabsShell")
    {
    }

    void runTest() override
    {
        firstOpenDefaultsToUserInterface();
        reopenRestoresPatchTab();
        pluginDeviceShowsLatencyWithoutMidiOrAudioTabs();
        standaloneDeviceOmitsLatency();
        writeAndCoerceLastTab();
        tightHostClampsAndCentresDialog();
        uiScaleKeepsIntegerRuleAndColumns();
    }

private:
    static juce::ValueTree makeState()
    {
        return juce::ValueTree("APVTS");
    }

    void firstOpenDefaultsToUserInterface()
    {
        beginTest("First open - missing last tab shows USER INTERFACE");

        auto state = makeState();
        const int tab = SettingsShellMetrics::readAndCoerceLastTab(state);

        expectEquals(tab, PluginIDs::Settings::LastTab::kUserInterface);
        expect(! state.hasProperty(PluginIDs::Settings::kLastSettingsTab));
        expectEquals(juce::String(PluginDisplayNames::Settings::kUserInterfaceTab),
                     juce::String("USER INTERFACE"));
    }

    void reopenRestoresPatchTab()
    {
        beginTest("Reopen - last PATCH tab is restored");

        auto state = makeState();
        SettingsShellMetrics::writeLastTab(state, PluginIDs::Settings::LastTab::kPatch);

        expectEquals(SettingsShellMetrics::readAndCoerceLastTab(state),
                     PluginIDs::Settings::LastTab::kPatch);
        expectEquals(static_cast<int>(state.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kPatch);
    }

    void pluginDeviceShowsLatencyWithoutMidiOrAudioTabs()
    {
        beginTest("Plugin DEVICE - latency helper; five tabs with product labels");

        expect(SettingsShellMetrics::deviceShowsHardwareLatency(true));
        expectEquals(SettingsShellMetrics::tabCount(), PluginIDs::Settings::LastTab::kCount);
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(0)), juce::String("USER INTERFACE"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(1)), juce::String("DEVICE"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(2)), juce::String("PATCH"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(3)), juce::String("PATCH MUTATOR"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(4)), juce::String("MASTER"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(5)), juce::String());
        expectEquals(juce::String(PluginDisplayNames::Settings::kHardwareLatencyLabel),
                     juce::String("HARDWARE LATENCY"));
    }

    void standaloneDeviceOmitsLatency()
    {
        beginTest("Standalone DEVICE - EPROM TYPE only");

        expect(! SettingsShellMetrics::deviceShowsHardwareLatency(false));
        expectEquals(juce::String(PluginDisplayNames::Settings::kEpromTypeLabel),
                     juce::String("EPROM TYPE"));
    }

    void writeAndCoerceLastTab()
    {
        beginTest("Last tab - write helper and unknown id coerce");

        auto written = makeState();
        SettingsShellMetrics::writeLastTab(written, PluginIDs::Settings::LastTab::kPatchMutator);
        expectEquals(static_cast<int>(written.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kPatchMutator);
        expectEquals(SettingsShellMetrics::readAndCoerceLastTab(written),
                     PluginIDs::Settings::LastTab::kPatchMutator);

        SettingsShellMetrics::writeLastTab(written, 99);
        expectEquals(static_cast<int>(written.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kUserInterface);

        auto coerced = makeState();
        coerced.setProperty(PluginIDs::Settings::kLastSettingsTab, 99, nullptr);
        const int tab = SettingsShellMetrics::readAndCoerceLastTab(coerced);
        expectEquals(tab, PluginIDs::Settings::LastTab::kUserInterface);
        expectEquals(static_cast<int>(coerced.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kUserInterface);
        expectEquals(PluginIDs::Settings::LastTab::normalize(0),
                     PluginIDs::Settings::LastTab::kDefault);
        expectEquals(PluginIDs::Settings::LastTab::normalize(-3),
                     PluginIDs::Settings::LastTab::kDefault);
    }

    void tightHostClampsAndCentresDialog()
    {
        beginTest("Tight host - dialog is clamped and centred");

        const juce::Rectangle<int> editor(0, 0, 200, 160);
        const auto bounds = SettingsShellMetrics::centredClampedDialog(editor, 460, 220);

        expectEquals(bounds.getWidth(), 200);
        expectEquals(bounds.getHeight(), 160);
        expectEquals(bounds.getCentreX(), editor.getCentreX());
        expectEquals(bounds.getCentreY(), editor.getCentreY());
        expect(editor.contains(bounds));
    }

    void expectIntegerRuleAndColumns(float scale)
    {
        const int labelW = SettingsShellMetrics::scaledLabelWidth(scale);
        expectEquals(SettingsShellMetrics::controlColumnX(16, scale), 16 + labelW);

        expectEquals(SettingsShellMetrics::scaledBodyWidth(scale),
                     SettingsShellMetrics::scaledRailWidth(scale)
                         + SettingsShellMetrics::scaledRuleGutter(scale)
                         + SettingsShellMetrics::scaledContentWidth(scale));

        const juce::Rectangle<int> railInner(0, 16, 148, 100);
        const int expectedStroke = scale >= 2.0f
            ? SettingsShellMetrics::kRuleThicknessAt200
            : SettingsShellMetrics::kRuleThicknessUntil200;
        expectEquals(SettingsShellMetrics::ruleStrokeThickness(scale), expectedStroke);

        const int ruleX = SettingsShellMetrics::ruleFillX(16, scale);
        expect(ruleX >= 16 + SettingsShellMetrics::scaledRailWidth(scale));
        expect(ruleX + expectedStroke
               <= 16 + SettingsShellMetrics::scaledRailWidth(scale)
                      + SettingsShellMetrics::scaledRuleGutter(scale));

        for (int tab = 0; tab < SettingsShellMetrics::tabCount(); ++tab)
        {
            const auto row = SettingsShellMetrics::tabRowBounds(tab, railInner, scale);
            expect(row.isEmpty() || railInner.contains(row));
        }

        const juce::Rectangle<int> shortRail(0, 0, 148, 30);
        const auto clipped = SettingsShellMetrics::tabRowBounds(4, shortRail, 1.0f);
        expect(clipped.isEmpty() || shortRail.contains(clipped));
    }

    void uiScaleKeepsIntegerRuleAndColumns()
    {
        beginTest("UI Scale 50-200 - integer rule and columns");

        const float scales[] = { 0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f };
        for (const float scale : scales)
            expectIntegerRuleAndColumns(scale);

        expect(SettingsShellMetrics::paddedBodyDesignHeight()
               == SettingsShellMetrics::kPadding * 2
                      + SettingsShellMetrics::tallestPageContentHeight());
        expect(SettingsShellMetrics::tallestPageContentHeight()
               > SettingsShellMetrics::railLabelStackHeight());
    }
};

static SettingsTabsShellTests settingsTabsShellTests;
