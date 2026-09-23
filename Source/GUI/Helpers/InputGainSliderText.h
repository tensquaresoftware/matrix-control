#pragma once

#include <cmath>

#include <juce_core/juce_core.h>

#include "GUI/Widgets/Slider.h"
#include "Shared/Definitions/PluginAudioConstants.h"

namespace TSS
{
    inline constexpr const char* kInputGainEditAllowedCharacters = "0123456789-+. dbDBinfINF";

    // Display / parse helpers for the header INPUT GAIN slider (index storage, dB text).
    inline juce::String formatInputGainSliderValue(double sliderIndex)
    {
        const int index = static_cast<int>(std::round(sliderIndex));

        if (index <= PluginAudioConstants::kInputGainSilenceIndex)
            return "-" + juce::String::charToString(static_cast<juce::juce_wchar>(0x221E)) + " dB";

        const auto db = PluginAudioConstants::inputGainIndexToDb(index);
        return juce::String(static_cast<int>(std::round(db))) + " dB";
    }

    inline juce::String inputGainEditAllowedCharacters()
    {
        return juce::String(kInputGainEditAllowedCharacters)
             + juce::String::charToString(static_cast<juce::juce_wchar>(0x221E));
    }

    inline juce::String stripInputGainUnitSuffix(juce::String text)
    {
        auto trimmed = text.trim();

        if (trimmed.endsWithIgnoreCase("db"))
            trimmed = trimmed.dropLastCharacters(2).trim();

        return trimmed;
    }

    inline bool isInputGainSilenceToken(const juce::String& token)
    {
        const auto lower = token.toLowerCase();
        const auto infinityGlyph = juce::String::charToString(static_cast<juce::juce_wchar>(0x221E));

        return lower == "-inf" || token == "-" + infinityGlyph;
    }

    inline bool tryParseFiniteInputGainDb(const juce::String& token, float& outGainDb)
    {
        auto numeric = token.trim();

        if (numeric.startsWithChar('+'))
            numeric = numeric.substring(1).trim();

        if (numeric.isEmpty())
            return false;

        int index = 0;

        if (numeric[0] == '-')
        {
            if (numeric.length() == 1)
                return false;

            index = 1;
        }

        bool sawDigit = false;
        bool sawDot = false;

        for (; index < numeric.length(); ++index)
        {
            const auto character = numeric[index];

            if (character >= '0' && character <= '9')
            {
                sawDigit = true;
            }
            else if (character == '.' && ! sawDot)
            {
                sawDot = true;
            }
            else
            {
                return false;
            }
        }

        if (! sawDigit)
            return false;

        const float gainDb = static_cast<float>(numeric.getDoubleValue());

        if (! std::isfinite(gainDb))
            return false;

        outGainDb = gainDb;
        return true;
    }

    // Parses typed edit text in display units (dB). On success writes the slider index.
    // Accepts optional "dB" suffix, "-inf" / "-∞" for silence, optional leading '+', and finite dB.
    inline bool parseInputGainSliderText(const juce::String& text, double& outSliderIndex)
    {
        const auto token = stripInputGainUnitSuffix(text);

        if (token.isEmpty())
            return false;

        if (isInputGainSilenceToken(token))
        {
            outSliderIndex = static_cast<double>(PluginAudioConstants::kInputGainSilenceIndex);
            return true;
        }

        float gainDb = 0.0f;

        if (! tryParseFiniteInputGainDb(token, gainDb))
            return false;

        outSliderIndex = static_cast<double>(PluginAudioConstants::inputGainDbToIndex(gainDb));
        return true;
    }

    inline float inputGainNormalizedFillFromIndex(double sliderIndex)
    {
        return PluginAudioConstants::inputGainIndexToNormalizedFill(
            static_cast<int>(std::round(sliderIndex)));
    }

    inline SliderConfig makeInputGainSliderConfig()
    {
        SliderConfig config;
        config.minValue = static_cast<double>(PluginAudioConstants::kInputGainSilenceIndex);
        config.maxValue = static_cast<double>(PluginAudioConstants::kInputGainMaxIndex);
        config.defaultValue = static_cast<double>(PluginAudioConstants::kInputGainDefaultIndex);
        config.step = 1.0;
        config.normalizedFill = inputGainNormalizedFillFromIndex;
        config.formatValue = formatInputGainSliderValue;
        config.parseValue = parseInputGainSliderText;
        config.allowedEditCharacters = inputGainEditAllowedCharacters();
        return config;
    }
}
