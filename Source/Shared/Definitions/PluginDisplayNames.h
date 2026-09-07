#pragma once

#include <juce_core/juce_core.h>

// PluginDisplayNames.h
// Central file for all plugin display names (GUI strings)
// Contains ONLY constant string definitions - no code/logic
// Single source of truth for all plugin display names

namespace PluginDisplayNames
{
    constexpr const char* kPluginName = "MATRIX-CONTROL";

    namespace HeaderPanel
    {
        constexpr const char* kEditorMidiFromLabel = "MIDI FROM";
        constexpr const char* kMidiToLabel         = "MIDI TO";
        constexpr const char* kKeyboardFromLabel   = "KEYBOARD FROM";
        constexpr const char* kAudioFromLabel      = "AUDIO FROM";
        constexpr const char* kInputGainLabel      = "INPUT GAIN";
        constexpr const char* kNoInputSentinel     = "NO INPUT";
        constexpr const char* kNoOutputSentinel    = "NO OUTPUT";
        constexpr const char* kHostDisplay         = "HOST";
        constexpr const char* kSettingsButton      = "SETTINGS...";
        constexpr const char* kAudioMidiButton     = "AUDIO/MIDI...";
        constexpr const char* kAboutButton         = "ABOUT...";
        constexpr const char* kLogoSkinSection     = "SKIN";
        constexpr const char* kLogoUiScaleSection  = "UI SCALE";
        constexpr const char* kPanic               = "PANIC";
        constexpr const char* kUndo                = "UNDO";
        constexpr const char* kRedo                = "REDO";
    }

    namespace FooterPanel
    {
        constexpr const char* kDeviceLabel = "DEVICE";
        constexpr const char* kDeviceNotConnectedDetail = "NOT CONNECTED";
        constexpr const char* kDeviceUnknownDetail = "UNKNOWN";

        // Deprecated alias — prefer kDeviceNotConnectedDetail for footer device status.
        constexpr const char* kNoDevice = kDeviceNotConnectedDetail;

        constexpr const char* kSeverityInfoPrefix = "INFO";
        constexpr const char* kSeverityWarningPrefix = "WARNING";
        constexpr const char* kSeverityErrorPrefix = "ERROR";

        constexpr const char* kMidiQueuePressureBadge = "MIDI";
        constexpr const char* kMidiQueuePressureMessage =
            "Send queue backed up — click PANIC to clear stuck notes.";

        // Left-zone guidance while FR-2 device lock is active (D-038 — footer only, no modal).
        constexpr const char* kDeviceLockGuidance =
            "No synth detected - check MIDI cables, MIDI FROM / MIDI TO options, and power-cycle your Matrix synth.";

        // Presence inquiry timed out while still detected — synth likely overloaded, not unplugged.
        constexpr const char* kDeviceUnresponsiveGuidance =
            "Synth not responding (often after heavy SysEx edits) - stop editing, power-cycle the Matrix, then click PANIC if a note is still stuck.";

        // Connected Oberheim Matrix-family device whose member is not Matrix-1000 / Matrix-6/6R.
        constexpr const char* kUnsupportedMatrixDeviceFooter =
            "Connected Matrix device is not supported - editing is locked. Use MIDI FROM / MIDI TO to change ports.";

        // Standalone: MIDI FROM and KEYBOARD FROM must open distinct input devices.
        constexpr const char* kMidiFromKeyboardFromConflictFooter =
            "MIDI FROM and KEYBOARD FROM must use different MIDI input devices.";
    }

    namespace About
    {
        const juce::String kWindowTitle              = "ABOUT";
        constexpr const char* kProductTitle          = "MATRIX-CONTROL";
        constexpr const char* kTagline               = "Modern SysEx MIDI editor for the Oberheim Matrix-1000/6/6R synthesizers.";
        constexpr const char* kOrganizationLabel     = "Organization";
        constexpr const char* kOrganizationValue     = "Ten Square Software";
        constexpr const char* kAuthorLabel           = "Author";
        constexpr const char* kAuthorValue           = "Guillaume DUPONT";
        constexpr const char* kEmailLabel            = "Email";
        constexpr const char* kEmailDisplay          = "tensquaresoftware@gmail.com";
        constexpr const char* kEmailUrl              = "mailto:tensquaresoftware@gmail.com";
        constexpr const char* kGitHubLabel           = "GitHub";
        constexpr const char* kGitHubDisplay         = "github.com/tensquaresoftware/matrix-control";
        constexpr const char* kGitHubUrl             = "https://github.com/tensquaresoftware/matrix-control";
        constexpr const char* kLinkedInLabel         = "LinkedIn";
        constexpr const char* kLinkedInDisplay       = "in/guillaumedupontmontpellier/";
        constexpr const char* kLinkedInUrl           = "https://www.linkedin.com/in/guillaumedupontmontpellier/";
        constexpr const char* kVersionLabel          = "Version";
        constexpr const char* kReleaseDateLabel      = "Release date";
    }

    namespace Settings
    {
        const juce::String kWindowTitle                = "SETTINGS";
        constexpr const char* kSkinLabel               = "SKIN :";
        constexpr const char* kUiScaleLabel            = "UI SCALE :";
        constexpr const char* kHardwareLatencyLabel    = "HARDWARE LATENCY";
        constexpr const char* kAudioFromLabel          = "AUDIO FROM :";
        constexpr const char* kInputGainLabel          = "INPUT GAIN :";
        constexpr const char* kPatchSection            = "PATCH";
        constexpr const char* kPatchMutatorSection     = "PATCH MUTATOR";
        constexpr const char* kMasterSection           = "MASTER";
        constexpr const char* kMatrix1000PatchesLabel  = "MATRIX-1000 PATCHES";
        constexpr const char* kComputerPatchesLabel    = "COMPUTER PATCHES";
        constexpr const char* kUnsavedStateLabel       = "UNSAVED STATE";
        constexpr const char* kDeleteWarningLabel      = "DELETE WARNING";
        constexpr const char* kDefragHistoryLabel      = "DEFRAG HISTORY";
        constexpr const char* kMasterOperationsLabel   = "MASTER OPERATIONS";
        constexpr const char* kComingSoon              = "Coming soon...";
        constexpr const char* kDisplaySysexNames       = "DISPLAY SYSEX NAMES";
        constexpr const char* kDisplayFileNames        = "DISPLAY FILE NAMES";
        constexpr const char* kAskOncePerLoad          = "ASK ONCE PER LOAD";
        constexpr const char* kAlwaysWarn              = "ALWAYS WARN";
        constexpr const char* kNeverWarn               = "NEVER WARN";
        constexpr const char* kDisplayMusicalNames     = "DISPLAY MUSICAL NAMES";
        constexpr const char* kDisplayHardwareNames    = "DISPLAY HARDWARE NAMES";
    }

