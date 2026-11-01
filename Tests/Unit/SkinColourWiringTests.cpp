#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/Led.h"
#include "GUI/Widgets/ModuleHeader.h"
#include "GUI/Widgets/ParameterCell.h"
#include "GUI/Widgets/PeakIndicator.h"
#include "GUI/Widgets/Slider.h"
#include "Shared/Definitions/PluginIDs.h"
#include "SkinColourWiringTestSupport.h"

using TSS::SkinColourProperty;
using TSS::SkinColourState;
using TSS::SkinObjectType;
using SkinColourWiringTestSupport::MinimalAudioProcessor;
using SkinColourWiringTestSupport::makeModuleHeaderDimensions;
using SkinColourWiringTestSupport::paintDistinctPastillesForSets;

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

        beginTest("parameterCell_sliderLookUsesModuleColourSet");
        assertParameterCellSliderLookUsesModuleColourSet();

        beginTest("moduleHeader_masterEditButtonsUseK2");
        assertModuleHeaderMasterEditButtonsUseK2();
    }

private:
    void expectSliderTrack(const TSS::Slider& slider,
                           const TSS::ISkin& skin,
                           TSS::ColourSet colourSet)
    {
        expect(slider.look_.trackEnabled
               == skin.getColour(SkinObjectType::kSlider,
                                 SkinColourProperty::kTrack,
                                 SkinColourState::kDefault,
                                 colourSet));
    }

    void expectButtonBackgroundOff(const TSS::Button& button,
                                   const TSS::ISkin& skin,
                                   TSS::ColourSet colourSet)
    {
        expect(button.look_.backgroundOff
               == skin.getColour(SkinObjectType::kButton,
                                 SkinColourProperty::kBackground,
                                 SkinColourState::kOff,
                                 colourSet));
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

    struct SliderCellSpec
    {
        TSS::ISkin& skin;
        WidgetFactory& factory;
        juce::AudioProcessorValueTreeState& apvts;
        ParameterCell::ModuleType moduleType;
    };

    ParameterCell makeSliderCell(const SliderCellSpec& spec)
    {
        const ParameterCellDimensions dimensions {};
        return ParameterCell({
            .skin = spec.skin,
            .factory = spec.factory,
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency,
            .type = ParameterCell::ParameterType::Slider,
            .moduleType = spec.moduleType,
            .apvts = spec.apvts,
            .dimensions = dimensions,
        });
    }

    void assertParameterCellSliderLookUsesModuleColourSet()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        MinimalAudioProcessor processor;
        WidgetFactory factory(processor.apvts);

        auto patchCell = makeSliderCell({
            .skin = *skinA,
            .factory = factory,
            .apvts = processor.apvts,
            .moduleType = ParameterCell::ModuleType::PatchEdit,
        });
        expect(patchCell.getColourSet() == TSS::ColourSet::k1);
        expect(patchCell.getSlider() != nullptr);
        expectSliderTrack(*patchCell.getSlider(), *skinA, TSS::ColourSet::k1);

        auto masterCell = makeSliderCell({
            .skin = *skinA,
            .factory = factory,
            .apvts = processor.apvts,
            .moduleType = ParameterCell::ModuleType::MasterEdit,
        });
        expect(masterCell.getColourSet() == TSS::ColourSet::k2);
        expect(masterCell.getSlider() != nullptr);
        expectSliderTrack(*masterCell.getSlider(), *skinA, TSS::ColourSet::k2);
        expect(masterCell.getSlider()->look_.trackEnabled
               != patchCell.getSlider()->look_.trackEnabled);

        masterCell.setSkin(*skinB);
        expect(masterCell.getColourSet() == TSS::ColourSet::k2);
        expectSliderTrack(*masterCell.getSlider(), *skinB, TSS::ColourSet::k2);
    }

    struct InitOnlyHeaderSpec
    {
        TSS::ISkin& skin;
        WidgetFactory& factory;
        juce::AudioProcessorValueTreeState& apvts;
        TSS::ModuleHeader::ColumnLayout layout;
        juce::String moduleId;
        juce::String initWidgetId;
    };

    TSS::ModuleHeader makeInitOnlyHeader(const InitOnlyHeaderSpec& spec)
    {
        const auto dimensions = makeModuleHeaderDimensions();
        return TSS::ModuleHeader({
            .skin = spec.skin,
            .widgetFactory = spec.factory,
            .apvts = spec.apvts,
            .dimensions = dimensions,
            .moduleId = spec.moduleId,
            .columnLayout = spec.layout,
            .buttonSet = TSS::ModuleHeader::ButtonSet::InitOnly,
            .initWidgetId = spec.initWidgetId,
            .copyWidgetId = {},
            .pasteWidgetId = {},
            .pasteEnabledPropertyId = {},
        });
    }

    void assertModuleHeaderMasterEditButtonsUseK2()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::Black);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        MinimalAudioProcessor processor;
        WidgetFactory factory(processor.apvts);

        auto patchHeader = makeInitOnlyHeader({
            .skin = *skinA,
            .factory = factory,
            .apvts = processor.apvts,
            .layout = TSS::ModuleHeader::ColumnLayout::PatchEdit,
            .moduleId = PluginIDs::PatchEditSection::Dco1Module::kGroupId,
            .initWidgetId = PluginIDs::PatchEditSection::Dco1Module::StandaloneWidgets::kInit,
        });
        expect(patchHeader.getButtonColourSet() == TSS::ColourSet::k1);
        expect(patchHeader.getInitButton() != nullptr);
        expectButtonBackgroundOff(*patchHeader.getInitButton(), *skinA, TSS::ColourSet::k1);

        auto masterHeader = makeInitOnlyHeader({
            .skin = *skinA,
            .factory = factory,
            .apvts = processor.apvts,
            .layout = TSS::ModuleHeader::ColumnLayout::MasterEdit,
            .moduleId = PluginIDs::MasterEditSection::MidiModule::kGroupId,
            .initWidgetId = PluginIDs::MasterEditSection::MidiModule::StandaloneWidgets::kInit,
        });
        expect(masterHeader.getButtonColourSet() == TSS::ColourSet::k2);
        expect(masterHeader.getInitButton() != nullptr);
        expectButtonBackgroundOff(*masterHeader.getInitButton(), *skinA, TSS::ColourSet::k2);
        expect(masterHeader.getInitButton()->look_.backgroundOff
               != patchHeader.getInitButton()->look_.backgroundOff);

        masterHeader.setSkin(*skinB);
        expect(masterHeader.getButtonColourSet() == TSS::ColourSet::k2);
        expectButtonBackgroundOff(*masterHeader.getInitButton(), *skinB, TSS::ColourSet::k2);
    }

};

static SkinColourWiringTests skinColourWiringTests;
