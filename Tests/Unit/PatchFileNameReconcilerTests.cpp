#include <juce_core/juce_core.h>

#include "Core/Init/InitDefaults.h"
#include "Core/Models/PatchModel.h"
#include "Core/Services/PatchFileNameReconciler.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Policy = PluginIDs::Settings::NameReconciliationPolicy;

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
            Policy::kPreferFilename,
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
            Policy::kPreferInternal,
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
            Policy::kPreferFilename,
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
            Policy::kPreferFilename,
            nullptr);

        expect(! result.hadMismatch);
        expect(! result.usedFilename);
        expect(! result.cancelled);
        expect(result.resolvedName == "BASS");
    }
};

static PatchFileNameReconcilerTests patchFileNameReconcilerTests;
