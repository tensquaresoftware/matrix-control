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

        inline int numberPadKeyCodeForDigit(int digit)
        {
            switch (digit)
            {
                case 0: return juce::KeyPress::numberPad0;
                case 1: return juce::KeyPress::numberPad1;
                case 2: return juce::KeyPress::numberPad2;
                case 3: return juce::KeyPress::numberPad3;
                case 4: return juce::KeyPress::numberPad4;
                case 5: return juce::KeyPress::numberPad5;
                case 6: return juce::KeyPress::numberPad6;
                case 7: return juce::KeyPress::numberPad7;
                case 8: return juce::KeyPress::numberPad8;
                case 9: return juce::KeyPress::numberPad9;
                default: return 0;
            }
        }

        /** FR AZERTY / ISO unshifted glyphs that share the physical digit-row key. */
        inline bool matchesAzertyDigitPeer(const juce::KeyPress& key,
                                           int digit,
                                           juce::ModifierKeys command)
        {
            switch (digit)
            {
                case 0:
                {
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
                case 1:
                    return matchesAnyText(key, '&', command);
                case 2:
                {
                    constexpr int kEAcute = 0x00e9; // é
                    constexpr int kEAcuteUpper = 0x00c9; // É
                    return matchesAnyText(key, kEAcute, command)
                        || matchesAnyText(key, kEAcuteUpper, command)
                        || matchesKey(key, kEAcuteUpper, command, static_cast<juce::juce_wchar>(kEAcute));
                }
                default:
                    return false;
            }
        }

        /**
            Matches Cmd/Ctrl (+ optional Shift) with main-row digit, numpad digit,
            or AZERTY/ISO peers for the same physical key (0→à, 1→&, 2→é, …).
        */
        inline bool matchesDigitShortcut(const juce::KeyPress& key, int digit)
        {
            if (digit < 0 || digit > 9)
                return false;

            using juce::ModifierKeys;
            const auto command = ModifierKeys::commandModifier;
            const auto commandShift = command | ModifierKeys::shiftModifier;
            const int asciiDigit = static_cast<int>('0') + digit;
            const int numPad = numberPadKeyCodeForDigit(digit);

            return matchesAnyText(key, asciiDigit, command)
                || matchesAnyText(key, asciiDigit, commandShift)
                || matchesKey(key, numPad, command)
                || matchesKey(key, numPad, commandShift)
                || matchesAzertyDigitPeer(key, digit, command);
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

            if (matchesDigitShortcut(key, 0))
                return EditorChromeShortcut::kUiScaleReset;

            return EditorChromeShortcut::kNone;
        }
    }

    /**
        Matches platform chrome KeyPresses using keyCode + modifiers.
        Tolerates textCharacter == 0 (macOS clears it when Command is down).
        Accepts '=' and Shift+'=' / Shift+'+' as zoom-in peers of '+' (layouts without a dedicated + key).
        UI Scale reset also accepts the physical main-row "0" key on AZERTY/ISO (à / ò) and Cmd/Ctrl+Shift+0.
        Skin selection uses Cmd/Ctrl+1..N for SkinVariants ids (1=Black, 2=Cream, …).
    */
    inline EditorChromeShortcut classifyEditorChromeShortcut(const juce::KeyPress& key)
    {
        const auto commaShortcut = EditorChromeShortcutDetail::classifyCommaShortcut(key);
        if (commaShortcut != EditorChromeShortcut::kNone)
            return commaShortcut;

        return EditorChromeShortcutDetail::classifyUiScaleShortcut(key);
    }

    /** True when digit maps to a shipping skin id (1=Black, 2=Cream). Extend when adding skins. */
    inline bool isAssignedSkinVariantId(int skinVariantId)
    {
        return skinVariantId == 1 || skinVariantId == 2;
    }

    /**
        Returns SkinVariants id (1=Black, 2=Cream, …) for Cmd/Ctrl+digit, or 0 if none / unassigned.
        Future skins keep using the next digit; bump isAssignedSkinVariantId when they ship.
    */
    inline int classifySkinVariantShortcut(const juce::KeyPress& key)
    {
        for (int digit = 1; digit <= 9; ++digit)
        {
            if (! EditorChromeShortcutDetail::matchesDigitShortcut(key, digit))
                continue;

            if (isAssignedSkinVariantId(digit))
                return digit;
        }

        return 0;
    }

    inline bool isEditorChromeShortcut(const juce::KeyPress& key)
    {
        return classifyEditorChromeShortcut(key) != EditorChromeShortcut::kNone
            || classifySkinVariantShortcut(key) != 0;
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

        inline juce::String skinVariant(int digit)
        {
#if JUCE_MAC
            return macCommandGlyph() + " " + juce::String(digit);
#else
            return "Ctrl+" + juce::String(digit);
#endif
        }
    }

    /** System UI font — PT Sans Narrow lacks ⌘ / ⌥ glyphs used in Mac shortcut hints. */
    inline juce::Font shortcutHintFont(float height)
    {
        return juce::Font(juce::FontOptions{}.withHeight(height));
    }
}
