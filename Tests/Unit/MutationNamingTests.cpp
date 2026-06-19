#include <cstring>

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchMutator/MutationHistoryStore.h"
#include "Core/Services/PatchMutator/MutationNaming.h"

class MutationNamingTests : public juce::UnitTest
{
public:
    MutationNamingTests() : juce::UnitTest("MutationNaming") {}

    void runTest() override
    {
        formatRootLabel_firstRoot();
        formatRootLabel_padded();
        formatRetryLabel_firstRetry();
        formatPatchName_rootOnly();
        formatPatchName_retry();
        formatPatchName_maxBoundary();
        formatExportStem_matchesPatchName();
        applyPatchName_root_roundTrip();
        applyPatchName_retry_roundTrip();
        applyPatchName_preservesOtherBytes();
        invalidRootIndex_rejected();
        invalidRetryIndex_rejected();
    }

private:
    static Core::PatchModel makeModelWithByteMarkers()
    {
        Core::PatchModel model;
        std::memset(model.data(), 0xAA, Core::PatchModel::kBufferSize);
        model.data()[8] = 0xBB;
        model.data()[9] = 0xCC;
        return model;
    }

    void formatRootLabel_firstRoot()
    {
        beginTest("formatRootLabel_firstRoot");

        expectEquals(Core::MutationNaming::formatRootLabel(0), juce::String("M00"));
    }

    void formatRootLabel_padded()
    {
        beginTest("formatRootLabel_padded");

        expectEquals(Core::MutationNaming::formatRootLabel(5), juce::String("M05"));
        expectEquals(Core::MutationNaming::formatRootLabel(99), juce::String("M99"));
    }

    void formatRetryLabel_firstRetry()
    {
        beginTest("formatRetryLabel_firstRetry");

        expectEquals(Core::MutationNaming::formatRetryLabel(0), juce::String("R00"));
    }

    void formatPatchName_rootOnly()
    {
        beginTest("formatPatchName_rootOnly");

        expectEquals(Core::MutationNaming::formatPatchName(5, Core::MutationHistoryStore::kRootOnly),
                     juce::String("M05"));
    }

    void formatPatchName_retry()
    {
        beginTest("formatPatchName_retry");

        const auto name = Core::MutationNaming::formatPatchName(5, 2);
        expectEquals(name, juce::String("M05-R02"));
        expectEquals(name.length(), 7);
    }

    void formatPatchName_maxBoundary()
    {
        beginTest("formatPatchName_maxBoundary");

        const auto name = Core::MutationNaming::formatPatchName(99, 99);
        expectEquals(name, juce::String("M99-R99"));
        expectEquals(name.length(), 7);
    }

    void formatExportStem_matchesPatchName()
    {
        beginTest("formatExportStem_matchesPatchName");

        expectEquals(Core::MutationNaming::formatExportStem(5, 2),
                     Core::MutationNaming::formatPatchName(5, 2));
    }

    void applyPatchName_root_roundTrip()
    {
        beginTest("applyPatchName_root_roundTrip");

        auto model = makeModelWithByteMarkers();
        expect(Core::MutationNaming::applyPatchName(model, 3));
        expectEquals(model.getName(), juce::String("M03"));
    }

    void applyPatchName_retry_roundTrip()
    {
        beginTest("applyPatchName_retry_roundTrip");

        auto model = makeModelWithByteMarkers();
        expect(Core::MutationNaming::applyPatchName(model, 5, 0));
        expectEquals(model.getName(), juce::String("M05-R00"));
    }

    void applyPatchName_preservesOtherBytes()
    {
        beginTest("applyPatchName_preservesOtherBytes");

        auto model = makeModelWithByteMarkers();
        const auto byte8Before = model.data()[8];
        const auto byte9Before = model.data()[9];

        expect(Core::MutationNaming::applyPatchName(model, 1, 1));

        expectEquals(static_cast<int>(model.data()[8]), static_cast<int>(byte8Before));
        expectEquals(static_cast<int>(model.data()[9]), static_cast<int>(byte9Before));
    }

    void invalidRootIndex_rejected()
    {
        beginTest("invalidRootIndex_rejected");

        expect(Core::MutationNaming::formatPatchName(-1, Core::MutationHistoryStore::kRootOnly).isEmpty());
        expect(Core::MutationNaming::formatPatchName(100, Core::MutationHistoryStore::kRootOnly).isEmpty());

        auto model = makeModelWithByteMarkers();
        const auto byte8Before = model.data()[8];
        expect(! Core::MutationNaming::applyPatchName(model, -1));
        expectEquals(static_cast<int>(model.data()[8]), static_cast<int>(byte8Before));
    }

    void invalidRetryIndex_rejected()
    {
        beginTest("invalidRetryIndex_rejected");

        expectEquals(Core::MutationNaming::formatPatchName(0, Core::MutationHistoryStore::kRootOnly),
                     juce::String("M00"));
        expect(Core::MutationNaming::formatPatchName(0, 100).isEmpty());
        expect(Core::MutationNaming::formatPatchName(0, -2).isEmpty());

        auto model = makeModelWithByteMarkers();
        const auto byte8Before = model.data()[8];
        expect(! Core::MutationNaming::applyPatchName(model, 0, 100));
        expectEquals(static_cast<int>(model.data()[8]), static_cast<int>(byte8Before));
    }
};

static MutationNamingTests mutationNamingTests;
