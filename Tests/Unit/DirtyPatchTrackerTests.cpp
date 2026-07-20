#include <cstring>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsFactory.h"
#include "Core/Models/ApvtsMasterMapper.h"
#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/MasterModel.h"
#include "Core/Models/PatchModel.h"
#include "Core/Models/PatchNameSyncer.h"
#include "Core/Services/DirtyPatchTracker.h"
#include "Core/Services/UnsavedEditWarningPolicy.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginIDs.h"

namespace
{
    class TestDirtyPatchProcessor : public juce::AudioProcessor
    {
    public:
        TestDirtyPatchProcessor()
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "P", ApvtsFactory::createParameterLayout())
        {
        }

        juce::AudioProcessorValueTreeState apvts;

        const juce::String getName() const override { return "Test"; }
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

    const PluginDescriptors::IntParameterDescriptor& firstPatchIntDescriptor()
    {
        using namespace PluginDescriptors::PatchEditSection;
        jassert(! Dco1Module::kIntParameters.empty());
        return Dco1Module::kIntParameters.front();
    }
}

class DirtyPatchTrackerTests : public juce::UnitTest
{
public:
    DirtyPatchTrackerTests() : juce::UnitTest("DirtyPatchTracker") {}

    void runTest() override
    {
        testNoBaselineIsClean();
        testCaptureThenUnchangedIsClean();
        testCaptureThenParamEditIsDirty();
        testCaptureThenNameEditIsDirty();
        testReCaptureClearsDirty();
        testClearSnapshotResetsBaseline();
        testApvtsSyncPathMatchesPackedCompare();
        testApvtsNameEditIsDirtyViaSyncHelper();
        testMasterOnlyChangeDoesNotDirty();
        testUnsavedWarningPolicy_neverWarnSkipsPrompt();
        testUnsavedWarningPolicy_warnAlwaysWhenDirty();
        testUnsavedWarningPolicy_cleanNeverPrompts();
    }

private:
    void testNoBaselineIsClean()
    {
        beginTest("No baseline — not dirty");

        Core::DirtyPatchTracker tracker;
        Core::PatchModel model;
        model.setName("BASELINE");

        expect(! tracker.hasSnapshot());
        expect(! tracker.isDirty(model));
    }

    void testCaptureThenUnchangedIsClean()
    {
        beginTest("Capture then unchanged — clean");

        Core::DirtyPatchTracker tracker;
        Core::PatchModel model;
        model.setName("CLEAN");
        model.setValue(firstPatchIntDescriptor(), 42);

        tracker.captureSnapshot(model);

        expect(tracker.hasSnapshot());
        expect(! tracker.isDirty(model));
    }

    void testCaptureThenParamEditIsDirty()
    {
        beginTest("Capture then param edit — dirty");

        Core::DirtyPatchTracker tracker;
        Core::PatchModel model;
        model.setName("EDIT");
        model.setValue(firstPatchIntDescriptor(), 10);
        tracker.captureSnapshot(model);

        model.setValue(firstPatchIntDescriptor(), 20);

        expect(tracker.isDirty(model));
    }

    void testCaptureThenNameEditIsDirty()
    {
        beginTest("Capture then name edit — dirty");

        Core::DirtyPatchTracker tracker;
        Core::PatchModel model;
        model.setName("ORIGNAME");
        tracker.captureSnapshot(model);

        model.setName("NEWNAMEX");

        expect(tracker.isDirty(model));
    }

    void testReCaptureClearsDirty()
    {
        beginTest("Re-capture after edit — clean (save-style reset)");

        Core::DirtyPatchTracker tracker;
        Core::PatchModel model;
        model.setName("SAVEAS");
        tracker.captureSnapshot(model);

        model.setName("CHANGED!");
        expect(tracker.isDirty(model));

        tracker.captureSnapshot(model);
        expect(tracker.hasSnapshot());
        expect(! tracker.isDirty(model));
    }

    void testClearSnapshotResetsBaseline()
    {
        beginTest("Clear snapshot — no baseline, not dirty");

        Core::DirtyPatchTracker tracker;
        Core::PatchModel model;
        model.setName("CLEARME");
        tracker.captureSnapshot(model);
        model.setName("DIRTYNOW");
        expect(tracker.isDirty(model));

        tracker.clearSnapshot();

        expect(! tracker.hasSnapshot());
        expect(! tracker.isDirty(model));
    }

