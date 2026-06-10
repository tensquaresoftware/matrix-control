#include "WidgetDimensionRegistry.h"

#include "GUI/Layout/Design/Design.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    using namespace TSS::Design;

    bool endsWith(const juce::String& s, const char* suffix)
    {
        return s.endsWith(suffix);
    }
}

std::optional<int> WidgetDimensionRegistry::resolveStandaloneButtonWidth(const juce::String& widgetId)
{
    using namespace PluginIDs;
    namespace BW = TSS::Design::Atoms::Widths::Button;

    if (widgetId == PatchManagerSection::BankUtilityModule::StandaloneWidgets::kLockBank)
        return BW::kPatchManagerLockBank;

    if (widgetId.startsWith("bankUtilitySelectBank"))
        return BW::kPatchManagerBankSelect;

    if (widgetId == PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch)
        return BW::kInternalPatchesInit;
    if (widgetId == PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch)
        return BW::kInternalPatchesCopy;
    if (widgetId == PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch)
        return BW::kInternalPatchesPaste;
    if (widgetId == PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch)
        return BW::kInternalPatchesStore;

    if (widgetId == PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder)
        return BW::kComputerPatchesLoad;
    if (widgetId == PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs)
        return BW::kComputerPatchesSaveAs;
    if (widgetId == PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile)
        return BW::kComputerPatchesSave;

    if (widgetId == PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate)
        return BW::kPatchMutatorMutate;

    if (widgetId == PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry)
        return BW::kPatchMutatorRetry;

    if (widgetId == PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare)
        return BW::kPatchMutatorCompare;

    if (widgetId == PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete)
        return BW::kPatchMutatorDelete;

    if (widgetId == PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear)
        return BW::kPatchMutatorClear;

    if (widgetId == PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport)
        return BW::kPatchMutatorExport;

    if (widgetId.startsWith("patchMutatorEnable"))
        return BW::kInit;

    if (endsWith(widgetId, "LoadPrevious") || endsWith(widgetId, "LoadNext"))
        return BW::kInit;

    if (endsWith(widgetId, "Init"))
        return BW::kInit;
    if (endsWith(widgetId, "Copy"))
        return BW::kCopy;
    if (endsWith(widgetId, "Paste"))
        return BW::kPaste;

    return std::nullopt;
}

bool WidgetDimensionRegistry::isStandaloneButtonWidthResolvable(const juce::String& widgetId)
{
    return resolveStandaloneButtonWidth(widgetId).has_value();
}
