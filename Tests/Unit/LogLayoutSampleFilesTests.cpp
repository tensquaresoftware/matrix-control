#include <cstdlib>

#include <juce_core/juce_core.h>

#include "Core/Loggers/ApvtsLogger.h"
#include "Core/Loggers/LogSessionHeader.h"
#include "Core/Loggers/MidiLogger.h"
#include "Core/MIDI/SysEx/SysExConstants.h"

// Opt-in layout dump for Guillaume: MC_WRITE_SAMPLE_LOGS=1 writes realistic MIDI + APVTS
// sample files to the Desktop so SysEx wrapping and header fields can be reviewed.
class LogLayoutSampleFilesTests : public juce::UnitTest
{
public:
    LogLayoutSampleFilesTests()
        : juce::UnitTest("LogLayoutSampleFiles")
    {
    }

    void runTest() override
    {
        beginTest("Write sample MIDI + APVTS logs to Desktop when MC_WRITE_SAMPLE_LOGS=1");

        if (std::getenv("MC_WRITE_SAMPLE_LOGS") == nullptr)
        {
            expect(true, "Skipped (set MC_WRITE_SAMPLE_LOGS=1 to regenerate Desktop samples)");
            return;
        }

        const auto desktop = juce::File::getSpecialLocation(juce::File::userDesktopDirectory);
        expect(desktop.isDirectory());

        LogSessionHeader::setReportingInstanceId(LogSessionHeader::allocateInstanceId());
        LogSessionHeader::setAudioSnapshot(48000.0, 512);

        // Test harness is not a real plugin wrap - force representative labels for layout review.
        LogSessionHeader::setNextHeaderFormatAndHost("Standalone", "n/a");
        writeMidiSample(desktop.getChildFile("Matrix-Control-sample-midi-log.txt"));

        LogSessionHeader::setNextHeaderFormatAndHost("AU", "Logic Pro");
        writeApvtsSample(desktop.getChildFile("Matrix-Control-sample-apvts-log.txt"));
    }

private:
    static juce::MemoryBlock makeRemoteEditSysEx()
    {
        const juce::uint8 bytes[] = {
            SysExConstants::kSysExStart,
            SysExConstants::kManufacturerIdOberheim,
            SysExConstants::kDeviceIdMatrix1000,
            SysExConstants::Opcode::kRemoteParameterEdit,
            0x00, // parameter number (DCO1 Frequency)
            0x20, // value
            0x5C, // checksum placeholder
            SysExConstants::kSysExEnd
        };
        return juce::MemoryBlock(bytes, sizeof(bytes));
    }

    static juce::MemoryBlock makeMatrixModBusSysEx()
    {
        const juce::uint8 bytes[] = {
            SysExConstants::kSysExStart,
            SysExConstants::kManufacturerIdOberheim,
            SysExConstants::kDeviceIdMatrix1000,
            SysExConstants::Opcode::kRemoteParameterEditMatrix,
            0x00, // bus
            0x01, // source
            0x3F, // amount
            0x02, // destination
            SysExConstants::kSysExEnd
        };
        return juce::MemoryBlock(bytes, sizeof(bytes));
    }

    static juce::MemoryBlock makeLongPatchLikeSysEx()
    {
        juce::MemoryBlock block;
        const juce::uint8 header[] = {
            SysExConstants::kSysExStart,
            SysExConstants::kManufacturerIdOberheim,
            SysExConstants::kDeviceIdMatrix1000,
            SysExConstants::Opcode::kSinglePatchData,
            0x00
        };
        block.append(header, sizeof(header));

        // Enough nibbles to force several wrapped hex lines (layout review).
        for (int i = 0; i < 96; ++i)
        {
            const juce::uint8 nibble = static_cast<juce::uint8>(i & 0x0F);
            block.append(&nibble, 1);
        }

        const juce::uint8 tail[] = { 0x2A, SysExConstants::kSysExEnd };
        block.append(tail, sizeof(tail));
        return block;
    }

