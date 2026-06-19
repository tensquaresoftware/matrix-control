#include "Core/Services/PatchMutator/MutationAlgorithm.h"

#include <cmath>
#include <cstring>

#include "Core/Models/ApvtsPatchMapper.h"
#include "Shared/Definitions/PluginIDs.h"

namespace Core
{

namespace
{
    constexpr int kMatrixModOffsetMin = 104;
    constexpr int kMatrixModOffsetMax = 133;
    constexpr size_t kMutableRangeStart = 8;
} // namespace

bool MutationAlgorithm::apply(PatchModel& inOut, const MutationRecipe& recipe, IRandomSource& rng) const
{
    const int amountPercent = clampPercent(recipe.amountPercent);
    const int randomPercent = clampPercent(recipe.randomPercent);

    if (amountPercent == 0 || randomPercent == 0)
        return false;

    PatchModel before;
    before.loadFrom(inOut.data());

    const double amount = static_cast<double>(amountPercent) / 100.0;
    const double random = static_cast<double>(randomPercent) / 100.0;
    const bool matrixModScopeActive = true;

    for (const auto& descriptor : ApvtsPatchMapper::buildIntDescriptors())
    {
        if (! isIntDescriptorEligible(descriptor, recipe, matrixModScopeActive))
            continue;

        const int minValue = descriptor.minValue;
        const int maxValue = descriptor.maxValue;
        const int oldValue = inOut.getValue(descriptor);
        const int range = maxValue - minValue;
        if (range <= 0)
            continue;

        const double halfSpan = (static_cast<double>(range) / 2.0) * amount;
        const int spread = roundHalfUp(halfSpan * random);

        if (spread == 0)
            continue;

        const int delta = uniformRandomInt(rng, -spread, spread);
        const int newValue = juce::jlimit(minValue, maxValue, oldValue + delta);
        inOut.setValue(descriptor, newValue);
    }

    for (const auto& descriptor : ApvtsPatchMapper::buildChoiceDescriptors())
    {
        if (! isChoiceDescriptorEligible(descriptor, recipe, matrixModScopeActive))
            continue;

        const int choiceCount = descriptor.choices.size();
        if (choiceCount <= 1)
            continue;

        const double pMutate = amount * random;
        if (rng.nextFloat() >= static_cast<float>(pMutate))
            continue;

        const int newIndex = uniformRandomInt(rng, 0, choiceCount - 1);
        inOut.setChoiceIndex(descriptor, newIndex);
    }

    for (size_t i = 0; i < PatchModel::kNameLength; ++i)
    {
        if (inOut.data()[i] != before.data()[i])
        {
            inOut.data()[i] = before.data()[i];
            jassertfalse;
        }
    }

    return anyByteChangedInRange(before, inOut, kMutableRangeStart, PatchModel::kBufferSize);
}

int MutationAlgorithm::clampPercent(int value) noexcept
{
    return juce::jlimit(0, 100, value);
}

int MutationAlgorithm::roundHalfUp(double value) noexcept
{
    return static_cast<int>(std::floor(value + 0.5));
}

int MutationAlgorithm::uniformRandomInt(IRandomSource& rng, int lo, int hi)
{
    jassert(lo <= hi);
    return lo + rng.nextInt(hi - lo + 1);
}

bool MutationAlgorithm::isMatrixModOffset(int sysExOffset) noexcept
{
    return sysExOffset >= kMatrixModOffsetMin && sysExOffset <= kMatrixModOffsetMax;
}

bool MutationAlgorithm::isModuleEnabled(const MutationRecipe& recipe, const juce::String& parentGroupId)
{
    using namespace PluginIDs::PatchEditSection;

    if (parentGroupId == Dco1Module::kGroupId)
        return recipe.enableDco1;
    if (parentGroupId == Dco2Module::kGroupId)
        return recipe.enableDco2;
    if (parentGroupId == VcfVcaModule::kGroupId)
        return recipe.enableVcfVca;
    if (parentGroupId == FmTrackModule::kGroupId)
        return recipe.enableFmTrack;
    if (parentGroupId == RampPortamentoModule::kGroupId)
        return recipe.enableRampPortamento;
    if (parentGroupId == Envelope1Module::kGroupId)
        return recipe.enableEnvelope1;
    if (parentGroupId == Envelope2Module::kGroupId)
        return recipe.enableEnvelope2;
    if (parentGroupId == Envelope3Module::kGroupId)
        return recipe.enableEnvelope3;
    if (parentGroupId == Lfo1Module::kGroupId)
        return recipe.enableLfo1;
    if (parentGroupId == Lfo2Module::kGroupId)
        return recipe.enableLfo2;

    return false;
}

bool MutationAlgorithm::isIntDescriptorEligible(const PluginDescriptors::IntParameterDescriptor& descriptor,
                                                const MutationRecipe& recipe,
                                                bool matrixModScopeActive)
{
    if (isMatrixModOffset(descriptor.sysExOffset))
        return matrixModScopeActive;

    return isModuleEnabled(recipe, descriptor.parentGroupId);
}

bool MutationAlgorithm::isChoiceDescriptorEligible(const PluginDescriptors::ChoiceParameterDescriptor& descriptor,
                                                   const MutationRecipe& recipe,
                                                   bool matrixModScopeActive)
{
    if (isMatrixModOffset(descriptor.sysExOffset))
        return matrixModScopeActive;

    return isModuleEnabled(recipe, descriptor.parentGroupId);
}

bool MutationAlgorithm::anyByteChangedInRange(const PatchModel& before,
                                              const PatchModel& after,
                                              size_t startIndex,
                                              size_t endExclusive)
{
    for (size_t i = startIndex; i < endExclusive; ++i)
    {
        if (before.data()[i] != after.data()[i])
            return true;
    }

    return false;
}

} // namespace Core
