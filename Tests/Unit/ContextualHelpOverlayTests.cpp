#include <limits>

#include <juce_core/juce_core.h>

#include "GUI/About/AboutPanel.h"
#include "GUI/Helpers/ContextualHelpBinder.h"
#include "GUI/Helpers/ContextualHelpOverlay.h"
#include "GUI/Helpers/LockDimmingFilmPolicy.h"
#include "GUI/Settings/SettingsAudioPage.h"
#include "GUI/Settings/SettingsMidiPage.h"
#include "GUI/Settings/SettingsPanel.h"
#include "Shared/Definitions/MatrixDeviceTypes.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
template <typename T>
constexpr bool kHasRegisterContextualHelpFooterResolver = requires(
    T& panel, TSS::ContextualHelpBinder::FooterResolver resolver)
{
    panel.registerContextualHelp(std::move(resolver));
};

template <typename T>
constexpr bool kHasRegisterContextualHelpBinderRef = requires(
    T& page, TSS::ContextualHelpBinder& binder)
{
    page.registerContextualHelp(binder);
};

static_assert(! kHasRegisterContextualHelpFooterResolver<SettingsPanel>,
              "Wave 3: Settings modal must not register furtive HELP");
static_assert(! kHasRegisterContextualHelpFooterResolver<AboutPanel>,
              "Wave 3: About modal must not register furtive HELP");
static_assert(! kHasRegisterContextualHelpBinderRef<SettingsMidiPage>,
              "Wave 3: Settings MIDI page must not register furtive HELP");
static_assert(! kHasRegisterContextualHelpBinderRef<SettingsAudioPage>,
              "Wave 3: Settings AUDIO page must not register furtive HELP");
} // namespace

namespace MutatorHelp = PluginDisplayNames::PatchManagerSection::PatchMutatorModule::ContextualHelp;
namespace HeaderHelp = PluginDisplayNames::HeaderPanel::ContextualHelp;
namespace PatchEditHelp = PluginDisplayNames::PatchEditSection::ContextualHelp;
namespace MasterEditHelp = PluginDisplayNames::MasterEditSection::ContextualHelp;
namespace MatrixModHelp = PluginDisplayNames::MatrixModulationSection::ContextualHelp;
namespace PatchManagerHelp = PluginDisplayNames::PatchManagerSection::ContextualHelp;

