// This file defines Matrix Modulation Section descriptor data: header standalone widgets,
// source/destination choices, and modulation bus arrays. Extracted from PluginDescriptors.cpp
// for modularity. Consumers use these definitions via PluginDescriptors::MatrixModulationSection.

#include "PluginDescriptors.h"

#include <array>

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // Matrix Modulation Section | Header Standalone Widgets
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> MatrixModulationSection::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::MatrixModulationSection::StandaloneWidgets::kMatrixModulationInit,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::MatrixModulationSection::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };


    // =================================================================================================================
    // Matrix Modulation Section | Source/Destination Choices & Modulation Buses
    // =================================================================================================================

    namespace MatrixModulationDetail
    {
        // Source Choices ----------------------------------------------------------------------------------------------
        const juce::StringArray kSourceChoices = PluginHelpers::makeStringArray({
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kNone,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope1,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope2,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kEnvelope3,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo1,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLfo2,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVibrato,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp1,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kRamp2,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboard,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPortamento,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kTrack,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kKeyboardGate,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kVelocity,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kReleaseVelocity,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPressure,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal1,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kPedal2,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever1,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever2,
            PluginDisplayNames::ChoiceLists::ModulationBus::Source::kLever3
        });

        // Destination Choices -----------------------------------------------------------------------------------------
        const juce::StringArray kDestinationChoices = PluginHelpers::makeStringArray({
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kNone, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1Frequency, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1PulseWidth, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1WaveShape,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco2Frequency, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco2PulseWidth, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco2WaveShape,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kDco1Dco2Mix, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVcfFmByDco1, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVcfFrequency, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVcfResonance,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVca1Volume, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kVca2Volume,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Delay, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Attack, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Decay,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Release, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv1Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Delay, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Attack, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Decay,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Release, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv2Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Delay, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Attack, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Decay,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Release, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kEnv3Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo1Speed, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo1Amplitude, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo2Speed, PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kLfo2Amplitude,
            PluginDisplayNames::ChoiceLists::ModulationBus::Destination::kPortamentoRate
        });


        // =============================================================================================================
        // Matrix Modulation Section | Bus 0
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus0Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus0Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus0,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus0Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 105,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus0Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 104,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus0,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 106,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 1
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus1Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus1Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus1,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus1Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 108,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus1Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 107,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus1Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus1,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 109,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 2
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus2Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus2Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus2,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus2Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 111,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus2Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 110,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus2Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus2,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 112,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 3
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus3Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus3Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus3,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus3Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 114,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus3Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 113,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus3Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus3,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 115,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 4
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus4Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus4Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus4,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus4Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 117,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus4Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 116,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus4Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus4,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 118,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 5
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus5Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus5Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus5,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus5Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 120,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus5Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 119,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus5Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus5,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 121,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 6
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus6Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus6Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus6,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus6Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 123,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus6Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 122,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus6Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus6,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 124,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 7
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus7Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus7Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus7,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus7Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 126,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus7Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 125,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus7Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus7,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 127,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 8
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus8Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus8Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus8,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus8Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 129,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus8Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 128,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus8Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus8,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 130,
            .sysExId = kNoSysExId
        } };


        // =============================================================================================================
        // Matrix Modulation Section | Bus 9
        // =============================================================================================================

        // Standalone Widgets ------------------------------------------------------------------------------------------
        const std::vector<StandaloneWidgetDescriptor> bus9Standalone = { {
            .widgetId = PluginIDs::MatrixModulationSection::ModulationBus::StandaloneWidgets::kBus9Init,
            .displayName = PluginDisplayNames::MatrixModulationSection::ModulationBus::StandaloneWidgets::kInitBus9,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .widgetType = StandaloneWidgetType::kButton,
        } };

        // Int Parameters ----------------------------------------------------------------------------------------------
        const std::vector<IntParameterDescriptor> bus9Int = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Amount,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kAmount,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 132,
            .sysExId = kNoSysExId
        } };

        // Choice Parameters -------------------------------------------------------------------------------------------
        const std::vector<ChoiceParameterDescriptor> bus9Choice = { {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Source,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kSource,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .choices = kSourceChoices,
            .defaultIndex = 0,
            .sysExOffset = 131,
            .sysExId = kNoSysExId
        }, {
            .parameterId = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus9Destination,
            .displayName = PluginDisplayNames::MatrixModulationSection::Header::kDestination,
            .parentGroupId = PluginIDs::MatrixModulationSection::ModulationBus::kBus9,
            .choices = kDestinationChoices,
            .defaultIndex = 0,
            .sysExOffset = 133,
            .sysExId = kNoSysExId
        } };
    }


    // =================================================================================================================
    // Matrix Modulation Section | Modulation Bus Arrays (public API)
    // =================================================================================================================

    const std::array<std::vector<StandaloneWidgetDescriptor>, ::Matrix1000Limits::kModulationBusCount> MatrixModulationSection::kModulationBusStandaloneWidgets = {{
        MatrixModulationDetail::bus0Standalone,
        MatrixModulationDetail::bus1Standalone,
        MatrixModulationDetail::bus2Standalone,
        MatrixModulationDetail::bus3Standalone,
        MatrixModulationDetail::bus4Standalone,
        MatrixModulationDetail::bus5Standalone,
        MatrixModulationDetail::bus6Standalone,
        MatrixModulationDetail::bus7Standalone,
        MatrixModulationDetail::bus8Standalone,
        MatrixModulationDetail::bus9Standalone
    }};

    const std::array<std::vector<IntParameterDescriptor>, ::Matrix1000Limits::kModulationBusCount> MatrixModulationSection::kModulationBusIntParameters = {{
        MatrixModulationDetail::bus0Int,
        MatrixModulationDetail::bus1Int,
        MatrixModulationDetail::bus2Int,
        MatrixModulationDetail::bus3Int,
        MatrixModulationDetail::bus4Int,
        MatrixModulationDetail::bus5Int,
        MatrixModulationDetail::bus6Int,
        MatrixModulationDetail::bus7Int,
        MatrixModulationDetail::bus8Int,
        MatrixModulationDetail::bus9Int
    }};

    const std::array<std::vector<ChoiceParameterDescriptor>, ::Matrix1000Limits::kModulationBusCount> MatrixModulationSection::kModulationBusChoiceParameters = {{
        MatrixModulationDetail::bus0Choice,
        MatrixModulationDetail::bus1Choice,
        MatrixModulationDetail::bus2Choice,
        MatrixModulationDetail::bus3Choice,
        MatrixModulationDetail::bus4Choice,
        MatrixModulationDetail::bus5Choice,
        MatrixModulationDetail::bus6Choice,
        MatrixModulationDetail::bus7Choice,
        MatrixModulationDetail::bus8Choice,
        MatrixModulationDetail::bus9Choice
    }};
}
