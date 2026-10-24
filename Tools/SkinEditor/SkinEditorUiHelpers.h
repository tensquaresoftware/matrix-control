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
}
