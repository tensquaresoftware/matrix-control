#include "DirtyPatchTracker.h"

#include <cstring>

#include "Core/Models/ApvtsPatchMapper.h"
#include "Core/Models/PatchNameSyncer.h"

namespace Core
{

void DirtyPatchTracker::captureSnapshot(const PatchModel& model)
{
    snapshot_.loadFrom(model.data());
    hasSnapshot_ = true;
}

void DirtyPatchTracker::clearSnapshot() noexcept
{
    hasSnapshot_ = false;
    std::memset(snapshot_.data(), 0, PatchModel::kBufferSize);
}

bool DirtyPatchTracker::hasSnapshot() const noexcept
{
    return hasSnapshot_;
}

bool DirtyPatchTracker::isDirty(const PatchModel& current) const
{
    if (! hasSnapshot_)
        return false;

    return std::memcmp(snapshot_.data(), current.data(), PatchModel::kBufferSize) != 0;
}

bool DirtyPatchTracker::syncApvtsAndIsDirty(ApvtsPatchMapper& mapper,
                                            PatchNameSyncer& nameSyncer,
                                            PatchModel& model)
{
    mapper.apvtsToBuffer();
    nameSyncer.apvtsToBuffer();
    return isDirty(model);
}

} // namespace Core
