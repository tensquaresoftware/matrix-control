#pragma once

/** Pure Hold-mapping mode decisions for Skin Editor (no JUCE Component deps). */
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
}
