#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Skins/ColourChart.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"
#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Widgets/PeakIndicator.h"

using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::SkinObjectType;

namespace
{
    class MinimalAudioProcessor : public juce::AudioProcessor
    {
    public:
        MinimalAudioProcessor()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", {})
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
}

class SkinColourWiringTests : public juce::UnitTest
{
public:
    SkinColourWiringTests()
        : juce::UnitTest("SkinColourWiring")
    {
    }

    void runTest() override
    {
        beginTest("lookFromSkin_resolvesRequestedColourSet");
        assertLookFromSkinResolvesColourSet(TSS::Skin::ColourVariant::Black);
        assertLookFromSkinResolvesColourSet(TSS::Skin::ColourVariant::Cream);

        beginTest("headerVsSettings_peakAndLedUseDistinctColourSets");
        assertHeaderVsSettingsPeakAndLedDistinctSets();

        beginTest("led_setSkinPreservesColourSet");
        assertLedSetSkinPreservesColourSet();

        beginTest("peakIndicator_setSkinPreservesColourSet");
        assertPeakIndicatorSetSkinPreservesColourSet();

        beginTest("parameterCell_moduleTypeMapsToColourSet");
        assertParameterCellModuleTypeMapsToColourSet();

        beginTest("settingsAndComputerPatches_buttonLikeAndNumberBoxUseK4");
        assertSettingsAndComputerPatchesUseK4();

        beginTest("headerInputGain_sliderUsesK3");
        assertHeaderInputGainSliderUsesK3();

        beginTest("blackCream_identicalSeedKeepsLooksAcrossSets");
        assertBlackCreamIdenticalSeedLooksAcrossSets(TSS::Skin::ColourVariant::Black);
        assertBlackCreamIdenticalSeedLooksAcrossSets(TSS::Skin::ColourVariant::Cream);
    }

private:
    void paintDistinctPastillesForSets(TSS::Skin& skin)
    {
        using O = SkinObjectType;
        using P = SkinColourProperty;
        using S = SkinColourState;
        using C = TSS::ColourSet;

        skin.setColour(TSS::skinColourKey(O::kPeakIndicator, P::kBackground, S::kOn, C::k3),
                       juce::Colour(ColourChart::kYellow));
        skin.setColour(TSS::skinColourKey(O::kPeakIndicator, P::kBackground, S::kOn, C::k4),
                       juce::Colour(ColourChart::kGrey600));
        skin.setColour(TSS::skinColourKey(O::kLed, P::kBackground, S::kOn, C::k3),
                       juce::Colour(ColourChart::kOrange));
        skin.setColour(TSS::skinColourKey(O::kLed, P::kBackground, S::kOn, C::k4),
                       juce::Colour(ColourChart::kBlue));
        skin.setColour(TSS::skinColourKey(O::kSlider, P::kTrack, S::kDefault, C::k1),
                       juce::Colour(ColourChart::kEmerald400));
        skin.setColour(TSS::skinColourKey(O::kSlider, P::kTrack, S::kDefault, C::k2),
                       juce::Colour(ColourChart::kOrange));
        skin.setColour(TSS::skinColourKey(O::kSlider, P::kTrack, S::kDefault, C::k3),
                       juce::Colour(ColourChart::kYellow));
        skin.setColour(TSS::skinColourKey(O::kSliderButtonLike, P::kTrack, S::kDefault, C::k4),
                       juce::Colour(ColourChart::kGrey600));
        skin.setColour(TSS::skinColourKey(O::kSliderButtonLike, P::kTrack, S::kDefault, C::k1),
                       juce::Colour(ColourChart::kEmerald400));
        skin.setColour(TSS::skinColourKey(O::kComboBoxButtonLike, P::kBackground, S::kDefault, C::k4),
                       juce::Colour(ColourChart::kGrey600));
        skin.setColour(TSS::skinColourKey(O::kComboBoxButtonLike, P::kBackground, S::kDefault, C::k1),
                       juce::Colour(ColourChart::kEmerald400));
        skin.setColour(TSS::skinColourKey(O::kNumberBox, P::kBackground, S::kDefault, C::k4),
                       juce::Colour(ColourChart::kGrey600));
        skin.setColour(TSS::skinColourKey(O::kNumberBox, P::kBackground, S::kDefault, C::k1),
                       juce::Colour(ColourChart::kEmerald400));
    }

