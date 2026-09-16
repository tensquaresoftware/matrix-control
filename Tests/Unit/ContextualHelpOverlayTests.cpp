#include <limits>

#include <juce_core/juce_core.h>

#include "GUI/Helpers/ContextualHelpOverlay.h"
#include "GUI/Helpers/LockDimmingFilmPolicy.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"

namespace MutatorHelp = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::ContextualHelp;

class ContextualHelpOverlayTests : public juce::UnitTest
{
public:
    ContextualHelpOverlayTests()
        : juce::UnitTest("ContextualHelpOverlay")
    {
    }

    void runTest() override
    {
        hoverSetsOverlayWithoutTouchingSticky();
        leaveClearsOverlayAndRestoresStickyPreference();
        focusAndBlurUseSameOverlayGateAsHover();
        compareStickyRemainsInApvtsWhileHelpCovers();
        compareFilmOnlyExposesCompareHole();
        adjacentTraverseSupersedesPendingClear();
        stickyWriterDuringHelpLeavesOverlayUntilClear();
        warningAndErrorAreCoveredWhileOverlayActive();
        mutatorPopupDefersClearUntilIdle();
        helpCopyConstantsMatchSpec();
        epochOwnershipGatesClear();
        epochOwnershipGatesFooterHandshake();
    }

private:
    void hoverSetsOverlayWithoutTouchingSticky()
    {
        beginTest("Hover help - overlay active; sticky string untouched");

        juce::String sticky = "Patch Mutator: Compare mode - editing locked.";
        TSS::ContextualHelpOverlay overlay;

        overlay.setDetail(MutatorHelp::kMutate);

        expect(overlay.isActive());
        expectEquals(overlay.getDetail(), juce::String(MutatorHelp::kMutate));
        expectEquals(sticky, juce::String("Patch Mutator: Compare mode - editing locked."));
        expect(TSS::shouldPaintContextualHelpOverSticky(overlay.isActive()));
    }

    void leaveClearsOverlayAndRestoresStickyPreference()
    {
        beginTest("Leave restore - clear overlay paints sticky again");

        TSS::ContextualHelpOverlay overlay;
        overlay.setDetail(MutatorHelp::kMode);
        expect(TSS::shouldClearContextualHelpOverlay(3, 3, false));

        overlay.clear();

        expect(! overlay.isActive());
        expect(! TSS::shouldPaintContextualHelpOverSticky(overlay.isActive()));
    }

    void focusAndBlurUseSameOverlayGateAsHover()
    {
        beginTest("Focus / blur - same set and deferred-clear gate as hover");

        TSS::ContextualHelpOverlay overlay;
        overlay.setDetail(MutatorHelp::kPitch);
        expect(overlay.isActive());

        expect(TSS::shouldClearContextualHelpOverlay(1, 1, false));
        expect(! TSS::shouldClearContextualHelpOverlay(1, 1, true));
    }

    void compareStickyRemainsInApvtsWhileHelpCovers()
    {
        beginTest("Compare + C - help covers paint; sticky property value unchanged");

        juce::String apvtsSticky =
            PluginDisplayNames::PatchManagerSection::PatchMutatorModule::Messages::kCompareLockedFooter;
        const auto stickyBefore = apvtsSticky;

        TSS::ContextualHelpOverlay overlay;
        overlay.setDetail(MutatorHelp::kCompare);

        expect(TSS::shouldPaintContextualHelpOverSticky(true));
        expectEquals(apvtsSticky, stickyBefore);
        expectEquals(overlay.getDetail(), juce::String(MutatorHelp::kCompare));
    }

    void compareFilmOnlyExposesCompareHole()
    {
        beginTest("Compare + other - film active with COMPARE hole only");

        const auto mode = TSS::resolveLockDimmingFilmMode(true,
                                                          MatrixDeviceTypes::Type::kMatrix1000,
                                                          true,
                                                          false);
        expect(mode == TSS::LockDimmingFilmMode::kHeaderFooterCompare);
        expect(TSS::lockDimmingFilmIncludesCompareHole(mode));
        expect(TSS::lockDimmingFilmIsActive(mode));
    }

    void adjacentTraverseSupersedesPendingClear()
    {
        beginTest("Adjacent traverse - newer generation cancels pending clear");

        expect(! TSS::shouldClearContextualHelpOverlay(5, 6, false));
        expect(TSS::shouldClearContextualHelpOverlay(6, 6, false));
    }

    void stickyWriterDuringHelpLeavesOverlayUntilClear()
    {
        beginTest("Sticky writer during help - overlay stays until leave clear");

        juce::String apvtsSticky = "Patch Mutator: Mutation history flushed.";
        TSS::ContextualHelpOverlay overlay;
        overlay.setDetail(MutatorHelp::kExport);

        apvtsSticky = "Patch Mutator: Exported 3 mutation file(s) to /tmp/out.";

        expect(overlay.isActive());
        expectEquals(overlay.getDetail(), juce::String(MutatorHelp::kExport));
        expect(apvtsSticky.startsWith("Patch Mutator: Exported "));

        overlay.clear();
        expect(! overlay.isActive());
        expect(apvtsSticky.startsWith("Patch Mutator: Exported "));
    }

    void warningAndErrorAreCoveredWhileOverlayActive()
    {
        beginTest("Warning/Error sticky + hover - overlay always covers while active");

        expect(TSS::shouldPaintContextualHelpOverSticky(true));
        expect(! TSS::shouldPaintContextualHelpOverSticky(false));
    }

