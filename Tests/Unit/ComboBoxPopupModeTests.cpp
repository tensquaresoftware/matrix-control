#include <juce_core/juce_core.h>

#include "GUI/Widgets/ComboBox.h"

class ComboBoxPopupModeTests : public juce::UnitTest
{
public:
    ComboBoxPopupModeTests()
        : juce::UnitTest("ComboBoxPopupMode")
    {
    }

    void runTest() override
    {
        beginTest("closedChromeForPopupMode_scrollableIsThick_multiColumnIsFocusRing");

        expect(TSS::ComboBox::closedChromeForPopupMode(TSS::ComboBox::PopupMode::Scrollable)
               == TSS::ComboBoxClosedChrome::AlwaysThickBorder);
        expect(TSS::ComboBox::closedChromeForPopupMode(TSS::ComboBox::PopupMode::MultiColumn)
               == TSS::ComboBoxClosedChrome::FocusRing);

        beginTest("usesScrollablePopup_matchesPopupMode");

        expect(TSS::ComboBox::usesScrollablePopup(TSS::ComboBox::PopupMode::Scrollable));
        expect(! TSS::ComboBox::usesScrollablePopup(TSS::ComboBox::PopupMode::MultiColumn));
    }
};

static ComboBoxPopupModeTests comboBoxPopupModeTests;
