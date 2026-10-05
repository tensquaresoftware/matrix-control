#include <juce_core/juce_core.h>
#include <juce_data_structures/juce_data_structures.h>

#include "GUI/Settings/AudioDeviceSetupSync.h"
#include "GUI/Settings/SettingsShellMetrics.h"
#include "GUI/Widgets/RadioButtonGroupLayout.h"
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
        reopenRestoresAudioTab();
        pluginDeviceShowsLatencyWithoutAudioTab();
        standaloneIncludesAudioTab();
        writeAndCoerceLastTab();
        pluginCoercesStaleAudioLastTab();
        radioButtonGroupWrapPolicy();
        stereoPairMaskRoundTrip();
        applyStereoPairClearsUseDefaultFlags();
        asioDeviceListsStayLinked();
        asioApplyPathResolvesEndpointNames();
        headerAudioProductCopy();
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
        const int tab = SettingsShellMetrics::readAndCoerceLastTab(state, false);

        expectEquals(tab, PluginIDs::Settings::LastTab::kUserInterface);
        expect(! state.hasProperty(PluginIDs::Settings::kLastSettingsTab));
        expectEquals(juce::String(PluginDisplayNames::Settings::kUserInterfaceTab),
                     juce::String("USER INTERFACE"));
    }

    void reopenRestoresPatchTab()
    {
        beginTest("Reopen - last PATCH tab is restored");

        auto state = makeState();
        SettingsShellMetrics::writeLastTab(state, PluginIDs::Settings::LastTab::kPatch, false);

        expectEquals(SettingsShellMetrics::readAndCoerceLastTab(state, false),
                     PluginIDs::Settings::LastTab::kPatch);
        expectEquals(static_cast<int>(state.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kPatch);
    }

    void reopenRestoresAudioTab()
    {
        beginTest("Reopen - last AUDIO tab is restored in standalone");

        auto state = makeState();
        SettingsShellMetrics::writeLastTab(state, PluginIDs::Settings::LastTab::kAudio, false);

        expectEquals(SettingsShellMetrics::readAndCoerceLastTab(state, false),
                     PluginIDs::Settings::LastTab::kAudio);
        expectEquals(static_cast<int>(state.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kAudio);
    }

    void pluginDeviceShowsLatencyWithoutAudioTab()
    {
        beginTest("Plugin DEVICE - latency helper; five tabs without AUDIO");

        expect(SettingsShellMetrics::deviceShowsHardwareLatency(true));
        expect(! SettingsShellMetrics::showsAudioTab(true));
        expectEquals(SettingsShellMetrics::tabCount(true), PluginIDs::Settings::LastTab::kPluginCount);
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(0, true)), juce::String("USER INTERFACE"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(1, true)), juce::String("DEVICE"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(2, true)), juce::String("PATCH"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(3, true)), juce::String("PATCH MUTATOR"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(4, true)), juce::String("MASTER"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(5, true)), juce::String());
        expectEquals(juce::String(PluginDisplayNames::Settings::kHardwareLatencyLabel),
                     juce::String("HARDWARE LATENCY"));
    }

    void standaloneIncludesAudioTab()
    {
        beginTest("Standalone - AUDIO tab after DEVICE");

        expect(SettingsShellMetrics::showsAudioTab(false));
        expectEquals(SettingsShellMetrics::tabCount(false), PluginIDs::Settings::LastTab::kStandaloneCount);
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(0, false)), juce::String("USER INTERFACE"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(1, false)), juce::String("DEVICE"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(2, false)), juce::String("AUDIO"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(3, false)), juce::String("PATCH"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(4, false)), juce::String("PATCH MUTATOR"));
        expectEquals(juce::String(SettingsShellMetrics::tabLabel(5, false)), juce::String("MASTER"));
        expectEquals(PluginIDs::Settings::LastTab::idAt(2, false),
                     PluginIDs::Settings::LastTab::kAudio);
    }

    void writeAndCoerceLastTab()
    {
        beginTest("Last tab - write helper and unknown id coerce");

        auto written = makeState();
        SettingsShellMetrics::writeLastTab(written, PluginIDs::Settings::LastTab::kPatchMutator, false);
        expectEquals(static_cast<int>(written.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kPatchMutator);
        expectEquals(SettingsShellMetrics::readAndCoerceLastTab(written, false),
                     PluginIDs::Settings::LastTab::kPatchMutator);

        SettingsShellMetrics::writeLastTab(written, 99, false);
        expectEquals(static_cast<int>(written.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kUserInterface);

        auto coerced = makeState();
        coerced.setProperty(PluginIDs::Settings::kLastSettingsTab, 99, nullptr);
        const int tab = SettingsShellMetrics::readAndCoerceLastTab(coerced, false);
        expectEquals(tab, PluginIDs::Settings::LastTab::kUserInterface);
        expectEquals(static_cast<int>(coerced.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kUserInterface);
        expectEquals(PluginIDs::Settings::LastTab::normalize(0, false),
                     PluginIDs::Settings::LastTab::kDefault);
        expectEquals(PluginIDs::Settings::LastTab::normalize(-3, false),
                     PluginIDs::Settings::LastTab::kDefault);
    }

    void pluginCoercesStaleAudioLastTab()
    {
        beginTest("Plugin - stale AUDIO last-tab coerces to first tab");

        auto state = makeState();
        state.setProperty(PluginIDs::Settings::kLastSettingsTab,
                          PluginIDs::Settings::LastTab::kAudio,
                          nullptr);
        expectEquals(SettingsShellMetrics::readAndCoerceLastTab(state, true),
                     PluginIDs::Settings::LastTab::kUserInterface);
        expectEquals(static_cast<int>(state.getProperty(PluginIDs::Settings::kLastSettingsTab)),
                     PluginIDs::Settings::LastTab::kUserInterface);
    }

    void radioButtonGroupWrapPolicy()
    {
        beginTest("RadioButtonGroup - wrap stays in control column width");

        const TSS::RadioButtonGroupLayout::OptionMetrics metrics { 64, 20, 8, 4 };
        expectEquals(TSS::RadioButtonGroupLayout::stereoPairCount(6), 3);
        expectEquals(TSS::RadioButtonGroupLayout::stereoPairLabel(0), juce::String("1 + 2"));
        expectEquals(TSS::RadioButtonGroupLayout::stereoPairLabel(1), juce::String("3 + 4"));
        expectEquals(TSS::RadioButtonGroupLayout::stereoPairLabel(2), juce::String("5 + 6"));
        // Two 64 px options + gap fit in 140 px; three options wrap.
        expectEquals(TSS::RadioButtonGroupLayout::preferredHeight(2, 140, metrics), 20);
        expect(TSS::RadioButtonGroupLayout::preferredHeight(3, 140, metrics) > 20);
        expect(TSS::RadioButtonGroupLayout::preferredHeight(6, 140, metrics) > 20);
        const auto second = TSS::RadioButtonGroupLayout::optionBounds(2, 6, { 0, 0, 140, 48 }, metrics);
        expect(second.getY() > 0);
    }

    void stereoPairMaskRoundTrip()
    {
        beginTest("RadioButtonGroup - stereo pair mask encode/decode");

        const auto mask2 = TSS::RadioButtonGroupLayout::stereoPairMask(2);
        expect(mask2[4]);
        expect(mask2[5]);
        expect(! mask2[0]);
        expectEquals(TSS::RadioButtonGroupLayout::selectedStereoPairIndex(mask2, 4), 2);

        juce::BigInteger empty;
        expectEquals(TSS::RadioButtonGroupLayout::selectedStereoPairIndex(empty, 3), -1);

        juce::BigInteger mid;
        mid.setBit(2);
        expectEquals(TSS::RadioButtonGroupLayout::selectedStereoPairIndex(mid, 3), -1);

        juce::BigInteger multi = TSS::RadioButtonGroupLayout::stereoPairMask(0);
        multi |= TSS::RadioButtonGroupLayout::stereoPairMask(1);
        expectEquals(TSS::RadioButtonGroupLayout::selectedStereoPairIndex(multi, 3), -1);
    }

    void applyStereoPairClearsUseDefaultFlags()
    {
        beginTest("AUDIO apply - stereo pair clears useDefault channel flags");

        juce::AudioDeviceManager::AudioDeviceSetup setup;
        setup.useDefaultInputChannels = true;
        setup.useDefaultOutputChannels = true;

        AudioDeviceSetupSync::applyStereoPairToSetup(setup, true, 1);
        expect(! setup.useDefaultInputChannels);
        expect(setup.useDefaultOutputChannels);
        expectEquals(TSS::RadioButtonGroupLayout::selectedStereoPairIndex(setup.inputChannels, 4), 1);

        AudioDeviceSetupSync::applyStereoPairToSetup(setup, false, 2);
        expect(! setup.useDefaultOutputChannels);
        expectEquals(TSS::RadioButtonGroupLayout::selectedStereoPairIndex(setup.outputChannels, 4), 2);
    }

    void asioDeviceListsStayLinked()
    {
        beginTest("ASIO - input and output device names stay linked");

        expect(AudioDeviceSetupSync::isAsioDeviceType("ASIO"));
        expect(AudioDeviceSetupSync::isAsioDeviceType("ASIO Fireface USB"));
        expect(! AudioDeviceSetupSync::isAsioDeviceType("CoreAudio"));

        juce::String input = "Scarlett 6i6";
        juce::String output = "Other";
        AudioDeviceSetupSync::linkAsioDeviceNames(input, output, false);
        expectEquals(input, juce::String("Scarlett 6i6"));
        expectEquals(output, juce::String("Scarlett 6i6"));

        input = "A";
        output = "B";
        AudioDeviceSetupSync::linkAsioDeviceNames(input, output, true);
        expectEquals(input, juce::String("B"));
        expectEquals(output, juce::String("B"));

        input = "Keep";
        output = {};
        AudioDeviceSetupSync::linkAsioDeviceNames(input, output, true);
        expect(input.isEmpty());
        expect(output.isEmpty());
    }

    void asioApplyPathResolvesEndpointNames()
    {
        beginTest("AUDIO apply - ASIO resolve links prefer-output; non-ASIO stays independent");

        juce::String input = "In A";
        juce::String output = "Out B";
        AudioDeviceSetupSync::resolveEndpointNamesForApply("ASIO", input, output, true);
        expectEquals(input, juce::String("Out B"));
        expectEquals(output, juce::String("Out B"));

        input = "In A";
        output = "Out B";
        AudioDeviceSetupSync::resolveEndpointNamesForApply("CoreAudio", input, output, true);
        expectEquals(input, juce::String("In A"));
        expectEquals(output, juce::String("Out B"));

        input = "Keep";
        output = {};
        AudioDeviceSetupSync::resolveEndpointNamesForApply("ASIO Fireface", input, output, true);
        expect(input.isEmpty());
        expect(output.isEmpty());
    }

    void headerAudioProductCopy()
    {
        beginTest("Header AUDIO - cartouche and Settings copy without Audio/MIDI door");

        expectEquals(juce::String(PluginDisplayNames::HeaderPanel::kAudioCartoucheLabel),
                     juce::String("AUDIO"));
        expectEquals(juce::String(PluginDisplayNames::HeaderPanel::kInputGainLabel),
                     juce::String("INPUT GAIN"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kSynthFromLabel),
                     juce::String("SYNTH FROM"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kInputChannelsLabel),
                     juce::String("INPUT CHANNELS"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kOutputChannelsLabel),
                     juce::String("OUTPUT CHANNELS"));
        expectEquals(juce::String(PluginDisplayNames::Settings::kPlayTestToneButton),
                     juce::String("PLAY TEST TONE"));
        expectEquals(juce::String(PluginDisplayNames::HeaderPanel::kSettingsButton),
                     juce::String("SETTINGS..."));
    }

    void tightHostClampsAndCentresDialog()
    {
        beginTest("Tight host - dialog is clamped and centred");

        const juce::Rectangle<int> editor(0, 0, 200, 160);
        const auto bounds = SettingsShellMetrics::centredClampedDialog(editor, 520, 360);

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

        const int railW = SettingsShellMetrics::kRailWidth;
        const juce::Rectangle<int> railInner(0, SettingsShellMetrics::kPadding, railW, 100);
        const int expectedStroke = scale >= 2.0f
            ? SettingsShellMetrics::kRuleThicknessAt200
            : SettingsShellMetrics::kRuleThicknessUntil200;
        expectEquals(SettingsShellMetrics::ruleStrokeThickness(scale), expectedStroke);

        const int ruleX = SettingsShellMetrics::ruleFillX(16, scale);
        expect(ruleX >= 16 + SettingsShellMetrics::scaledRailWidth(scale));
        expect(ruleX + expectedStroke
               <= 16 + SettingsShellMetrics::scaledRailWidth(scale)
                      + SettingsShellMetrics::scaledRuleGutter(scale));

        for (int tab = 0; tab < SettingsShellMetrics::tabCount(false); ++tab)
        {
            const auto row = SettingsShellMetrics::tabRowBounds(tab, railInner, scale);
            expect(row.isEmpty() || railInner.contains(row));
        }

        const juce::Rectangle<int> shortRail(0, 0, railW, 30);
        const auto clipped = SettingsShellMetrics::tabRowBounds(4, shortRail, 1.0f);
        expect(clipped.isEmpty() || shortRail.contains(clipped));
        expectEquals(SettingsShellMetrics::kDesignWidth, 400);
        expectEquals(SettingsShellMetrics::kControlColumnWidth, 140);
    }

    void uiScaleKeepsIntegerRuleAndColumns()
    {
        beginTest("UI Scale 50-200 - integer rule and columns");

        const float scales[] = { 0.5f, 0.75f, 1.0f, 1.25f, 1.5f, 1.75f, 2.0f };
        for (const float scale : scales)
            expectIntegerRuleAndColumns(scale);

        expect(SettingsShellMetrics::paddedBodyDesignHeight(false)
               == SettingsShellMetrics::kPadding * 2
                      + SettingsShellMetrics::tallestPageContentHeight());
        expect(SettingsShellMetrics::tallestPageContentHeight()
               > SettingsShellMetrics::railLabelStackHeight(false));
    }
};

static SettingsTabsShellTests settingsTabsShellTests;
