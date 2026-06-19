#include <array>
#include <cstring>
#include <vector>

#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Services/PatchMutator/MutationAlgorithm.h"

class MutationAlgorithmTests : public juce::UnitTest
{
public:
    MutationAlgorithmTests() : juce::UnitTest("MutationAlgorithm") {}

    void runTest() override
    {
        gv01_amountZero_noOp();
        gv01_randomZero_noOp();
        gv02_dco1Only_changesDco1();
        gv03_matrixModOnly();
        gv04_choiceDeterministic();
        allTogglesOff_matrixModStillMutates();
        moduleToggleMask_disablesModule();
    }

private:
    static Core::PatchModel makeInitPatchModel()
    {
        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        return model;
    }

    static Core::MutationRecipe makeFullRecipe()
    {
        Core::MutationRecipe recipe;
        recipe.amountPercent = 100;
        recipe.randomPercent = 100;
        return recipe;
    }

    static Core::MutationRecipe recipeWithOnlyDco1Enabled()
    {
        auto recipe = makeFullRecipe();
        recipe.enableDco1 = true;
        return recipe;
    }

    static bool buffersEqual(const Core::PatchModel& a, const Core::PatchModel& b)
    {
        return std::memcmp(a.data(), b.data(), Core::PatchModel::kBufferSize) == 0;
    }

    static bool bytesEqualInRange(const Core::PatchModel& a,
                                  const Core::PatchModel& b,
                                  size_t start,
                                  size_t endExclusive)
    {
        for (size_t i = start; i < endExclusive; ++i)
        {
            if (a.data()[i] != b.data()[i])
                return false;
        }

        return true;
    }

    static bool anyByteDiffersInSet(const Core::PatchModel& before,
                                    const Core::PatchModel& after,
                                    const std::vector<size_t>& offsets)
    {
        for (const auto offset : offsets)
        {
            if (before.data()[offset] != after.data()[offset])
                return true;
        }

        return false;
    }

    void gv01_amountZero_noOp()
    {
        beginTest("gv01_amountZero_noOp");

        const auto input = makeInitPatchModel();
        auto working = input;

        Core::MutationRecipe recipe;
        recipe.amountPercent = 0;
        recipe.randomPercent = 50;
        recipe.enableDco1 = true;

        Core::SeededRandom rng(0x12345678U);
        Core::MutationAlgorithm algorithm;

        expect(! algorithm.apply(working, recipe, rng));
        expect(buffersEqual(input, working));
    }

    void gv01_randomZero_noOp()
    {
        beginTest("gv01_randomZero_noOp");

        const auto input = makeInitPatchModel();
        auto working = input;

        Core::MutationRecipe recipe;
        recipe.amountPercent = 50;
        recipe.randomPercent = 0;
        recipe.enableDco1 = true;

        Core::SeededRandom rng(0x12345678U);
        Core::MutationAlgorithm algorithm;

        expect(! algorithm.apply(working, recipe, rng));
        expect(buffersEqual(input, working));
    }

    void gv02_dco1Only_changesDco1()
    {
        beginTest("gv02_dco1Only_changesDco1");

        const auto input = makeInitPatchModel();
        auto working = input;

        const auto recipe = recipeWithOnlyDco1Enabled();
        Core::SeededRandom rng(0x6D757461U);
        Core::MutationAlgorithm algorithm;

        expect(algorithm.apply(working, recipe, rng));
        expect(bytesEqualInRange(input, working, 0, 8));

        const std::vector<size_t> dco1Offsets { 9, 10, 11, 13, 25, 86, 87 };
        expect(anyByteDiffersInSet(input, working, dco1Offsets));
        expectEquals(static_cast<int>(working.data()[8]), static_cast<int>(input.data()[8]));
    }

    void gv03_matrixModOnly()
    {
        beginTest("gv03_matrixModOnly");

        const auto input = makeInitPatchModel();
        auto working = input;

        Core::MutationRecipe recipe;
        recipe.amountPercent = 50;
        recipe.randomPercent = 100;

        Core::SeededRandom rng(0x4D61746DU);
        Core::MutationAlgorithm algorithm;

        expect(algorithm.apply(working, recipe, rng));
        expect(bytesEqualInRange(input, working, 0, 8));
        expect(bytesEqualInRange(input, working, 8, 104));

        bool matrixModChanged = false;
        for (size_t i = 104; i <= 133; ++i)
        {
            if (input.data()[i] != working.data()[i])
            {
                matrixModChanged = true;
                break;
            }
        }

        expect(matrixModChanged);
    }

    void gv04_choiceDeterministic()
    {
        beginTest("gv04_choiceDeterministic");

        const auto input = makeInitPatchModel();
        auto working = input;

        const auto recipe = recipeWithOnlyDco1Enabled();
        Core::SeededRandom rng(0x43484F31U);
        Core::MutationAlgorithm algorithm;

        expect(algorithm.apply(working, recipe, rng));
        expectEquals(static_cast<int>(working.data()[13]), 1);
    }

    void allTogglesOff_matrixModStillMutates()
    {
        beginTest("allTogglesOff_matrixModStillMutates");

        const auto input = makeInitPatchModel();
        auto working = input;

        Core::MutationRecipe recipe;
        recipe.amountPercent = 50;
        recipe.randomPercent = 100;

        Core::SeededRandom rng(0x4D61746DU);
        Core::MutationAlgorithm algorithm;

        expect(algorithm.apply(working, recipe, rng));

        bool matrixModChanged = false;
        for (size_t i = 104; i <= 133; ++i)
        {
            if (input.data()[i] != working.data()[i])
            {
                matrixModChanged = true;
                break;
            }
        }

        expect(matrixModChanged);
    }

    void moduleToggleMask_disablesModule()
    {
        beginTest("moduleToggleMask_disablesModule");

        const auto input = makeInitPatchModel();
        auto working = input;

        Core::MutationRecipe recipe = makeFullRecipe();
        recipe.enableDco2 = true;

        Core::SeededRandom rng(0x6D757461U);
        Core::MutationAlgorithm algorithm;

        expect(algorithm.apply(working, recipe, rng));

        const std::vector<size_t> dco1Offsets { 9, 10, 11, 13, 25, 86, 87 };
        for (const auto offset : dco1Offsets)
            expectEquals(static_cast<int>(working.data()[offset]), static_cast<int>(input.data()[offset]));
    }
};

static MutationAlgorithmTests mutationAlgorithmTests;
