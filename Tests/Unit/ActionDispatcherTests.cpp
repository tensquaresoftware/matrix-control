#include <juce_core/juce_core.h>

#include "Core/Actions/ActionDispatcher.h"
#include "Core/Actions/ActionPropertyRegistry.h"
#include "Core/Actions/IActionHandler.h"
#include "Shared/Definitions/PluginIDs.h"

namespace PatchEdit = PluginIDs::PatchEditSection;
namespace BankUtility = PluginIDs::PatchManagerSection::BankUtilityModule::StandaloneWidgets;
namespace PatchMutator = PluginIDs::PatchManagerSection::PatchMutatorModule::StandaloneWidgets;

class RecordingActionHandler : public Core::IActionHandler
{
public:
    explicit RecordingActionHandler(juce::String handlerLabel)
        : handlerLabel_(std::move(handlerLabel))
    {
    }

    void handleAction(const juce::String& propertyId, const juce::var& newValue) override
    {
        ++callCount_;
        lastPropertyId_ = propertyId;
        lastValue_ = newValue;
    }

    int callCount() const { return callCount_; }
    const juce::String& lastPropertyId() const { return lastPropertyId_; }
    const juce::var& lastValue() const { return lastValue_; }
    const juce::String& handlerLabel() const { return handlerLabel_; }

private:
    juce::String handlerLabel_;
    int callCount_ = 0;
    juce::String lastPropertyId_;
    juce::var lastValue_;
};

class TestableActionDispatcher
{
public:
    TestableActionDispatcher()
        : dispatcher_(moduleHandler_, patchManagerHandler_, mutatorHandler_)
    {
    }

    Core::ActionDispatcher& dispatcher() { return dispatcher_; }

    RecordingActionHandler& moduleHandler() { return moduleHandler_; }
    RecordingActionHandler& patchManagerHandler() { return patchManagerHandler_; }
    RecordingActionHandler& mutatorHandler() { return mutatorHandler_; }

private:
    RecordingActionHandler moduleHandler_{ "module" };
    RecordingActionHandler patchManagerHandler_{ "patchManager" };
    RecordingActionHandler mutatorHandler_{ "mutator" };
    Core::ActionDispatcher dispatcher_;
};

class ActionDispatcherTests : public juce::UnitTest
{
public:
    ActionDispatcherTests() : juce::UnitTest("ActionDispatcher") {}

    void runTest() override
    {
        testRegistryContainsRepresentativeIds();
        testPasteEnabledIsNotActionProperty();
        testDispatcherRoutesModuleCopy();
        testDispatcherRoutesPatchManagerBankSelect();
        testDispatcherRoutesMutatorMutate();
        testDispatcherIgnoresUnregisteredId();
        testTimestampValuePassesThrough();
    }

private:
    void testRegistryContainsRepresentativeIds()
    {
        beginTest("Registry contains representative action IDs from each handler domain");

        expect(Core::ActionPropertyRegistry::isActionProperty(PatchEdit::Dco1Module::StandaloneWidgets::kCopy));
        expect(Core::ActionPropertyRegistry::isActionProperty(BankUtility::kSelectBank3));
        expect(Core::ActionPropertyRegistry::isActionProperty(PatchMutator::kMutate));
    }

    void testPasteEnabledIsNotActionProperty()
    {
        beginTest("PasteEnabled uiMirror properties are not action properties");

        expect(! Core::ActionPropertyRegistry::isActionProperty(PatchEdit::Dco1Module::StandaloneWidgets::kPasteEnabled));
    }

    void testDispatcherRoutesModuleCopy()
    {
        beginTest("Dispatcher routes module copy actions to ModuleActionHandler");

        TestableActionDispatcher harness;
        const juce::int64 timestamp = 1'704'000'000'000;
        harness.dispatcher().onActionPropertyChanged(
            PatchEdit::Dco1Module::StandaloneWidgets::kCopy,
            timestamp);

        expectEquals(harness.moduleHandler().callCount(), 1);
        expectEquals(harness.patchManagerHandler().callCount(), 0);
        expectEquals(harness.mutatorHandler().callCount(), 0);
        expect(harness.moduleHandler().lastPropertyId() == PatchEdit::Dco1Module::StandaloneWidgets::kCopy);
    }

    void testDispatcherRoutesPatchManagerBankSelect()
    {
        beginTest("Dispatcher routes bank utility actions to PatchManagerActionHandler");

        TestableActionDispatcher harness;
        harness.dispatcher().onActionPropertyChanged(BankUtility::kSelectBank3, juce::int64(42));

        expectEquals(harness.moduleHandler().callCount(), 0);
        expectEquals(harness.patchManagerHandler().callCount(), 1);
        expectEquals(harness.mutatorHandler().callCount(), 0);
        expect(harness.patchManagerHandler().lastPropertyId() == BankUtility::kSelectBank3);
    }

    void testDispatcherRoutesMutatorMutate()
    {
        beginTest("Dispatcher routes mutator actions to MutatorActionHandler");

        TestableActionDispatcher harness;
        harness.dispatcher().onActionPropertyChanged(PatchMutator::kMutate, juce::int64(99));

        expectEquals(harness.moduleHandler().callCount(), 0);
        expectEquals(harness.patchManagerHandler().callCount(), 0);
        expectEquals(harness.mutatorHandler().callCount(), 1);
        expect(harness.mutatorHandler().lastPropertyId() == PatchMutator::kMutate);
    }

    void testDispatcherIgnoresUnregisteredId()
    {
        beginTest("Dispatcher ignores unregistered property IDs");

        TestableActionDispatcher harness;
        harness.dispatcher().onActionPropertyChanged("unknownActionProperty", juce::int64(1));

        expectEquals(harness.moduleHandler().callCount(), 0);
        expectEquals(harness.patchManagerHandler().callCount(), 0);
        expectEquals(harness.mutatorHandler().callCount(), 0);
    }

    void testTimestampValuePassesThrough()
    {
        beginTest("Dispatcher forwards timestamp var unchanged to handler");

        TestableActionDispatcher harness;
        const juce::int64 timestamp = 1'704'111'222'333;
        harness.dispatcher().onActionPropertyChanged(PatchMutator::kRetry, timestamp);

        expect(harness.mutatorHandler().lastValue().isInt64());
        expectEquals(static_cast<juce::int64>(harness.mutatorHandler().lastValue()), timestamp);
    }
};

static ActionDispatcherTests actionDispatcherTests;
