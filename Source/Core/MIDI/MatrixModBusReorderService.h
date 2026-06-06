#pragma once

namespace Core
{
    class ApvtsPatchMapper;
    class MatrixModBusParameterSysExDispatcher;
    class PatchModel;

    /// Swaps Matrix Mod bus contents (source / amount / destination) between two row indices.
    /// Bus row labels stay fixed; only packed triplets permute. Message-thread only.
    class MatrixModBusReorderService
    {
    public:
        MatrixModBusReorderService(PatchModel& patchModel,
                                   ApvtsPatchMapper& apvtsPatchMapper,
                                   const MatrixModBusParameterSysExDispatcher& sysExDispatcher);

        void swapBusContents(int busA, int busB);

    private:
        static bool isValidBusIndex(int busIndex) noexcept;

        PatchModel& patchModel_;
        ApvtsPatchMapper& apvtsPatchMapper_;
        const MatrixModBusParameterSysExDispatcher& sysExDispatcher_;
    };

}
