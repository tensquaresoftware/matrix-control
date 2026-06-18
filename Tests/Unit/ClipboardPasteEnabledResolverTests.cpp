#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Core/Services/ClipboardPasteEnabledResolver.h"
#include "Core/Services/ClipboardService.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchEdit = PluginIDs::PatchEditSection;

class ClipboardPasteEnabledResolverTests : public juce::UnitTest
{
public:
    ClipboardPasteEnabledResolverTests() : juce::UnitTest("ClipboardPasteEnabledResolver") {}

    void runTest() override
    {
        testEmptyClipboardAllPasteDisabled();
        testModuleCopyDco1EnablesDcoTargets();
        testModuleCopyEnv1EnablesAllEnvelopes();
        testFullPatchCopyDisablesModulePasteEnablesInternal();
        testMatrixModCopyDisablesModuleAndInternalEnablesMatrixMod();
        testModeSwitchFullPatchThenDco1CopyUpdatesMirrors();
        testPatchModuleKindFromWidgetId();
    }

private:
    void expectAllModulePasteDisabled(const Core::ClipboardPasteEnabledState& state)
    {
        expect(!state.dco1, "dco1 paste disabled");
        expect(!state.dco2, "dco2 paste disabled");
        expect(!state.env1, "env1 paste disabled");
        expect(!state.env2, "env2 paste disabled");
        expect(!state.env3, "env3 paste disabled");
        expect(!state.lfo1, "lfo1 paste disabled");
        expect(!state.lfo2, "lfo2 paste disabled");
    }

    void testEmptyClipboardAllPasteDisabled()
    {
        beginTest("emptyClipboard_allPasteDisabled");

        Core::ClipboardService clipboard;
        const auto state = Core::resolvePasteEnabled(clipboard);

        expect(!clipboard.hasContent(), "clipboard empty");
        expectAllModulePasteDisabled(state);
        expect(!state.internalPatches, "internal paste disabled");
        expect(!state.matrixModulation, "matrix mod paste disabled");
    }

    void testModuleCopyDco1EnablesDcoTargets()
    {
        beginTest("moduleCopy_dco1_enablesDcoPasteTargets");

        Core::ClipboardService clipboard;
        Core::PatchModel model;
        clipboard.copyModule(Core::PatchModuleKind::Dco1, model);

        const auto state = Core::resolvePasteEnabled(clipboard);

        expect(state.dco1, "same dco family paste enabled");
        expect(state.dco2, "compatible dco2 paste enabled");
        expect(!state.env1 && !state.env2 && !state.env3, "envelope paste disabled");
        expect(!state.lfo1 && !state.lfo2, "lfo paste disabled");
        expect(!state.internalPatches, "internal paste disabled");
        expect(!state.matrixModulation, "matrix mod paste disabled");
    }

    void testModuleCopyEnv1EnablesAllEnvelopes()
    {
        beginTest("moduleCopy_env1_enablesEnvFamily");

        Core::ClipboardService clipboard;
        Core::PatchModel model;
        clipboard.copyModule(Core::PatchModuleKind::Env1, model);

        const auto state = Core::resolvePasteEnabled(clipboard);

        expect(state.env1, "env1 paste enabled");
        expect(state.env2, "env2 paste enabled");
        expect(state.env3, "env3 paste enabled");
        expect(!state.dco1 && !state.dco2 && !state.lfo1 && !state.lfo2, "non-envelope modules disabled");
        expect(!state.internalPatches, "internal paste disabled");
        expect(!state.matrixModulation, "matrix mod paste disabled");
    }

    void testFullPatchCopyDisablesModulePasteEnablesInternal()
    {
        beginTest("fullPatchCopy_disablesAllModulePaste_enablesInternal");

        Core::ClipboardService clipboard;
        Core::PatchModel model;
        clipboard.copyFullPatch(model);

        const auto state = Core::resolvePasteEnabled(clipboard);

        expectAllModulePasteDisabled(state);
        expect(state.internalPatches, "internal paste enabled");
        expect(!state.matrixModulation, "matrix mod paste disabled");
    }

    void testMatrixModCopyDisablesModuleAndInternalEnablesMatrixMod()
    {
        beginTest("matrixModCopy_disablesModuleAndInternal_enablesMatrixMod");

        Core::ClipboardService clipboard;
        Core::PatchModel model;
        clipboard.copyMatrixModulation(model);

        const auto state = Core::resolvePasteEnabled(clipboard);

        expectAllModulePasteDisabled(state);
        expect(!state.internalPatches, "internal paste disabled");
        expect(state.matrixModulation, "matrix mod paste enabled");
    }

    void testModeSwitchFullPatchThenDco1CopyUpdatesMirrors()
    {
        beginTest("modeSwitch_fullPatchThenDco1Copy_updatesMirrors");

        Core::ClipboardService clipboard;
        Core::PatchModel model;

        clipboard.copyFullPatch(model);
        auto state = Core::resolvePasteEnabled(clipboard);
        expect(state.internalPatches, "full patch enables internal paste");
        expectAllModulePasteDisabled(state);

        clipboard.copyModule(Core::PatchModuleKind::Dco1, model);
        state = Core::resolvePasteEnabled(clipboard);

        expect(!state.internalPatches, "module copy disables internal paste");
        expect(!state.matrixModulation, "module copy disables matrix mod paste");
        expect(state.dco1, "dco1 paste enabled");
        expect(state.dco2, "compatible module paste enabled");
    }

    void testPatchModuleKindFromWidgetId()
    {
        beginTest("patchModuleKindFromWidgetId_mapsCopyAndPasteIds");

        const auto dco1Copy = Core::patchModuleKindFromWidgetId(PatchEdit::Dco1Module::StandaloneWidgets::kCopy);
        expect(dco1Copy.has_value() && *dco1Copy == Core::PatchModuleKind::Dco1, "dco1 copy maps");

        const auto env2Paste = Core::patchModuleKindFromWidgetId(PatchEdit::Envelope2Module::StandaloneWidgets::kPaste);
        expect(env2Paste.has_value() && *env2Paste == Core::PatchModuleKind::Env2, "env2 paste maps");

        expect(!Core::patchModuleKindFromWidgetId("internalPatchesCopy").has_value(), "non-module id rejected");
    }
};

static ClipboardPasteEnabledResolverTests clipboardPasteEnabledResolverTests;
