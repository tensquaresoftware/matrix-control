// Patch Edit Section descriptors — Envelope 1 and Envelope 2 modules.
// Envelope 3 lives in PluginDescriptorsPatchEditEnvelope3.cpp.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // Patch Edit Section | Envelope 1 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Envelope1Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Envelope1Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kDelay,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kDelay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 50,
            .sysExId = 50
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kAttack,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kAttack,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 51,
            .sysExId = 51
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kDecay,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kDecay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 52,
            .sysExId = 52
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kSustain,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kSustain,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 50,
            .sysExOffset = 53,
            .sysExId = 53
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kRelease,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kRelease,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 54,
            .sysExId = 54
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitude,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 40,
            .sysExOffset = 55,
            .sysExId = 55
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kAmplitudeModByVelocity,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kAmplitudeModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 94,
            .sysExId = 56
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Envelope1Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kTriggerMode,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::TriggerMode::kStrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kSreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
            .defaultIndex = 0,
            .sysExOffset = 49,
            .sysExId = 57
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kEnvelopeMode,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kEnvelopeMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
            .defaultIndex = 0,
            .sysExOffset = 57,
            .sysExId = 58
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope1Module::ParameterWidgets::kLfo1Trigger,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope1Module::kLfo1Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope1Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
            .defaultIndex = 0,
            .sysExOffset = 56,
            .sysExId = 59
        }
    };


    // =================================================================================================================
    // Patch Edit Section | Envelope 2 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Envelope2Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Envelope2Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kDelay,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kDelay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 59,
            .sysExId = 60
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kAttack,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kAttack,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 60,
            .sysExId = 61
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kDecay,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kDecay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 61,
            .sysExId = 62
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kSustain,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kSustain,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 50,
            .sysExOffset = 62,
            .sysExId = 63
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kRelease,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kRelease,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 10,
            .sysExOffset = 63,
            .sysExId = 64
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitude,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 40,
            .sysExOffset = 64,
            .sysExId = 65
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kAmplitudeModByVelocity,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kAmplitudeModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 95,
            .sysExId = 66
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Envelope2Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kTriggerMode,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::TriggerMode::kStrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kSreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kMreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmtrig,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXreset,
                PluginDisplayNames::ChoiceLists::TriggerMode::kXmrst }),
            .defaultIndex = 0,
            .sysExOffset = 58,
            .sysExId = 67
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kEnvelopeMode,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kEnvelopeMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
            .defaultIndex = 0,
            .sysExOffset = 66,
            .sysExId = 68
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope2Module::ParameterWidgets::kLfo1Trigger,
            .hostName = PluginDisplayNames::HostParameterNames::PatchEditSection::Envelope2Module::kLfo1Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope2Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
            .defaultIndex = 0,
            .sysExOffset = 65,
            .sysExId = 69
        }
    };
}
