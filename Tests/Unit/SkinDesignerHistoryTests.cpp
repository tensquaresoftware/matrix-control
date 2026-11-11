#include <juce_core/juce_core.h>
#include <juce_gui_basics/juce_gui_basics.h>

#include "GUI/Skins/Dev/DevSkinLayer.h"
#include "Tools/SkinDesigner/SkinDesignerHistory.h"

class SkinDesignerHistoryTests : public juce::UnitTest
{
public:
    SkinDesignerHistoryTests()
        : juce::UnitTest("SkinDesignerHistory")
    {
    }

    void runTest() override
    {
        beginTest("history_editThenUndoRedo_walksStates");
        assertEditThenUndoRedo();
        beginTest("history_undoOnEmpty_isNoOp");
        assertUndoRedoOnEmpty();
        beginTest("history_newEditAfterUndo_clearsRedo");
        assertNewEditClearsRedo();
        beginTest("history_gesture_coalescesIntoOneStep");
        assertGestureCoalesces();
        beginTest("history_gestureWithoutChange_pushesNothing");
        assertEmptyGestureAddsNoStep();
        beginTest("history_commitOnEnd_discretePressesAreSeparateSteps");
        assertCommitOnEndDiscretePresses();
        beginTest("history_reset_clearsStacksAndMarksClean");
        assertResetClears();
        beginTest("history_cleanMarker_followsUndoRedo");
        assertCleanMarker();
        beginTest("history_cleanMarker_lostWhenRedoBranchDropped");
        assertCleanMarkerLostAfterBranchDrop();
        beginTest("history_cap_dropsOldestSteps");
        assertCapDropsOldest();
        beginTest("history_syncCurrentSelection_updatesIdsWithoutNewStep");
        assertSyncCurrentSelection();
        beginTest("history_layerSnapshot_restoresPaletteThroughReplaceAll");
        assertLayerRoundTrip();
        beginTest("history_compareOn_disablesDocumentHistory");
        assertCompareDisablesHistory();
        beginTest("history_textFieldFocus_defersDocumentShortcut");
        assertTextFieldDefersShortcut();
    }

private:
    using Snapshot = SkinDesignerHistory::Snapshot;
    using History = SkinDesignerHistory::History;

    static Snapshot tagged(const juce::String& tag)
    {
        Snapshot snapshot;
        snapshot.designerSkinId = tag;
        return snapshot;
    }

    void expectCurrent(const History& history, const juce::String& tag)
    {
        expectEquals(history.current().designerSkinId, tag);
    }

    void assertEditThenUndoRedo()
    {
        History history;
        history.reset(tagged("a"));

        history.recordChange(tagged("b"));
        expect(history.canUndo());
        expect(! history.canRedo());

        const auto* undone = history.undo();
        expect(undone != nullptr);
        expectEquals(undone->designerSkinId, juce::String("a"));
        expect(history.canRedo());
        expect(! history.canUndo());

        const auto* redone = history.redo();
        expect(redone != nullptr);
        expectEquals(redone->designerSkinId, juce::String("b"));
        expectCurrent(history, "b");
    }

    void assertUndoRedoOnEmpty()
    {
        History history;
        history.reset(tagged("a"));

        expect(history.undo() == nullptr);
        expect(history.redo() == nullptr);
        expect(! history.canUndo());
        expect(! history.canRedo());
        expectCurrent(history, "a");
    }

    void assertNewEditClearsRedo()
    {
        History history;
        history.reset(tagged("a"));
        history.recordChange(tagged("b"));
        (void) history.undo();
        expect(history.canRedo());

        history.recordChange(tagged("c"));

        expect(! history.canRedo());
        expectEquals((int) history.undoDepth(), 1);
        expectCurrent(history, "c");
        expectEquals(history.undo()->designerSkinId, juce::String("a"));
    }

    void assertGestureCoalesces()
    {
        History history;
        history.reset(tagged("start"));

        history.beginGesture();
        history.recordChange(tagged("drag1"));
        history.recordChange(tagged("drag2"));
        history.recordChange(tagged("drag3"));
        history.endGesture();

        expectEquals((int) history.undoDepth(), 1);
        expectCurrent(history, "drag3");
        expectEquals(history.undo()->designerSkinId, juce::String("start"));
        expect(! history.canUndo());

        history.recordChange(tagged("after"));
        expectEquals((int) history.undoDepth(), 1);
    }

    void assertEmptyGestureAddsNoStep()
    {
        History history;
        history.reset(tagged("start"));

        history.beginGesture();
        history.endGesture();

        expect(! history.canUndo());
        history.recordChange(tagged("next"));
        expectEquals((int) history.undoDepth(), 1);
    }

    /**
     * Mirrors COLOR well policy: live samples during a press are not recorded; one
     * recordChange runs when the press ends. Discrete presses must stay separate.
     */
    void assertCommitOnEndDiscretePresses()
    {
        History history;
        history.reset(tagged("start"));

        // Press 1 (would have been many drag samples) — commit once at end.
        history.recordChange(tagged("clickA"));
        // Press 2
        history.recordChange(tagged("clickB"));
        // Press 3
        history.recordChange(tagged("clickC"));

        expectEquals((int) history.undoDepth(), 3);
        expectEquals(history.undo()->designerSkinId, juce::String("clickB"));
        expectEquals(history.undo()->designerSkinId, juce::String("clickA"));
        expectEquals(history.undo()->designerSkinId, juce::String("start"));
        expect(! history.canUndo());
    }

