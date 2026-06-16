#include <juce_core/juce_core.h>

#include "GUI/Panels/Reusable/ModulePanelConfigBuilder.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginHelpers.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    void expectDescriptorListsResolve(juce::UnitTest& test)
    {
        const std::vector<const std::vector<PluginDescriptors::IntParameterDescriptor>*> intLists = {
            &PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters,
            &PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters,
            &PluginDescriptors::MasterEditSection::MidiModule::kIntParameters
        };

        for (const auto* parameters : intLists)
        {
            for (const auto& param : *parameters)
            {
                const auto kind = PluginHelpers::resolveParameterWidgetKind(param.parameterId);
                test.expect(kind == PluginHelpers::ParameterWidgetKind::Slider,
                            "Int descriptor should resolve to Slider: " + param.parameterId);
            }
        }

        const std::vector<const std::vector<PluginDescriptors::ChoiceParameterDescriptor>*> choiceLists = {
            &PluginDescriptors::PatchEditSection::Dco1Module::kChoiceParameters,
            &PluginDescriptors::PatchEditSection::RampPortamentoModule::kChoiceParameters,
            &PluginDescriptors::MasterEditSection::MidiModule::kChoiceParameters
        };

        for (const auto* parameters : choiceLists)
        {
            for (const auto& param : *parameters)
            {
                const auto kind = PluginHelpers::resolveParameterWidgetKind(param.parameterId);
                test.expect(kind == PluginHelpers::ParameterWidgetKind::ComboBox,
                            "Choice descriptor should resolve to ComboBox: " + param.parameterId);
            }
        }
    }

    void expectDco1LayoutBuilds(juce::UnitTest& test)
    {
        const auto layout = makePatchEditModuleLayout(
            PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
            PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kCopy,
            PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kPaste,
            {
                PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
                PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync
            });

        const auto config = buildModulePanelConfig(layout);
        test.expect(config.parameters.size() == 2, "Dco1 sample layout should have two parameter rows");
        if (config.parameters.size() >= 2)
        {
            test.expect(config.parameters[0].parameterType == ParameterCell::ParameterType::Slider,
                        "Frequency should resolve to Slider");
            test.expect(config.parameters[1].parameterType == ParameterCell::ParameterType::ComboBox,
                        "Sync should resolve to ComboBox");
        }
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
        beginTest("Descriptor parameter IDs resolve via PluginHelpers");
        expectDescriptorListsResolve(*this);

        beginTest("buildModulePanelConfig maps descriptor kinds to parameter cells");
        expectDco1LayoutBuilds(*this);

        beginTest("Empty parameter ID maps to separator row");
        expectSeparatorRow(*this);
    }
};

static ModulePanelConfigBuilderTests modulePanelConfigBuilderTests;
