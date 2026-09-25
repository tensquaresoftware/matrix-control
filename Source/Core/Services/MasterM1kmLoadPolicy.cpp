#include "Core/Services/MasterM1kmLoadPolicy.h"

#include <cstring>

#include "Core/Models/MasterModel.h"

namespace Core
{
    namespace MasterM1kmLoadPolicy
    {
        void apply(juce::uint8* packed172,
                   MasterM1kmGroupsPolicy policy,
                   const juce::uint8* initDefaults) noexcept
        {
            if (packed172 == nullptr || initDefaults == nullptr)
                return;

            if (policy == MasterM1kmGroupsPolicy::kFullMaster)
                return;

            std::memcpy(packed172 + kGroupsOffset,
                        initDefaults + kGroupsOffset,
                        kGroupsCount);
            std::memcpy(packed172 + kCascadeOffset,
                        initDefaults + kCascadeOffset,
                        kCascadeCount);
        }

        void loadPackedIntoModel(MasterModel& model,
                                 const juce::uint8* packed172,
                                 MasterM1kmGroupsPolicy policy,
                                 const juce::uint8* initDefaults) noexcept
        {
            if (packed172 == nullptr || initDefaults == nullptr)
                return;

            juce::uint8 scratch[SysExConstants::kMasterPackedDataSize] = {};
            std::memcpy(scratch, packed172, SysExConstants::kMasterPackedDataSize);
            apply(scratch, policy, initDefaults);
            model.loadFrom(scratch);
        }
    }
} // namespace Core
