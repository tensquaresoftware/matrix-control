#pragma once

#include <stdexcept>

#include <juce_core/juce_core.h>

namespace WidgetFactoryErrors
{
    enum class Severity
    {
        Info,      // Message informatif (gris)
        Success,   // Opération réussie (vert)
        Warning,   // Avertissement (orange)
        Error      // Erreur (rouge)
    };
}

class WidgetFactoryException : public std::runtime_error
{
public:
    explicit WidgetFactoryException(
        const juce::String& inMessage,
        WidgetFactoryErrors::Severity inSeverity = WidgetFactoryErrors::Severity::Error)
        : std::runtime_error(inMessage.toStdString()),
          message(inMessage),
          severity(inSeverity)
    {
        logToLogger();
    }
    
    const juce::String& getMessage() const { return message; }
    WidgetFactoryErrors::Severity getSeverity() const { return severity; }
    
    juce::String getSeverityString() const
    {
        switch (severity)
        {
            case WidgetFactoryErrors::Severity::Info: return "INFO";
            case WidgetFactoryErrors::Severity::Success: return "SUCCESS";
            case WidgetFactoryErrors::Severity::Warning: return "WARNING";
            case WidgetFactoryErrors::Severity::Error: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
private:
    juce::String message;
    WidgetFactoryErrors::Severity severity;
    
    void logToLogger() const
    {
        juce::Logger::writeToLog(
            "WidgetFactory [" + getSeverityString() + "]: " + message
        );
    }
};

class ParameterNotFoundException : public WidgetFactoryException
{
public:
    explicit ParameterNotFoundException(const juce::String& inParameterId)
        : WidgetFactoryException(
              "Parameter not found: " + inParameterId,
              WidgetFactoryErrors::Severity::Error),
          parameterId(inParameterId)
    {}
    
    const juce::String& getParameterId() const { return parameterId; }
    
private:
    juce::String parameterId;
};

class InvalidParameterException : public WidgetFactoryException
{
public:
    explicit InvalidParameterException(
        const juce::String& inParameterId,
        const juce::String& inReason,
        WidgetFactoryErrors::Severity inSeverity = WidgetFactoryErrors::Severity::Error)
        : WidgetFactoryException(
              "Invalid parameter '" + inParameterId + "': " + inReason,
              inSeverity),
          parameterId(inParameterId),
          reason(inReason)
    {}
    
    const juce::String& getParameterId() const { return parameterId; }
    const juce::String& getReason() const { return reason; }
    
private:
    juce::String parameterId;
    juce::String reason;
};

class WidgetNotFoundException : public WidgetFactoryException
{
public:
    explicit WidgetNotFoundException(const juce::String& inWidgetId)
        : WidgetFactoryException(
              "Widget not found: " + inWidgetId,
              WidgetFactoryErrors::Severity::Error),
          widgetId(inWidgetId)
    {}
    
    const juce::String& getWidgetId() const { return widgetId; }
    
private:
    juce::String widgetId;
};

class InvalidWidgetTypeException : public WidgetFactoryException
{
public:
    explicit InvalidWidgetTypeException(
        const juce::String& inWidgetId,
        const juce::String& inExpectedType,
        const juce::String& inActualType)
        : WidgetFactoryException(
              "Widget '" + inWidgetId + "' has invalid type. Expected: " + 
              inExpectedType + ", Got: " + inActualType,
              WidgetFactoryErrors::Severity::Error),
          widgetId(inWidgetId),
          expectedType(inExpectedType),
          actualType(inActualType)
    {}
    
    const juce::String& getWidgetId() const { return widgetId; }
    const juce::String& getExpectedType() const { return expectedType; }
    const juce::String& getActualType() const { return actualType; }
    
private:
    juce::String widgetId;
    juce::String expectedType;
    juce::String actualType;
};
