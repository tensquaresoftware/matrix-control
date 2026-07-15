#pragma once

#include <juce_core/juce_core.h>

#include "Core/Models/PatchModel.h"
#include "Shared/Definitions/PluginDescriptors.h"

namespace Core
{

    struct MutationRecipe
    {
        int amountPercent = 0;
        int randomPercent = 0;
        bool enableDco1 = false;
        bool enableDco2 = false;
        bool enableVcfVca = false;
        bool enableFmTrack = false;
        bool enableRampPortamento = false;
        bool enableEnvelope1 = false;
        bool enableEnvelope2 = false;
        bool enableEnvelope3 = false;
        bool enableLfo1 = false;
        bool enableLfo2 = false;
        bool enableMatrixMod = false;

        bool hasAnyModuleEnabled() const noexcept
        {
            return enableDco1 || enableDco2 || enableVcfVca || enableFmTrack
                   || enableRampPortamento || enableEnvelope1 || enableEnvelope2
                   || enableEnvelope3 || enableLfo1 || enableLfo2 || enableMatrixMod;
        }
    };

    struct IRandomSource
    {
        virtual ~IRandomSource() = default;
        virtual float nextFloat() = 0;
        virtual int nextInt(int rangeSize) = 0;
    };

    class SeededRandom final : public IRandomSource
    {
    public:
        explicit SeededRandom(juce::uint32 seed) { rng_.setSeed(seed); }

        float nextFloat() override { return rng_.nextFloat(); }
        int nextInt(int rangeSize) override { return rng_.nextInt(rangeSize); }

    private:
        juce::Random rng_;
    };

    class JuceRandomSource final : public IRandomSource
    {
    public:
        explicit JuceRandomSource(juce::Random& rng) : rng_(rng) {}

        float nextFloat() override { return rng_.nextFloat(); }
        int nextInt(int rangeSize) override { return rng_.nextInt(rangeSize); }

    private:
        juce::Random& rng_;
    };

    class MutationAlgorithm
    {
    public:
        bool apply(PatchModel& inOut, const MutationRecipe& recipe, IRandomSource& rng) const;

    private:
        static int clampPercent(int value) noexcept;
        static int roundHalfUp(double value) noexcept;
        static int uniformRandomInt(IRandomSource& rng, int lo, int hi);
        static bool isMatrixModOffset(int sysExOffset) noexcept;
        static bool isModuleEnabled(const MutationRecipe& recipe, const juce::String& parentGroupId);
        static bool isIntDescriptorEligible(const PluginDescriptors::IntParameterDescriptor& descriptor,
                                            const MutationRecipe& recipe,
                                            bool matrixModScopeActive);
        static bool isChoiceDescriptorEligible(const PluginDescriptors::ChoiceParameterDescriptor& descriptor,
                                               const MutationRecipe& recipe,
                                               bool matrixModScopeActive);
        static bool anyByteChangedInRange(const PatchModel& before,
                                          const PatchModel& after,
                                          size_t startIndex,
                                          size_t endExclusive);
    };

} // namespace Core
