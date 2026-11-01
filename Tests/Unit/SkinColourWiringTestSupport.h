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

    inline void paintPeakLedAndSliderPastilles(TSS::Skin& skin)
    {
        using O = TSS::SkinObjectType;
        using P = TSS::SkinColourProperty;
        using S = TSS::SkinColourState;
        using C = TSS::ColourSet;

        paintKey(skin, TSS::skinColourKey(O::kPeakIndicator, P::kBackground, S::kOn, C::k3),
                 juce::Colour(ColourChart::kYellow));
        paintKey(skin, TSS::skinColourKey(O::kPeakIndicator, P::kBackground, S::kOn, C::k4),
                 juce::Colour(ColourChart::kGrey600));
        paintKey(skin, TSS::skinColourKey(O::kLed, P::kBackground, S::kOn, C::k3),
                 juce::Colour(ColourChart::kOrange));
        paintKey(skin, TSS::skinColourKey(O::kLed, P::kBackground, S::kOn, C::k4),
                 juce::Colour(ColourChart::kBlue));
        paintKey(skin, TSS::skinColourKey(O::kSlider, P::kTrack, S::kDefault, C::k1),
                 juce::Colour(ColourChart::kEmerald400));
        paintKey(skin, TSS::skinColourKey(O::kSlider, P::kTrack, S::kDefault, C::k2),
                 juce::Colour(ColourChart::kOrange));
        paintKey(skin, TSS::skinColourKey(O::kSlider, P::kTrack, S::kDefault, C::k3),
                 juce::Colour(ColourChart::kYellow));
    }

    inline void paintButtonLikeAndNumberBoxPastilles(TSS::Skin& skin)
    {
        using O = TSS::SkinObjectType;
        using P = TSS::SkinColourProperty;
        using S = TSS::SkinColourState;
        using C = TSS::ColourSet;

        paintKey(skin, TSS::skinColourKey(O::kSliderButtonLike, P::kTrack, S::kDefault, C::k4),
                 juce::Colour(ColourChart::kGrey600));
        paintKey(skin, TSS::skinColourKey(O::kSliderButtonLike, P::kTrack, S::kDefault, C::k1),
                 juce::Colour(ColourChart::kEmerald400));
        paintKey(skin, TSS::skinColourKey(O::kComboBoxButtonLike, P::kBackground, S::kDefault, C::k4),
                 juce::Colour(ColourChart::kGrey600));
        paintKey(skin, TSS::skinColourKey(O::kComboBoxButtonLike, P::kBackground, S::kDefault, C::k1),
                 juce::Colour(ColourChart::kEmerald400));
        paintKey(skin, TSS::skinColourKey(O::kNumberBox, P::kBackground, S::kDefault, C::k4),
                 juce::Colour(ColourChart::kGrey600));
        paintKey(skin, TSS::skinColourKey(O::kNumberBox, P::kBackground, S::kDefault, C::k1),
                 juce::Colour(ColourChart::kEmerald400));
        paintKey(skin, TSS::skinColourKey(O::kNumberBox, P::kText, S::kFocus, C::k1),
                 juce::Colour(ColourChart::kEmerald400));
        paintKey(skin, TSS::skinColourKey(O::kNumberBox, P::kText, S::kFocus, C::k4),
                 juce::Colour(ColourChart::kRed500));
    }

    inline void paintButtonAndReadOnlyPastilles(TSS::Skin& skin)
    {
        using O = TSS::SkinObjectType;
        using P = TSS::SkinColourProperty;
        using S = TSS::SkinColourState;
        using C = TSS::ColourSet;

        paintKey(skin, TSS::skinColourKey(O::kButton, P::kBackground, S::kOff, C::k1),
                 juce::Colour(ColourChart::kEmerald400));
        paintKey(skin, TSS::skinColourKey(O::kButton, P::kBackground, S::kOff, C::k2),
                 juce::Colour(ColourChart::kOrange));
        paintKey(skin, TSS::skinColourKey(O::kReadOnlyValueField, P::kBackground, S::kDefault, C::k1),
                 juce::Colour(ColourChart::kEmerald400));
        paintKey(skin, TSS::skinColourKey(O::kReadOnlyValueField, P::kBackground, S::kDefault, C::k4),
                 juce::Colour(ColourChart::kGrey600));
        paintKey(skin, TSS::skinColourKey(O::kReadOnlyValueField, P::kText, S::kDefault, C::k1),
                 juce::Colour(ColourChart::kBlue));
        paintKey(skin, TSS::skinColourKey(O::kReadOnlyValueField, P::kText, S::kDefault, C::k4),
                 juce::Colour(ColourChart::kYellow));
    }

    inline void paintDistinctPastillesForSets(TSS::Skin& skin)
    {
        paintPeakLedAndSliderPastilles(skin);
        paintButtonLikeAndNumberBoxPastilles(skin);
        paintButtonAndReadOnlyPastilles(skin);
    }
}
