#include "ClipboardPasteEnabledResolver.h"

#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

namespace
{
    namespace PatchEdit = PluginIDs::PatchEditSection;

    std::optional<PatchModuleKind> kindFromCopyOrPasteId(const juce::String& widgetId)
    {
        if (widgetId == PatchEdit::Dco1Module::StandaloneWidgets::kCopy
            || widgetId == PatchEdit::Dco1Module::StandaloneWidgets::kPaste)
            return PatchModuleKind::Dco1;

        if (widgetId == PatchEdit::Dco2Module::StandaloneWidgets::kCopy
            || widgetId == PatchEdit::Dco2Module::StandaloneWidgets::kPaste)
            return PatchModuleKind::Dco2;

        if (widgetId == PatchEdit::Envelope1Module::StandaloneWidgets::kCopy
            || widgetId == PatchEdit::Envelope1Module::StandaloneWidgets::kPaste)
            return PatchModuleKind::Env1;

        if (widgetId == PatchEdit::Envelope2Module::StandaloneWidgets::kCopy
            || widgetId == PatchEdit::Envelope2Module::StandaloneWidgets::kPaste)
            return PatchModuleKind::Env2;

        if (widgetId == PatchEdit::Envelope3Module::StandaloneWidgets::kCopy
            || widgetId == PatchEdit::Envelope3Module::StandaloneWidgets::kPaste)
            return PatchModuleKind::Env3;

        if (widgetId == PatchEdit::Lfo1Module::StandaloneWidgets::kCopy
            || widgetId == PatchEdit::Lfo1Module::StandaloneWidgets::kPaste)
            return PatchModuleKind::Lfo1;

        if (widgetId == PatchEdit::Lfo2Module::StandaloneWidgets::kCopy
            || widgetId == PatchEdit::Lfo2Module::StandaloneWidgets::kPaste)
            return PatchModuleKind::Lfo2;

        return std::nullopt;
    }
}

ClipboardPasteEnabledState resolvePasteEnabled(const ClipboardService& clipboard)
{
    ClipboardPasteEnabledState state;
    state.dco1 = clipboard.canPasteModule(PatchModuleKind::Dco1);
    state.dco2 = clipboard.canPasteModule(PatchModuleKind::Dco2);
    state.env1 = clipboard.canPasteModule(PatchModuleKind::Env1);
    state.env2 = clipboard.canPasteModule(PatchModuleKind::Env2);
    state.env3 = clipboard.canPasteModule(PatchModuleKind::Env3);
    state.lfo1 = clipboard.canPasteModule(PatchModuleKind::Lfo1);
    state.lfo2 = clipboard.canPasteModule(PatchModuleKind::Lfo2);
    state.internalPatches = clipboard.canPasteFullPatch();
    state.matrixModulation = clipboard.canPasteMatrixModulation();
    return state;
}

std::optional<PatchModuleKind> patchModuleKindFromWidgetId(const juce::String& widgetId)
{
    return kindFromCopyOrPasteId(widgetId);
}

} // namespace Core
