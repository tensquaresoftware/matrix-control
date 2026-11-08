#pragma once

#include <juce_audio_processors/juce_audio_processors.h>

#include "Core/Factories/ApvtsFactory.h"
#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"

namespace SkinColourWiringTestSupport
{
    class MinimalAudioProcessor : public juce::AudioProcessor
    {
    public:
        MinimalAudioProcessor()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", ApvtsFactory::createParameterLayout())
        {
        }

        juce::AudioProcessorValueTreeState apvts;

        const juce::String getName() const override { return "Test"; }
        void prepareToPlay(double, int) override {}
        void releaseResources() override {}
        void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override {}
        juce::AudioProcessorEditor* createEditor() override { return nullptr; }
        bool hasEditor() const override { return false; }
        bool acceptsMidi() const override { return false; }
        bool producesMidi() const override { return false; }
        bool isMidiEffect() const override { return false; }
        double getTailLengthSeconds() const override { return 0.0; }
        int getNumPrograms() override { return 1; }
        int getCurrentProgram() override { return 0; }
        void setCurrentProgram(int) override {}
        const juce::String getProgramName(int) override { return {}; }
        void changeProgramName(int, const juce::String&) override {}
        void getStateInformation(juce::MemoryBlock&) override {}
        void setStateInformation(const void*, int) override {}
    };

    inline ModuleHeaderDimensions makeModuleHeaderDimensions()
    {
        return ModuleHeaderDimensions{
            .height = 24,
            .buttonHeight = 18,
            .patchEditTitleBandWidth = 120,
            .masterEditTitleBandWidth = 120,
            .patchManagerTitleBandWidth = 120,
            .initWidth = 40,
            .copyWidth = 40,
            .pasteWidth = 40,
            .textLeftPadding = 4,
            .textAreaHeight = 16,
            .lineThickness = 2,
        };
    }

    inline void paintKey(TSS::Skin& skin, TSS::SkinColourKey key, juce::Colour colour)
    {
        skin.setColour(key, colour);
    }

    inline juce::Colour distinctPastilleForSet(TSS::ColourSet colourSet)
    {
        switch (colourSet)
        {
            case TSS::ColourSet::k1: return juce::Colour(ColourChart::kBlue400);
            case TSS::ColourSet::k2: return juce::Colour(ColourChart::kOrange400);
            case TSS::ColourSet::k3: return juce::Colour(ColourChart::kYellow400);
            case TSS::ColourSet::k4: return juce::Colour(ColourChart::kGrey600);
        }
        return juce::Colour(ColourChart::kWhite);
    }

    inline void paintEverySupportedSet(TSS::Skin& skin, TSS::SkinColourKey key)
    {
        for (const auto colourSet : TSS::kAllColourSets)
        {
            if (! TSS::supportsColourSet(key.object, colourSet))
                continue;
            key.colourSet = colourSet;
            paintKey(skin, key, distinctPastilleForSet(colourSet));
        }
    }

    /** One distinct pastille per supported colour set on representative MCS keys. */
    inline void paintDistinctPastillesForSets(TSS::Skin& skin)
    {
        using O = TSS::SkinObjectType;
        using P = TSS::SkinColourProperty;
        using S = TSS::SkinColourState;

        paintEverySupportedSet(skin, TSS::skinColourKey(O::kSlider, P::kTrack));
        paintEverySupportedSet(skin, TSS::skinColourKey(O::kComboBox, P::kBackground));
        paintEverySupportedSet(skin, TSS::skinColourKey(O::kPopupMenu, P::kBorder));
        paintEverySupportedSet(skin, TSS::skinColourKey(O::kLed, P::kBackground, S::kOn));
        paintEverySupportedSet(skin, TSS::skinColourKey(O::kPeakIndicator, P::kBackground, S::kOn));
    }
}
