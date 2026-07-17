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
    // EPROM class: optimised firmware when Device Inquiry version string contains
    // (case-insensitive substring) TAUNTEK, GLIGLI, or NORDCORE. Matching is
    // best-effort until SM-1 hardware confirms reply strings.
    //
    // Device family: member bytes from Device Inquiry reply, compared via
    // SysExConstants::DeviceInquiry (M-1000 D-080 + provisional M-6/6R).
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

        static SysExDelayProfile fromDeviceInquiry(const DeviceIdInfo& info);
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
