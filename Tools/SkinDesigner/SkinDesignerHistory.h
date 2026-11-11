#pragma once

#include <cstddef>
#include <deque>
#include <vector>

#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinTypes.h"

namespace SkinDesignerHistory
{
    /** Whole Designer document at one point in time (restored through Layer::replaceAll). */
    struct Snapshot
    {
        std::vector<TSS::DevSkin::NamedSkin> skins;
        juce::String activeSkinId;
        juce::String designerSkinId;
    };

    /** Compare shows Product read-only: document Undo/Redo stays off while Compare is on. */
    inline bool documentHistoryAllowed(bool compareProductView) noexcept
    {
        return ! compareProductView;
    }

    /**
     * Hex/RGB and modal rename keep their own Undo while a TextEditor has focus.
     * Pass juce::Component::getCurrentlyFocusedComponent() from the Session key handler.
     */
    inline bool shouldDeferDocumentUndoRedoToTextField(juce::Component* focused) noexcept
    {
        if (focused == nullptr)
            return false;
        return dynamic_cast<juce::TextEditor*>(focused) != nullptr
               || focused->findParentComponentOfClass<juce::TextEditor>() != nullptr;
    }

    /**
     * Snapshot-based Undo/Redo stack for Designer documents.
     *
     * The history always holds the current committed state. A change records the
     * state AFTER the edit; the previous current state moves to the Undo stack.
     * One gesture (colour drag, nudge session) produces at most one Undo step.
     */
    class History
    {
    public:
        static constexpr std::size_t kDefaultMaxUndoSteps = 100;

        explicit History(std::size_t maxUndoSteps = kDefaultMaxUndoSteps);

        /** Drop all steps; the given snapshot becomes the current state and the clean marker. */
        void reset(Snapshot baseline);

        /** Commit the state after an edit. Clears Redo; coalesces inside an open gesture. */
        void recordChange(Snapshot after);

        void beginGesture() noexcept;
        void endGesture() noexcept;
        bool isGestureActive() const noexcept { return gestureActive_; }

        bool canUndo() const noexcept { return ! past_.empty(); }
        bool canRedo() const noexcept { return ! future_.empty(); }

        /** Moves one step back. Returns the state to restore, or nullptr when Undo is empty. */
        const Snapshot* undo();
        /** Moves one step forward. Returns the state to restore, or nullptr when Redo is empty. */
        const Snapshot* redo();

        const Snapshot& current() const noexcept { return current_.snapshot; }

        /**
         * Update the selection ids of the current state after a silent select.
         * No new step, no change to skins or to the clean marker.
         */
        void syncCurrentSelection(const juce::String& activeSkinId,
                                  const juce::String& designerSkinId);

        /** Remember the current state as the last saved one (Save / Save as). */
        void markClean() noexcept;
        /** True when the current state is the saved one (Save is not needed). */
        bool isAtCleanMarker() const noexcept { return current_.stateId == cleanStateId_; }

        std::size_t undoDepth() const noexcept { return past_.size(); }
        std::size_t redoDepth() const noexcept { return future_.size(); }

    private:
        struct Entry
        {
            Snapshot snapshot;
            juce::int64 stateId = 0;
        };

        juce::int64 nextStateId() noexcept { return ++lastStateId_; }
        void closeGestureStep() noexcept;

        std::size_t maxUndoSteps_;
        std::deque<Entry> past_;
        std::deque<Entry> future_;
        Entry current_;
        juce::int64 lastStateId_ = 0;
        juce::int64 cleanStateId_ = 0;
        bool gestureActive_ = false;
        bool gestureStepPushed_ = false;
    };
}