    namespace Dialogs
    {
        namespace UnsavedEditConfirm
        {
            constexpr const char* kTitle = "Unsaved patch";
            constexpr const char* kBodyStore =
                "This patch is not stored in the synth's current RAM slot yet "
                "(edits and/or an INIT that was never stored).\n\n"
                "Store writes it to the current RAM location. Discard abandons it and continues. "
                "Cancel keeps editing.";
            constexpr const char* kBodySave =
                "This patch has changes that were not saved to its .syx file.\n\n"
                "Save overwrites that file. Discard abandons the changes and continues. "
                "Cancel keeps editing.";
            constexpr const char* kBodySaveAs =
                "This patch has changes that were not saved as a .syx file.\n\n"
                "Save As writes a new file. Discard abandons the changes and continues. "
                "Cancel keeps editing.";
            constexpr const char* kCancel  = "Cancel";
            constexpr const char* kDiscard = "Discard";
            constexpr const char* kStore   = "Store";
            constexpr const char* kSave    = "Save";
            constexpr const char* kSaveAs  = "Save As";
        }

        namespace PatchNameReconciliation
        {
            constexpr const char* kTitle = "Patch name mismatch";
            constexpr const char* kBodyTemplate =
                "Internal name: {INTERNAL}\nFilename: {FILENAME}\n\nWhich name should be used for this load?";
            constexpr const char* kInternal = "Internal";
            constexpr const char* kFilename = "Filename";
            constexpr const char* kCancel   = "Cancel";
        }

        namespace InvalidSaveAsPatchName
        {
            constexpr const char* kTitle = "Invalid patch file name";
            constexpr const char* kBody =
                "Patch file names must be 1 to 8 characters only "
                "(A-Z, 0-9, space, hyphen -, or underscore _).\n\n"
                "Lowercase is OK (saved as uppercase). "
                "Longer names, accents, and other symbols are not allowed.\n\n"
                "Example: RESO is OK; RESONANCE is too long.";
            constexpr const char* kOk = "OK";
        }

        namespace MasterInitConfirm
        {
            constexpr const char* kTitle = "RESET MASTER MODULE?";
            constexpr const char* kBodyTemplate =
                "This will reset all parameters in the {MODULE} module to init defaults and send a full master SysEx to the synth. Other master modules will not be changed.";
            constexpr const char* kConfirm = "Reset";
            constexpr const char* kCancel  = "Cancel";
        }

        namespace MutatorHistoryDefrag
        {
            constexpr const char* kTitle = "Mutation history full";
            constexpr const char* kBody =
                "Defrag will compact mutation history and preserve the current selection. Continue?";
            constexpr const char* kConfirm = "Defrag";
            constexpr const char* kCancel  = "Cancel";
        }

        namespace MutatorFlushConfirm
        {
            constexpr const char* kTitle = "Flush mutation history?";
            constexpr const char* kBody =
                "This clears the Patch Mutator history for this session.\n"
                "The initial patch snapshot is kept.\n\n"
                "Continue to flush, or Cancel to keep the history.";
            constexpr const char* kCancel   = "Cancel";
            constexpr const char* kContinue = "Continue";
        }

        namespace MutatorDeleteConfirm
        {
            constexpr const char* kTitle = "Delete mutation?";
            constexpr const char* kBody =
                "This removes the selected mutation or retry from Patch Mutator history.\n"
                "Deleting a root mutation also removes all of its retries.\n\n"
                "Delete to remove it, or Cancel to keep the history.";
            constexpr const char* kCancel        = "Cancel";
            constexpr const char* kDelete        = "Delete";
            constexpr const char* kDontAskAgain  = "Don't ask again";
        }

        namespace BankImportConfirm
        {
            constexpr const char* kTitle = "Import bank?";
            constexpr const char* kBody =
                "This overwrites patches on the device with files from the selected folder.\n\n"
                "Continue to import, or Cancel to keep the device unchanged.";
            constexpr const char* kCancel   = "Cancel";
            constexpr const char* kContinue = "Continue";
        }

        namespace BankPasteConfirm
        {
            constexpr const char* kTitle = "Paste bank?";
            constexpr const char* kCancel   = "Cancel";
            constexpr const char* kContinue = "Continue";

            inline juce::String formatBody(int sourceBank, int targetBank)
            {
                return "This overwrites all 100 patches in bank " + juce::String(targetBank)
                    + " with the copied bank " + juce::String(sourceBank) + ".\n\n"
                    + "Continue to paste, or Cancel to keep the device unchanged.";
            }
        }

        namespace BankExportOverwriteConfirm
        {
            constexpr const char* kTitle = "Replace export folder?";
            constexpr const char* kBody =
                "An export folder already exists at this location.\n\n"
                "Continue will delete that folder completely, recreate it empty, "
                "then export a fresh copy of the bank.\n\n"
                "Cancel leaves the existing folder untouched.";
            constexpr const char* kCancel   = "Cancel";
            constexpr const char* kContinue = "Continue";
        }

        namespace BankTransferProgress
        {
            constexpr const char* kExportTitle = "EXPORTING BANK TO DISK";
            constexpr const char* kImportTitle = "IMPORTING BANK";
            constexpr const char* kCopyTitle   = "COPYING BANK";
            constexpr const char* kPasteTitle  = "PASTING BANK";
            constexpr const char* kCancel      = "CANCEL";
            constexpr const char* kDestinationFolderLabel = "Destination folder :";
            constexpr const char* kSourceFolderLabel = "Source folder :";
            constexpr const char* kDestinationLabel = "Destination :";
            constexpr const char* kSourceLabel = "Source :";
            constexpr const char* kClipboardLabel = "Clipboard";

            inline juce::String formatExportProgressMessage(int bank)
            {
                return "Exporting bank " + juce::String(bank) + " :";
            }

            inline juce::String formatExportProgressMessageNoBank()
            {
                return "Exporting patches :";
            }

            inline juce::String formatCopyProgressMessage(int bank)
            {
                return "Reading source bank " + juce::String(bank) + " :";
            }

            inline juce::String formatPasteSafetyCopyMessage(int destinationBank)
            {
                return "Saving safety copy of destination bank " + juce::String(destinationBank) + " :";
            }