namespace
{
    // Approved Wave 2 HELP copy (spec frozen table + Internal/Computer groups).
    constexpr const char* kWave2TitleHelpExpected[][2] = {
        { HeaderHelp::kLogo,
          "MATRIX-CONTROL: Opens the logo menu for SETTINGS, About, Skin, and UI Scale." },
        { HeaderHelp::kSettings,
          "MATRIX-CONTROL: Opens SETTINGS (MIDI ports, standalone audio, paths, warnings, master utility, shortcuts)." },
        { HeaderHelp::kAbout, "MATRIX-CONTROL: Shows product version, links, and credits." },
        { HeaderHelp::kSkin, "MATRIX-CONTROL: Chooses the visual skin for the editor." },
        { HeaderHelp::kUiScale, "MATRIX-CONTROL: Sets the user interface scale." },
        { PatchEditHelp::kSectionTitle,
          "PATCH EDIT: Edits the current patch and shows envelopes, Track Generator, and patch name." },
        { MatrixModHelp::kSectionTitle,
          "MATRIX MODULATION: Routes modulation sources to destinations across ten buses." },
        { PatchManagerHelp::kSectionTitle,
          "PATCH MANAGER: Navigates banks and patches, stores to memory or disk, imports or exports banks, and inits, copies, or pastes patches." },
        { MasterEditHelp::kSectionTitle,
          "MASTER EDIT: Edits global settings for all patches (Matrix-1000 only)." },
        { PluginDisplayNames::PatchEditSection::Dco1Module::ContextualHelp::kTitle,
          "DCO 1: Oscillator 1 controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::Dco2Module::ContextualHelp::kTitle,
          "DCO 2: Oscillator 2 controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::VcfVcaModule::ContextualHelp::kTitle,
          "VCF/VCA: Filter and amplifier controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::FmTrackModule::ContextualHelp::kTitle,
          "FM/TRACK: Frequency modulation and Track Generator controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::RampPortamentoModule::ContextualHelp::kTitle,
          "RAMP/PORTAMENTO: Ramp generator and portamento controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::Envelope1Module::ContextualHelp::kTitle,
          "ENV 1: Envelope 1 controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::Envelope2Module::ContextualHelp::kTitle,
          "ENV 2: Envelope 2 controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::Envelope3Module::ContextualHelp::kTitle,
          "ENV 3: Envelope 3 controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::Lfo1Module::ContextualHelp::kTitle,
          "LFO 1: Low-frequency oscillator 1 controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::Lfo2Module::ContextualHelp::kTitle,
          "LFO 2: Low-frequency oscillator 2 controls for the current patch." },
        { PluginDisplayNames::PatchEditSection::PatchNameModule::ContextualHelp::kTitle,
          "PATCH NAME: Shows the 8-character patch name - double-click to rename when editable." },
        { PluginDisplayNames::MasterEditSection::MidiModule::ContextualHelp::kTitle,
          "MIDI: Global MIDI channel and related controls for the synthesizer." },
        { PluginDisplayNames::MasterEditSection::VibratoModule::ContextualHelp::kTitle,
          "VIBRATO: Global vibrato controls for the synthesizer." },
        { PluginDisplayNames::MasterEditSection::MiscModule::ContextualHelp::kTitle,
          "MISC: Global bend, pressure, tuning, memory protect, plus global Unison and Unison Detune." },
        { PluginDisplayNames::PatchManagerSection::BankUtilityModule::ContextualHelp::kTitle,
          "BANK UTILITY: Selects a bank and copies, pastes, imports, or exports it (Matrix-1000 only)." },
        { PluginDisplayNames::PatchManagerSection::InternalPatchesModule::ContextualHelp::kTitle,
          "INTERNAL PATCHES: Click the title to focus this module and navigate synthesizer memory (banks and patches)." },
        { PluginDisplayNames::PatchManagerSection::InternalPatchesModule::ContextualHelp::kBrowser,
          "INTERNAL PATCHES: Bank and patch number browser for synthesizer memory." },
        { PluginDisplayNames::PatchManagerSection::InternalPatchesModule::ContextualHelp::kMemory,
          "INTERNAL PATCHES: Init, copy, paste, and store for the selected memory slot." },
        { PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::ContextualHelp::kTitle,
          "COMPUTER PATCHES: Click the title to focus this module and navigate .syx / .m1kp patch files on disk." },
        { PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::ContextualHelp::kBrowser,
          "COMPUTER PATCHES: Folder browser for patch files on disk." },
        { PluginDisplayNames::PatchManagerSection::ComputerPatchesModule::ContextualHelp::kStorage,
          "COMPUTER PATCHES: Open a folder and save the current patch as a .syx file." },
        { MutatorHelp::kTitle,
          "PATCH MUTATOR: Mutates the current patch and browses mutation history." },
    };
}

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
        warningStickyCoveredWhileOverlayActive();
        errorStickyWinsOverOverlay();
        mutatorPopupDefersClearUntilIdle();
        helpCopyConstantsMatchSpec();
        wave2TitleHelpCopyConstantsMatchSpec();
        wave3CentreBandHelpCopyConstantsMatchSpec();
        epochOwnershipGatesClear();
        epochOwnershipGatesFooterHandshake();
        preferenceEnabledOnlyForShow();
        preferenceInvalidDefaultsToShow();
        hideClearsOverlayDetailPolicy();
    }

