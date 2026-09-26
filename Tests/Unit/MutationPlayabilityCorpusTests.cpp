#include "MutationCalibrationTestSupport.h"
#include "MutationPlayabilityOracle.h"

#include <utility>
#include <vector>

using namespace MutationCalibrationTestSupport;

// Complementary fuzz: ROM + silence traps + Init stay structurally playable after
// MutationAlgorithm::apply. Targeted regressions stay in MutationAntiSilenceTests.
class MutationPlayabilityCorpusTests : public juce::UnitTest
{
public:
    MutationPlayabilityCorpusTests() : juce::UnitTest("MutationPlayability") {}

    void runTest() override
    {
        corpus_romFixturesArePresent();
        corpus_mutateKeepsPlayability();
        corpus_retrySliceKeepsPlayability();
        should_deadWritableTrapBecomesPlayable();
    }

private:
    struct NamedSeed
    {
        juce::String identity;
        Core::PatchModel model;
    };

    static juce::String modeLabel(Core::MutationMode mode)
    {
        switch (mode)
        {
            case Core::MutationMode::kKindred: return "KINDRED";
            case Core::MutationMode::kDrift:   return "DRIFT";
            case Core::MutationMode::kWarp:    return "WARP";
            case Core::MutationMode::kWild:    return "WILD";
        }

        return "UNKNOWN";
    }

    static juce::String pitchLabel(Core::MutationPitchMode pitch)
    {
        switch (pitch)
        {
            case Core::MutationPitchMode::kPreserve:  return "PRESERVE";
            case Core::MutationPitchMode::kConsonant: return "CONSONANT";
            case Core::MutationPitchMode::kDissonant: return "DISSONANT";
            case Core::MutationPitchMode::kFree:      return "FREE";
        }

        return "UNKNOWN";
    }

    struct PlayabilityFailureContext
    {
        juce::String seedIdentity;
        Core::MutationMode mode = Core::MutationMode::kDrift;
        Core::MutationPitchMode pitch = Core::MutationPitchMode::kPreserve;
        juce::uint32 rngSeed = 0;
    };

    juce::String failureMessage(const PlayabilityFailureContext& context,
                                const MutationPlayabilityOracle::Result& oracle) const
    {
        return "seed=" + context.seedIdentity
               + " MODE=" + modeLabel(context.mode)
               + " PITCH=" + pitchLabel(context.pitch)
               + " rng=" + juce::String(static_cast<juce::int64>(context.rngSeed))
               + " rule=" + juce::String(oracle.ruleId);
    }

    void expectPlayable(const NamedSeed& seed,
                        Core::MutationMode mode,
                        Core::MutationPitchMode pitch,
                        juce::uint32 rngSeed)
    {
        const auto recipe = makeFullyEnabledRecipe(mode, pitch);
        const auto result = mutated(seed.model, recipe, rngSeed);
        const auto oracle = MutationPlayabilityOracle::check(result);
        expect(oracle.passed,
               failureMessage({ seed.identity, mode, pitch, rngSeed }, oracle));
    }

    std::vector<NamedSeed> buildCorpusSeeds()
    {
        std::vector<NamedSeed> seeds;

        for (const auto* relativePath : kRomCorpusRelativePaths)
        {
            NamedSeed entry;
            entry.identity = relativePath;
            const bool loaded = loadPatchFromFixtureRelative(relativePath, entry.model);
            expect(loaded, juce::String("ROM fixture missing: ") + relativePath);
            if (! loaded)
                continue;

            seeds.push_back(std::move(entry));
        }

        seeds.push_back({ "trap:deadAmplitude", makeTrapDeadAmplitude() });
        seeds.push_back({ "trap:dualWaveOff", makeTrapDualWaveOff() });
        seeds.push_back({ "trap:filterSmother", makeTrapFilterSmother() });
        seeds.push_back({ "trap:mixTowardSilentDco", makeTrapMixTowardSilentDco() });
        seeds.push_back({ "trap:negativeMmVca1", makeTrapNegativeMmTowardVca1() });
        seeds.push_back({ "init:InitDefaults", makeInitPatchModel() });

        NamedSeed initFixture;
        initFixture.identity = "Matrix-Control/Init/PatchInit.syx";
        const bool initLoaded = loadPatchFromFixtureRelative("Matrix-Control/Init/PatchInit.syx", initFixture.model);
        expect(initLoaded, "Matrix-Control/Init/PatchInit.syx must load");
        if (initLoaded)
            seeds.push_back(std::move(initFixture));

        return seeds;
    }

    static constexpr Core::MutationMode kModes[] = {
        Core::MutationMode::kKindred,
        Core::MutationMode::kDrift,
        Core::MutationMode::kWarp,
        Core::MutationMode::kWild,
    };

    // CI PITCH subset: Preserve + Free + one Consonant (default octave window).
    static constexpr Core::MutationPitchMode kPitchSubset[] = {
        Core::MutationPitchMode::kPreserve,
        Core::MutationPitchMode::kFree,
        Core::MutationPitchMode::kConsonant,
    };

    void corpus_romFixturesArePresent()
    {
        beginTest("corpus_romFixturesArePresent");

        for (const auto* relativePath : kRomCorpusRelativePaths)
        {
            Core::PatchModel model;
            expect(loadPatchFromFixtureRelative(relativePath, model),
                   juce::String("ROM fixture must load: ") + relativePath);
        }
    }

    void corpus_mutateKeepsPlayability()
    {
        beginTest("corpus_mutateKeepsPlayability");

        const auto seeds = buildCorpusSeeds();
        const int trials = activeTrialCount();

        for (const auto& seed : seeds)
        {
            for (const auto mode : kModes)
            {
                for (const auto pitch : kPitchSubset)
                {
                    for (int trial = 0; trial < trials; ++trial)
                    {
                        const auto rngSeed = static_cast<juce::uint32>(0xC0B1 + trial);
                        expectPlayable(seed, mode, pitch, rngSeed);
                    }
                }
            }
        }
    }

    void corpus_retrySliceKeepsPlayability()
    {
        beginTest("corpus_retrySliceKeepsPlayability");

        // Thin RETRY slice: second apply from the same parent with a distinct RNG.
        const auto seeds = buildCorpusSeeds();

        for (const auto& seed : seeds)
        {
            for (const auto mode : kModes)
            {
                for (const auto pitch : kPitchSubset)
                {
                    expectPlayable(seed, mode, pitch, 0x7E71u);
                    expectPlayable(seed, mode, pitch, 0x7E72u);
                }
            }
        }
    }

    void should_deadWritableTrapBecomesPlayable()
    {
        beginTest("should_deadWritableTrapBecomesPlayable");

        const NamedSeed trap { "trap:deadAmplitude", makeTrapDeadAmplitude() };
        // Oracle must fail before repair so the Should case is meaningful.
        expect(! MutationPlayabilityOracle::check(trap.model).passed);

        const auto recipe = makeFullyEnabledRecipe(Core::MutationMode::kWild,
                                                   Core::MutationPitchMode::kFree);
        const auto repaired = mutated(trap.model, recipe, 0xDEAD01u);
        const auto oracle = MutationPlayabilityOracle::check(repaired);
        expect(oracle.passed,
               failureMessage({ trap.identity, Core::MutationMode::kWild,
                                Core::MutationPitchMode::kFree, 0xDEAD01u },
                              oracle));
    }
};

static MutationPlayabilityCorpusTests mutationPlayabilityCorpusTests;