    void assertResetClears()
    {
        History history;
        history.reset(tagged("a"));
        history.recordChange(tagged("b"));
        history.recordChange(tagged("c"));
        (void) history.undo();

        history.reset(tagged("loaded"));

        expect(! history.canUndo());
        expect(! history.canRedo());
        expect(history.isAtCleanMarker());
        expectCurrent(history, "loaded");
        expect(history.undo() == nullptr);
    }

    void assertCleanMarker()
    {
        History history;
        history.reset(tagged("saved"));
        expect(history.isAtCleanMarker());

        history.recordChange(tagged("edit"));
        expect(! history.isAtCleanMarker());

        (void) history.undo();
        expect(history.isAtCleanMarker());

        (void) history.redo();
        expect(! history.isAtCleanMarker());

        history.markClean();
        expect(history.isAtCleanMarker());

        history.recordChange(tagged("edit2"));
        expect(! history.isAtCleanMarker());
        (void) history.undo();
        expect(history.isAtCleanMarker());
        (void) history.undo();
        expect(! history.isAtCleanMarker());
    }

    void assertCleanMarkerLostAfterBranchDrop()
    {
        History history;
        history.reset(tagged("a"));
        history.recordChange(tagged("b"));
        history.markClean();
        (void) history.undo();

        history.recordChange(tagged("c"));

        expect(! history.isAtCleanMarker());
        (void) history.undo();
        expect(! history.isAtCleanMarker());
    }

    void assertCapDropsOldest()
    {
        constexpr int kCap = 3;
        History history(kCap);
        history.reset(tagged("0"));
        for (int i = 1; i <= 5; ++i)
            history.recordChange(tagged(juce::String(i)));

        expectEquals((int) history.undoDepth(), kCap);
        int undone = 0;
        while (history.undo() != nullptr)
            ++undone;
        expectEquals(undone, kCap);
        expectCurrent(history, "2");
    }

    void assertSyncCurrentSelection()
    {
        History history;
        auto baseline = tagged("a");
        baseline.activeSkinId = "active-a";
        baseline.skins.resize(2);
        history.reset(baseline);
        history.recordChange(baseline);
        const auto depthBefore = history.undoDepth();

        history.syncCurrentSelection("active-b", "designer-b");

        expectEquals(history.current().activeSkinId, juce::String("active-b"));
        expectEquals(history.current().designerSkinId, juce::String("designer-b"));
        expectEquals((int) history.current().skins.size(), 2);
        expectEquals((int) history.undoDepth(), (int) depthBefore);
        expect(! history.canRedo());
        expectEquals(history.undo()->designerSkinId, juce::String("a"));
    }

    static Snapshot captureLayer(const TSS::DevSkin::Layer& layer, const juce::String& designerId)
    {
        return { layer.skins(), layer.activeSkinId(), designerId };
    }

    static juce::uint32 argbOf(const TSS::DevSkin::Layer& layer,
                               const juce::String& skinId,
                               const juce::String& pastilleName)
    {
        if (const auto* skin = layer.findSkin(skinId))
            for (const auto& pastille : skin->palette)
                if (pastille.name == pastilleName)
                    return pastille.argb;
        return 0;
    }

    void assertLayerRoundTrip()
    {
        TSS::DevSkin::Layer layer;
        const auto created = layer.createEmptyDevSkin("History Skin");
        expect(created.wasOk());
        const auto skinId = layer.activeSkinId();

        const auto added = layer.addPastille(skinId, "Probe", 0xff102030u);
        expect(added.wasOk());

        History history;
        history.reset(captureLayer(layer, skinId));

        const auto changed = layer.setPastilleArgb(skinId, "Probe", 0xff405060u);
        expect(changed.wasOk());
        history.recordChange(captureLayer(layer, skinId));
        expectEquals((juce::int64) argbOf(layer, skinId, "Probe"), (juce::int64) 0xff405060u);

        const auto* undone = history.undo();
        expect(undone != nullptr);
        layer.replaceAll(undone->skins, undone->activeSkinId);
        expectEquals((juce::int64) argbOf(layer, skinId, "Probe"), (juce::int64) 0xff102030u);

        const auto* redone = history.redo();
        expect(redone != nullptr);
        layer.replaceAll(redone->skins, redone->activeSkinId);
        expectEquals((juce::int64) argbOf(layer, skinId, "Probe"), (juce::int64) 0xff405060u);
    }

    void assertCompareDisablesHistory()
    {
        expect(SkinDesignerHistory::documentHistoryAllowed(false));
        expect(! SkinDesignerHistory::documentHistoryAllowed(true));
    }

    void assertTextFieldDefersShortcut()
    {
        expect(! SkinDesignerHistory::shouldDeferDocumentUndoRedoToTextField(nullptr));

        juce::TextEditor editor;
        expect(SkinDesignerHistory::shouldDeferDocumentUndoRedoToTextField(&editor));

        juce::Component child;
        editor.addAndMakeVisible(child);
        expect(SkinDesignerHistory::shouldDeferDocumentUndoRedoToTextField(&child));

        juce::Component unrelated;
        expect(! SkinDesignerHistory::shouldDeferDocumentUndoRedoToTextField(&unrelated));
    }
};

static SkinDesignerHistoryTests skinDesignerHistoryTests;
