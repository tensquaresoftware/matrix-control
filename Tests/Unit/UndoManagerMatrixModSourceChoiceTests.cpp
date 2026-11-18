#include <limits>
#include <memory>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsFactory.h"
#include "Core/PluginProcessorInternal.h"
#include "GUI/Helpers/ApvtsUndoableParameterAttachments.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{

juce::AudioProcessorValueTreeState::ParameterLayout makeBus0SourceLayout()
{
    using namespace PluginDescriptors::MatrixModulationSection;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    const auto& sourceDesc = kModulationBusChoiceParameters[0][0];

    layout.add(std::make_unique<juce::AudioParameterChoice>(
        juce::ParameterID(sourceDesc.parameterId, 1),
        sourceDesc.hostName,
        sourceDesc.choices,
        sourceDesc.defaultIndex));

    return layout;
}

juce::AudioProcessorValueTreeState::ParameterLayout makeBus012SourceLayout()
{
    using namespace PluginDescriptors::MatrixModulationSection;

    juce::AudioProcessorValueTreeState::ParameterLayout layout;

    for (int bus = 0; bus < 3; ++bus)
    {
        const auto& sourceDesc = kModulationBusChoiceParameters[static_cast<size_t>(bus)][0];
        layout.add(std::make_unique<juce::AudioParameterChoice>(
            juce::ParameterID(sourceDesc.parameterId, 1),
            sourceDesc.hostName,
            sourceDesc.choices,
            sourceDesc.defaultIndex));
    }

    return layout;
}

juce::AudioProcessorValueTreeState::ParameterLayout makeBus0AmountLayout()
{
    using namespace PluginDescriptors::MatrixModulationSection;
    const auto& amountDesc = kModulationBusIntParameters[0][0];

    juce::AudioProcessorValueTreeState::ParameterLayout layout;
    layout.add(std::make_unique<juce::AudioParameterInt>(
        juce::ParameterID(amountDesc.parameterId, 1),
        amountDesc.hostName,
        amountDesc.minValue,
        amountDesc.maxValue,
        amountDesc.defaultValue));

    return layout;
}

void populateMatrixModSourceCombo(juce::ComboBox& comboBox)
{
    using namespace PluginDescriptors::MatrixModulationSection;
    const auto& sourceDesc = kModulationBusChoiceParameters[0][0];

    for (const auto& choice : sourceDesc.choices)
        comboBox.addItem(choice, comboBox.getNumItems() + 1);
}

class MatrixModSourceHarness : public juce::AudioProcessor
{
public:
    explicit MatrixModSourceHarness(juce::AudioProcessorValueTreeState::ParameterLayout layout,
                                    int maxStoredUnits = std::numeric_limits<int>::max(),
                                    int minTransactionsToKeep = 1)
        : juce::AudioProcessor(BusesProperties())
        , apvts(*this, &undoManager, "P", std::move(layout))
    {
        undoManager.setMaxNumberOfStoredUnits(maxStoredUnits, minTransactionsToKeep);
    }

    MatrixModSourceHarness()
        : MatrixModSourceHarness(makeBus0SourceLayout())
    {
    }

    juce::UndoManager undoManager;
    juce::AudioProcessorValueTreeState apvts;

    const juce::String sourceId =
        PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets::kBus0Source;

    int readSourceIndex(const juce::String& parameterId) const
    {
        return juce::roundToInt(apvts.getParameterAsValue(parameterId).getValue());
    }

    int readSourceIndex() const
    {
        return readSourceIndex(sourceId);
    }

    void writeSourceIndexViaParameterValue(int index)
    {
        undoManager.beginNewTransaction("source edit");
        apvts.getParameterAsValue(sourceId).setValue(static_cast<float>(index));
    }

    void writeSourceIndexViaNotifyingHost(int index)
    {
        if (auto* param = apvts.getParameter(sourceId))
        {
            undoManager.beginNewTransaction("source edit");
            param->setValueNotifyingHost(param->convertTo0to1(static_cast<float>(index)));
        }
    }

    void flushApvtsToTree()
    {
        apvts.state.getParent(); // keep tree alive
        juce::Thread::sleep(150);
    }

    const juce::String getName() const override { return "MatrixModSourceHarness"; }
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

class UndoManagerMatrixModSourceChoiceTests : public juce::UnitTest
{
public:
    UndoManagerMatrixModSourceChoiceTests()
        : juce::UnitTest("UndoManagerMatrixModSourceChoice")
    {
    }

