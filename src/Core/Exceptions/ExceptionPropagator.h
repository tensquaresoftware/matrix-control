#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Shared/Exceptions/WidgetFactoryExceptions.h"

class ExceptionPropagator
{
public:
    static void propagateToApvts(
        juce::AudioProcessorValueTreeState& apvts,
        const WidgetFactoryException& exception)
    {
        // Identifiants pour les propriétés APVTS
        static const juce::Identifier kMessageTextId("uiMessageText");
        static const juce::Identifier kMessageSeverityId("uiMessageSeverity");
        static const juce::Identifier kMessageTimestampId("uiMessageTimestamp");
        
        // Convertir la sévérité en String pour l'APVTS
        juce::String severityStr;
        switch (exception.getSeverity())
        {
            case WidgetFactoryErrors::Severity::Info:
                severityStr = "info";
                break;
            case WidgetFactoryErrors::Severity::Success:
                severityStr = "success";
                break;
            case WidgetFactoryErrors::Severity::Warning:
                severityStr = "warning";
                break;
            case WidgetFactoryErrors::Severity::Error:
                severityStr = "error";
                break;
        }
        
        // Mettre à jour l'APVTS (thread-safe)
        apvts.state.setProperty(kMessageTextId, exception.getMessage(), nullptr);
        apvts.state.setProperty(kMessageSeverityId, severityStr, nullptr);
        apvts.state.setProperty(kMessageTimestampId, juce::Time::getCurrentTime().toMilliseconds(), nullptr);
    }
    
    static void clearMessage(juce::AudioProcessorValueTreeState& apvts)
    {
        static const juce::Identifier kMessageTextId("uiMessageText");
        static const juce::Identifier kMessageSeverityId("uiMessageSeverity");
        
        apvts.state.setProperty(kMessageTextId, juce::String(), nullptr);
        apvts.state.setProperty(kMessageSeverityId, juce::String(), nullptr);
    }
};