    void assertLookFromSkinResolvesColourSet(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        paintDistinctPastillesForSets(*skin);

        const auto patchSlider = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k1);
        const auto masterSlider = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k2);
        expect(patchSlider.trackEnabled
               == skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));
        expect(masterSlider.trackEnabled
               == skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k2));
        expect(patchSlider.trackEnabled != masterSlider.trackEnabled);
    }

    void assertHeaderVsSettingsPeakAndLedDistinctSets()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        paintDistinctPastillesForSets(*skin);

        const auto headerPeak = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto settingsPeak = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(headerPeak.maxOn != settingsPeak.maxOn);
        expect(headerPeak.maxOn
               == skin->getColour(SkinObjectType::kPeakIndicator,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kOn,
                                  TSS::ColourSet::k3));
        expect(settingsPeak.maxOn
               == skin->getColour(SkinObjectType::kPeakIndicator,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kOn,
                                  TSS::ColourSet::k4));

        const auto headerLed = TSS::ledLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto settingsLed = TSS::ledLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(headerLed.maxOn != settingsLed.maxOn);
        expect(headerLed.maxOn
               == skin->getColour(SkinObjectType::kLed,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kOn,
                                  TSS::ColourSet::k3));
        expect(settingsLed.maxOn
               == skin->getColour(SkinObjectType::kLed,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kOn,
                                  TSS::ColourSet::k4));
    }

    void assertLedSetSkinPreservesColourSet()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        TSS::Led led(12, 12, TSS::ColourSet::k3);
        led.setSkin(*skinA);
        expect(led.getColourSet() == TSS::ColourSet::k3);
        expect(led.ledLook_.maxOn
               == skinA->getColour(SkinObjectType::kLed,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k3));

        led.setSkin(*skinB);
        expect(led.getColourSet() == TSS::ColourSet::k3);
        expect(led.ledLook_.maxOn
               == skinB->getColour(SkinObjectType::kLed,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k3));
    }

    void assertPeakIndicatorSetSkinPreservesColourSet()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        TSS::PeakIndicator peak(12, 12, TSS::ColourSet::k3);
        peak.setSkin(*skinA);
        expect(peak.getColourSet() == TSS::ColourSet::k3);
        expect(peak.ledLook_.maxOn
               == skinA->getColour(SkinObjectType::kPeakIndicator,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k3));

        peak.setSkin(*skinB);
        expect(peak.getColourSet() == TSS::ColourSet::k3);
        expect(peak.ledLook_.maxOn
               == skinB->getColour(SkinObjectType::kPeakIndicator,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k3));
    }

    void assertParameterCellModuleTypeMapsToColourSet()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        MinimalAudioProcessor processor;
        WidgetFactory factory(processor.apvts);
        const ParameterCellDimensions dimensions {};

        ParameterCell patchCell({
            .skin = *skin,
            .factory = factory,
            .parameterId = {},
            .type = ParameterCell::ParameterType::None,
            .moduleType = ParameterCell::ModuleType::PatchEdit,
            .apvts = processor.apvts,
            .dimensions = dimensions,
        });
        expect(patchCell.getColourSet() == TSS::ColourSet::k1);

        ParameterCell masterCell({
            .skin = *skin,
            .factory = factory,
            .parameterId = {},
            .type = ParameterCell::ParameterType::None,
            .moduleType = ParameterCell::ModuleType::MasterEdit,
            .apvts = processor.apvts,
            .dimensions = dimensions,
        });
        expect(masterCell.getColourSet() == TSS::ColourSet::k2);
    }

    void assertSettingsAndComputerPatchesUseK4()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        paintDistinctPastillesForSets(*skin);

        const auto latency = TSS::sliderLookButtonLikeFromSkin(*skin, TSS::ColourSet::k4);
        expect(latency.trackEnabled
               == skin->getColour(SkinObjectType::kSliderButtonLike,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(latency.trackEnabled
               != skin->getColour(SkinObjectType::kSliderButtonLike,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));

        const auto combo = TSS::comboBoxLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(combo.buttonLikeBackground
               == skin->getColour(SkinObjectType::kComboBoxButtonLike,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(combo.buttonLikeBackground
               != skin->getColour(SkinObjectType::kComboBoxButtonLike,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));

        const auto numberBox = TSS::numberBoxLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(numberBox.background
               == skin->getColour(SkinObjectType::kNumberBox,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k4));
        expect(numberBox.background
               != skin->getColour(SkinObjectType::kNumberBox,
                                  SkinColourProperty::kBackground,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));
    }

    void assertHeaderInputGainSliderUsesK3()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        paintDistinctPastillesForSets(*skin);

        const auto headerGain = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k3);
        expect(headerGain.trackEnabled
               == skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k3));
        expect(headerGain.trackEnabled
               != skin->getColour(SkinObjectType::kSlider,
                                  SkinColourProperty::kTrack,
                                  SkinColourState::kDefault,
                                  TSS::ColourSet::k1));
    }

    void assertBlackCreamIdenticalSeedLooksAcrossSets(TSS::Skin::ColourVariant variant)
    {
        const auto skin = TSS::Skin::create(variant);
        const auto k1 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k1);
        const auto k2 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k2);
        const auto k3 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto k4 = TSS::sliderLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(k1.trackEnabled == k2.trackEnabled
               && k1.trackEnabled == k3.trackEnabled
               && k1.trackEnabled == k4.trackEnabled);
        expect(k1.valueBarEnabled == k4.valueBarEnabled);
        expect(k1.textEnabled == k4.textEnabled);

        const auto peakK3 = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k3);
        const auto peakK4 = TSS::peakIndicatorLookFromSkin(*skin, TSS::ColourSet::k4);
        expect(peakK3.maxOn == peakK4.maxOn
               && peakK3.backgroundOff == peakK4.backgroundOff
               && peakK3.borderOff == peakK4.borderOff);
    }
};

static SkinColourWiringTests skinColourWiringTests;
