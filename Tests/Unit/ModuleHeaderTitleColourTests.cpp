#include <juce_graphics/juce_graphics.h>

#include "GUI/Widgets/ModuleHeaderTitleColour.h"

class ModuleHeaderTitleColourTests : public juce::UnitTest
{
public:
    ModuleHeaderTitleColourTests()
        : juce::UnitTest("ModuleHeaderTitleColour")
    {
    }

    void runTest() override
    {
        beginTest("hover_usesHoverColour_onlyWhenClickEnabled");

        const auto text = juce::Colours::white;
        const auto hover = juce::Colours::red;

        expect(TSS::resolveModuleHeaderTitleTextColour(true, true, text, hover) == hover);
        expect(TSS::resolveModuleHeaderTitleTextColour(true, false, text, hover) == text);
        expect(TSS::resolveModuleHeaderTitleTextColour(false, true, text, hover) == text);
        expect(TSS::resolveModuleHeaderTitleTextColour(false, false, text, hover) == text);

        beginTest("decoLine_matchesTitleHover_onlyWhenClickEnabled");

        const auto lineDefault = juce::Colours::blue;
        expect(TSS::resolveModuleHeaderDecoLineColour(true, true, hover, lineDefault) == hover);
        expect(TSS::resolveModuleHeaderDecoLineColour(true, false, hover, lineDefault) == lineDefault);
        expect(TSS::resolveModuleHeaderDecoLineColour(false, true, hover, lineDefault) == lineDefault);
        expect(TSS::resolveModuleHeaderDecoLineColour(false, false, hover, lineDefault) == lineDefault);
    }
};

static ModuleHeaderTitleColourTests moduleHeaderTitleColourTests;
