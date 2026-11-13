#pragma once

#include <algorithm>
#include <vector>

#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "Tools/SkinDesigner/SkinDesignerAlertWindow.h"

namespace SkinDesignerUi
{
    /** OPERATION badge + coaching when no designer skin is selected. */
    inline constexpr const char* kNoDesignerSkinBadge = "START";
    inline constexpr const char* kNoDesignerSkinHint =
        "Load a JSON file or create a skin to start.";

    /** Quit dirty dialog copy (Save / Discard / Cancel). */
    inline constexpr const char* kQuitUnsavedTitle = "Unsaved Designer changes";
    inline constexpr const char* kQuitUnsavedMessage =
        "Save the Designer skins, discard changes, or cancel?";
    inline constexpr const char* kQuitSaveButton = "Save";
    inline constexpr const char* kQuitDiscardButton = "Discard";
    inline constexpr const char* kQuitCancelButton = "Cancel";

    /** Status-bar prefixes for file actions (followed by the full path). */
    inline constexpr const char* kStatusLoaded = "Loaded: ";
    inline constexpr const char* kStatusSaved = "Saved: ";
    inline constexpr const char* kStatusSavedAs = "Saved as: ";

    inline bool isFilePathStatus(const juce::String& status)
    {
        return status.startsWith(kStatusLoaded) || status.startsWith(kStatusSaved)
               || status.startsWith(kStatusSavedAs);
    }

    /** True when quit must offer Save / Discard / Cancel. */
    inline bool shouldPromptQuitForUnsavedChanges(bool designerDirty,
                                                  bool hasDesignerSkin) noexcept
    {
        return designerDirty && hasDesignerSkin;
    }

    struct AskTextRequest
    {
        juce::String title;
        juce::String message;
        juce::String initial;
        int maxChars = 0; // > 0 applies TextEditor::setInputRestrictions
        /** Empty = length-only; otherwise printable charset passed to setInputRestrictions. */
        juce::String allowedCharacters;
    };

    /** Modal text prompt. Returns false on Cancel (out unchanged). */
    inline bool askText(const AskTextRequest& request, juce::String& out)
    {
        SkinDesignerChrome::AlertWindow window(request.title, request.message,
                                             juce::MessageBoxIconType::QuestionIcon);
        window.addTextEditor("value", request.initial, "Value");
        window.addButton("OK", 1, juce::KeyPress(juce::KeyPress::returnKey));
        window.addButton("Cancel", 0, juce::KeyPress(juce::KeyPress::escapeKey));
        if (auto* editor = window.getTextEditor("value"))
        {
            editor->setSelectAllWhenFocused(true);
            if (request.maxChars > 0)
                editor->setInputRestrictions(request.maxChars, request.allowedCharacters);
        }

        // Focus only sticks once the modal is on-screen and pumping messages.
        const juce::Component::SafePointer<SkinDesignerChrome::AlertWindow> safe(&window);
        juce::MessageManager::callAsync([safe]
        {
            if (safe == nullptr)
                return;
            safe->toFront(true);
            if (auto* editor = safe->getTextEditor("value"))
                editor->grabKeyboardFocus();
        });

        if (window.runModalLoop() != 1)
            return false;
        out = window.getTextEditorContents("value").trim();
        return true;
    }

    /** Legacy: Cancel and empty OK both return {}. Prefer AskTextRequest when clearing matters. */
    inline juce::String askText(const juce::String& title,
                                const juce::String& message,
                                const juce::String& initial)
    {
        juce::String out;
        if (! askText({ title, message, initial, 0, {} }, out))
            return {};
        return out;
    }

    /** Skin Designer UI: drop leading chart `k` (kBrandCream → BrandCream). */
    inline juce::String displayPastilleName(const juce::String& storedName)
    {
        return (storedName.startsWithChar('k') && storedName.length() > 1)
                   ? storedName.substring(1)
                   : storedName;
    }

    /** Product combo: 1=BLACK/GREEN … 6=CREAM/MINIMAL (logo order). */
    inline juce::String productSkinIdFromComboId(int comboId)
    {
        switch (comboId)
        {
            case 2: return TSS::DevSkin::kCompiledBlackChromaId;
            case 3: return TSS::DevSkin::kCompiledBlackMinimalId;
            case 4: return TSS::DevSkin::kCompiledCreamId;
            case 5: return TSS::DevSkin::kCompiledCreamChromaId;
            case 6: return TSS::DevSkin::kCompiledCreamMinimalId;
            default: return TSS::DevSkin::kCompiledBlackId;
        }
    }