    void testApvtsSyncPathMatchesPackedCompare()
    {
        beginTest("APVTS sync helper matches packed compare");

        TestDirtyPatchProcessor processor;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper(processor.apvts, model);
        Core::PatchNameSyncer nameSyncer(processor.apvts, model);
        Core::DirtyPatchTracker tracker;

        model.setName("APVTSTST");
        model.setValue(firstPatchIntDescriptor(), 7);
        mapper.bufferToApvts();
        nameSyncer.bufferToApvts();
        tracker.captureSnapshot(model);

        expect(! tracker.syncApvtsAndIsDirty(mapper, nameSyncer, model));

        auto* param = processor.apvts.getParameter(firstPatchIntDescriptor().parameterId);
        expect(param != nullptr, "Patch int param must exist in ApvtsFactory layout");
        if (param != nullptr)
        {
            const float normalised = param->convertTo0to1(
                static_cast<float>(juce::jlimit(firstPatchIntDescriptor().minValue,
                                                firstPatchIntDescriptor().maxValue,
                                                99)));
            param->setValueNotifyingHost(normalised);
        }

        expect(tracker.syncApvtsAndIsDirty(mapper, nameSyncer, model));
        expect(tracker.isDirty(model));
    }

    void testApvtsNameEditIsDirtyViaSyncHelper()
    {
        beginTest("APVTS name edit via sync helper — dirty");

        TestDirtyPatchProcessor processor;
        Core::PatchModel model;
        Core::ApvtsPatchMapper mapper(processor.apvts, model);
        Core::PatchNameSyncer nameSyncer(processor.apvts, model);
        Core::DirtyPatchTracker tracker;

        model.setName("OLDNAME0");
        mapper.bufferToApvts();
        nameSyncer.bufferToApvts();
        tracker.captureSnapshot(model);

        expect(! tracker.syncApvtsAndIsDirty(mapper, nameSyncer, model));

        processor.apvts.state.setProperty(
            PluginIDs::PatchEditSection::PatchNameModule::kPatchName,
            "NEWNAME1",
            nullptr);

        expect(tracker.syncApvtsAndIsDirty(mapper, nameSyncer, model));
        expect(tracker.isDirty(model));
    }

    void testMasterOnlyChangeDoesNotDirty()
    {
        beginTest("Master-only APVTS change — PATCH stays clean");

        TestDirtyPatchProcessor processor;
        Core::PatchModel patchModel;
        Core::MasterModel masterModel;
        Core::ApvtsPatchMapper patchMapper(processor.apvts, patchModel);
        Core::PatchNameSyncer nameSyncer(processor.apvts, patchModel);
        Core::ApvtsMasterMapper masterMapper(processor.apvts, masterModel);
        Core::DirtyPatchTracker tracker;

        patchModel.setName("MASTEROK");
        patchMapper.bufferToApvts();
        nameSyncer.bufferToApvts();
        masterMapper.bufferToApvts();
        tracker.captureSnapshot(patchModel);

        using namespace PluginDescriptors::MasterEditSection;
        expect(! MiscModule::kIntParameters.empty(),
               "Master Misc int params required for Master-only isolation coverage");
        {
            const auto& masterDesc = MiscModule::kIntParameters.front();
            const int original = masterModel.getValue(masterDesc);
            masterModel.setValue(masterDesc, original == masterDesc.maxValue
                                                 ? masterDesc.minValue
                                                 : original + 1);
            masterMapper.bufferToApvts();
        }

        expect(! tracker.syncApvtsAndIsDirty(patchMapper, nameSyncer, patchModel));
        expect(! tracker.isDirty(patchModel));
    }

    void testUnsavedWarningPolicy_neverWarnSkipsPrompt()
    {
        beginTest("Unsaved warning policy — never warn skips prompt");

        using namespace PluginIDs::Settings::UnsavedEditWarningPolicy;
        expect(! Core::UnsavedEditWarning::shouldPrompt(kNeverWarn, true));
    }

    void testUnsavedWarningPolicy_warnAlwaysWhenDirty()
    {
        beginTest("Unsaved warning policy — warn always when dirty");

        using namespace PluginIDs::Settings::UnsavedEditWarningPolicy;
        expect(Core::UnsavedEditWarning::shouldPrompt(kWarnAlways, true));
        expect(Core::UnsavedEditWarning::shouldPrompt(kDefault, true));
    }

    void testUnsavedWarningPolicy_cleanNeverPrompts()
    {
        beginTest("Unsaved warning policy — clean never prompts");

        using namespace PluginIDs::Settings::UnsavedEditWarningPolicy;
        expect(! Core::UnsavedEditWarning::shouldPrompt(kWarnAlways, false));
        expect(! Core::UnsavedEditWarning::shouldPrompt(kNeverWarn, false));
    }
};

static DirtyPatchTrackerTests dirtyPatchTrackerTests;