private:
    void hoverSetsOverlayWithoutTouchingSticky()
    {
        beginTest("Hover help - overlay active; sticky string untouched");

        juce::String sticky = "PATCH MUTATOR: Compare mode - editing locked.";
        TSS::ContextualHelpOverlay overlay;

        overlay.setDetail(MutatorHelp::kMutate);

        expect(overlay.isActive());
        expectEquals(overlay.getDetail(), juce::String(MutatorHelp::kMutate));
        expectEquals(sticky, juce::String("PATCH MUTATOR: Compare mode - editing locked."));
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
        beginTest("Leave clear gate - hover keeps HELP; no hover allows clear");

        TSS::ContextualHelpOverlay overlay;
        overlay.setDetail(MutatorHelp::kPitch);
        expect(overlay.isActive());

        // Third arg is pointer hover only (not keyboard focus).
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

        juce::String apvtsSticky = "PATCH MUTATOR: Mutation history flushed.";
        TSS::ContextualHelpOverlay overlay;
        overlay.setDetail(MutatorHelp::kExport);

        apvtsSticky = "PATCH MUTATOR: Exported 3 mutation file(s) to /tmp/out.";

        expect(overlay.isActive());
        expectEquals(overlay.getDetail(), juce::String(MutatorHelp::kExport));
        expect(apvtsSticky.startsWith("PATCH MUTATOR: Exported "));

        overlay.clear();
        expect(! overlay.isActive());
        expect(apvtsSticky.startsWith("PATCH MUTATOR: Exported "));
    }

    void warningStickyCoveredWhileOverlayActive()
    {
        beginTest("Warning sticky + hover - overlay covers while active");

        expect(TSS::shouldPaintContextualHelpOverSticky(true, false));
        expect(! TSS::shouldPaintContextualHelpOverSticky(false, false));
    }

    void errorStickyWinsOverOverlay()
    {
        beginTest("Error sticky - wins over HELP overlay while error present");

        expect(! TSS::shouldPaintContextualHelpOverSticky(true, true));
        expect(! TSS::shouldPaintContextualHelpOverSticky(false, true));
        expect(TSS::shouldPaintContextualHelpOverSticky(true, false));
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
        beginTest("Help copy - badge and 23 Mutator strings match approved spec");

        expectEquals(juce::String(PluginDisplayNames::FooterPanel::kContextualHelpBadge),
                     juce::String("HELP"));

        static constexpr const char* kExpected[][2] = {
            { MutatorHelp::kTitle, "PATCH MUTATOR: Mutates the current patch and browses mutation history." },
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

        expectEquals(static_cast<int>(sizeof(kExpected) / sizeof(kExpected[0])), 23);
        for (const auto& row : kExpected)
            expectEquals(juce::String(row[0]), juce::String(row[1]));
    }

    void wave2TitleHelpCopyConstantsMatchSpec()
    {
        beginTest("Help copy - Wave 2 section/module/logo strings match approved table");

        constexpr int kRowCount =
            static_cast<int>(sizeof(kWave2TitleHelpExpected) / sizeof(kWave2TitleHelpExpected[0]));
        expectEquals(kRowCount, 31);
        for (const auto& row : kWave2TitleHelpExpected)
            expectEquals(juce::String(row[0]), juce::String(row[1]));
    }

    void wave3CentreBandHelpCopyConstantsMatchSpec()
    {
        // String contract only — FooterPanel centreHitArea_ bind / HIDE-at-footer are Standalone UAT
        // (CONVENTIONS §8.5: no GUI component unit tests). Generic HIDE policy: hideClearsOverlayDetailPolicy.
        beginTest("Help copy - Wave 3 footer centre band and DEVICE strings match approved table");

        namespace FooterHelp = PluginDisplayNames::FooterPanel::ContextualHelp;
        expectEquals(
            juce::String(FooterHelp::kCentreBand),
            juce::String(
                "MIDI: Shows a red alert when the MIDI send queue is backed up - "
                "click PANIC button to clear stuck notes."));
        expectEquals(
            juce::String(FooterHelp::kDevice),
            juce::String(
                "DEVICE: Shows the connected synthesizer model and EPROM version, or connection status."));
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

    void preferenceEnabledOnlyForShow()
    {
        beginTest("Preference - SHOW enables HELP; HIDE disables");

        using namespace PluginIDs::Settings::ContextualHelp;

        expect(TSS::isContextualHelpPreferenceEnabled(kShow));
        expect(! TSS::isContextualHelpPreferenceEnabled(kHide));
        expectEquals(TSS::normalizeContextualHelpPreference(kShow), kShow);
        expectEquals(TSS::normalizeContextualHelpPreference(kHide), kHide);
    }

    void preferenceInvalidDefaultsToShow()
    {
        beginTest("Preference - missing / invalid id clamps to SHOW");

        using namespace PluginIDs::Settings::ContextualHelp;

        expectEquals(TSS::normalizeContextualHelpPreference(0), kDefault);
        expectEquals(TSS::normalizeContextualHelpPreference(-1), kDefault);
        expectEquals(TSS::normalizeContextualHelpPreference(99), kDefault);
        expect(TSS::isContextualHelpPreferenceEnabled(0));
        expect(TSS::isContextualHelpPreferenceEnabled(-1));
        expect(TSS::isContextualHelpPreferenceEnabled(99));
        expectEquals(kDefault, kShow);
    }

    void hideClearsOverlayDetailPolicy()
    {
        beginTest("Preference HIDE - clear overlay detail unconditionally");

        using namespace PluginIDs::Settings::ContextualHelp;

        expect(TSS::shouldClearContextualHelpOverlayForPreference(kHide));
        expect(! TSS::shouldClearContextualHelpOverlayForPreference(kShow));
        expect(! TSS::shouldClearContextualHelpOverlayForPreference(0));
        expect(! TSS::shouldClearContextualHelpOverlayForPreference(99));

        TSS::ContextualHelpOverlay overlay;
        overlay.setDetail(MutatorHelp::kMutate);
        expect(overlay.isActive());

        // Same production gate FooterPanel uses before clearContextualHelpOverlay.
        if (TSS::shouldClearContextualHelpOverlayForPreference(kHide))
            overlay.clear();

        expect(! overlay.isActive());
        expect(! TSS::shouldPaintContextualHelpOverSticky(overlay.isActive(), false));
    }
};

static ContextualHelpOverlayTests contextualHelpOverlayTests;
