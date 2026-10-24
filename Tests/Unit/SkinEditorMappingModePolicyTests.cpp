#include <juce_core/juce_core.h>

#include "Tools/SkinEditor/SkinEditorMappingModePolicy.h"

class SkinEditorMappingModePolicyTests : public juce::UnitTest
{
public:
    SkinEditorMappingModePolicyTests()
        : juce::UnitTest("SkinEditorMappingModePolicy")
    {
    }

    void runTest() override
    {
        colourThenRowsHoldOn();
        rowThenColoursHoldOn();
        returnClickChangesHeldColourAnchor();
        returnClickReArmsRowWithoutBind();
        holdOffOneShotReturnsIdle();
        escapeClearsToIdle();
        idleWitnessHidden();
        readOnlyPastilleDoesNotArm();
    }

private:
    void colourThenRowsHoldOn()
    {
        beginTest("Hold on - colour then rows: enter HeldColour then bind rows");

        using namespace SkinEditorMappingPolicy;
        expect(pastilleClick(Mode::kIdle, true, false)
               == PastilleIntent::kEnterOrChangeHeldColour);
        expect(rowClick(Mode::kHeldColour, true) == RowIntent::kBindHeldColour);
        expect(afterSuccessfulWrite(Mode::kHeldColour, true) == Mode::kHeldColour);
        expect(mappingWitnessVisible(Mode::kHeldColour));
    }

    void rowThenColoursHoldOn()
    {
        beginTest("Hold on - row then colours: enter ArmedRow then bind pastilles");

        using namespace SkinEditorMappingPolicy;
        expect(rowClick(Mode::kIdle, true) == RowIntent::kEnterOrChangeArmedRow);
        expect(pastilleClick(Mode::kArmedRow, true, false) == PastilleIntent::kBindArmedRow);
        expect(afterSuccessfulWrite(Mode::kArmedRow, true) == Mode::kArmedRow);
        expect(mappingWitnessVisible(Mode::kArmedRow));
    }

    void returnClickChangesHeldColourAnchor()
    {
        beginTest("Hold on - pastille while HeldColour changes anchor (no ArmedRow bind)");

        using namespace SkinEditorMappingPolicy;
        expect(pastilleClick(Mode::kHeldColour, true, false)
               == PastilleIntent::kEnterOrChangeHeldColour);
        expect(pastilleClick(Mode::kHeldColour, true, false)
               != PastilleIntent::kBindArmedRow);
    }

    void returnClickReArmsRowWithoutBind()
    {
        beginTest("Hold on - row while ArmedRow re-arms (no HeldColour bind)");

        using namespace SkinEditorMappingPolicy;
        expect(rowClick(Mode::kArmedRow, true) == RowIntent::kEnterOrChangeArmedRow);
        expect(rowClick(Mode::kArmedRow, true) != RowIntent::kBindHeldColour);
    }

    void holdOffOneShotReturnsIdle()
    {
        beginTest("Hold off - successful write returns Idle");

        using namespace SkinEditorMappingPolicy;
        expect(afterSuccessfulWrite(Mode::kHeldColour, false) == Mode::kIdle);
        expect(afterSuccessfulWrite(Mode::kArmedRow, false) == Mode::kIdle);
        expect(! mappingWitnessVisible(Mode::kIdle));
    }

    void escapeClearsToIdle()
    {
        beginTest("Esc - any mode returns Idle and hides witness");

        using namespace SkinEditorMappingPolicy;
        expect(afterEscape() == Mode::kIdle);
        expect(! mappingWitnessVisible(afterEscape()));
    }

    void idleWitnessHidden()
    {
        beginTest("Idle chrome - mapping witness not visible");

        using namespace SkinEditorMappingPolicy;
        expect(! mappingWitnessVisible(Mode::kIdle));
    }

    void readOnlyPastilleDoesNotArm()
    {
        beginTest("Read-only - pastille selects only; row ignored");

        using namespace SkinEditorMappingPolicy;
        expect(pastilleClick(Mode::kIdle, false, false) == PastilleIntent::kSelectReadOnly);
        expect(rowClick(Mode::kIdle, false) == RowIntent::kIgnore);
        expect(pastilleClick(Mode::kIdle, true, true) == PastilleIntent::kIgnoreEmpty);
    }
};

static SkinEditorMappingModePolicyTests skinEditorMappingModePolicyTests;
