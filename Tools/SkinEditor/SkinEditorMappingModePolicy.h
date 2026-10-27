#pragma once

#include <juce_core/juce_core.h>

/** Pure Hold-mapping mode decisions for Skin Editor (juce_core only, no Component deps). */
namespace SkinEditorMappingPolicy
{
    enum class Mode
    {
        kIdle,
        kHeldColour,
        kArmedRow
    };

    enum class PastilleIntent
    {
        kIgnoreEmpty,
        kSelectReadOnly,
        kBindArmedRow,
        kEnterOrChangeHeldColour
    };

    enum class RowIntent
    {
        kIgnore,
        kBindHeldColour,
        kEnterOrChangeArmedRow
    };

    inline PastilleIntent pastilleClick(Mode mode, bool canEdit, bool nameEmpty) noexcept
    {
        if (nameEmpty)
            return PastilleIntent::kIgnoreEmpty;
        if (! canEdit)
            return PastilleIntent::kSelectReadOnly;
        if (mode == Mode::kArmedRow)
            return PastilleIntent::kBindArmedRow;
        return PastilleIntent::kEnterOrChangeHeldColour;
    }

    inline RowIntent rowClick(Mode mode, bool canEdit) noexcept
    {
        if (! canEdit)
            return RowIntent::kIgnore;
        if (mode == Mode::kHeldColour)
            return RowIntent::kBindHeldColour;
        return RowIntent::kEnterOrChangeArmedRow;
    }

    /** After a successful (or no-op same) write: Hold keeps gesture mode; else Idle. */
    inline Mode afterSuccessfulWrite(Mode mode, bool hold) noexcept
    {
        return hold ? mode : Mode::kIdle;
    }

    inline Mode afterEscape() noexcept { return Mode::kIdle; }

    inline bool mappingWitnessVisible(Mode mode) noexcept
    {
        return mode != Mode::kIdle;
    }

    /** Mapped colour column vs Property/State (x in content coords; mappedColumnX from layout). */
    inline bool isMappedColourColumn(int x, int mappedColumnX) noexcept
    {
        return x >= mappedColumnX;
    }

    enum class MappedColourIntent
    {
        kNoOp,          // Unbound cell or unresolved pastille name
        kSelectForEdit  // Clear mapping mode to Idle; select pastille for COLOR / SV
    };

    inline MappedColourIntent mappedColourClick(bool cellUnbound, bool pastilleResolved) noexcept
    {
        if (cellUnbound || ! pastilleResolved)
            return MappedColourIntent::kNoOp;
        return MappedColourIntent::kSelectForEdit;
    }

    /** OPERATION next-step copy (ASCII). Prefix includes the active item identity. */
    inline constexpr const char* kHeldColourNextStep = "Click a Property or State cell to map";
    inline constexpr const char* kArmedRowNextStep = "Click a palette colour to map";

    inline juce::String heldColourWitnessPrefix(const juce::String& pastilleName)
    {
        return "Hold: " + pastilleName;
    }

    inline juce::String armedRowWitnessPrefix(const juce::String& object,
                                              const juce::String& property,
                                              const juce::String& state)
    {
        return "Target: " + object + " > " + property + " > " + state;
    }
}
