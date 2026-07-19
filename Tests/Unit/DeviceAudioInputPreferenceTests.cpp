#include <juce_core/juce_core.h>

#include "Core/Audio/DeviceAudioInputPreference.h"
#include "Core/MIDI/MasterEditGate.h"

class DeviceAudioInputPreferenceTests : public juce::UnitTest
{
public:
    DeviceAudioInputPreferenceTests() : juce::UnitTest("DeviceAudioInputPreference") {}

    void runTest() override
    {
        beginTest("preferred kind is none when undetected");
        expect(Core::preferredAudioFromKind(MatrixDeviceTypes::Type::kMatrix1000, false)
               == Core::PreferredAudioFromKind::kNone);
        expect(Core::preferredAudioFromKind(MatrixDeviceTypes::Type::kMatrix6, false)
               == Core::PreferredAudioFromKind::kNone);

        beginTest("Matrix-1000 prefers mono");
        expect(Core::preferredAudioFromKind(MatrixDeviceTypes::Type::kMatrix1000, true)
               == Core::PreferredAudioFromKind::kMono);

        beginTest("Matrix-6 family prefers stereo");
        expect(Core::preferredAudioFromKind(MatrixDeviceTypes::Type::kMatrix6, true)
               == Core::PreferredAudioFromKind::kStereo);
        expect(Core::preferredAudioFromKind(MatrixDeviceTypes::Type::kMatrix6R, true)
               == Core::PreferredAudioFromKind::kStereo);

        beginTest("unknown detected prefers none");
        expect(Core::preferredAudioFromKind(MatrixDeviceTypes::Type::kUnknown, true)
               == Core::PreferredAudioFromKind::kNone);

        beginTest("pick prefers first matching catalog id");
        const juce::StringArray ids { "mono:0", "mono:1", "stereo:0", "stereo:2" };
        expectEquals(Core::pickPreferredAudioFromSourceId(MatrixDeviceTypes::Type::kMatrix1000, true, ids),
                     juce::String("mono:0"));
        expectEquals(Core::pickPreferredAudioFromSourceId(MatrixDeviceTypes::Type::kMatrix6, true, ids),
                     juce::String("stereo:0"));

        beginTest("pick returns empty when no matching prefix");
        const juce::StringArray monoOnly { "mono:0" };
        expect(Core::pickPreferredAudioFromSourceId(MatrixDeviceTypes::Type::kMatrix6, true, monoOnly)
               .isEmpty());
        expect(Core::pickPreferredAudioFromSourceId(MatrixDeviceTypes::Type::kMatrix1000, false, ids)
               .isEmpty());
    }
};

class MasterEditGateTests : public juce::UnitTest
{
public:
    MasterEditGateTests() : juce::UnitTest("MasterEditGate") {}

    void runTest() override
    {
        beginTest("master edit allowed only for detected Matrix-1000");
        expect(Core::isMasterEditAllowed(true, MatrixDeviceTypes::Type::kMatrix1000));
        expect(! Core::isMasterEditAllowed(false, MatrixDeviceTypes::Type::kMatrix1000));
        expect(! Core::isMasterEditAllowed(true, MatrixDeviceTypes::Type::kMatrix6));
        expect(! Core::isMasterEditAllowed(true, MatrixDeviceTypes::Type::kMatrix6R));
        expect(! Core::isMasterEditAllowed(true, MatrixDeviceTypes::Type::kUnknown));
        expect(! Core::isMasterEditAllowed(false, MatrixDeviceTypes::Type::kMatrix6));
    }
};

static DeviceAudioInputPreferenceTests deviceAudioInputPreferenceTests;
static MasterEditGateTests masterEditGateTests;
