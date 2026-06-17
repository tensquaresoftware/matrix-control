#include <juce_core/juce_core.h>

#include "MigratedModulePanelLayouts.h"
#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"
#include "Shared/Exceptions/WidgetFactoryExceptions.h"

namespace
{
    void expectAllMigratedPanelLayoutsResolve(juce::UnitTest& test)
    {
        const auto layouts = MigratedModulePanelLayouts::all();
        test.expect(layouts.size() == 13, "Expected 13 migrated module panel layouts");

        for (const auto& [panelName, layout] : layouts)
        {
            for (const auto& parameterId : layout.orderedParameterIds)
            {
                if (parameterId.isEmpty())
                    continue;

                const auto kind = PluginHelpers::resolveParameterWidgetKind(parameterId);
                test.expect(kind.has_value(),
                            juce::String(panelName) + " parameter should resolve: " + parameterId);
            }

            const auto config = buildModulePanelConfig(layout);
            test.expect(config.parameters.size() == layout.orderedParameterIds.size(),
                        juce::String(panelName) + " config row count should match layout");
        }
    }

    void expectDescriptorKindSpotChecks(juce::UnitTest& test)
    {
        const auto sliderKind = PluginHelpers::resolveParameterWidgetKind(
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency);
        test.expect(sliderKind == PluginHelpers::ParameterWidgetKind::Slider,
                    "Int descriptor should resolve to Slider");

        const auto comboKind = PluginHelpers::resolveParameterWidgetKind(
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync);
        test.expect(comboKind == PluginHelpers::ParameterWidgetKind::ComboBox,
                    "Choice descriptor should resolve to ComboBox");
    }

    void expectSeparatorRow(juce::UnitTest& test)
    {
        const auto layout = makePatchEditInitOnlyModuleLayout(
            PluginIDs::PatchEditSection::FmTrackModule::kGroupId,
            PluginIDs::PatchEditSection::FmTrackModule::StandaloneWidgets::kInit,
            {
                PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kFmAmount,
                "",
                PluginIDs::PatchEditSection::FmTrackModule::ParameterWidgets::kTrackInput
            });

        const auto config = buildModulePanelConfig(layout);
        test.expect(config.parameters.size() == 3, "Separator layout should have three rows");
        if (config.parameters.size() == 3)
            test.expect(config.parameters[1].parameterType == ParameterCell::ParameterType::None,
                        "Empty ID should map to separator row");
    }

    void expectUnknownParameterIdThrows(juce::UnitTest& test)
    {
        const auto layout = makePatchEditInitOnlyModuleLayout(
            PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
            { "nonexistent.parameter.id" });

        bool threw = false;
        try
        {
            buildModulePanelConfig(layout);
        }
        catch (const ParameterNotFoundException&)
        {
            threw = true;
        }

        test.expect(threw, "Unknown parameter ID should throw ParameterNotFoundException");
    }

    void expectNoDescriptorIdCollisions(juce::UnitTest& test)
    {
        const std::vector<const std::vector<PluginDescriptors::IntParameterDescriptor>*> intLists = {
            &PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters,
            &PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters,
            &PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters,
            &PluginDescriptors::PatchEditSection::FmTrackModule::kIntParameters,
            &PluginDescriptors::PatchEditSection::RampPortamentoModule::kIntParameters,
            &PluginDescriptors::PatchEditSection::Envelope1Module::kIntParameters,
            &PluginDescriptors::PatchEditSection::Envelope2Module::kIntParameters,
            &PluginDescriptors::PatchEditSection::Envelope3Module::kIntParameters,
            &PluginDescriptors::PatchEditSection::Lfo1Module::kIntParameters,
            &PluginDescriptors::PatchEditSection::Lfo2Module::kIntParameters,
            &PluginDescriptors::MasterEditSection::MidiModule::kIntParameters,
            &PluginDescriptors::MasterEditSection::VibratoModule::kIntParameters,
            &PluginDescriptors::MasterEditSection::MiscModule::kIntParameters
        };

        for (const auto* parameters : intLists)
        {
            for (const auto& param : *parameters)
            {
                bool threw = false;
                try
                {
                    PluginHelpers::resolveParameterWidgetKind(param.parameterId);
                }
                catch (const InvalidParameterException&)
                {
                    threw = true;
                }

                test.expect(! threw,
                            "Descriptor ID should not exist in both int and choice collections: "
                                + param.parameterId);
            }
        }
    }
}

class ModulePanelConfigBuilderTests final : public juce::UnitTest
{
public:
    ModulePanelConfigBuilderTests()
        : juce::UnitTest("ModulePanelConfigBuilder", "GUI")
    {
    }

    void runTest() override
    {
        beginTest("All migrated panel layouts resolve via PluginHelpers");
        expectAllMigratedPanelLayoutsResolve(*this);

        beginTest("Descriptor kinds spot-check Slider vs ComboBox");
        expectDescriptorKindSpotChecks(*this);

        beginTest("Empty parameter ID maps to separator row");
        expectSeparatorRow(*this);

        beginTest("Unknown parameter ID throws ParameterNotFoundException");
        expectUnknownParameterIdThrows(*this);

        beginTest("No int/choice descriptor ID collisions");
        expectNoDescriptorIdCollisions(*this);
    }
};

static ModulePanelConfigBuilderTests modulePanelConfigBuilderTests;