            inline juce::String formatPasteWritingMessage(int destinationBank)
            {
                return "Writing clipboard into destination bank " + juce::String(destinationBank) + " :";
            }

            inline juce::String formatPasteRestoringMessage(int destinationBank)
            {
                return "Cancelling - restoring destination bank " + juce::String(destinationBank) + " :";
            }
        }
    }

    namespace Units
    {
        constexpr const char* kPercent = "%";
    }

    namespace Widgets
    {
        namespace NumberBox
        {
            // Undefined coordinate: one glyph per digit of the box range ("-" / "--").
            constexpr char kUndefinedValueDigit = '-';
        }
    }

    namespace ShortLabels
    {
        constexpr const char* kInit  = "I"; // Init buttons
        constexpr const char* kCopy  = "C"; // Copy buttons
        constexpr const char* kPaste = "P"; // Paste buttons
        constexpr const char* kIncompatiblePasteFooter =
            "Clipboard contents are not compatible with this module.";
        constexpr const char* kNothingToPasteFooter = "Nothing to paste.";
        constexpr const char* kPasteFailedFooter = "Paste failed.";
    }

    namespace ClipboardMessages
    {
        constexpr const char* kMatrixModulationName = "Matrix Modulation";

        inline juce::String formatModuleCopied(const juce::String& moduleName)
        {
            return moduleName + " copied to clipboard.";
        }

        inline juce::String formatEnvelopeShapeCopied(const juce::String& moduleName)
        {
            return moduleName
                + " envelope shape copied to clipboard (Delay, Attack, Decay, Sustain, Release).";
        }

        inline juce::String formatMatrixModulationCopied()
        {
            return juce::String(kMatrixModulationName) + " copied to clipboard.";
        }

        inline juce::String formatPatchCopied(const juce::String& sourceLabel)
        {
            return sourceLabel + " copied to clipboard.";
        }

        inline juce::String formatModulePasted(const juce::String& sourceName, const juce::String& targetName)
        {
            return sourceName + " pasted successfully to " + targetName + ".";
        }

        inline juce::String formatEnvelopeShapePasted(const juce::String& sourceName,
                                                      const juce::String& targetName)
        {
            return sourceName + " envelope shape pasted successfully to " + targetName
                + " (Delay, Attack, Decay, Sustain, Release).";
        }

        inline juce::String formatMatrixModulationPasted()
        {
            return juce::String(kMatrixModulationName) + " pasted successfully.";
        }

        inline juce::String formatPatchPasted(const juce::String& sourceLabel, const juce::String& targetLabel)
        {
            return sourceLabel + " pasted successfully to " + targetLabel + ".";
        }

        inline juce::String formatInternalPatchLocation(bool hasBank, int bank, int patch)
        {
            if (hasBank)
                return "BANK " + juce::String(bank) + " / PATCH " + juce::String(patch);

            return "PATCH " + juce::String(patch);
        }

        inline juce::String formatPasteFailed(const juce::String& sourceLabel, const juce::String& targetLabel)
        {
            if (sourceLabel.isNotEmpty() && targetLabel.isNotEmpty())
                return "Paste from " + sourceLabel + " to " + targetLabel + " failed.";
            if (sourceLabel.isNotEmpty())
                return "Paste from " + sourceLabel + " failed.";
            return "Paste failed.";
        }

        inline juce::String formatIncompatiblePaste(const juce::String& sourceLabel,
                                                    const juce::String& targetLabel)
        {
            if (sourceLabel.isNotEmpty() && targetLabel.isNotEmpty())
                return "Clipboard contents from " + sourceLabel
                    + " are not compatible with " + targetLabel + ".";
            if (sourceLabel.isNotEmpty())
                return "Clipboard contents from " + sourceLabel
                    + " are not compatible with this module.";
            return "Clipboard contents are not compatible with this module.";
        }
    }

    namespace ChoiceLists
    {
        namespace SkinVariants
        {
            constexpr const char* kBlack = "BLACK";
            constexpr const char* kCream = "CREAM";
        }

        namespace ScaleLevels
        {
            constexpr const char* k50  = "50%";
            constexpr const char* k75  = "75%";
            constexpr const char* k100 = "100%";
            constexpr const char* k125 = "125%";
            constexpr const char* k150 = "150%";
            constexpr const char* k175 = "175%";
            constexpr const char* k200 = "200%";
        }

        namespace MidiChannel
        {
            // Closed/open combo values: omit repeating "CHANNEL" — the parameter label already says it.
            constexpr const char* kOmni      = "OMNI";
            constexpr const char* kChannel1  = "1";
            constexpr const char* kChannel2  = "2";
            constexpr const char* kChannel3  = "3";
            constexpr const char* kChannel4  = "4";
            constexpr const char* kChannel5  = "5";
            constexpr const char* kChannel6  = "6";
            constexpr const char* kChannel7  = "7";
            constexpr const char* kChannel8  = "8";
            constexpr const char* kChannel9  = "9";
            constexpr const char* kChannel10 = "10";
            constexpr const char* kChannel11 = "11";
            constexpr const char* kChannel12 = "12";
            constexpr const char* kChannel13 = "13";
            constexpr const char* kChannel14 = "14";
            constexpr const char* kChannel15 = "15";
            constexpr const char* kChannel16 = "16";
            constexpr const char* kMonoG1    = "MONO G1";
            constexpr const char* kMonoG2    = "MONO G2";
            constexpr const char* kMonoG3    = "MONO G3";
            constexpr const char* kMonoG4    = "MONO G4";
            constexpr const char* kMonoG5    = "MONO G5";
            constexpr const char* kMonoG6    = "MONO G6";
            constexpr const char* kMonoG7    = "MONO G7";
            constexpr const char* kMonoG8    = "MONO G8";
            constexpr const char* kMonoG9    = "MONO G9";
        }
        
        namespace Sync
        {
            constexpr const char* kOff    = "OFF";
            constexpr const char* kSoft   = "SOFT";
            constexpr const char* kMedium = "MEDIUM";
            constexpr const char* kHard   = "HARD";
        }

        namespace WaveSelect
        {
            constexpr const char* kOff   = "OFF";
            constexpr const char* kPulse = "PULSE";
            constexpr const char* kWave  = "WAVE";
            constexpr const char* kBoth  = "BOTH";
            constexpr const char* kNoise = "NOISE";
        }

        namespace Levers
        {
            constexpr const char* kOff           = "OFF";
            constexpr const char* kLever1Bend    = "L1/BEND";
            constexpr const char* kLever2Vibrato = "L2/VIB";
            constexpr const char* kBoth          = "BOTH";
        }

