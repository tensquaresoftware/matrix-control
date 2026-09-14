#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Helpers/EditorChromeShortcuts.h"

class EditorChromeShortcutTests : public juce::UnitTest
{
public:
    EditorChromeShortcutTests() : juce::UnitTest("EditorChromeShortcut") {}

    void runTest() override
    {
        matchesCommandCommaAsSettings();
        matchesCommandAltCommaAsAudioMidi();
        matchesCommaWithNonZeroTextCharacter();
        matchesZoomKeysIncludingEqualsAndNumPad();
        matchesZoomKeysWithNonZeroTextCharacter();
        matchesAzertyZeroKeyPeersAsUiScaleReset();
        matchesSkinDigitShortcuts();
        rejectsUnassignedSkinDigits();
        rejectsWhenTextWouldConflictWithPlainKeys();
        macShortcutLabelsUseUnicodeCodePoints();
    }

private:
    void matchesCommandCommaAsSettings()
    {
        beginTest("matchesCommandCommaAsSettings");

        using juce::ModifierKeys;
        const juce::KeyPress key(',', ModifierKeys::commandModifier, 0);

        expect(TSS::isEditorChromeShortcut(key));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(key)),
                     static_cast<int>(TSS::EditorChromeShortcut::kOpenSettings));
    }

    void matchesCommandAltCommaAsAudioMidi()
    {
        beginTest("matchesCommandAltCommaAsAudioMidi");

        using juce::ModifierKeys;
        const juce::KeyPress key(',',
                                 ModifierKeys::commandModifier | ModifierKeys::altModifier,
                                 0);

        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(key)),
                     static_cast<int>(TSS::EditorChromeShortcut::kOpenAudioMidiSettings));
    }

    void matchesCommaWithNonZeroTextCharacter()
    {
        beginTest("matchesCommaWithNonZeroTextCharacter");

        using juce::ModifierKeys;
        const juce::KeyPress settings(',', ModifierKeys::commandModifier, ',');
        const juce::KeyPress audioMidi(',',
                                       ModifierKeys::commandModifier | ModifierKeys::altModifier,
                                       ',');

        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(settings)),
                     static_cast<int>(TSS::EditorChromeShortcut::kOpenSettings));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(audioMidi)),
                     static_cast<int>(TSS::EditorChromeShortcut::kOpenAudioMidiSettings));
    }

    void matchesZoomKeysIncludingEqualsAndNumPad()
    {
        beginTest("matchesZoomKeysIncludingEqualsAndNumPad");

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;
        const auto commandShift = command | ModifierKeys::shiftModifier;

        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('+', command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('=', command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('=', commandShift, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(juce::KeyPress::numberPadAdd, command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('-', command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleDecrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(juce::KeyPress::numberPadSubtract, command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleDecrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('0', command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(juce::KeyPress::numberPad0, command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
    }

    void matchesZoomKeysWithNonZeroTextCharacter()
    {
        beginTest("matchesZoomKeysWithNonZeroTextCharacter");

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;
        const auto commandShift = command | ModifierKeys::shiftModifier;

        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('+', command, '+'))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('=', command, '='))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('+', commandShift, '+'))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('=', commandShift, '='))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleIncrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('-', command, '-'))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleDecrease));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('0', command, '0'))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
    }

    void matchesAzertyZeroKeyPeersAsUiScaleReset()
    {
        beginTest("matchesAzertyZeroKeyPeersAsUiScaleReset");

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;
        const auto commandShift = command | ModifierKeys::shiftModifier;
        constexpr juce::juce_wchar kGraveA = 0x00e0;
        constexpr juce::juce_wchar kGraveAUpper = 0x00c0;
        constexpr juce::juce_wchar kGraveO = 0x00f2;
        constexpr juce::juce_wchar kGraveOUpper = 0x00d2;

        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(static_cast<int>(kGraveA), command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(static_cast<int>(kGraveAUpper), command, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(static_cast<int>(kGraveAUpper), command, kGraveA))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(static_cast<int>(kGraveOUpper), command, kGraveO))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('0', commandShift, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress(juce::KeyPress::numberPad0, commandShift, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kUiScaleReset));
    }

    void matchesSkinDigitShortcuts()
    {
        beginTest("matchesSkinDigitShortcuts");

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;
        const auto commandShift = command | ModifierKeys::shiftModifier;
        constexpr juce::juce_wchar kEAcute = 0x00e9;
        constexpr juce::juce_wchar kEAcuteUpper = 0x00c9;

        expectEquals(TSS::classifySkinVariantShortcut(juce::KeyPress('1', command, 0)), 1);
        expectEquals(TSS::classifySkinVariantShortcut(juce::KeyPress('2', command, 0)), 2);
        expectEquals(TSS::classifySkinVariantShortcut(juce::KeyPress('1', commandShift, 0)), 1);
        expectEquals(TSS::classifySkinVariantShortcut(
                         juce::KeyPress(juce::KeyPress::numberPad2, command, 0)),
                     2);
        expectEquals(TSS::classifySkinVariantShortcut(juce::KeyPress('&', command, 0)), 1);
        expectEquals(TSS::classifySkinVariantShortcut(
                         juce::KeyPress(static_cast<int>(kEAcuteUpper), command, kEAcute)),
                     2);
        expect(TSS::isEditorChromeShortcut(juce::KeyPress('1', command, 0)));
    }

    void rejectsUnassignedSkinDigits()
    {
        beginTest("rejectsUnassignedSkinDigits");

        using juce::ModifierKeys;
        const auto command = ModifierKeys::commandModifier;

        expectEquals(TSS::classifySkinVariantShortcut(juce::KeyPress('3', command, 0)), 0);
        expect(! TSS::isAssignedSkinVariantId(3));
        expect(! TSS::isEditorChromeShortcut(juce::KeyPress('3', command, 0)));
    }

    void rejectsWhenTextWouldConflictWithPlainKeys()
    {
        beginTest("rejectsWhenTextWouldConflictWithPlainKeys");

        using juce::ModifierKeys;

        expect(! TSS::isEditorChromeShortcut(juce::KeyPress(',', 0, ',')));
        expect(! TSS::isEditorChromeShortcut(
            juce::KeyPress('+', ModifierKeys::altModifier, 0)));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('=', ModifierKeys::commandModifier | ModifierKeys::altModifier, 0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kNone));
        expectEquals(static_cast<int>(TSS::classifyEditorChromeShortcut(
                         juce::KeyPress('0',
                                        ModifierKeys::commandModifier | ModifierKeys::altModifier,
                                        0))),
                     static_cast<int>(TSS::EditorChromeShortcut::kNone));
    }

    void macShortcutLabelsUseUnicodeCodePoints()
    {
        beginTest("macShortcutLabelsUseUnicodeCodePoints");

#if JUCE_MAC
        const auto settings = TSS::EditorChromeShortcutLabels::openSettings();
        const auto audioMidi = TSS::EditorChromeShortcutLabels::openAudioMidi();
        const auto step = TSS::EditorChromeShortcutLabels::uiScaleStep();
        const auto reset = TSS::EditorChromeShortcutLabels::uiScaleReset();

        expectEquals(static_cast<int>(settings[0]), 0x2318);
        expectEquals(static_cast<int>(step[0]), 0x2318);
        expectEquals(static_cast<int>(reset[0]), 0x2318);
        expectEquals(static_cast<int>(audioMidi[0]), 0x2325);
        expect(audioMidi.contains(TSS::EditorChromeShortcutLabels::macCommandGlyph()));
        expect(! settings.containsChar(static_cast<juce::juce_wchar>(0x00e2))); // not Latin-1 â
#else
        expect(TSS::EditorChromeShortcutLabels::openSettings() == "Ctrl+,");
#endif
    }
};

static EditorChromeShortcutTests editorChromeShortcutTests;