    inline int productComboIdFromSkinId(const juce::String& skinId)
    {
        if (skinId == TSS::DevSkin::kCompiledBlackChromaId
            || skinId == "compiled:black-duo")
            return 2;
        if (skinId == TSS::DevSkin::kCompiledBlackMinimalId)
            return 3;
        if (skinId == TSS::DevSkin::kCompiledCreamId)
            return 4;
        if (skinId == TSS::DevSkin::kCompiledCreamChromaId)
            return 5;
        if (skinId == TSS::DevSkin::kCompiledCreamMinimalId)
            return 6;
        return 1;
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

    /** Preferred if free, else "preferred 2", "preferred 3", … (palette names). */
    inline juce::String suggestUniquePastilleName(const TSS::DevSkin::NamedSkin& skin,
                                                 const juce::String& preferred)
    {
        const auto base = preferred.trim();
        if (base.isEmpty())
            return {};
        if (! pastilleNameTaken(skin, base))
            return base;

        for (int suffix = 2;; ++suffix)
        {
            const auto candidate = base + " " + juce::String(suffix);
            if (! pastilleNameTaken(skin, candidate))
                return candidate;
        }
    }

    /**
     * True when another pastille already listed in skin.mappings shares this
     * pastille's ARGB (own mapped uses do not count).
     */
    inline bool isPastilleArgbMappedUnderOtherName(const TSS::DevSkin::NamedSkin& skin,
                                                   const juce::String& pastilleName)
    {
        if (pastilleName.isEmpty())
            return false;

        juce::uint32 candidateArgb = 0;
        bool found = false;
        for (const auto& pastille : skin.palette)
        {
            if (pastille.name == pastilleName)
            {
                candidateArgb = pastille.argb;
                found = true;
                break;
            }
        }
        if (! found)
            return false;

        for (const auto& mapping : skin.mappings)
        {
            const auto& mappedName = mapping.second;
            if (mappedName == pastilleName)
                continue;

            for (const auto& pastille : skin.palette)
                if (pastille.name == mappedName && pastille.argb == candidateArgb)
                    return true;
        }
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

    /** Workspace name in the default pack file stem (Trials-style pack). */
    inline constexpr const char* kDefaultWorkspaceName = "Trials";

    /** Chooser results may lack an extension; saved packs are always .json. */
    inline juce::File withJsonExtension(const juce::File& file)
    {
        return file.hasFileExtension("json") ? file
                                             : juce::File(file.getFullPathName() + ".json");
    }

    /**
     * Suggested Save / Save as path: "Matrix-Control Skin Designer - {workspace}.json".
     * Uses preferredParentPath when it is an existing directory; else Documents.
     */
    inline juce::File defaultPackFile(const juce::String& workspaceName,
                                       const juce::String& preferredParentPath = {})
    {
        return resolveSkinJsonDirectory(preferredParentPath)
            .getChildFile("Matrix-Control Skin Designer - "
                          + sanitizeSkinNameForExportFileStem(workspaceName) + ".json");
    }

    /** Advance i/j over matching digit runs; returns <0 / 0 / >0 like strcmp. */
    inline int compareNaturalDigitRuns(const juce::String& left,
                                       int& leftIndex,
                                       const juce::String& right,
                                       int& rightIndex) noexcept
    {
        int leftEnd = leftIndex;
        int rightEnd = rightIndex;
        while (leftEnd < left.length() && juce::CharacterFunctions::isDigit(left[leftEnd]))
            ++leftEnd;
        while (rightEnd < right.length() && juce::CharacterFunctions::isDigit(right[rightEnd]))
            ++rightEnd;

        const auto leftValue = left.substring(leftIndex, leftEnd).getLargeIntValue();
        const auto rightValue = right.substring(rightIndex, rightEnd).getLargeIntValue();
        leftIndex = leftEnd;
        rightIndex = rightEnd;
        if (leftValue < rightValue)
            return -1;
        if (leftValue > rightValue)
            return 1;
        return 0;
    }

    /** Case-insensitive natural order so "Name 10" sorts after "Name 9". */
    inline int compareDesignerSkinNamesNatural(const juce::String& left,
                                             const juce::String& right) noexcept
    {
        int i = 0;
        int j = 0;
        while (i < left.length() && j < right.length())
        {
            const bool leftDigit = juce::CharacterFunctions::isDigit(left[i]);
            const bool rightDigit = juce::CharacterFunctions::isDigit(right[j]);
            if (leftDigit && rightDigit)
            {
                const int cmp = compareNaturalDigitRuns(left, i, right, j);
                if (cmp != 0)
                    return cmp;
                continue;
            }

            const auto leftUpper = juce::CharacterFunctions::toUpperCase(left[i]);
            const auto rightUpper = juce::CharacterFunctions::toUpperCase(right[j]);
            if (leftUpper != rightUpper)
                return leftUpper < rightUpper ? -1 : 1;
            ++i;
            ++j;
        }
        return (left.length() - i) - (right.length() - j);
    }

    /** Dev skins sorted for the Designer combo (natural name order). */
    inline std::vector<const TSS::DevSkin::NamedSkin*> sortedDesignerSkins(
        const TSS::DevSkin::Layer& layer)
    {
        std::vector<const TSS::DevSkin::NamedSkin*> designerSkins;
        for (const auto& skin : layer.skins())
        {
            if (skin.kind == TSS::DevSkin::SkinKind::kDev)
                designerSkins.push_back(&skin);
        }
        std::sort(designerSkins.begin(), designerSkins.end(),
                  [](const TSS::DevSkin::NamedSkin* a, const TSS::DevSkin::NamedSkin* b)
                  {
                      return compareDesignerSkinNamesNatural(a->name, b->name) < 0;
                  });
        return designerSkins;
    }
}
