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
        mappedColourColumnHitTest();
        mappedColourEditSelectsBoundOnly();
        mappedColourUnboundIsNoOp();
        propertyStateStillArmsOrBinds();
        heldAndArmedWitnessCopy();
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

    void mappedColourColumnHitTest()
    {
        beginTest("Mapped colour column - x at/after mappedX is edit hit");

        using namespace SkinEditorMappingPolicy;
        constexpr int mappedX = 200;
        expect(isMappedColourColumn(mappedX, mappedX));
        expect(isMappedColourColumn(mappedX + 10, mappedX));
        expect(! isMappedColourColumn(mappedX - 1, mappedX));
    }

    void mappedColourEditSelectsBoundOnly()
    {
        beginTest("Mapped colour - bound resolved pastille selects for edit; unresolved is no-op");

        using namespace SkinEditorMappingPolicy;
        expect(mappedColourClick(false, true) == MappedColourIntent::kSelectForEdit);
        expect(mappedColourClick(false, false) == MappedColourIntent::kNoOp);
    }

    void mappedColourUnboundIsNoOp()
    {
        beginTest("Mapped colour - Unbound cell is no-op");

        using namespace SkinEditorMappingPolicy;
        expect(mappedColourClick(true, true) == MappedColourIntent::kNoOp);
        expect(mappedColourClick(true, false) == MappedColourIntent::kNoOp);
    }

    void propertyStateStillArmsOrBinds()
    {
        beginTest("Property/State - Idle arms; HeldColour binds (unchanged policy)");

        using namespace SkinEditorMappingPolicy;
        expect(rowClick(Mode::kIdle, true) == RowIntent::kEnterOrChangeArmedRow);
        expect(rowClick(Mode::kHeldColour, true) == RowIntent::kBindHeldColour);
        expect(rowClick(Mode::kArmedRow, true) == RowIntent::kEnterOrChangeArmedRow);
    }

    void heldAndArmedWitnessCopy()
    {
        beginTest("OPERATION witness - Selected/Editing prefixes, Next steps, Hold colour suffix");

        using namespace SkinEditorMappingPolicy;
        expectEquals(juce::String(kSelectedColourRole), juce::String("Selected colour: "));
        expectEquals(juce::String(kHeldColourNextStep),
                     juce::String("Next: click a Property or State cell in a Mappings row."));
        expectEquals(juce::String(kSelectedTargetRole), juce::String("Selected target: "));
        expectEquals(juce::String(kArmedRowNextStep),
                     juce::String("Next: click a colour in Palette."));
        expectEquals(juce::String(kEditingColourRole), juce::String("Editing colour: "));
        expectEquals(juce::String(kEditingColourNextStep),
                     juce::String("Next: adjust in COLOR (affects every mapped use)."));
        expectEquals(withHoldColourSuffix(kHeldColourNextStep, false, true),
                     juce::String(kHeldColourNextStep));
        expectEquals(withHoldColourSuffix(kHeldColourNextStep, true, true),
                     juce::String(kHeldColourNextStep) + kHoldColourOnRowsSuffix);
        expectEquals(withHoldColourSuffix(kArmedRowNextStep, true, false),
                     juce::String(kArmedRowNextStep) + kHoldColourOnColoursSuffix);
        expect(mappingWitnessVisible(Mode::kHeldColour));
        expect(mappingWitnessVisible(Mode::kArmedRow));
        expect(mappingWitnessVisible(Mode::kEditingColour));
        expect(rowClick(Mode::kEditingColour, true) == RowIntent::kEnterOrChangeArmedRow);
        expect(pastilleClick(Mode::kEditingColour, true, false)
               == PastilleIntent::kEnterOrChangeHeldColour);
        expect(canBindPastille(false));
        expect(! canBindPastille(true));
        expect(pastilleClick(Mode::kArmedRow, true, false, true)
               == PastilleIntent::kEnterOrChangeHeldColour);
        expect(pastilleClick(Mode::kArmedRow, true, false, false)
               == PastilleIntent::kBindArmedRow);
        expect(rowClick(Mode::kHeldColour, true, true) == RowIntent::kIgnore);
        expect(rowClick(Mode::kHeldColour, true, false) == RowIntent::kBindHeldColour);
        expectEquals(juce::String(kCalibrateBeforeMappingMessage),
                     juce::String("Calibrate this colour in COLOR before mapping."));
        expectEquals(juce::String(kHeldPendingNextStep),
                     juce::String("Next: adjust in COLOR before mapping."));
    }
};

static SkinEditorMappingModePolicyTests skinEditorMappingModePolicyTests;
