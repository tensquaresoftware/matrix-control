// Patch Edit Section descriptors — Envelope 3 module.

#include "PluginDescriptors.h"

#include "PluginDisplayNames.h"
#include "PluginHelpers.h"
#include "PluginIDs.h"


namespace PluginDescriptors
{
    // =================================================================================================================
    // Patch Edit Section | Envelope 3 Module
    // =================================================================================================================

    // Standalone Widgets ----------------------------------------------------------------------------------------------
    const std::vector<StandaloneWidgetDescriptor> PatchEditSection::Envelope3Module::kStandaloneWidgets = {
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kInit,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kCopy,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        },
        {
            .widgetId = PluginIDs::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::StandaloneWidgets::kPaste,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .widgetType = StandaloneWidgetType::kButton,
        }
    };

    // Int Parameters --------------------------------------------------------------------------------------------------
    const std::vector<IntParameterDescriptor> PatchEditSection::Envelope3Module::kIntParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kDelay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 68,
            .sysExId = 70
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kAttack,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 69,
            .sysExId = 71
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kDecay,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 20,
            .sysExOffset = 70,
            .sysExId = 72
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kSustain,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 0,
            .sysExOffset = 71,
            .sysExId = 73
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kRelease,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 20,
            .sysExOffset = 72,
            .sysExId = 74
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitude,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = 0,
            .maxValue = 63,
            .defaultValue = 40,
            .sysExOffset = 73,
            .sysExId = 75
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kAmplitudeModByVelocity,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .minValue = -63,
            .maxValue = 63,
            .defaultValue = 63,
            .sysExOffset = 96,
            .sysExId = 76
        }
    };

    // Choice Parameters -----------------------------------------------------------------------------------------------
    const std::vector<ChoiceParameterDescriptor> PatchEditSection::Envelope3Module::kChoiceParameters = {
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kTriggerMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
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
            .sysExOffset = 67,
            .sysExId = 77
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kEnvelopeMode,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kNormal,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kDadr,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kFree,
                PluginDisplayNames::ChoiceLists::EnvelopeMode::kBoth }),
            .defaultIndex = 0,
            .sysExOffset = 75,
            .sysExId = 78
        },
        {
            .parameterId = PluginIDs::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger,
            .displayName = PluginDisplayNames::PatchEditSection::Envelope3Module::ParameterWidgets::kLfo1Trigger,
            .parentGroupId = PluginIDs::PatchEditSection::Envelope3Module::kGroupId,
            .choices = PluginHelpers::makeStringArray({
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kNormal,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kLfo1,
                PluginDisplayNames::ChoiceLists::Lfo1Trigger::kGLfo1 }),
            .defaultIndex = 0,
            .sysExOffset = 74,
            .sysExId = 79
        }
    };
}
