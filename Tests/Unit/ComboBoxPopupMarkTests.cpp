#include <juce_core/juce_core.h>

#include "GUI/Helpers/ComboBoxPopupMark.h"

class ComboBoxPopupMarkTests : public juce::UnitTest
{
public:
    ComboBoxPopupMarkTests() : juce::UnitTest("ComboBoxPopupMark") {}

    void runTest() override
    {
        beginTest("labelForItem - appends suffix only for marked id");

        using TSS::ComboBoxPopupMark::labelForItem;

        expectEquals(labelForItem("TAUNTEK", 3, 3, " *"), juce::String("TAUNTEK *"));
        expectEquals(labelForItem("FACTORY", 1, 3, " *"), juce::String("FACTORY"));
        expectEquals(labelForItem("TAUNTEK", 3, 0, " *"), juce::String("TAUNTEK"));
        expectEquals(labelForItem("TAUNTEK", 3, 3, {}), juce::String("TAUNTEK"));
    }
};

static ComboBoxPopupMarkTests comboBoxPopupMarkTests;
