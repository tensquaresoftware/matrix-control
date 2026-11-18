#include <set>
#include <vector>

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_core/juce_core.h>

#include "Core/Factories/ApvtsFactory.h"
#include "GUI/Factories/WidgetFactory.h"
#include "Shared/Definitions/PluginDescriptors.h"
#include "Shared/Definitions/PluginDisplayNames.h"
#include "Shared/Definitions/PluginIDs.h"
#include "SkinColourWiringTestSupport.h"

using SkinColourWiringTestSupport::MinimalAudioProcessor;

namespace
{
    class LayoutHost : public juce::AudioProcessor
    {
    public:
        explicit LayoutHost(juce::AudioProcessorValueTreeState::ParameterLayout layout)
            : juce::AudioProcessor(BusesProperties())
            , apvts(*this, nullptr, "HOST_NAME_TEST", std::move(layout))
        {
        }

        juce::AudioProcessorValueTreeState apvts;

        const juce::String getName() const override { return "HostParameterNameUniqueness"; }
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

    void collectHostNames(std::vector<juce::String>& out,
                          const std::vector<PluginDescriptors::IntParameterDescriptor>& ints)
    {
        for (const auto& d : ints)
            out.push_back(d.hostName);
    }

    void collectHostNames(std::vector<juce::String>& out,
                          const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& choices)
    {
        for (const auto& d : choices)
            out.push_back(d.hostName);
    }
}

class HostParameterNameUniquenessTests : public juce::UnitTest
{
public:
    HostParameterNameUniquenessTests()
        : juce::UnitTest("Host parameter name uniqueness", "APVTS")
    {
    }

    void runTest() override
    {
        runDescriptorHostNamesUniqueAndNonEmpty();
        runUiShortLabelsUnchangedForFrequencyCollision();
        runMixHostNamePreservesPoleOrder();
        runLayoutExposesHostNamesNotPathPrefixes();
    }

private:
    void runDescriptorHostNamesUniqueAndNonEmpty()
    {
        beginTest("All Int/Choice hostNames are unique and non-empty");

        std::vector<juce::String> hostNames;
        collectHostNames(hostNames, ApvtsFactory::getAllIntParameters());
        collectHostNames(hostNames, ApvtsFactory::getAllChoiceParameters());
        collectHostNames(hostNames,
                         PluginDescriptors::PatchManagerSection::PatchMutatorModule::kIntParameters);

        expectEquals(static_cast<int>(hostNames.size()), 151);

        std::set<juce::String> unique;
        for (const auto& name : hostNames)
        {
            expect(name.isNotEmpty(), "hostName must be non-empty");
            expect(!name.contains("Patch Edit |"), "host leaf must not embed path-style prefix");
            const auto inserted = unique.insert(name).second;
            expect(inserted, "duplicate hostName: " + name);
        }

        expectEquals(static_cast<int>(unique.size()), 151);
    }

    void runUiShortLabelsUnchangedForFrequencyCollision()
    {
        beginTest("UI short FREQUENCY labels stay shared; host leaves are distinct");

        const auto& dco1 = PluginDescriptors::PatchEditSection::Dco1Module::kIntParameters;
        const auto& dco2 = PluginDescriptors::PatchEditSection::Dco2Module::kIntParameters;
        const auto& vcf = PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters;

        const auto* f1 = findInt(dco1, PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency);
        const auto* f2 = findInt(dco2, PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency);
        const auto* fv = findInt(vcf, PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency);

        expect(f1 != nullptr && f2 != nullptr && fv != nullptr);
        if (f1 == nullptr || f2 == nullptr || fv == nullptr)
            return;

        expectEquals(f1->displayName,
                     juce::String(PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency));
        expectEquals(f2->displayName,
                     juce::String(PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency));
        expectEquals(fv->displayName,
                     juce::String(PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency));

        expectEquals(f1->hostName,
                     juce::String(PluginDisplayNames::HostParameterNames::PatchEditSection::Dco1Module::kFrequency));
        expectEquals(f2->hostName,
                     juce::String(PluginDisplayNames::HostParameterNames::PatchEditSection::Dco2Module::kFrequency));
        expectEquals(fv->hostName,
                     juce::String(PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kFrequency));

        expect(f1->hostName != f2->hostName);
        expect(f1->hostName != fv->hostName);
        expect(f2->hostName != fv->hostName);

        expectWidgetFactoryShortFrequencyLabels();
    }