    void writeMidiSample(const juce::File& path)
    {
        path.deleteFile();

        auto& midi = MidiLogger::getInstance();
        midi.setLogLevel(MidiLogger::LogLevel::kVerbose);
        midi.setLogToConsole(false);
        midi.setLogToFile(true, path);

        midi.logInfo("Diagnostic logging enabled");
        midi.logInfo("MIDI output port opened: IAC Driver Bus 1");
        midi.logWarning("Port busy while reopening SYNTH TO - retrying once");
        midi.logError("Failed to open MIDI input: Matrix-1000 (device unplugged)");

        midi.logProgramChange(23, "to device");
        midi.logProgramChange(0, "from device");

        const auto remoteEdit = makeRemoteEditSysEx();
        midi.logSysExSent(remoteEdit, "Remote Parameter Edit - DCO1 Frequency");

        const auto matrixMod = makeMatrixModBusSysEx();
        midi.logSysExSent(matrixMod, "Matrix Mod bus 1 edit");

        const auto patchDump = makeLongPatchLikeSysEx();
        midi.logSysExSent(patchDump, "Single Patch Data (layout sample, truncated body)");
        midi.logSysExReceived(patchDump, "Single Patch Data");

        const juce::uint8 decimalPayload[] = { 16, 6, 6, 0, 32, 92 };
        midi.logSysExDataDecimal(decimalPayload, sizeof(decimalPayload), "Remote edit payload without F0/F7");

        midi.logInfo(
            "Very long informational line used to verify word-wrapping in the MIDI logger "
            "when the message exceeds the configured one-hundred-twenty-character line width used by support logs.");

        midi.setLogToFile(false);
        expect(path.existsAsFile());
        expect(path.loadFileAsString().contains("Organization:"));
        expect(path.loadFileAsString().contains("Ten Square Software"));
        expect(path.loadFileAsString().contains("SENT SysEx"));
    }

    static void emitApvtsLifecycleSamples(ApvtsLogger& apvts)
    {
        apvts.logInfo("Diagnostic logging enabled");
        apvts.logWarning("APVTS undo transaction coalesced after rapid Matrix Mod edits");
        apvts.logError("Failed to restore DAW state chunk - using factory defaults");
        apvts.logDebug("ValueTree child added: MATRIX_MOD_BUS");
        apvts.logDebug("ValueTree child removed: MATRIX_MOD_BUS");
        apvts.logDebug("ValueTree child order changed");
        apvts.logDebug("ValueTree parent changed");
        apvts.logAttachmentCreated("dco1Frequency", "Slider");
        apvts.logAttachmentDestroyed("dco1Frequency");
        apvts.logStateReplaced();
        apvts.logStateLoaded("DAW state");
        apvts.logStateLoaded("User preset: Soft Brass Bank A");
    }

    static void emitApvtsParameterSamples(ApvtsLogger& apvts)
    {
        apvts.logParameterChanged("dco1Frequency", 0.35f, 0.62f, "GUI");
        apvts.logParameterChanged(
            "patchEdit.envelope1.attackWithAnIntentionallyLongParameterIdForWrap",
            0.0f,
            0.55f,
            "Automation");
        apvts.logValueTreePropertyChanged({
            juce::Identifier("dco1Frequency"), juce::var(32), juce::var(64), "Message", {}
        });
        apvts.logValueTreePropertyChanged({
            juce::Identifier("lfo1Waveform"), juce::var(0), juce::var(3), "Message", "Triangle"
        });
        apvts.logValueTreePropertyChanged({
            juce::Identifier("mutatorMutate"),
            juce::var(),
            juce::var(static_cast<juce::int64>(1'700'000'000'000LL)),
            "Message",
            {}
        });
    }

    static void emitApvtsDomainSamples(ApvtsLogger& apvts)
    {
        apvts.logValueTreePropertyChanged({
            juce::Identifier("computerPatchesSelectPatch"),
            juce::var(0),
            juce::var(19),
            "Message",
            "Orbital"
        });
        apvts.logValueTreePropertyChanged({
            juce::Identifier("uiMessageText"),
            juce::var(),
            juce::var("MASTER: Could not read Master parameters from the synth. "
                      "Kept current Master Edit settings. Check SYNTH FROM."),
            "Message",
            {}
        });
        apvts.logInfo(
            "Very long informational line used to verify word-wrapping in the APVTS logger "
            "when diagnostic support files need readable multi-line continuation layout "
            "across the one-hundred-twenty-character support log column.");
    }

    void writeApvtsSample(const juce::File& path)
    {
        path.deleteFile();

        auto& apvts = ApvtsLogger::getInstance();
        apvts.setLogLevel(ApvtsLogger::LogLevel::kVerbose);
        apvts.setLogToConsole(false);
        apvts.setLogToFile(true, path);

        emitApvtsLifecycleSamples(apvts);
        emitApvtsParameterSamples(apvts);
        emitApvtsDomainSamples(apvts);

        apvts.setLogToFile(false);
        expect(path.existsAsFile());
        expect(path.loadFileAsString().contains("Matrix-Control"));
        expect(path.loadFileAsString().contains("Parameter changed:"));
    }
};

static LogLayoutSampleFilesTests logLayoutSampleFilesTests;