    void mutatorPopupDefersClearUntilIdle()
    {
        beginTest("Mutator popup - defer clear while focus inside or modal active");

        expect(TSS::shouldDeferContextualHelpClearForMutatorPopup(true, false));
        expect(TSS::shouldDeferContextualHelpClearForMutatorPopup(false, true));
        expect(TSS::shouldDeferContextualHelpClearForMutatorPopup(true, true));
        expect(! TSS::shouldDeferContextualHelpClearForMutatorPopup(false, false));
    }

    void helpCopyConstantsMatchSpec()
    {
        beginTest("Help copy - badge and 22 Mutator strings match approved spec");

        expectEquals(juce::String(PluginDisplayNames::FooterPanel::kContextualHelpBadge),
                     juce::String("HELP"));

        static constexpr const char* kExpected[][2] = {
            { MutatorHelp::kMode, "PATCH MUTATOR: Sets how far mutations stray - Kindred, Drift, Warp, or Wild." },
            { MutatorHelp::kPitch, "PATCH MUTATOR: Controls how DCO pitch may move - Keep, Consonant, Dissonant, or Free." },
            { MutatorHelp::kHistory, "PATCH MUTATOR: Recalls a mutation or retry from this session." },
            { MutatorHelp::kMutate, "PATCH MUTATOR: Creates a new variation from the current recipe and sends it to the synthesizer." },
            { MutatorHelp::kRetry, "PATCH MUTATOR: Rolls again from the same mutation root." },
            { MutatorHelp::kHistoryPrevious, "PATCH MUTATOR: Steps backward through session history." },
            { MutatorHelp::kHistoryNext, "PATCH MUTATOR: Steps forward through session history." },
            { MutatorHelp::kCompare, "PATCH MUTATOR: Compares with the origin patch and locks editing until you click [C] button again." },
            { MutatorHelp::kDelete, "PATCH MUTATOR: Deletes the selected history entry." },
            { MutatorHelp::kFlush, "PATCH MUTATOR: Flushes the whole session mutation history." },
            { MutatorHelp::kExport, "PATCH MUTATOR: Exports the session mutations as SysEx files." },
            { MutatorHelp::kEnableDco1, "PATCH MUTATOR: Include DCO 1 module in the mutation recipe." },
            { MutatorHelp::kEnableDco2, "PATCH MUTATOR: Include DCO 2 module in the mutation recipe." },
            { MutatorHelp::kEnableVcfVca, "PATCH MUTATOR: Include VCF/VCA module in the recipe." },
            { MutatorHelp::kEnableFmTrack, "PATCH MUTATOR: Include FM/TRACK module in the recipe." },
            { MutatorHelp::kEnableRampPortamento, "PATCH MUTATOR: Include RAMP/PORTAMENTO module in the recipe." },
            { MutatorHelp::kEnableEnvelope1, "PATCH MUTATOR: Include ENV 1 module in the recipe." },
            { MutatorHelp::kEnableEnvelope2, "PATCH MUTATOR: Include ENV 2 module in the recipe." },
            { MutatorHelp::kEnableEnvelope3, "PATCH MUTATOR: Include ENV 3 module in the recipe." },
            { MutatorHelp::kEnableLfo1, "PATCH MUTATOR: Include LFO 1 module in the recipe." },
            { MutatorHelp::kEnableLfo2, "PATCH MUTATOR: Include LFO 2 module in the recipe." },
            { MutatorHelp::kEnableMatrixMod, "PATCH MUTATOR: Include MATRIX MODULATION module in the recipe." },
        };

        expectEquals(static_cast<int>(sizeof(kExpected) / sizeof(kExpected[0])), 22);
        for (const auto& row : kExpected)
            expectEquals(juce::String(row[0]), juce::String(row[1]));
    }

    void epochOwnershipGatesClear()
    {
        beginTest("Epoch ownership - only matching positive epoch may clear");

        expect(TSS::shouldClearContextualHelpOverlayForEpoch(3, 3));
        expect(! TSS::shouldClearContextualHelpOverlayForEpoch(2, 3));
        expect(! TSS::shouldClearContextualHelpOverlayForEpoch(0, 0));
        expect(! TSS::shouldClearContextualHelpOverlayForEpoch(-1, -1));
    }

    void epochOwnershipGatesFooterHandshake()
    {
        beginTest("Epoch ownership - stale clear leaves newer Footer overlay detail");

        TSS::ContextualHelpOverlay overlay;
        int epoch = 0;

        epoch = TSS::nextContextualHelpOverlayEpoch(epoch);
        overlay.setDetail("A");
        const int epochA = epoch;

        epoch = TSS::nextContextualHelpOverlayEpoch(epoch);
        overlay.setDetail("B");
        const int epochB = epoch;

        expect(! TSS::clearContextualHelpOverlayDetailIfEpoch(overlay, epochA, epoch));
        expect(overlay.isActive());
        expectEquals(overlay.getDetail(), juce::String("B"));

        expect(TSS::clearContextualHelpOverlayDetailIfEpoch(overlay, epochB, epoch));
        expect(! overlay.isActive());

        expectEquals(TSS::nextContextualHelpOverlayEpoch(std::numeric_limits<int>::max()), 1);
    }
};

static ContextualHelpOverlayTests contextualHelpOverlayTests;