        namespace KeyboardPortamento
        {
            constexpr const char* kOff        = "OFF";
            constexpr const char* kKeyboard   = "KEYBD";
            constexpr const char* kPortamento = "PORTA";
        }

        namespace OnOff
        {
            constexpr const char* kOff = "OFF";
            constexpr const char* kOn  = "ON";
        }

        namespace TriggerMode
        {
            constexpr const char* kStrig  = "STRIG";
            constexpr const char* kMtrig  = "MTRIG";
            constexpr const char* kSreset = "SRESET";
            constexpr const char* kMreset = "MRESET";
            constexpr const char* kXtrig  = "XTRIG";
            constexpr const char* kXmtrig = "XMTRIG";
            constexpr const char* kXreset = "XRESET";
            constexpr const char* kXmrst  = "XMRST";
        }

        namespace EnvelopeMode
        {
            constexpr const char* kNormal = "NORMAL";
            constexpr const char* kDadr   = "DADR";
            constexpr const char* kFree   = "FREE";
            constexpr const char* kBoth   = "BOTH";
        }

        namespace Lfo1Trigger
        {
            constexpr const char* kNormal           = "NORMAL";
            constexpr const char* kLfo1             = "LFO 1";
            constexpr const char* kGLfo1            = "G-LFO 1";
            constexpr const char* kGatedLfo1Trigger = "GATED LFO 1 TRIGGER";
        }

        namespace LfoTriggerMode
        {
            constexpr const char* kOff   = "OFF";
            constexpr const char* kStrig = "STRIG";
            constexpr const char* kMtrig = "MTRIG";
            constexpr const char* kXtrig = "XTRIG";
        }

        namespace LfoWaveform
        {
            constexpr const char* kTriangle = "TRIANGLE";
            constexpr const char* kUpsaw    = "UPSAW";
            constexpr const char* kDnsaw    = "DNSAW";
            constexpr const char* kSquare   = "SQUARE";
            constexpr const char* kRandom   = "RANDOM";
            constexpr const char* kNoise    = "NOISE";
            constexpr const char* kSampled  = "SAMPLED";
        }

        namespace RampTrigger
        {
            constexpr const char* kStrig  = "STRIG";
            constexpr const char* kMtrig  = "MTRIG";
            constexpr const char* kExtrig = "EXTRIG";
            constexpr const char* kGatedx = "GATEDX";
        }

        namespace PortamentoMode
        {
            constexpr const char* kLinear = "LINEAR";
            constexpr const char* kConst  = "CONST";
            constexpr const char* kExpo   = "EXPO";
        }

        namespace PortamentoKeyboardMode
        {
            constexpr const char* kRotate = "ROTATE";
            constexpr const char* kReasgn = "REASGN";
            constexpr const char* kUnison = "UNISON";
            constexpr const char* kRearob = "REAROB";
        }

        namespace ModulationBus
        {
            namespace Source
            {
                constexpr const char* kNone            = "NONE";
                constexpr const char* kEnvelope1       = "ENV 1";
                constexpr const char* kEnvelope2       = "ENV 2";
                constexpr const char* kEnvelope3       = "ENV 3";
                constexpr const char* kLfo1            = "LFO 1";
                constexpr const char* kLfo2            = "LFO 2";
                constexpr const char* kVibrato         = "VIBRATO";
                constexpr const char* kRamp1           = "RAMP 1";
                constexpr const char* kRamp2           = "RAMP 2";
                constexpr const char* kKeyboard        = "KEYBD";
                constexpr const char* kPortamento      = "PORTA";
                constexpr const char* kTrack           = "TRACK";
                constexpr const char* kKeyboardGate    = "KB GATE";
                constexpr const char* kVelocity        = "VELOCITY";
                constexpr const char* kReleaseVelocity = "REL VEL";
                constexpr const char* kPressure        = "PRESSURE";
                constexpr const char* kPedal1          = "PEDAL 1";
                constexpr const char* kPedal2          = "PEDAL 2";
                constexpr const char* kLever1          = "LEVER 1";
                constexpr const char* kLever2          = "LEVER 2";
                constexpr const char* kLever3          = "LEVER 3";
            }

            namespace Destination
            {
                constexpr const char* kNone           = "NONE";
                constexpr const char* kDco1Frequency  = "DCO 1 FREQUENCY";
                constexpr const char* kDco1PulseWidth = "DCO 1 PULSE WIDTH";
                constexpr const char* kDco1WaveShape  = "DCO 1 WAVE SHAPE";
                constexpr const char* kDco2Frequency  = "DCO 2 FREQUENCY";
                constexpr const char* kDco2PulseWidth = "DCO 2 PULSE WIDTH";
                constexpr const char* kDco2WaveShape  = "DCO 2 WAVE SHAPE";
                // Mix polarity (confirmed on hardware): 0 = DCO 2 only, 63 = DCO 1 only.
                constexpr const char* kDco1Dco2Mix    = "DCO 2 | DCO 1 MIX";
                constexpr const char* kVcfFmByDco1    = "VCF FM BY DCO 1";
                constexpr const char* kVcfFrequency   = "VCF FREQUENCY";
                constexpr const char* kVcfResonance   = "VCF RESONANCE";
                constexpr const char* kVca1Volume     = "VCA 1 VOLUME";
                constexpr const char* kVca2Volume     = "VCA 2 VOLUME";
                constexpr const char* kEnv1Delay      = "ENV 1 DELAY";
                constexpr const char* kEnv1Attack     = "ENV 1 ATTACK";
                constexpr const char* kEnv1Decay      = "ENV 1 DECAY";
                constexpr const char* kEnv1Release    = "ENV 1 RELEASE";
                constexpr const char* kEnv1Amplitude  = "ENV 1 AMPLITUDE";
                constexpr const char* kEnv2Delay      = "ENV 2 DELAY";
                constexpr const char* kEnv2Attack     = "ENV 2 ATTACK";
                constexpr const char* kEnv2Decay      = "ENV 2 DECAY";
                constexpr const char* kEnv2Release    = "ENV 2 RELEASE";
                constexpr const char* kEnv2Amplitude  = "ENV 2 AMPLITUDE";
                constexpr const char* kEnv3Delay      = "ENV 3 DELAY";
                constexpr const char* kEnv3Attack     = "ENV 3 ATTACK";
                constexpr const char* kEnv3Decay      = "ENV 3 DECAY";
                constexpr const char* kEnv3Release    = "ENV 3 RELEASE";
                constexpr const char* kEnv3Amplitude  = "ENV 3 AMPLITUDE";
                constexpr const char* kLfo1Speed      = "LFO 1 SPEED";
                constexpr const char* kLfo1Amplitude  = "LFO 1 AMPLITUDE";
                constexpr const char* kLfo2Speed      = "LFO 2 SPEED";
                constexpr const char* kLfo2Amplitude  = "LFO 2 AMPLITUDE";
                constexpr const char* kPortamentoRate = "PORTAMENTO RATE";
            }
        }
    }

