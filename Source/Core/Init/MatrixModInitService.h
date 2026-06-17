#pragma once

namespace Core
{
    class ApvtsPatchMapper;
    class MatrixModBusParameterSysExDispatcher;
    class PatchModel;

    /// Resets Matrix Mod buses to hardcoded NONE/0%/NONE defaults (FR-15, D-034).
    /// Message-thread only — no INIT file I/O.
    class MatrixModInitService
    {
    public:
        // Aligned with PluginDescriptorsMatrixModulation defaultValue/defaultIndex fields.
        static constexpr int kDefaultSourceIndex = 0;
        static constexpr int kDefaultAmount = 0;
        static constexpr int kDefaultDestinationIndex = 0;

        MatrixModInitService(PatchModel& patchModel,
                             ApvtsPatchMapper& apvtsPatchMapper,
                             const MatrixModBusParameterSysExDispatcher& sysExDispatcher);

        void initBus(int busIndex);
        void initAllBuses();

    private:
        void resetBusInModel(int busIndex);
        static bool isValidBusIndex(int busIndex) noexcept;

        PatchModel& patchModel_;
        ApvtsPatchMapper& apvtsPatchMapper_;
        const MatrixModBusParameterSysExDispatcher& sysExDispatcher_;
    };

} // namespace Core
