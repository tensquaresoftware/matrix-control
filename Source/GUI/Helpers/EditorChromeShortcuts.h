#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

namespace TSS
{
    /** Classification of Settings / Audio-MIDI / UI Scale chrome shortcuts. */
    enum class EditorChromeShortcut
    {
        kNone,
        kOpenSettings,
        kOpenAudioMidiSettings,
        kUiScaleIncrease,
        kUiScaleDecrease,
        kUiScaleReset
    };

    namespace EditorChromeShortcutDetail
    {
        inline bool matchesKey(const juce::KeyPress& key,
                               int keyCode,
                               juce::ModifierKeys modifiers,
                               juce::juce_wchar textCharacter = 0)
        {
            return key == juce::KeyPress(keyCode, modifiers, textCharacter);
        }

        inline EditorChromeShortcut classifyCommaShortcut(const juce::KeyPress& key)
        {
            using juce::ModifierKeys;
            const auto command = ModifierKeys::commandModifier;
            const auto commandAlt = command | ModifierKeys::altModifier;

            if (matchesKey(key, ',', commandAlt) || matchesKey(key, ',', commandAlt, ','))
                return EditorChromeShortcut::kOpenAudioMidiSettings;

            if (matchesKey(key, ',', command) || matchesKey(key, ',', command, ','))
                return EditorChromeShortcut::kOpenSettings;

            return EditorChromeShortcut::kNone;
        }

        inline bool matchesAnyText(const juce::KeyPress& key,
                                   int keyCode,
                                   juce::ModifierKeys modifiers)
        {
            return matchesKey(key, keyCode, modifiers)
                || matchesKey(key, keyCode, modifiers, static_cast<juce::juce_wchar>(keyCode));
        }

        inline bool matchesUiScaleResetShortcut(const juce::KeyPress& key)
        {
            using juce::ModifierKeys;
            const auto command = ModifierKeys::commandModifier;
            const auto commandShift = command | ModifierKeys::shiftModifier;

            // QWERTY / layouts where the main-row key prints '0' unshifted.
            if (matchesAnyText(key, '0', command)
                || matchesAnyText(key, '0', commandShift)
                || matchesKey(key, juce::KeyPress::numberPad0, command)
                || matchesKey(key, juce::KeyPress::numberPad0, commandShift))
                return true;

            // Physical main-row "0" key on ISO layouts (FR AZERTY: à/0, IT: ò/0).
            // macOS JUCE uppercases charactersIgnoringModifiers (à → À) for keyCode.
            constexpr int kGraveA = 0x00e0; // à
            constexpr int kGraveAUpper = 0x00c0; // À
            constexpr int kGraveO = 0x00f2; // ò
            constexpr int kGraveOUpper = 0x00d2; // Ò

            return matchesAnyText(key, kGraveA, command)
                || matchesAnyText(key, kGraveAUpper, command)
                || matchesKey(key, kGraveAUpper, command, static_cast<juce::juce_wchar>(kGraveA))
                || matchesAnyText(key, kGraveO, command)
                || matchesAnyText(key, kGraveOUpper, command)
                || matchesKey(key, kGraveOUpper, command, static_cast<juce::juce_wchar>(kGraveO));
        }

        inline EditorChromeShortcut classifyUiScaleShortcut(const juce::KeyPress& key)
        {
            using juce::ModifierKeys;
            const auto command = ModifierKeys::commandModifier;
            const auto commandShift = command | ModifierKeys::shiftModifier;

            if (matchesAnyText(key, '+', command)
                || matchesAnyText(key, '=', command)
                || matchesAnyText(key, '+', commandShift)
                || matchesAnyText(key, '=', commandShift)
                || matchesKey(key, juce::KeyPress::numberPadAdd, command))
                return EditorChromeShortcut::kUiScaleIncrease;

            if (matchesAnyText(key, '-', command)
                || matchesKey(key, juce::KeyPress::numberPadSubtract, command))
                return EditorChromeShortcut::kUiScaleDecrease;

            if (matchesUiScaleResetShortcut(key))
                return EditorChromeShortcut::kUiScaleReset;

            return EditorChromeShortcut::kNone;
        }
    }

    /**
        Matches platform chrome KeyPresses using keyCode + modifiers.
        Tolerates textCharacter == 0 (macOS clears it when Command is down).
        Accepts '=' and Shift+'=' / Shift+'+' as zoom-in peers of '+' (layouts without a dedicated + key).
        UI Scale reset also accepts the physical main-row "0" key on AZERTY/ISO (à / ò) and Cmd/Ctrl+Shift+0.
    */
    inline EditorChromeShortcut classifyEditorChromeShortcut(const juce::KeyPress& key)
    {
        const auto commaShortcut = EditorChromeShortcutDetail::classifyCommaShortcut(key);
        if (commaShortcut != EditorChromeShortcut::kNone)
            return commaShortcut;

        return EditorChromeShortcutDetail::classifyUiScaleShortcut(key);
    }

    inline bool isEditorChromeShortcut(const juce::KeyPress& key)
    {
        return classifyEditorChromeShortcut(key) != EditorChromeShortcut::kNone;
    }

    /** Platform-local shortcut glyphs for menus / Settings (English UI copy).
        Build Mac symbols from juce_wchar code points — raw UTF-8 in `const char*`
        was being taken as Latin-1 (â / ¥ tofu) when copied into juce::String. */
    namespace EditorChromeShortcutLabels
    {
        inline juce::String macCommandGlyph()
        {
            return juce::String::charToString(static_cast<juce::juce_wchar>(0x2318)); // ⌘
        }

        inline juce::String macOptionGlyph()
        {
            return juce::String::charToString(static_cast<juce::juce_wchar>(0x2325)); // ⌥
        }

        inline juce::String openSettings()
        {
#if JUCE_MAC
            return macCommandGlyph() + " ,";
#else
            return "Ctrl+,";
#endif
        }

        inline juce::String openAudioMidi()
        {
#if JUCE_MAC
            return macOptionGlyph() + " " + macCommandGlyph() + " ,";
#else
            return "Ctrl+Alt+,";
#endif
        }

        inline juce::String uiScaleStep()
        {
#if JUCE_MAC
            return macCommandGlyph() + " +/-";
#else
            return "Ctrl+/-";
#endif
        }

        inline juce::String uiScaleReset()
        {
#if JUCE_MAC
            return macCommandGlyph() + " 0";
#else
            return "Ctrl+0";
#endif
        }

        inline juce::String uiScaleGroup()
        {
#if JUCE_MAC
            return macCommandGlyph() + " +/- / " + macCommandGlyph() + " 0";
#else
            return "Ctrl+/- / Ctrl+0";
#endif
        }
    }

    /** System UI font — PT Sans Narrow lacks ⌘ / ⌥ glyphs used in Mac shortcut hints. */
    inline juce::Font shortcutHintFont(float height)
    {
        return juce::Font(juce::FontOptions{}.withHeight(height));
    }
}
