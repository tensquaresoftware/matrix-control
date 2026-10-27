#pragma once

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"

namespace SkinEditorUi
{
    /** Palette / Mappings coaching when no editor skin is selected. */
    inline constexpr const char* kNoEditorSkinHint =
        "Create a skin or duplicate the product skin to start.";

    inline juce::String askText(const juce::String& title,
                                const juce::String& message,
                                const juce::String& initial)
    {
        juce::AlertWindow window(title, message, juce::MessageBoxIconType::QuestionIcon);
        window.addTextEditor("value", initial, "Value");
        window.addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        if (auto* editor = window.getTextEditor("value"))
            editor->setSelectAllWhenFocused(true);

        // Focus only sticks once the modal is on-screen and pumping messages.
        const juce::Component::SafePointer<juce::AlertWindow> safe(&window);
        juce::MessageManager::callAsync([safe]
        {
            if (safe == nullptr)
                return;
            safe->toFront(true);
            if (auto* editor = safe->getTextEditor("value"))
                editor->grabKeyboardFocus();
        });

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

    inline bool pastilleNameTaken(const TSS::DevSkin::NamedSkin& skin, const juce::String& name)
    {
        for (const auto& pastille : skin.palette)
            if (pastille.name.equalsIgnoreCase(name))
                return true;
        return false;
    }

    /** Next free "Temp N" (case-insensitive) in this skin's palette. */
    inline juce::String suggestUniqueTempPastilleName(const TSS::DevSkin::NamedSkin& skin)
    {
        for (int n = 1;; ++n)
        {
            const auto candidate = "Temp " + juce::String(n);
            if (! pastilleNameTaken(skin, candidate))
                return candidate;
        }
    }

    inline bool pastilleArgbTaken(const TSS::DevSkin::NamedSkin& skin, juce::uint32 argb)
    {
        for (const auto& pastille : skin.palette)
            if (pastille.argb == argb)
                return true;
        return false;
    }

    /** Opaque random ARGB that does not collide with any pastille in this skin. */
    inline juce::uint32 suggestUniqueOpaqueArgb(const TSS::DevSkin::NamedSkin& skin)
    {
        constexpr int kRandomAttempts = 64;
        auto& rng = juce::Random::getSystemRandom();
        for (int attempt = 0; attempt < kRandomAttempts; ++attempt)
        {
            const auto argb = 0xff000000u
                              | (static_cast<juce::uint32>(rng.nextInt()) & 0x00ffffffu);
            if (! pastilleArgbTaken(skin, argb))
                return argb;
        }

        // Deterministic opaque scan — always terminates (uint wrap-safe).
        for (juce::uint32 rgb = 0;; ++rgb)
        {
            const auto argb = 0xff000000u | rgb;
            if (! pastilleArgbTaken(skin, argb))
                return argb;
            if (rgb == 0x00ffffffu)
                break;
        }
        return 0xff000000u;
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

    /** Existing directory, or Documents when preferredParentPath is missing/invalid. */
    inline juce::File resolveSkinJsonDirectory(const juce::String& preferredParentPath = {})
    {
        juce::File parent(preferredParentPath);
        if (parent.isDirectory())
            return parent;
        return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
    }

    /**
     * Suggested export path: "Matrix-Control Skin Editor - {name}.json".
     * Uses preferredParentPath when it is an existing directory; else Documents.
     */
    inline juce::File defaultEditorSkinExportFile(const juce::String& skinName,
                                                  const juce::String& preferredParentPath = {})
    {
        return resolveSkinJsonDirectory(preferredParentPath)
            .getChildFile("Matrix-Control Skin Editor - "
                          + sanitizeSkinNameForExportFileStem(skinName) + ".json");
    }
}
