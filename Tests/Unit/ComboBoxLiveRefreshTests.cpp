#include <vector>

#include <juce_core/juce_core.h>

#include "GUI/Helpers/ComboBoxLiveRefresh.h"

class ComboBoxLiveRefreshTests : public juce::UnitTest
{
public:
    ComboBoxLiveRefreshTests() : juce::UnitTest("ComboBoxLiveRefresh") {}

    void runTest() override
    {
        testPlanRefreshMatrix();
        testIdentifiersEqual();
        testRebuildPreservingOpenPopupSequence();
    }

private:
    void testPlanRefreshMatrix()
    {
        beginTest("planRefresh - unchanged skips rebuild whether open or closed");

        using Action = TSS::ComboBoxLiveRefresh::Action;
        expect(TSS::ComboBoxLiveRefresh::planRefresh(false, true) == Action::kSkipRebuild);
        expect(TSS::ComboBoxLiveRefresh::planRefresh(true, true) == Action::kSkipRebuild);

        beginTest("planRefresh - changed closed rebuilds only; open dismisses and reopens");

        expect(TSS::ComboBoxLiveRefresh::planRefresh(false, false) == Action::kRebuildOnly);
        expect(TSS::ComboBoxLiveRefresh::planRefresh(true, false) == Action::kDismissRebuildReopen);
    }

    void testIdentifiersEqual()
    {
        beginTest("identifiersEqual - size and content");

        const std::vector<juce::String> a { "dev-a", "dev-b" };
        const std::vector<juce::String> b { "dev-a", "dev-b" };
        const std::vector<juce::String> c { "dev-a", "dev-c" };
        const std::vector<juce::String> empty;

        expect(TSS::ComboBoxLiveRefresh::identifiersEqual(a, b));
        expect(! TSS::ComboBoxLiveRefresh::identifiersEqual(a, c));
        expect(! TSS::ComboBoxLiveRefresh::identifiersEqual(a, empty));
        expect(TSS::ComboBoxLiveRefresh::identifiersEqual(empty, empty));
    }

    void testRebuildPreservingOpenPopupSequence()
    {
        beginTest("rebuildPreservingOpenPopup - wasOpen true runs dismiss, rebuild, show");

        juce::StringArray steps;
        TSS::ComboBoxLiveRefresh::rebuildPreservingOpenPopup(
            true,
            [&steps]() { steps.add("dismiss"); },
            [&steps]() { steps.add("rebuild"); },
            [&steps]() { steps.add("show"); });

        expectEquals(steps.size(), 3);
        expectEquals(steps[0], juce::String("dismiss"));
        expectEquals(steps[1], juce::String("rebuild"));
        expectEquals(steps[2], juce::String("show"));

        beginTest("rebuildPreservingOpenPopup - wasOpen false runs rebuild only");

        steps.clear();
        TSS::ComboBoxLiveRefresh::rebuildPreservingOpenPopup(
            false,
            [&steps]() { steps.add("dismiss"); },
            [&steps]() { steps.add("rebuild"); },
            [&steps]() { steps.add("show"); });

        expectEquals(steps.size(), 1);
        expectEquals(steps[0], juce::String("rebuild"));
    }
};

static ComboBoxLiveRefreshTests comboBoxLiveRefreshTests;