    void expectWidgetFactoryShortFrequencyLabels()
    {
        MinimalAudioProcessor processor;
        WidgetFactory factory(processor.apvts);
        const auto dco1Ui = factory.getParameterDisplayName(
            PluginIDs::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency);
        const auto dco2Ui = factory.getParameterDisplayName(
            PluginIDs::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency);
        const auto vcfUi = factory.getParameterDisplayName(
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency);

        expect(dco1Ui.has_value() && dco2Ui.has_value() && vcfUi.has_value());
        if (!dco1Ui.has_value() || !dco2Ui.has_value() || !vcfUi.has_value())
            return;

        expectEquals(*dco1Ui,
                     juce::String(PluginDisplayNames::PatchEditSection::Dco1Module::ParameterWidgets::kFrequency));
        expectEquals(*dco2Ui,
                     juce::String(PluginDisplayNames::PatchEditSection::Dco2Module::ParameterWidgets::kFrequency));
        expectEquals(*vcfUi,
                     juce::String(PluginDisplayNames::PatchEditSection::VcfVcaModule::ParameterWidgets::kFrequency));
    }

    void runMixHostNamePreservesPoleOrder()
    {
        beginTest("VCF/VCA Mix hostName keeps DCO 2 | DCO 1 polarity text");

        const auto* mix = findInt(
            PluginDescriptors::PatchEditSection::VcfVcaModule::kIntParameters,
            PluginIDs::PatchEditSection::VcfVcaModule::ParameterWidgets::kBalance);

        expect(mix != nullptr);
        if (mix == nullptr)
            return;

        expectEquals(
            mix->hostName,
            juce::String(PluginDisplayNames::HostParameterNames::PatchEditSection::VcfVcaModule::kBalance));
        expect(mix->hostName.contains("DCO 2 | DCO 1 Mix"));
        expect(!mix->hostName.contains("DCO 1 | DCO 2"));
    }

    void runLayoutExposesHostNamesNotPathPrefixes()
    {
        beginTest("APVTS layout parameter names match hostName leaves");

        LayoutHost host(ApvtsFactory::createParameterLayout());

        auto ints = ApvtsFactory::getAllIntParameters();
        auto choices = ApvtsFactory::getAllChoiceParameters();

        std::set<juce::String> layoutNames;
        for (auto* param : host.getParameters())
        {
            const auto name = param->getName(1024);
            expect(name.isNotEmpty());
            expect(!name.startsWithIgnoreCase("Patch Edit |"));
            expect(layoutNames.insert(name).second, "duplicate layout name: " + name);

            const auto* withId = dynamic_cast<const juce::AudioProcessorParameterWithID*>(param);
            expect(withId != nullptr, "layout params should expose ParameterID");
            if (withId == nullptr)
                continue;

            const auto& id = withId->paramID;
            if (const auto* d = findIntById(ints, id))
                expectEquals(name, d->hostName);
            else if (const auto* c = findChoiceById(choices, id))
                expectEquals(name, c->hostName);
            else
                expect(false, "layout ParameterID not in Int/Choice catalogs: " + id);
        }

        expect(layoutNames.size() >= 149);
    }

    static const PluginDescriptors::IntParameterDescriptor* findInt(
        const std::vector<PluginDescriptors::IntParameterDescriptor>& list,
        const char* parameterId)
    {
        for (const auto& d : list)
            if (d.parameterId == parameterId)
                return &d;
        return nullptr;
    }

    static const PluginDescriptors::IntParameterDescriptor* findIntById(
        const std::vector<PluginDescriptors::IntParameterDescriptor>& list,
        const juce::String& parameterId)
    {
        for (const auto& d : list)
            if (d.parameterId == parameterId)
                return &d;
        return nullptr;
    }

    static const PluginDescriptors::ChoiceParameterDescriptor* findChoiceById(
        const std::vector<PluginDescriptors::ChoiceParameterDescriptor>& list,
        const juce::String& parameterId)
    {
        for (const auto& d : list)
            if (d.parameterId == parameterId)
                return &d;
        return nullptr;
    }
};

static HostParameterNameUniquenessTests hostParameterNameUniquenessTests;
