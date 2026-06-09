#include <juce_core/juce_core.h>

#include "GUI/Layout/Design/Design.h"
#include "GUI/Layout/WidgetDimensionRegistry.h"
#include "Shared/Definitions/Matrix1000Limits.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    namespace BW = TSS::Design::Atoms::Widths::Button;

    void expectButtonWidth(juce::UnitTest& test,
                           const juce::String& widgetId,
                           int expectedWidth,
                           const juce::String& context)
    {
        const auto width = WidgetDimensionRegistry::resolveStandaloneButtonWidth(widgetId);
        test.expect(width.has_value(), context + ": no resolvable width");
        if (! width.has_value())
            return;

        test.expect(*width == expectedWidth,
                    context + ": expected " + juce::String(expectedWidth)
                        + " got " + juce::String(*width));
    }

    void expectAllStandaloneButtonsResolvable(juce::UnitTest& test)
    {
        const std::vector<const std::vector<PluginDescriptors::StandaloneWidgetDescriptor>*> widgetSets = {
            &PluginDescriptors::PatchEditSection::Dco1Module::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::Dco2Module::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::VcfVcaModule::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::FmTrackModule::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::RampPortamentoModule::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::Envelope1Module::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::Envelope2Module::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::Envelope3Module::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::Lfo1Module::kStandaloneWidgets,
            &PluginDescriptors::PatchEditSection::Lfo2Module::kStandaloneWidgets,
            &PluginDescriptors::MatrixModulationSection::kStandaloneWidgets,
            &PluginDescriptors::PatchManagerSection::BankUtilityModule::kStandaloneWidgets,
            &PluginDescriptors::PatchManagerSection::InternalPatchesModule::kStandaloneWidgets,
            &PluginDescriptors::PatchManagerSection::ComputerPatchesModule::kStandaloneWidgets,
            &PluginDescriptors::PatchManagerSection::PatchMutatorModule::kStandaloneWidgets,
            &PluginDescriptors::MasterEditSection::kStandaloneWidgets,
        };

        for (const auto* widgets : widgetSets)
        {
            for (const auto& widget : *widgets)
            {
                if (widget.widgetType != PluginDescriptors::StandaloneWidgetType::kButton)
                    continue;

                const auto width = WidgetDimensionRegistry::resolveStandaloneButtonWidth(widget.widgetId);
                test.expect(width.has_value(),
                            "Missing registry width for button widgetId: " + widget.widgetId);
                if (width.has_value())
                    test.expect(*width > 0,
                                "Non-positive registry width for button widgetId: " + widget.widgetId);
            }
        }

        for (int bus = 0; bus < Matrix1000Limits::kModulationBusCount; ++bus)
        {
            for (const auto& widget : PluginDescriptors::MatrixModulationSection::kModulationBusStandaloneWidgets[static_cast<size_t>(bus)])
            {
                if (widget.widgetType != PluginDescriptors::StandaloneWidgetType::kButton)
                    continue;

                const auto width = WidgetDimensionRegistry::resolveStandaloneButtonWidth(widget.widgetId);
                test.expect(width.has_value(),
                            "Missing registry width for bus button widgetId: " + widget.widgetId);
                if (width.has_value())
                    test.expect(*width > 0,
                                "Non-positive registry width for bus button widgetId: " + widget.widgetId);
            }
        }
    }

    void expectExplicitButtonWidths(juce::UnitTest& test)
    {
        using namespace PluginIDs;

        expectButtonWidth(test,
                          PatchManagerSection::BankUtilityModule::StandaloneWidgets::kLockBank,
                          BW::kPatchManagerLockBank,
                          "lock bank");
        expectButtonWidth(test,
                          PatchManagerSection::BankUtilityModule::StandaloneWidgets::kSelectBank0,
                          BW::kPatchManagerBankSelect,
                          "select bank");

        expectButtonWidth(test,
                          PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kInitPatch,
                          BW::kInternalPatchesInit,
                          "internal init");
        expectButtonWidth(test,
                          PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kCopyPatch,
                          BW::kInternalPatchesCopy,
                          "internal copy");
        expectButtonWidth(test,
                          PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kPastePatch,
                          BW::kInternalPatchesPaste,
                          "internal paste");
        expectButtonWidth(test,
                          PatchManagerSection::InternalPatchesModule::StandaloneWidgets::kStorePatch,
                          BW::kInternalPatchesStore,
                          "internal store");

        expectButtonWidth(test,
                          PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kOpenPatchFolder,
                          BW::kComputerPatchesLoad,
                          "computer load");
        expectButtonWidth(test,
                          PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchAs,
                          BW::kComputerPatchesSaveAs,
                          "computer save as");
        expectButtonWidth(test,
                          PatchManagerSection::ComputerPatchesModule::StandaloneWidgets::kSavePatchFile,
                          BW::kComputerPatchesSave,
                          "computer save");

        expectButtonWidth(test,
                          PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kMutate,
                          BW::kPatchMutatorMutate,
                          "mutate");
        expectButtonWidth(test,
                          PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kRetry,
                          BW::kPatchMutatorMutate,
                          "retry");
        expectButtonWidth(test,
                          PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kCompare,
                          BW::kPatchMutatorCompare,
                          "compare");
        expectButtonWidth(test,
                          PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kDelete,
                          BW::kPatchMutatorDelete,
                          "delete");
        expectButtonWidth(test,
                          PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kClear,
                          BW::kPatchMutatorClear,
                          "clear");
        expectButtonWidth(test,
                          PatchManagerSection::PatchMutatorModule::StandaloneWidgets::kExport,
                          BW::kPatchMutatorExport,
                          "export");

        expectButtonWidth(test,
                          PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
                          BW::kInit,
                          "dco1 init suffix");
        expectButtonWidth(test,
                          PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
                          BW::kCopy,
                          "dco1 copy suffix");
        expectButtonWidth(test,
                          PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
                          BW::kPaste,
                          "dco1 paste suffix");
    }
}

class WidgetDimensionRegistryTests : public juce::UnitTest
{
public:
    WidgetDimensionRegistryTests() : juce::UnitTest("WidgetDimensionRegistry Tests") {}

    void runTest() override
    {
        beginTest("All standalone button descriptors resolve a width");
        expectAllStandaloneButtonsResolvable(*this);

        beginTest("Explicit standalone button widths match design atoms");
        expectExplicitButtonWidths(*this);
    }
};

static WidgetDimensionRegistryTests widgetDimensionRegistryTests;