    void runTest() override
    {
        parameterValuePathRecordsUndoAfterFlush();
        threeSequentialEditsAllowThreeUndos();
        undoableComboBoxAttachmentRecordsUndoAfterSelection();
        undoableComboBoxAttachmentRecordsUndoAfterPopupStyleSelection();
        threeBusSourceAttachmentsAllowThreeUndos();
        fullApvtsLayoutWithProductionUndoLimitsAllowsThreeSequentialUndos();
        threePopupStyleBusSelectionsAllowThreeUndos();
        undoableSliderAttachmentRecordsOneTransactionPerDragSession();
        notifyingHostWithFlushRecordsUndoAfterCompleteGesture();
    }

private:
    void parameterValuePathRecordsUndoAfterFlush()
    {
        beginTest("parameterValuePathRecordsUndoAfterFlush");

        MatrixModSourceHarness harness;
        harness.writeSourceIndexViaParameterValue(1);
        expect(harness.undoManager.canUndo());
        expectEquals(harness.readSourceIndex(), 1);

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(), 0);
    }

    void undoableComboBoxAttachmentRecordsUndoAfterSelection()
    {
        beginTest("undoableComboBoxAttachmentRecordsUndoAfterSelection");

        MatrixModSourceHarness harness;
        using namespace PluginDescriptors::MatrixModulationSection;
        const auto& sourceDesc = kModulationBusChoiceParameters[0][0];

        juce::ComboBox comboBox;

        for (const auto& choice : sourceDesc.choices)
            comboBox.addItem(choice, comboBox.getNumItems() + 1);

        TSS::ApvtsUndoableComboBoxAttachment attachment(harness.apvts, harness.sourceId, comboBox);

        comboBox.setSelectedItemIndex(1, juce::sendNotificationSync);
        expect(harness.undoManager.canUndo());
        expectEquals(harness.readSourceIndex(), 1);

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(), 0);
    }

    void fullApvtsLayoutWithProductionUndoLimitsAllowsThreeSequentialUndos()
    {
        beginTest("fullApvtsLayoutWithProductionUndoLimitsAllowsThreeSequentialUndos");

        MatrixModSourceHarness harness(
            ApvtsFactory::createParameterLayout(),
            PluginProcessorInternal::kEditorialUndoMaxStoredUnits,
            PluginProcessorInternal::kEditorialUndoMaxStoredUnits);
        namespace BusIds = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets;

        const juce::String sourceIds[] = { BusIds::kBus0Source, BusIds::kBus1Source, BusIds::kBus2Source };
        juce::ComboBox comboBoxes[3];
        std::unique_ptr<TSS::ApvtsUndoableComboBoxAttachment> attachments[3];

        for (int bus = 0; bus < 3; ++bus)
        {
            populateMatrixModSourceCombo(comboBoxes[bus]);
            attachments[bus] = std::make_unique<TSS::ApvtsUndoableComboBoxAttachment>(
                harness.apvts,
                sourceIds[bus],
                comboBoxes[bus]);
        }

        for (int bus = 0; bus < 3; ++bus)
        {
            comboBoxes[bus].setSelectedItemIndex(bus + 1, juce::sendNotificationSync);
            expectEquals(harness.readSourceIndex(sourceIds[bus]), bus + 1);
            expect(harness.undoManager.canUndo());
        }

        expectEquals(static_cast<int>(harness.undoManager.getUndoDescriptions().size()), 3);
        expect(harness.undoManager.getNumberOfUnitsTakenUpByStoredCommands()
               > PluginProcessorInternal::kEditorialUndoMaxStoredUnits);

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(sourceIds[2]), 0);
        expect(harness.undoManager.canUndo());
        expect(harness.undoManager.canRedo());

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(sourceIds[1]), 0);
        expect(harness.undoManager.canUndo());

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(sourceIds[0]), 0);
        expect(! harness.undoManager.canUndo());
    }

    void threeBusSourceAttachmentsAllowThreeUndos()
    {
        beginTest("threeBusSourceAttachmentsAllowThreeUndos");

        MatrixModSourceHarness harness(makeBus012SourceLayout());
        namespace BusIds = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets;

        const juce::String sourceIds[] = { BusIds::kBus0Source, BusIds::kBus1Source, BusIds::kBus2Source };
        juce::ComboBox comboBoxes[3];
        std::unique_ptr<TSS::ApvtsUndoableComboBoxAttachment> attachments[3];

        for (int bus = 0; bus < 3; ++bus)
        {
            populateMatrixModSourceCombo(comboBoxes[bus]);
            attachments[bus] = std::make_unique<TSS::ApvtsUndoableComboBoxAttachment>(
                harness.apvts,
                sourceIds[bus],
                comboBoxes[bus]);
        }

        for (int bus = 0; bus < 3; ++bus)
        {
            comboBoxes[bus].setSelectedItemIndex(bus + 1, juce::sendNotificationSync);
            expectEquals(harness.readSourceIndex(sourceIds[bus]), bus + 1);
            expect(harness.undoManager.canUndo());
        }

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(sourceIds[2]), 0);
        expectEquals(harness.readSourceIndex(sourceIds[1]), 2);
        expectEquals(harness.readSourceIndex(sourceIds[0]), 1);

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(sourceIds[1]), 0);
        expectEquals(harness.readSourceIndex(sourceIds[0]), 1);

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(sourceIds[0]), 0);
        expect(! harness.undoManager.canUndo());
    }

    void undoableComboBoxAttachmentRecordsUndoAfterPopupStyleSelection()
    {
        beginTest("undoableComboBoxAttachmentRecordsUndoAfterPopupStyleSelection");

        MatrixModSourceHarness harness;

        juce::ComboBox comboBox;
        populateMatrixModSourceCombo(comboBox);
        TSS::ApvtsUndoableComboBoxAttachment attachment(harness.apvts, harness.sourceId, comboBox);

        const int targetIndex = 3;
        const auto itemId = comboBox.getItemId(targetIndex);
        comboBox.setSelectedId(itemId, juce::sendNotificationSync);

        expect(harness.undoManager.canUndo());
        expectEquals(harness.readSourceIndex(), targetIndex);

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(), 0);
        expect(! harness.undoManager.canUndo());
    }

    void threePopupStyleBusSelectionsAllowThreeUndos()
    {
        beginTest("threePopupStyleBusSelectionsAllowThreeUndos");

        MatrixModSourceHarness harness(makeBus012SourceLayout());
        namespace BusIds = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets;

        const juce::String sourceIds[] = { BusIds::kBus0Source, BusIds::kBus1Source, BusIds::kBus2Source };
        juce::ComboBox comboBoxes[3];
        std::unique_ptr<TSS::ApvtsUndoableComboBoxAttachment> attachments[3];

        for (int bus = 0; bus < 3; ++bus)
        {
            populateMatrixModSourceCombo(comboBoxes[bus]);
            attachments[bus] = std::make_unique<TSS::ApvtsUndoableComboBoxAttachment>(
                harness.apvts,
                sourceIds[bus],
                comboBoxes[bus]);
        }

        for (int bus = 0; bus < 3; ++bus)
        {
            const int targetIndex = bus + 1;
            comboBoxes[bus].setSelectedId(comboBoxes[bus].getItemId(targetIndex), juce::sendNotificationSync);
            expectEquals(harness.readSourceIndex(sourceIds[bus]), targetIndex);
            expect(harness.undoManager.canUndo());
        }

        for (int bus = 2; bus >= 0; --bus)
        {
            expect(harness.undoManager.canUndo());
            harness.undoManager.undo();
            expectEquals(harness.readSourceIndex(sourceIds[bus]), 0);
        }

        expect(! harness.undoManager.canUndo());
    }

    void undoableSliderAttachmentRecordsOneTransactionPerDragSession()
    {
        beginTest("undoableSliderAttachmentRecordsOneTransactionPerDragSession");

        MatrixModSourceHarness harness(makeBus0AmountLayout());
        namespace BusIds = PluginIDs::MatrixModulationSection::ModulationBus::ParameterWidgets;

        juce::Slider slider;
        TSS::ApvtsUndoableSliderAttachment attachment(harness.apvts, BusIds::kBus0Amount, slider);

        for (int value = 1; value <= 3; ++value)
        {
            juce::Slider::ScopedDragNotification dragSession(slider);
            slider.setValue(static_cast<double>(value), juce::sendNotificationSync);
        }

        expectEquals(static_cast<int>(harness.undoManager.getUndoDescriptions().size()), 3);

        for (int expected = 2; expected >= 0; --expected)
        {
            expect(harness.undoManager.canUndo());
            harness.undoManager.undo();
            expectEquals(juce::roundToInt(harness.apvts.getParameterAsValue(BusIds::kBus0Amount).getValue()),
                         expected);
        }

        expect(! harness.undoManager.canUndo());
    }

    void notifyingHostWithFlushRecordsUndoAfterCompleteGesture()
    {
        beginTest("notifyingHostWithFlushRecordsUndoAfterCompleteGesture");

        MatrixModSourceHarness harness;
        harness.undoManager.clearUndoHistory();

        if (auto* param = harness.apvts.getParameter(harness.sourceId))
        {
            harness.undoManager.beginNewTransaction("source edit");
            param->setValueNotifyingHost(param->convertTo0to1(1.0f));
        }

        (void) harness.apvts.copyState();
        expect(harness.undoManager.canUndo());

        harness.undoManager.undo();
        expectEquals(harness.readSourceIndex(), 0);
    }

    void threeSequentialEditsAllowThreeUndos()
    {
        beginTest("threeSequentialEditsAllowThreeUndos");

        MatrixModSourceHarness harness;

        for (int index = 1; index <= 3; ++index)
        {
            harness.writeSourceIndexViaParameterValue(index);
            expect(harness.undoManager.canUndo());
        }

        expectEquals(harness.readSourceIndex(), 3);

        for (int expected = 2; expected >= 0; --expected)
        {
            expect(harness.undoManager.canUndo());
            harness.undoManager.undo();
            expectEquals(harness.readSourceIndex(), expected);
        }

        expect(! harness.undoManager.canUndo());
    }
};

static UndoManagerMatrixModSourceChoiceTests undoManagerMatrixModSourceChoiceTests;

} // namespace
