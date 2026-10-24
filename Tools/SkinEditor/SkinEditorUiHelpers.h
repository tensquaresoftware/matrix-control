#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

namespace SkinEditorUi
{
    inline juce::String askText(const juce::String& title,
                                const juce::String& message,
                                const juce::String& initial)
    {
        juce::AlertWindow window(title, message, juce::MessageBoxIconType::QuestionIcon);
        window.addTextEditor("value", initial, "Value");
        window.addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        if (window.runModalLoop() != 1)
            return {};
        return window.getTextEditorContents("value").trim();
    }

    inline bool skinNameTaken(const TSS::DevSkin::Layer& layer, const juce::String& name)
    {
        for (const auto& skin : layer.skins())
            if (skin.name.equalsIgnoreCase(name))
                return true;
        return false;
    }

    /** Preferred if free, else "preferred 2", "preferred 3", … */
    inline juce::String suggestUniqueSkinName(const TSS::DevSkin::Layer& layer,
                                              const juce::String& preferred)
    {
        const auto base = preferred.trim();
        if (base.isEmpty())
            return {};
        if (! skinNameTaken(layer, base))
            return base;

        for (int suffix = 2;; ++suffix)
        {
            const auto candidate = base + " " + juce::String(suffix);
            if (! skinNameTaken(layer, candidate))
                return candidate;
        }
    }

    inline bool isOsForbiddenFileNameChar(juce::juce_wchar character) noexcept
    {
        constexpr juce::juce_wchar kForbidden[] = {
            '/', '\\', ':', '*', '?', '"', '<', '>', '|'
        };
        if (character < 32)
            return true;
        for (const auto forbidden : kForbidden)
            if (character == forbidden)
                return true;
        return false;
    }

    inline juce::String collapseSpacesAndTrimDots(juce::String text)
    {
        while (text.contains("  "))
            text = text.replace("  ", " ");
        text = text.trim();
        while (text.endsWithChar('.'))
            text = text.dropLastCharacters(1).trimEnd();
        return text;
    }

    /**
     * OS-safe file-stem fragment from a free-text skin name.
     * Forbidden path chars become spaces (e.g. "Skin: Night" -> "Skin Night");
     * whitespace is collapsed. Empty result falls back to "Untitled".
     */
    inline juce::String sanitizeSkinNameForExportFileStem(juce::String name)
    {
        name = name.trim();
        juce::String out;
        out.preallocateBytes(static_cast<size_t>(name.length()));
        for (int i = 0; i < name.length(); ++i)
            out += isOsForbiddenFileNameChar(name[i]) ? juce::juce_wchar(' ') : name[i];

        out = collapseSpacesAndTrimDots(std::move(out));
        return out.isNotEmpty() ? out : juce::String("Untitled");
    }

    /** Suggested Documents path: "Matrix-Control Skin Editor - {name}.json". */
    inline juce::File defaultEditorSkinExportFile(const juce::String& skinName)
    {
        return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
            .getChildFile("Matrix-Control Skin Editor - "
                          + sanitizeSkinNameForExportFileStem(skinName) + ".json");
    }
}
