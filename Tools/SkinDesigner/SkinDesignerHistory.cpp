#include "Tools/SkinDesigner/SkinDesignerHistory.h"

#include <utility>

namespace SkinDesignerHistory
{
    History::History(std::size_t maxUndoSteps)
        : maxUndoSteps_(maxUndoSteps > 0 ? maxUndoSteps : 1)
    {
    }

    void History::reset(Snapshot baseline)
    {
        past_.clear();
        future_.clear();
        closeGestureStep();
        gestureActive_ = false;
        current_ = { std::move(baseline), nextStateId() };
        cleanStateId_ = current_.stateId;
    }

    void History::recordChange(Snapshot after)
    {
        const bool coalesce = gestureActive_ && gestureStepPushed_;
        if (! coalesce)
        {
            past_.push_back(std::move(current_));
            while (past_.size() > maxUndoSteps_)
                past_.pop_front();
            future_.clear();
            gestureStepPushed_ = gestureActive_;
        }
        current_ = { std::move(after), nextStateId() };
    }

    void History::beginGesture() noexcept
    {
        gestureActive_ = true;
        gestureStepPushed_ = false;
    }

    void History::endGesture() noexcept
    {
        closeGestureStep();
        gestureActive_ = false;
    }

    void History::closeGestureStep() noexcept
    {
        gestureStepPushed_ = false;
    }

    const Snapshot* History::undo()
    {
        endGesture();
        if (past_.empty())
            return nullptr;

        future_.push_back(std::move(current_));
        current_ = std::move(past_.back());
        past_.pop_back();
        return &current_.snapshot;
    }

    const Snapshot* History::redo()
    {
        endGesture();
        if (future_.empty())
            return nullptr;

        past_.push_back(std::move(current_));
        current_ = std::move(future_.back());
        future_.pop_back();
        return &current_.snapshot;
    }

    void History::syncCurrentSelection(const juce::String& activeSkinId,
                                       const juce::String& designerSkinId)
    {
        current_.snapshot.activeSkinId = activeSkinId;
        current_.snapshot.designerSkinId = designerSkinId;
    }

    void History::markClean() noexcept
    {
        cleanStateId_ = current_.stateId;
    }
}
