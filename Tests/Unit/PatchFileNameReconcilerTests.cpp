#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Policy = PluginIDs::Settings::ComputerPatchesNamesPolicy;

class PatchFileNameReconcilerTests : public juce::UnitTest
{
public:
    PatchFileNameReconcilerTests() : juce::UnitTest("PatchFileNameReconciler") {}

    void runTest() override
    {
        reconcile_namesMatch();
        reconcile_preferInternal();
        reconcile_preferFilename();
        reconcile_askOnceCancel();
        reconcile_askOnceFilename();
        reconcile_sanitizerReuse();
        reconcileForcedFilename_ignoresInternalName();
        reconcileForcedFilename_sanitizesStemExamples();
    }

private:
    void reconcile_namesMatch()
    {
        beginTest("reconcile_namesMatch");

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("BASS");

        const auto result = Core::PatchFileNameReconciler::reconcile(
            model,
            "BASS",
            Policy::kDisplayFileNames,
            nullptr);

        expect(! result.hadMismatch);
        expect(! result.usedFilename);
        expect(! result.cancelled);
        expect(result.resolvedName == "BASS");
        expect(model.getName() == "BASS");
    }

    void reconcile_preferInternal()
    {
        beginTest("reconcile_preferInternal");

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("INSIDE");

        const auto result = Core::PatchFileNameReconciler::reconcile(
            model,
            "OUTSIDE",
            Policy::kDisplaySysexNames,
            nullptr);

        expect(result.hadMismatch);
        expect(! result.usedFilename);
        expect(result.resolvedName == "INSIDE");
        expect(model.getName() == "INSIDE");
    }

    void reconcile_preferFilename()
    {
        beginTest("reconcile_preferFilename");

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("INSIDE");

        const auto result = Core::PatchFileNameReconciler::reconcile(
            model,
            "OUTSIDE",
            Policy::kDisplayFileNames,
            nullptr);

        expect(result.hadMismatch);
        expect(result.usedFilename);
        expect(result.resolvedName == "OUTSIDE");
        expect(model.getName() == "OUTSIDE");
    }

    void reconcile_askOnceCancel()
    {
        beginTest("reconcile_askOnceCancel");

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("INSIDE");

        const auto result = Core::PatchFileNameReconciler::reconcile(
            model,
            "OUTSIDE",
            Policy::kAskOncePerLoad,
            [](juce::String, juce::String) -> std::optional<Core::NameReconciliationChoice>
            {
                return std::nullopt;
            });

        expect(result.cancelled);
        expect(model.getName() == "INSIDE");
    }

    void reconcile_askOnceFilename()
    {
        beginTest("reconcile_askOnceFilename");

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("INSIDE");

        const auto result = Core::PatchFileNameReconciler::reconcile(
            model,
            "OUTSIDE",
            Policy::kAskOncePerLoad,
            [](juce::String, juce::String) -> std::optional<Core::NameReconciliationChoice>
            {
                return Core::NameReconciliationChoice::kFilename;
            });

        expect(result.hadMismatch);
        expect(result.usedFilename);
        expect(result.resolvedName == "OUTSIDE");
        expect(model.getName() == "OUTSIDE");
    }

    void reconcile_sanitizerReuse()
    {
        beginTest("reconcile_sanitizerReuse");

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("bass");

        const auto result = Core::PatchFileNameReconciler::reconcile(
            model,
            "BASS",
            Policy::kDisplayFileNames,
            nullptr);

        expect(! result.hadMismatch);
        expect(! result.usedFilename);
        expect(! result.cancelled);
        expect(result.resolvedName == "BASS");
    }

    void reconcileForcedFilename_ignoresInternalName()
    {
        beginTest("reconcileForcedFilename_ignoresInternalName");

        Core::PatchModel model;
        model.loadFrom(Core::InitDefaults::patchData());
        model.setName("BNK7: 57");

        const auto result = Core::PatchFileNameReconciler::reconcileForcedFilename(model, "CleanBss");

        expect(result.hadMismatch);
        expect(result.usedFilename);
        expect(! result.cancelled);
        expectEquals(result.resolvedName, juce::String("CLEANBSS"));
        expectEquals(model.getName(), juce::String("CLEANBSS"));
    }

    void reconcileForcedFilename_sanitizesStemExamples()
    {
        beginTest("reconcileForcedFilename_sanitizesStemExamples");

        auto expectStem = [this](const juce::String& stem, const juce::String& expected)
        {
            Core::PatchModel model;
            model.loadFrom(Core::InitDefaults::patchData());
            model.setName("I N I T");
            const auto result = Core::PatchFileNameReconciler::reconcileForcedFilename(model, stem);
            expectEquals(model.getName(), expected);
            expectEquals(result.resolvedName, expected);
            expect(result.usedFilename);
        };

        expectStem("Rich Pad", "RICH PAD");
        expectStem("MyBeautifulPad", "MYBEAUTI");
        expectStem("Pad#1!!", "PAD1");
        expectStem("@@@", "PATCH");
    }
};

static PatchFileNameReconcilerTests patchFileNameReconcilerTests;
