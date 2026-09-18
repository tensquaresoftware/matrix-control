#pragma once

struct DeviceIdInfo;

namespace Core
{
    enum class MatrixDeviceFamily
    {
        kMatrix1000,
        kMatrix6Or6R
    };

    enum class EpromClass
    {
        kStock,
        kOptimised
    };

    // Inter-SysEx delay profiles (D-078, NFR-2).
    //
    // EPROM class: Settings EPROM TYPE is the SSOT (FACTORY/UNKNOWN = stock;
    // GLIGLI/TAUNTEK/UNTERGEEK = optimised). Device Inquiry version is never used
    // for live delay class.
    //
    // Device family: member bytes from Device Inquiry reply (or Settings device type).
    // Unknown members fall back to stock M-1000 delay (Story 2.2).
    //
    // Optimised delay values are placeholders; SM-1 hardware gate may tune.
    class SysExDelayProfile
    {
    public:
        static constexpr int kStockDelayMsMatrix1000 { 10 };
        static constexpr int kStockDelayMsMatrix6 { 20 };
        static constexpr int kOptimisedDelayMsMatrix1000 { 5 };  // SM-1 hardware gate may tune
        static constexpr int kOptimisedDelayMsMatrix6 { 10 };    // SM-1 hardware gate may tune

        static SysExDelayProfile fromSettings(int epromTypeId, MatrixDeviceFamily deviceFamily) noexcept;
        static SysExDelayProfile fromDeviceInquiry(const DeviceIdInfo& info, int epromTypeId);
        static SysExDelayProfile stockDefault() noexcept;

        int getDelayMs() const noexcept;
        EpromClass getEpromClass() const noexcept;
        MatrixDeviceFamily getDeviceFamily() const noexcept;

    private:
        SysExDelayProfile(EpromClass epromClass, MatrixDeviceFamily deviceFamily) noexcept;

        EpromClass epromClass_;
        MatrixDeviceFamily deviceFamily_;
    };
}
