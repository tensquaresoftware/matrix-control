#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "GUI/Factories/WidgetFactory.h"
#include "GUI/Looks/LookBuilders.h"
#include "GUI/Layout/WidgetDimensions.h"
#include "GUI/Skins/Skin.h"
#include "GUI/Skins/SkinValues.h"
#include "GUI/Widgets/Button.h"
#include "GUI/Widgets/ComboBox.h"
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
        assertLookFromSkinResolvesColourSet(TSS::Skin::ColourVariant::BlackDuo);
        assertLookFromSkinResolvesColourSet(TSS::Skin::ColourVariant::Cream);

        beginTest("headerVsModales_peakAndLedUseZoneColourSets");
        assertHeaderVsSettingsPeakAndLedDistinctSets();

        beginTest("led_setSkinPreservesColourSet");
        assertLedSetSkinPreservesColourSet();

        beginTest("peakIndicator_setSkinPreservesColourSet");
        assertPeakIndicatorSetSkinPreservesColourSet();

        beginTest("parameterCell_moduleTypeMapsToZone");
        assertParameterCellModuleTypeMapsToZone();

        beginTest("parameterCell_sliderLookUsesZoneColourSet");
        assertParameterCellSliderLookUsesZoneColourSet();

        beginTest("parameterCell_comboLookUsesZoneColourSet");
        assertParameterCellComboLookUsesZoneColourSet();

        beginTest("moduleHeader_patchAndMasterButtonsShareButtonUcs");
        assertModuleHeaderButtonsShareButtonUcs();
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

    void expectComboBackground(const TSS::ComboBox& combo,
                               const TSS::ISkin& skin,
                               TSS::ColourSet colourSet)
    {
        expect(combo.look_.backgroundEnabled
               == skin.getColour(SkinObjectType::kComboBox,
                                 SkinColourProperty::kBackground,
                                 SkinColourState::kDefault,
                                 colourSet));
        expect(combo.getPopupMenuLook().border
               == skin.getColour(SkinObjectType::kPopupMenu,
                                 SkinColourProperty::kBorder,
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
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        paintDistinctPastillesForSets(*skin);

        for (const auto object : { SkinObjectType::kLed, SkinObjectType::kPeakIndicator })
        {
            const auto headerSet = TSS::zoneColourSet(object, TSS::SkinZone::kHeader);
            const auto modalesSet = TSS::zoneColourSet(object, TSS::SkinZone::kModales);
            expect(headerSet == TSS::ColourSet::k1);
            expect(modalesSet == TSS::ColourSet::k2);

            const auto build = object == SkinObjectType::kLed ? &TSS::ledLookFromSkin
                                                              : &TSS::peakIndicatorLookFromSkin;
            const auto headerLook = build(*skin, headerSet);
            const auto modalesLook = build(*skin, modalesSet);
            expect(headerLook.maxOn != modalesLook.maxOn);
            expect(headerLook.maxOn
                   == skin->getColour(object, SkinColourProperty::kBackground, SkinColourState::kOn, headerSet));
            expect(modalesLook.maxOn
                   == skin->getColour(object, SkinColourProperty::kBackground, SkinColourState::kOn, modalesSet));
        }
    }

    void assertLedSetSkinPreservesColourSet()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        TSS::Led led(12, 12, TSS::ColourSet::k2);
        led.setSkin(*skinA);
        expect(led.getColourSet() == TSS::ColourSet::k2);
        expect(led.ledLook_.maxOn
               == skinA->getColour(SkinObjectType::kLed,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k2));

        led.setSkin(*skinB);
        expect(led.getColourSet() == TSS::ColourSet::k2);
        expect(led.ledLook_.maxOn
               == skinB->getColour(SkinObjectType::kLed,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k2));
    }

    void assertPeakIndicatorSetSkinPreservesColourSet()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        TSS::PeakIndicator peak(12, 12, TSS::ColourSet::k2);
        peak.setSkin(*skinA);
        expect(peak.getColourSet() == TSS::ColourSet::k2);
        expect(peak.ledLook_.maxOn
               == skinA->getColour(SkinObjectType::kPeakIndicator,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k2));

        peak.setSkin(*skinB);
        expect(peak.getColourSet() == TSS::ColourSet::k2);
        expect(peak.ledLook_.maxOn
               == skinB->getColour(SkinObjectType::kPeakIndicator,
                                   SkinColourProperty::kBackground,
                                   SkinColourState::kOn,
                                   TSS::ColourSet::k2));
    }

    void assertParameterCellModuleTypeMapsToZone()
    {
        const auto skin = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
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
        expect(patchCell.getZone() == TSS::SkinZone::kPatch);

        ParameterCell masterCell({
            .skin = *skin,
            .factory = factory,
            .parameterId = {},
            .type = ParameterCell::ParameterType::None,
            .moduleType = ParameterCell::ModuleType::MasterEdit,
            .apvts = processor.apvts,
            .dimensions = dimensions,
        });
        expect(masterCell.getZone() == TSS::SkinZone::kMaster);
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

    void assertParameterCellSliderLookUsesZoneColourSet()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
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
        expect(patchCell.getZone() == TSS::SkinZone::kPatch);
        expect(patchCell.getSlider() != nullptr);
        expectSliderTrack(*patchCell.getSlider(), *skinA, TSS::ColourSet::k1);

        auto masterCell = makeSliderCell({
            .skin = *skinA,
            .factory = factory,
            .apvts = processor.apvts,
            .moduleType = ParameterCell::ModuleType::MasterEdit,
        });
        expect(masterCell.getZone() == TSS::SkinZone::kMaster);
        expect(masterCell.getSlider() != nullptr);
        expectSliderTrack(*masterCell.getSlider(), *skinA, TSS::ColourSet::k2);
        expect(masterCell.getSlider()->look_.trackEnabled
               != patchCell.getSlider()->look_.trackEnabled);

        masterCell.setSkin(*skinB);
        expect(masterCell.getZone() == TSS::SkinZone::kMaster);
        expectSliderTrack(*masterCell.getSlider(), *skinB, TSS::ColourSet::k2);
    }

    struct ComboCellSpec
    {
        TSS::ISkin& skin;
        WidgetFactory& factory;
        juce::AudioProcessorValueTreeState& apvts;
        ParameterCell::ModuleType moduleType;
    };

    ParameterCell makeComboCell(const ComboCellSpec& spec)
    {
        const ParameterCellDimensions dimensions {};
        return ParameterCell({
            .skin = spec.skin,
            .factory = spec.factory,
            .parameterId = PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kSync,
            .type = ParameterCell::ParameterType::ComboBox,
            .moduleType = spec.moduleType,
            .apvts = spec.apvts,
            .dimensions = dimensions,
        });
    }

    void assertParameterCellComboLookUsesZoneColourSet()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);
        paintDistinctPastillesForSets(*skinA);
        paintDistinctPastillesForSets(*skinB);

        MinimalAudioProcessor processor;
        WidgetFactory factory(processor.apvts);

        auto patchCell = makeComboCell({
            .skin = *skinA,
            .factory = factory,
            .apvts = processor.apvts,
            .moduleType = ParameterCell::ModuleType::PatchEdit,
        });
        expect(patchCell.getZone() == TSS::SkinZone::kPatch);
        expect(patchCell.getComboBox() != nullptr);
        expectComboBackground(*patchCell.getComboBox(), *skinA, TSS::ColourSet::k1);

        auto masterCell = makeComboCell({
            .skin = *skinA,
            .factory = factory,
            .apvts = processor.apvts,
            .moduleType = ParameterCell::ModuleType::MasterEdit,
        });
        expect(masterCell.getZone() == TSS::SkinZone::kMaster);
        expect(masterCell.getComboBox() != nullptr);
        expectComboBackground(*masterCell.getComboBox(), *skinA, TSS::ColourSet::k2);
        expect(masterCell.getComboBox()->look_.backgroundEnabled
               != patchCell.getComboBox()->look_.backgroundEnabled);

        masterCell.setSkin(*skinB);
        expect(masterCell.getZone() == TSS::SkinZone::kMaster);
        expectComboBackground(*masterCell.getComboBox(), *skinB, TSS::ColourSet::k2);
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

    void assertModuleHeaderButtonsShareButtonUcs()
    {
        const auto skinA = TSS::Skin::create(TSS::Skin::ColourVariant::BlackDuo);
        const auto skinB = TSS::Skin::create(TSS::Skin::ColourVariant::Cream);

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
        expect(masterHeader.getInitButton() != nullptr);
        expectButtonBackgroundOff(*masterHeader.getInitButton(), *skinA, TSS::ColourSet::k1);
        expect(masterHeader.getInitButton()->look_.backgroundOff
               == patchHeader.getInitButton()->look_.backgroundOff);

        masterHeader.setSkin(*skinB);
        expectButtonBackgroundOff(*masterHeader.getInitButton(), *skinB, TSS::ColourSet::k1);
    }
};

static SkinColourWiringTests skinColourWiringTests;
