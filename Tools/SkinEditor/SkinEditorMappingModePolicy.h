#pragma once

#include <juce_core/juce_core.h>

/** Pure Hold-mapping mode decisions for Skin Editor (juce_core only, no Component deps). */
namespace SkinEditorMappingPolicy
{
    enum class Mode
    {
        kIdle,
        kHeldColour,
        kArmedRow,
        kEditingColour // Mapped colour cell: edit in COLOR; not a bind gesture
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

    /**
     * Pending (uncalibrated) pastilles may be selected / edited, but not mapped.
     * Same-RGB twins may exist in the palette; bind is blocked when another mapped
     * pastille already uses this RGB (see MainComponent collision probe).
     */
    inline bool canBindPastille(bool pending, bool sameRgbMappedCollision = false) noexcept
    {
        return ! pending && ! sameRgbMappedCollision;
    }

    /** bindBlocked = ! canBindPastille(pending, sameRgbMapped) — held/edit instead of bind. */
    inline PastilleIntent pastilleClick(Mode mode,
                                        bool canEdit,
                                        bool nameEmpty,
                                        bool bindBlocked = false) noexcept
    {
        if (nameEmpty)
            return PastilleIntent::kIgnoreEmpty;
        if (! canEdit)
            return PastilleIntent::kSelectReadOnly;
        if (mode == Mode::kArmedRow)
        {
            if (bindBlocked)
                return PastilleIntent::kEnterOrChangeHeldColour;
            return PastilleIntent::kBindArmedRow;
        }
        // Idle, HeldColour, EditingColour: take / retarget the held colour.
        return PastilleIntent::kEnterOrChangeHeldColour;
    }

    /** bindBlocked — Ignore instead of bind (pending or same-RGB mapped collision). */
    inline RowIntent rowClick(Mode mode, bool canEdit, bool bindBlocked = false) noexcept
    {
        if (! canEdit)
            return RowIntent::kIgnore;
        if (mode == Mode::kHeldColour)
        {
            if (bindBlocked)
                return RowIntent::kIgnore;
            return RowIntent::kBindHeldColour;
        }
        // Idle, ArmedRow, EditingColour: arm / re-arm the row.
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
        kSelectForEdit  // Enter EditingColour; select pastille for COLOR / SV
    };

    inline MappedColourIntent mappedColourClick(bool cellUnbound, bool pastilleResolved) noexcept
    {
        if (cellUnbound || ! pastilleResolved)
            return MappedColourIntent::kNoOp;
        return MappedColourIntent::kSelectForEdit;
    }

    /**
     * OPERATION copy (ASCII). Layout paints: role + [swatch] + identity + arrow + next.
     * Em dash is avoided (font / ASCII policy); the arrow is a painted picto.
     */
    // Trailing space after ':' — layout uses a tight gap, not kChromeBadgeGap.
    inline constexpr const char* kSelectedColourRole = "Selected colour: ";
    inline constexpr const char* kEditingColourRole = "Editing colour: ";
    inline constexpr const char* kSelectedTargetRole = "Selected target: ";
    inline constexpr const char* kHeldColourNextStep =
        "Next: click a Property or State cell in a Mappings row.";
    inline constexpr const char* kArmedRowNextStep = "Next: click a colour in Palette.";
    inline constexpr const char* kEditingColourNextStep =
        "Next: adjust in COLOR (affects every mapped use).";
    inline constexpr const char* kHoldColourOnRowsSuffix =
        " [Hold colour] is active: keep clicking rows.";
    inline constexpr const char* kHoldColourOnColoursSuffix =
        " [Hold colour] is active: keep clicking colours.";
    inline constexpr const char* kCalibrateBeforeMappingMessage =
        "Calibrate this colour in COLOR before mapping.";
    inline constexpr const char* kHeldPendingNextStep =
        "Next: adjust in COLOR before mapping.";
    /** OPERATION badge when held colour RGB is already mapped under another pastille name. */
    inline constexpr const char* kSameColourBadge = "SAME COLOUR";
    inline constexpr const char* kSameColourMappedNextStep =
        "Next: adjust in COLOR before mapping (same RGB already mapped).";
    inline constexpr const char* kSameColourMappedStatusMessage =
        "Adjust in COLOR: this RGB is already mapped under another name.";

    /** Append Hold colour coaching when the Hold colour toggle is on (bind gestures only). */
    inline juce::String withHoldColourSuffix(const juce::String& nextStep,
                                             bool holdColourOn,
                                             bool keepClickingRows) noexcept
    {
        if (! holdColourOn)
            return nextStep;
        return nextStep
               + (keepClickingRows ? kHoldColourOnRowsSuffix : kHoldColourOnColoursSuffix);
    }
}