    namespace Mode
    {
        constexpr const char* kMaster = "MASTER";
        constexpr const char* kPatch  = "PATCH";
    }

    namespace MasterEditSection
    {
        constexpr const char* kName = "MASTER EDIT";

        namespace MidiModule
        {
            constexpr const char* kName = "MIDI";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kChannel      = "CHANNEL";
                constexpr const char* kMidiEcho     = "MIDI ECHO";
                constexpr const char* kControllers  = "CONTROLLERS";
                constexpr const char* kPatchChanges = "PATCH CHANGES";
                constexpr const char* kPedal1Select = "PEDAL 1 SELECT";
                constexpr const char* kPedal2Select = "PEDAL 2 SELECT";
                constexpr const char* kLever2Select = "LEVER 2 SELECT";
                constexpr const char* kLever3Select = "LEVER 3 SELECT";
            }
        }

        namespace VibratoModule
        {
            constexpr const char* kName = "VIBRATO";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed          = "SPEED";
                constexpr const char* kSpeedModSource = "SPEED MOD SOURCE";
                constexpr const char* kSpeedModAmount = "SPEED MOD AMOUNT";
                constexpr const char* kWaveform       = "WAVEFORM";
                constexpr const char* kAmplitude      = "AMPLITUDE";
                constexpr const char* kAmpModSource   = "AMP MOD SOURCE";
                constexpr const char* kAmpModAmount   = "AMP MOD AMOUNT";
            }
        }

        namespace MiscModule
        {
            constexpr const char* kName = "MISC";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kMasterTune          = "MASTER TUNE";
                constexpr const char* kMasterTranspose     = "MASTER TRANSPOSE";
                constexpr const char* kBendRange           = "BEND RANGE (+/-)";
                constexpr const char* kUnisonEnable        = "UNISON";
                constexpr const char* kVolumeInvertEnable  = "VOLUME INVERT";
                constexpr const char* kBankLockEnable      = "BANK LOCK";
                constexpr const char* kMemoryProtectEnable = "MEMORY PROTECT";
            }
        }
    }

    namespace PatchEditSection
    {
        constexpr const char* kName = "PATCH EDIT";

        namespace Dco1Module
        {
            constexpr const char* kName = "DCO 1";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFrequency           = "FREQUENCY";
                constexpr const char* kFrequencyModByLfo1  = "FREQ < LFO 1";
                constexpr const char* kPulseWidth          = "PULSE WIDTH";
                constexpr const char* kPulseWidthModByLfo2 = "PW < LFO 2";
                constexpr const char* kWaveShape           = "WAVE SHAPE";
                constexpr const char* kSync                = "SYNC";
                constexpr const char* kWaveSelect          = "WAVE SELECT";
                constexpr const char* kLevers              = "LEVERS";
                constexpr const char* kKeyboardPortamento  = "KEYBD/PORTA";
                constexpr const char* kKeyClick            = "KEY CLICK";
            }
        }

        namespace Dco2Module
        {
            constexpr const char* kName = "DCO 2";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFrequency           = "FREQUENCY";
                constexpr const char* kFrequencyModByLfo1  = "FREQ < LFO 1";
                constexpr const char* kDetune              = "DETUNE";
                constexpr const char* kPulseWidth          = "PULSE WIDTH";
                constexpr const char* kPulseWidthModByLfo2 = "PW < LFO 2";
                constexpr const char* kWaveShape           = "WAVE SHAPE";
                constexpr const char* kWaveSelect          = "WAVE SELECT";
                constexpr const char* kLevers              = "LEVERS";
                constexpr const char* kKeyboardPortamento  = "KEYBD/PORTA";
                constexpr const char* kKeyClick            = "KEY CLICK";
            }
        }

        namespace VcfVcaModule
        {
            constexpr const char* kName = "VCF/VCA";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kBalance                = ChoiceLists::ModulationBus::Destination::kDco1Dco2Mix;
                constexpr const char* kFrequency              = "FREQUENCY";
                constexpr const char* kFrequencyModByEnv1     = "FREQ < ENV 1";
                constexpr const char* kFrequencyModByPressure = "FREQ < PRESSURE";
                constexpr const char* kResonance              = "RESONANCE";
                constexpr const char* kVca1Volume             = "VCA 1 VOLUME";
                constexpr const char* kVca1ModByVelocity      = "VCA 1 < VELOCITY";
                constexpr const char* kVca2ModByEnv2          = "VCA 2 < ENV 2";
                constexpr const char* kLevers                 = "LEVERS";
                constexpr const char* kKeyboardPortamento     = "KEYBD/PORTA";
            }
        }

        namespace FmTrackModule
        {
            constexpr const char* kName = "FM/TRACK";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kFmAmount        = "VCF FM AMOUNT";
                constexpr const char* kFmModByEnv3     = "FM < ENV 3";
                constexpr const char* kFmModByPressure = "FM < PRESSURE";
                constexpr const char* kTrackPoint1     = "TRACK POINT 1";
                constexpr const char* kTrackPoint2     = "TRACK POINT 2";
                constexpr const char* kTrackPoint3     = "TRACK POINT 3";
                constexpr const char* kTrackPoint4     = "TRACK POINT 4";
                constexpr const char* kTrackPoint5     = "TRACK POINT 5";
                constexpr const char* kTrackInput      = "TRACK INPUT";
            }
        }

        namespace RampPortamentoModule
        {
            constexpr const char* kName = "RAMP/PORTAMENTO";
            constexpr const char* kLegatoPortaUnisonBlockedFooter =
                "LEGATO PORTA is not available when KEYBOARD MODE is UNISON.";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit = ShortLabels::kInit;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kRamp1Rate               = "RAMP 1 RATE";
                constexpr const char* kRamp2Rate               = "RAMP 2 RATE";
                constexpr const char* kPortamentoRate          = "PORTAMENTO RATE";
                constexpr const char* kPortamentoModByVelocity = "PORTA < VELOCITY";
                constexpr const char* kRamp1Trigger            = "RAMP 1 TRIGGER";
                constexpr const char* kRamp2Trigger            = "RAMP 2 TRIGGER";
                constexpr const char* kPortamentoMode          = "PORTA MODE";
                constexpr const char* kPortamentoLegato        = "LEGATO PORTA";
                constexpr const char* kPortamentoKeyboardMode  = "KEYBOARD MODE";
            }
        }

        namespace Envelope1Module
        {
            constexpr const char* kName = "ENV 1";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "DELAY";
                constexpr const char* kAttack                 = "ATTACK";
                constexpr const char* kDecay                  = "DECAY";
                constexpr const char* kSustain                = "SUSTAIN";
                constexpr const char* kRelease                = "RELEASE";
                constexpr const char* kAmplitude              = "AMPLITUDE";
                constexpr const char* kAmplitudeModByVelocity = "AMP < VELOCITY";
                constexpr const char* kTriggerMode            = "TRIGGER MODE";
                constexpr const char* kEnvelopeMode           = "ENVELOPE MODE";
                constexpr const char* kLfo1Trigger            = "LFO 1 TRIGGER";
            }
        }

        namespace Envelope2Module
        {
            constexpr const char* kName = "ENV 2";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "DELAY";
                constexpr const char* kAttack                 = "ATTACK";
                constexpr const char* kDecay                  = "DECAY";
                constexpr const char* kSustain                = "SUSTAIN";
                constexpr const char* kRelease                = "RELEASE";
                constexpr const char* kAmplitude              = "AMPLITUDE";
                constexpr const char* kAmplitudeModByVelocity = "AMP < VELOCITY";
                constexpr const char* kTriggerMode            = "TRIGGER MODE";
                constexpr const char* kEnvelopeMode           = "ENVELOPE MODE";
                constexpr const char* kLfo1Trigger            = "LFO 1 TRIGGER";
            }
        }

        namespace Envelope3Module
        {
            constexpr const char* kName = "ENV 3";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kDelay                  = "DELAY";
                constexpr const char* kAttack                 = "ATTACK";
                constexpr const char* kDecay                  = "DECAY";
                constexpr const char* kSustain                = "SUSTAIN";
                constexpr const char* kRelease                = "RELEASE";
                constexpr const char* kAmplitude              = "AMPLITUDE";
                constexpr const char* kAmplitudeModByVelocity = "AMP < VELOCITY";
                constexpr const char* kTriggerMode            = "TRIGGER MODE";
                constexpr const char* kEnvelopeMode           = "ENVELOPE MODE";
                constexpr const char* kLfo1Trigger            = "LFO 1 TRIGGER";
            }
        }

        namespace Lfo1Module
        {
            constexpr const char* kName = "LFO 1";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed               = "SPEED";
                constexpr const char* kSpeedModByPressure  = "SPEED < PRESSURE";
                constexpr const char* kRetriggerPoint      = "RETRIGGER POINT";
                constexpr const char* kAmplitude           = "AMPLITUDE";
                constexpr const char* kAmplitudeModByRamp1 = "AMP < RAMP 1";
                constexpr const char* kWaveform            = "WAVEFORM";
                constexpr const char* kTriggerMode         = "TRIGGER MODE";
                constexpr const char* kLag                 = "LAG";
                constexpr const char* kSampleInput         = "SAMPLE INPUT";
            }
        }

        namespace Lfo2Module
        {
            constexpr const char* kName = "LFO 2";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }

            namespace ParameterWidgets
            {
                constexpr const char* kSpeed               = "SPEED";
                constexpr const char* kSpeedModByKeyboard  = "SPEED < KEYBD";
                constexpr const char* kRetriggerPoint      = "RETRIGGER POINT";
                constexpr const char* kAmplitude           = "AMPLITUDE";
                constexpr const char* kAmplitudeModByRamp2 = "AMP < RAMP 2";
                constexpr const char* kWaveform            = "WAVEFORM";
                constexpr const char* kTriggerMode         = "TRIGGER MODE";
                constexpr const char* kLag                 = "LAG";
                constexpr const char* kSampleInput         = "SAMPLE INPUT";
            }
        }

        namespace PatchNameModule
        {
            constexpr const char* kName = "PATCH NAME";

            namespace StandaloneWidgets
            {
                // Patch names are 8 characters long in the Oberheim Matrix-1000 :
                constexpr const char* kDefaultPatchName = "--------";
                // Assigned after full-patch INIT when the template name is blank — distinct from
                // session-idle kDefaultPatchName and from Mutator Compare secondary "INITIAL".
                constexpr const char* kInitPatchName = "INIT";
                // Secondary line literal while Compare is active — distinct from the Mxx / Mxx-Ryy
                // Mutator history labels used the rest of the time.
                constexpr const char* kCompareSecondaryLabel = "INITIAL";
            }

            // Footer copy for the inline rename custom-caret editor. English only.
            namespace Messages
            {
                constexpr const char* kInvalidCharacterFooter = "Invalid character for patch name";
            }
        }
    }

    namespace MatrixModulationSection
    {
        constexpr const char* kName = "MATRIX MODULATION";

        namespace Header
        {
            constexpr const char* kBusNumber   = "#";
            constexpr const char* kSource      = "SOURCE";
            constexpr const char* kAmount      = "AMOUNT";
            constexpr const char* kDestination = "DESTINATION";
            constexpr const char* kIncompatiblePasteFooter =
                "Clipboard contents are not compatible with Matrix Modulation paste.";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }
        }

        namespace ModulationBus
        {
            constexpr const char* kBus0 = "0";
            constexpr const char* kBus1 = "1";
            constexpr const char* kBus2 = "2";
            constexpr const char* kBus3 = "3";
            constexpr const char* kBus4 = "4";
            constexpr const char* kBus5 = "5";
            constexpr const char* kBus6 = "6";
            constexpr const char* kBus7 = "7";
            constexpr const char* kBus8 = "8";
            constexpr const char* kBus9 = "9";

            namespace StandaloneWidgets
            {
                constexpr const char* kInitBus0 = ShortLabels::kInit;
                constexpr const char* kInitBus1 = ShortLabels::kInit;
                constexpr const char* kInitBus2 = ShortLabels::kInit;
                constexpr const char* kInitBus3 = ShortLabels::kInit;
                constexpr const char* kInitBus4 = ShortLabels::kInit;
                constexpr const char* kInitBus5 = ShortLabels::kInit;
                constexpr const char* kInitBus6 = ShortLabels::kInit;
                constexpr const char* kInitBus7 = ShortLabels::kInit;
                constexpr const char* kInitBus8 = ShortLabels::kInit;
                constexpr const char* kInitBus9 = ShortLabels::kInit;
            }
        }
    }

    namespace PatchManagerSection
    {
        constexpr const char* kName = "PATCH MANAGER";

        namespace BankUtilityModule
        {
            constexpr const char* kName = "BANK UTILITY";
            constexpr const char* kMatrix6ExportFolderName = "PATCHES";
            constexpr const char* kImportRomBlockedFooterMessage =
                "Import is only available on Matrix-1000 RAM banks 0 and 1.";
            constexpr const char* kPasteRomBlockedFooterMessage =
                "Paste is only available on Matrix-1000 RAM banks 0 and 1.";
            constexpr const char* kPasteClipboardFailedFooterMessage =
                "Bank paste failed - clipboard contents could not be read.";
            constexpr const char* kBankTransferBusyFooterMessage =
                "Bank transfer in progress - wait for it to finish.";
            constexpr const char* kDeviceUnavailableFooterMessage =
                "Bank transfer requires a connected, supported Matrix device.";
            constexpr const char* kSnapshotFailedFooterMessage =
                "Could not read the current bank from the device - import aborted, nothing was written.";
            constexpr const char* kPasteSnapshotFailedFooterMessage =
                "Could not read the current bank from the device - paste aborted, nothing was written.";
            constexpr const char* kExportCancelledFooterMessage = "Export cancelled - no files kept for this run.";
            constexpr const char* kImportCancelledFooterMessage = "Import cancelled - device restored to its prior state.";
            constexpr const char* kCopyCancelledFooterMessage = "Bank copy cancelled - clipboard unchanged.";
            constexpr const char* kCopyFailedFooterMessage =
                "Bank copy failed - clipboard unchanged. Check the connection and try again.";
            constexpr const char* kPasteCancelledFooterMessage = "Paste cancelled - device restored to its prior state.";
            constexpr const char* kFolderNotWritableFooterMessage = "Could not create or write to the export folder.";
            constexpr const char* kImportRestoreFailedFooterMessage =
                "Import cancelled - the device could not be fully restored. Check the connection and try again.";
            constexpr const char* kPasteRestoreFailedFooterMessage =
                "Paste cancelled - the device could not be fully restored. Check the connection and try again.";
            constexpr const char* kExportingMessage = "Exporting bank to disk...";
            constexpr const char* kImportingReadingMessage = "Reading bank safety copy from device :";
            constexpr const char* kImportingWritingMessage = "Writing patches to the device :";
            constexpr const char* kImportingRestoringMessage = "Cancelling - restoring device :";
            constexpr const char* kPastingWritingMessage = "Writing clipboard into destination bank :";

            namespace StandaloneWidgets
            {
                constexpr const char* kSelectBank0  = "0";
                constexpr const char* kSelectBank1  = "1";
                constexpr const char* kSelectBank2  = "2";
                constexpr const char* kSelectBank3  = "3";
                constexpr const char* kSelectBank4  = "4";
                constexpr const char* kSelectBank5  = "5";
                constexpr const char* kSelectBank6  = "6";
                constexpr const char* kSelectBank7  = "7";
                constexpr const char* kSelectBank8  = "8";
                constexpr const char* kSelectBank9  = "9";
                constexpr const char* kCopyBank     = "COPY";
                constexpr const char* kPasteBank    = "PASTE";
                constexpr const char* kImportBank   = "IMPORT";
                constexpr const char* kExportBank   = "EXPORT";
            }

            namespace FooterMessages
            {
                inline juce::String formatExportSuccess(bool hasBankConcept,
                                                        int bank,
                                                        const juce::String& folderPath)
                {
                    const auto savedClause = "100 patches saved to " + folderPath + ".";

                    if (hasBankConcept)
                        return "Bank " + juce::String(bank) + " exported successfully : " + savedClause;

                    return "Patches exported successfully : " + savedClause;
                }

                inline juce::String formatImportSuccess(int found, int valid, int imported)
                {
                    return "Import complete - found " + juce::String(found)
                        + ", valid " + juce::String(valid)
                        + ", imported " + juce::String(imported);
                }

                inline juce::String formatImportNoValidFiles(int found)
                {
                    return "No valid .syx files to import (found " + juce::String(found) + ")";
                }

                inline juce::String formatCopySuccess(int bank)
                {
                    return "Bank " + juce::String(bank) + " copied to clipboard.";
                }

                inline juce::String formatPasteSuccess(int sourceBank, int targetBank)
                {
                    return "Bank " + juce::String(sourceBank) + " pasted to bank "
                        + juce::String(targetBank) + ".";
                }
            }
        }

        namespace InternalPatchesModule
        {
            constexpr const char* kName = "INTERNAL PATCHES";

            namespace StandaloneWidgets
            {
                constexpr const char* kBrowser            = "BROWSER";
                constexpr const char* kMemory             = "MEMORY";
                constexpr const char* kLoadPreviousPatch  = "<";
                constexpr const char* kLoadNextPatch      = ">";
                constexpr const char* kCurrentBankNumber  = "CURRENT BANK";
                constexpr const char* kCurrentPatchNumber = "CURRENT PATCH";
                constexpr const char* kInitPatch          = "INIT";
                constexpr const char* kCopyPatch          = "COPY";
                constexpr const char* kPastePatch         = "PASTE";
                constexpr const char* kStorePatch         = "STORE";
            }
        }

        namespace ComputerPatchesModule
        {
            constexpr const char* kName = "COMPUTER PATCHES";
            constexpr const char* kEmptySentinel = "<EMPTY!>";
            constexpr const char* kSelectSentinel = "<SELECT>";

            namespace FooterMessages
            {
                inline juce::String formatScanSummary(int validCount, int invalidCount)
                {
                    return juce::String(validCount) + " valid, " + juce::String(invalidCount) + " invalid";
                }

                inline juce::String formatSaveSuccess(const juce::String& fileName)
                {
                    return "Saved " + fileName;
                }

                inline juce::String formatLoadSuccess(const juce::String& fileName)
                {
                    return "Loaded " + fileName;
                }

                inline juce::String formatReconciliationNotice(const juce::String& resolvedName,
                                                               bool usedFilename)
                {
                    if (usedFilename)
                        return "Loaded " + resolvedName + " (filename used)";

                    return "Loaded " + resolvedName + " (internal name used)";
                }

                constexpr const char* kEmptyFolder = "0 files in folder";
                constexpr const char* kFolderNotFound = "Folder not found";
                constexpr const char* kLoadSelectionStale = "Selection out of date";
                constexpr const char* kPatchFileNotFound = "Patch file not found";
                constexpr const char* kInvalidSaveStem =
                    "Invalid patch file name (use A-Z, 0-9, space, -, _; max 8)";
            }

            namespace StandaloneWidgets
            {
                constexpr const char* kBrowser               = "BROWSER";
                constexpr const char* kStorage               = "STORAGE";
                constexpr const char* kLoadPreviousPatchFile = "<";
                constexpr const char* kLoadNextPatchFile     = ">";
                constexpr const char* kSelectPatchFile       = "SELECT A PATCH";
                constexpr const char* kOpenPatchFolder       = "OPEN";
                constexpr const char* kSavePatchAsFile       = "SAVE AS";
                constexpr const char* kSavePatchFile         = "SAVE";
            }
        }

        namespace PatchMutatorModule
        {
            constexpr const char* kName = "PATCH MUTATOR";

            namespace StandaloneWidgets
            {
                constexpr const char* kAmount         = "AMOUNT";
                constexpr const char* kRandom         = "RANDOM";
                constexpr const char* kMode           = "MODE";
                constexpr const char* kPitch          = "PITCH";
                constexpr const char* kHistory        = "HISTORY";
                constexpr const char* kEmptyHistorySentinel = "<EMPTY>";
                const juce::String    kHistoryRootSentinel =
                    juce::String::fromUTF8("\xe2\x80\x94"); // em dash U+2014 — root-only History R row

                constexpr const char* kHistoryPrevious = "<";
                constexpr const char* kHistoryNext     = ">";
                constexpr const char* kMutate         = "MUTATE";
                constexpr const char* kRetry          = "RETRY";
                constexpr const char* kCompare        = "C";
                constexpr const char* kDelete         = "D";
                constexpr const char* kClear          = "F"; // face = Flush; wire id remains Clear
                constexpr const char* kExport         = "E";

                constexpr const char* kEnableDco1           = "D1";
                constexpr const char* kEnableDco2           = "D2";
                constexpr const char* kEnableVcfVca         = "F/A";
                constexpr const char* kEnableFmTrack        = "F/T";
                constexpr const char* kEnableRampPortamento = "R/P";
                constexpr const char* kEnableEnvelope1      = "E1";
                constexpr const char* kEnableEnvelope2      = "E2";
                constexpr const char* kEnableEnvelope3      = "E3";
                constexpr const char* kEnableLfo1           = "L1";
                constexpr const char* kEnableLfo2           = "L2";
                constexpr const char* kEnableMatrixMod      = "MM";
            }

            // MODE / PITCH combo items — ALL CAPS, ordered like Core::MutationMode and
            // Core::MutationPitchMode so the stored index and the menu row always agree.
            namespace ChoiceLists
            {
                namespace MutationMode
                {
                    constexpr const char* kKindred = "KINDRED";
                    constexpr const char* kDrift   = "DRIFT";
                    constexpr const char* kWarp    = "WARP";
                    constexpr const char* kWild    = "WILD";

                    constexpr const char* kAll[] = { kKindred, kDrift, kWarp, kWild };

                    static_assert(sizeof(kAll) / sizeof(kAll[0]) == 4,
                                  "MODE ChoiceList length must stay aligned with Core::MutationMode");
                }

                namespace MutationPitch
                {
                    // Short face label: PRESERVE truncates in the 56 px slot; KEEP fits.
                    constexpr const char* kPreserve  = "KEEP";
                    constexpr const char* kConsonant = "CONSONANT";
                    constexpr const char* kDissonant = "DISSONANT";
                    constexpr const char* kFree      = "FREE";

                    // Closed-face abbreviations for the 56 px PITCH control (menu stays full).
                    constexpr const char* kConsonantClosed = "CONS";
                    constexpr const char* kDissonantClosed = "DIS";

                    constexpr const char* kAll[] = { kPreserve, kConsonant, kDissonant, kFree };

                    static_assert(sizeof(kAll) / sizeof(kAll[0]) == 4,
                                  "PITCH ChoiceList length must stay aligned with Core::MutationPitchMode");

                    // Octave window rows under CONSONANT / DISSONANT, e.g. "± 3 OCT".
                    // Clamp matches MutationCalibration::kMin/MaxPitchOctaves (1..5).
                    inline juce::String formatOctaveWindow(int octaves)
                    {
                        constexpr int kMinOctaves = 1;
                        constexpr int kMaxOctaves = 5;
                        const int clamped = juce::jlimit(kMinOctaves, kMaxOctaves, octaves);
                        return juce::String::fromUTF8("\xc2\xb1") + " " + juce::String(clamped) + " OCT";
                    }

                    // Closed face companion, e.g. "±5" — same ± glyph as the open menu.
                    inline juce::String formatOctaveWindowClosed(int octaves)
                    {
                        constexpr int kMinOctaves = 1;
                        constexpr int kMaxOctaves = 5;
                        const int clamped = juce::jlimit(kMinOctaves, kMaxOctaves, octaves);
                        return juce::String::fromUTF8("\xc2\xb1") + juce::String(clamped);
                    }
                }
            }

            // Footer + dialog copy for the session-load / Compare features. English only.
            namespace Messages
            {
                constexpr const char* kCompareLockedFooter =
                    "Compare mode - editing and patch/bank changes are locked. "
                    "Click C again to exit.";

                constexpr const char* kHistoryGateTitle = "Unsaved mutations";
                constexpr const char* kHistoryGateMessage =
                    "You have mutations in the current session that are not exported. "
                    "Export them before changing patch?";
                constexpr const char* kHistoryGateExport = "Export";
                constexpr const char* kHistoryGateCancel = "Cancel";
                constexpr const char* kHistoryGateDiscard = "Discard";

                constexpr const char* kExportCollisionTitle = "Export folder exists";
                constexpr const char* kExportCollisionMessage =
                    "A session folder with this name already exists.";
                constexpr const char* kExportCollisionOverwrite = "Overwrite";
                constexpr const char* kExportCollisionKeep = "Keep both";
                constexpr const char* kExportCollisionCancel = "Cancel";

                constexpr const char* kExportCancelledFooter = "Export cancelled.";

                constexpr const char* kDeviceDumpFailedFooter =
                    "Could not read the patch from the synth. Keeping the current editor buffer. "
                    "Check that MIDI FROM is the synth MIDI OUT.";
                constexpr const char* kDeviceDumpAbortedEditedFooter =
                    "Synth patch load cancelled because the editor changed while waiting. "
                    "Bank and patch numbers were restored; your edits were kept.";
            }
        }
    } 
}