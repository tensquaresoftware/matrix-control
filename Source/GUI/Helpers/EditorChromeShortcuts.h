#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "Shared/Definitions/PluginIDs.h"

namespace TSS
{
    /** Classification of Settings / UI Scale chrome shortcuts. */
    enum class EditorChromeShortcut
    {
        kNone,
        kOpenSettings,
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
            // keyCode + optional textCharacter (0 = matchesAnyText only).
            // 0→à/ò, 1→&, 2→é, 3→", 4→', 5→(, 6→§.
            struct Peer { int keyCode; juce::juce_wchar textCharacter; };
            static constexpr Peer kPeers[][4] = {
                { { 0x00e0, 0 }, { 0x00c0, 0x00e0 }, { 0x00f2, 0 }, { 0x00d2, 0x00f2 } },
                { { '&', 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
                { { 0x00e9, 0 }, { 0x00c9, 0x00e9 }, { 0, 0 }, { 0, 0 } },
                { { '"', 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
                { { '\'', 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
                { { '(', 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } },
                { { 0x00a7, 0 }, { 0, 0 }, { 0, 0 }, { 0, 0 } }
            };

            if (digit < 0 || digit > 6)
                return false;

            for (const auto& peer : kPeers[digit])
            {
                if (peer.keyCode == 0)
                    continue;

                if (peer.textCharacter != 0
                    && matchesKey(key, peer.keyCode, command, peer.textCharacter))
                    return true;

                if (matchesAnyText(key, peer.keyCode, command))
                    return true;
            }

            return false;
        }

        /**
            Matches Cmd/Ctrl (+ optional Shift) with main-row digit, numpad digit,
            or AZERTY/ISO peers for the same physical key (0→à, 1→&, 2→é, 3→", 4→', 5→(, 6→§).
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
                || matchesAzertyDigitPeer(key, digit, command)
                || matchesAzertyDigitPeer(key, digit, commandShift);
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
        Skin selection uses Cmd/Ctrl+1..N in combo order (1=BLACK, 2=BLACK/CHROMA, 3=CREAM, …).
    */
    inline EditorChromeShortcut classifyEditorChromeShortcut(const juce::KeyPress& key)
    {
        const auto commaShortcut = EditorChromeShortcutDetail::classifyCommaShortcut(key);
        if (commaShortcut != EditorChromeShortcut::kNone)
            return commaShortcut;

        return EditorChromeShortcutDetail::classifyUiScaleShortcut(key);
    }

    /** Maps shortcut digit (combo order) to SkinVariants id. Digits 1-6 only (bases). */
    inline int skinVariantIdForShortcutDigit(int digit)
    {
        using namespace PluginIDs::Settings::SkinVariants;
        switch (digit)
        {
            case 1: return kBlack;
            case 2: return kBlackChroma;
            case 3: return kBlackMinimal;
            case 4: return kCream;
            case 5: return kCreamChroma;
            case 6: return kCreamMinimal;
            default: return 0;
        }
    }

    /** True when digit is assigned to a shipping skin in combo order. */
    inline bool isAssignedSkinVariantShortcutDigit(int digit)
    {
        return skinVariantIdForShortcutDigit(digit) != 0;
    }

    /**
        Returns SkinVariants id for Cmd/Ctrl+digit in combo order, or 0 if none / unassigned.
        Digits follow logo order (1=BLACK/GREEN … 6=CREAM/MINIMAL); ids stay stable for prefs
        (digit 5 = CREAM/CHROMA id 6, digit 6 = CREAM/MINIMAL id 5).
    */
    inline int classifySkinVariantShortcut(const juce::KeyPress& key)
    {
        for (int digit = 1; digit <= 9; ++digit)
        {
            if (! EditorChromeShortcutDetail::matchesDigitShortcut(key, digit))
                continue;

            if (const int skinId = skinVariantIdForShortcutDigit(digit); skinId != 0)
                return skinId;
        }

        return 0;
    }

    /** Cmd/Ctrl(+Shift)+arrow skin cycle: direction -1 = previous, +1 = next; 0 = none. */
    struct SkinArrowCycleShortcut
    {
        int direction = 0;
        bool global = false;

        bool isValid() const noexcept { return direction != 0; }
    };

    inline SkinArrowCycleShortcut classifySkinArrowCycleShortcut(const juce::KeyPress& key)
    {
        const auto mods = key.getModifiers();
        if (! mods.isCommandDown() || mods.isAltDown())
            return {};

        // Up/left = previous; down/right = next (paired axes, not up/right vs down/left).
        const int code = key.getKeyCode();
        int direction = 0;
        if (code == juce::KeyPress::upKey || code == juce::KeyPress::leftKey)
            direction = -1;
        else if (code == juce::KeyPress::downKey || code == juce::KeyPress::rightKey)
            direction = 1;
        else
            return {};

        return { direction, mods.isShiftDown() };
    }

    inline bool isEditorChromeShortcut(const juce::KeyPress& key)
    {
        return classifyEditorChromeShortcut(key) != EditorChromeShortcut::kNone
            || classifySkinVariantShortcut(key) != 0
            || classifySkinArrowCycleShortcut(key).isValid();
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

        inline juce::String skinFamilyCycle()
        {
#if JUCE_MAC
            return macCommandGlyph() + " arrows";
#else
            return "Ctrl+arrows";
#endif
        }

        inline juce::String skinGlobalCycle()
        {
#if JUCE_MAC
            return macCommandGlyph() + " Shift+arrows";
#else
            return "Ctrl+Shift+arrows";
#endif
        }
    }

    /** System UI font — PT Sans Narrow lacks ⌘ / ⌥ glyphs used in Mac shortcut hints. */
    inline juce::Font shortcutHintFont(float height)
    {
        return juce::Font(juce::FontOptions{}.withHeight(height));
    }
}
