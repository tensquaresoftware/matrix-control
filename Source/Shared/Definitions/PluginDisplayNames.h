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

        namespace ContextualHelp
        {
            constexpr const char* kMidiFrom =
                "SESSION: Selects the MIDI input that receives SysEx and notes from the synthesizer.";
            constexpr const char* kMidiTo =
                "SESSION: Selects the MIDI output that sends edits and notes to the synthesizer.";
            constexpr const char* kKeyboardFrom =
                "SESSION: Selects a separate MIDI keyboard input for playing (Standalone application only).";
            constexpr const char* kHost =
                "SESSION: Keyboard MIDI comes from the DAW host (Plugin only).";
            constexpr const char* kAudioFrom =
                "SESSION: Selects the audio input used for monitoring through the plugin.";
            constexpr const char* kInputGain =
                "SESSION: Sets monitoring level for the selected audio input.";
            constexpr const char* kUndo =
                "SESSION: Undoes the last Patch or master edit in this session.";
            constexpr const char* kRedo =
                "SESSION: Redoes the last undone edit.";
            constexpr const char* kPanic =
                "SESSION: Sends MIDI panic to clear stuck notes and ease a backed-up send queue.";
            constexpr const char* kLogo =
                "SESSION: Opens the logo menu for Settings, Audio/MIDI, About, Skin, and UI Scale.";
            constexpr const char* kSettings =
                "SESSION: Opens plugin Settings (paths, warnings, master utility, shortcuts).";
            constexpr const char* kAudioMidi =
                "SESSION: Opens the host Audio/MIDI device settings (Standalone application only).";
            constexpr const char* kAbout =
                "SESSION: Shows product version, links, and credits.";
            constexpr const char* kSkin =
                "SESSION: Chooses the visual skin for the editor.";
            constexpr const char* kUiScale =
                "SESSION: Sets the user interface scale.";
            constexpr const char* kKeyboardFromActivityLed =
                "SESSION: Lights when MIDI activity arrives on Keyboard From or Host.";
            constexpr const char* kMidiFromActivityLed =
                "SESSION: Lights when MIDI activity arrives from the synthesizer on MIDI From.";
            constexpr const char* kMidiToActivityLed =
                "SESSION: Lights when MIDI activity is sent to the synthesizer on MIDI To.";
            constexpr const char* kAudioPeakIndicator =
                "SESSION: Shows the peak level of the monitored audio input.";
        }
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
            "Send queue backed up - click PANIC to clear stuck notes.";

        constexpr const char* kContextualHelpBadge = "HELP";

        namespace ContextualHelp
        {
            constexpr const char* kDevice =
                "DEVICE: Shows the connected synthesizer model and EPROM version, or connection status.";
        }

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

        namespace ContextualHelp
        {
            constexpr const char* kEmail =
                "ABOUT: Opens email to Ten Square Software.";
            constexpr const char* kGitHub =
                "ABOUT: Opens the Matrix-Control GitHub repository.";
            constexpr const char* kLinkedIn =
                "ABOUT: Opens the author LinkedIn profile.";
        }
    }

    namespace Settings
    {
        const juce::String kWindowTitle                = "SETTINGS";
        constexpr const char* kSkinLabel               = "SKIN :";
        constexpr const char* kUiScaleLabel            = "UI SCALE :";
        constexpr const char* kHardwareLatencyLabel    = "HARDWARE LATENCY";
        constexpr const char* kAudioFromLabel          = "AUDIO FROM :";
        constexpr const char* kInputGainLabel          = "INPUT GAIN :";
        constexpr const char* kDeviceSection           = "DEVICE";
        constexpr const char* kPatchSection            = "PATCH";
        constexpr const char* kPatchMutatorSection     = "PATCH MUTATOR";
        constexpr const char* kMasterSection           = "MASTER";
        constexpr const char* kEpromTypeLabel          = "EPROM TYPE";
        constexpr const char* kMatrix1000PatchesLabel  = "MATRIX-1000 PATCHES";
        constexpr const char* kComputerPatchesLabel    = "COMPUTER PATCHES";
        constexpr const char* kUnsavedStateLabel       = "UNSAVED STATE";
        constexpr const char* kDeleteWarningLabel      = "DELETE WARNING";
        constexpr const char* kDefragHistoryLabel      = "DEFRAG HISTORY";
        constexpr const char* kInitTemplateLabel       = "INIT TEMPLATE";
        constexpr const char* kUtilityLabel            = "UTILITY";
        constexpr const char* kSaveAsInitButton        = "SAVE AS INIT";
        constexpr const char* kDeleteButton            = "DELETE";
        constexpr const char* kLoadButton              = "LOAD";
        constexpr const char* kSaveAsButton            = "SAVE AS";
        constexpr const char* kInitButton              = "INIT";
        constexpr const char* kComingSoon              = "Coming soon...";
        constexpr const char* kDisplaySysexNames       = "DISPLAY SYSEX NAMES";
        constexpr const char* kDisplayFileNames        = "DISPLAY FILE NAMES";
        constexpr const char* kAskOncePerLoad          = "ASK ONCE PER LOAD";
        constexpr const char* kAlwaysWarn              = "ALWAYS WARN";
        constexpr const char* kNeverWarn               = "NEVER WARN";
        constexpr const char* kDisplayMusicalNames     = "DISPLAY MUSICAL NAMES";
        constexpr const char* kDisplayHardwareNames    = "DISPLAY HARDWARE NAMES";
        constexpr const char* kEpromTypeFactory        = "FACTORY";
        constexpr const char* kEpromTypeGligli         = "GLIGLI";
        constexpr const char* kEpromTypeTauntek        = "TAUNTEK";
        constexpr const char* kEpromTypeUntergeek      = "UNTERGEEK";
        constexpr const char* kEpromTypeUnknown        = "UNKNOWN";
        constexpr const char* kLoadMasterDialogTitle   = "Load Master";
        constexpr const char* kSaveMasterAsDialogTitle = "Save Master As";

        namespace FooterMessages
        {
            constexpr const char* kPatchInitTemplateSaved =
                "Patch init template saved (PatchInit.syx)";
            constexpr const char* kMasterInitTemplateSaved =
                "MASTER init template saved (MasterInit.syx)";
            constexpr const char* kInitTemplateWriteFailed =
                "Could not write init template";
            constexpr const char* kPatchInitTemplateDeleted =
                "Patch init template deleted (PatchInit.syx)";
            constexpr const char* kMasterInitTemplateDeleted =
                "MASTER init template deleted (MasterInit.syx)";
            constexpr const char* kInitTemplateDeleteFailed =
                "Could not delete init template";
            constexpr const char* kMasterLoaded = "MASTER loaded";
            constexpr const char* kMasterSaved = "MASTER saved";
            constexpr const char* kMasterFileFailed = "MASTER file failed";
            constexpr const char* kMasterPullFailed =
                "MASTER: Could not read Master parameters from the synth. Keeping current Master Edit settings. "
                "Check that MIDI FROM is the synth MIDI OUT.";
            constexpr const char* kRenameBeforeSave =
                "Rename patch (replace * INIT *) before SAVE / SAVE AS";
        }

        namespace ContextualHelp
        {
            constexpr const char* kMatrix1000Patches =
                "SETTINGS: Chooses how Matrix-1000 patch names are displayed.";
            constexpr const char* kComputerPatches =
                "SETTINGS: Chooses how computer .syx patch names are displayed.";
            constexpr const char* kUnsavedState =
                "SETTINGS: Chooses when to warn about unsaved patch changes.";
            constexpr const char* kPatchSaveAsInit =
                "SETTINGS: Saves the current patch as the patch init template.";
            constexpr const char* kPatchDeleteInit =
                "SETTINGS: Deletes the saved patch init template.";
            constexpr const char* kDeleteWarning =
                "SETTINGS: Chooses when Patch Mutator delete asks for confirmation.";
            constexpr const char* kDefragHistory =
                "SETTINGS: Coming soon - renumber mutation history when full.";
            constexpr const char* kHardwareLatency =
                "SETTINGS: Sets host audio latency compensation for this plugin instance.";
            constexpr const char* kEpromType =
                "SETTINGS: Declares the synth EPROM type used for MIDI timing and future features.";
            constexpr const char* kMasterLoad =
                "SETTINGS: Loads a Master settings file into the editor.";
            constexpr const char* kMasterSaveAs =
                "SETTINGS: Saves current Master settings as a new file.";
            constexpr const char* kMasterInit =
                "SETTINGS: Resets Master settings to the master init template.";
            constexpr const char* kMasterSaveAsInit =
                "SETTINGS: Saves current Master settings as the master init template.";
            constexpr const char* kMasterDeleteInit =
                "SETTINGS: Deletes the saved master init template.";
        }

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

        namespace EpromTypePrompt
        {
            constexpr const char* kTitle = "DEVICE SETUP";
            constexpr const char* kBody =
                "Select MIDI ports and the EPROM type installed in your synth. "
                "This affects MIDI timing and future features.";
            constexpr const char* kBodySuggestionSuffix =
                " A suggestion is preselected from the reported firmware version when possible.";
            constexpr const char* kMidiFromLabel = "MIDI FROM";
            constexpr const char* kMidiToLabel = "MIDI TO";
            constexpr const char* kSearching = "SEARCHING";
            constexpr const char* kEpromTypeLabel = "EPROM TYPE";
            constexpr const char* kConfirm = "CONFIRM";
            constexpr const char* kSpecifyLater = "SPECIFY LATER";
        }

        namespace MasterGlobalInitConfirm
        {
            constexpr const char* kTitle = "RESET ALL MASTER MODULES?";
            constexpr const char* kBody =
                "This will reset MIDI, VIBRATO, and MISC to the Master init template "
                "(or built-in defaults) and send a full master SysEx to the synth.";
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

        namespace DeleteInitTemplateConfirm
        {
            constexpr const char* kTitle = "Delete init template?";
            constexpr const char* kBodyPatch =
                "This removes the system Patch init template (PatchInit.syx).\n"
                "The next Patch INIT will use the built-in defaults.\n\n"
                "DELETE to remove it, or Cancel to keep the file.";
            constexpr const char* kBodyMaster =
                "This removes the system Master init template (MasterInit.syx).\n"
                "The next Master INIT will use the built-in defaults.\n\n"
                "DELETE to remove it, or Cancel to keep the file.";
            constexpr const char* kCancel = "Cancel";
            constexpr const char* kDelete = "DELETE";
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
        constexpr const char* kMatrixModulationName = "MATRIX MODULATION";

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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "MIDI: Resets this module to its init values.";
                constexpr const char* kChannel =
                    "MIDI: Sets the synthesizer MIDI channel.";
                constexpr const char* kMidiEcho =
                    "MIDI: Echoes incoming MIDI back out when enabled.";
                constexpr const char* kControllers =
                    "MIDI: Enables or disables MIDI controller reception.";
                constexpr const char* kPatchChanges =
                    "MIDI: Enables or disables MIDI program changes.";
                constexpr const char* kPedal1Select =
                    "MIDI: Assigns the function for pedal 1.";
                constexpr const char* kPedal2Select =
                    "MIDI: Assigns the function for pedal 2.";
                constexpr const char* kLever2Select =
                    "MIDI: Assigns the function for lever 2.";
                constexpr const char* kLever3Select =
                    "MIDI: Assigns the function for lever 3.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "VIBRATO: Resets this module to its init values.";
                constexpr const char* kSpeed =
                    "VIBRATO: Sets global vibrato rate.";
                constexpr const char* kWaveform =
                    "VIBRATO: Chooses the vibrato waveform.";
                constexpr const char* kAmplitude =
                    "VIBRATO: Sets global vibrato depth.";
                constexpr const char* kSpeedModSource =
                    "VIBRATO: Selects what modulates vibrato speed.";
                constexpr const char* kSpeedModAmount =
                    "VIBRATO: How much the speed mod source affects vibrato rate.";
                constexpr const char* kAmpModSource =
                    "VIBRATO: Selects what modulates vibrato depth.";
                constexpr const char* kAmpModAmount =
                    "VIBRATO: How much the amp mod source affects vibrato depth.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "MISC: Resets this module to its init values.";
                constexpr const char* kMasterTune =
                    "MISC: Fine-tunes overall instrument pitch.";
                constexpr const char* kMasterTranspose =
                    "MISC: Transposes the instrument in semitones.";
                constexpr const char* kBendRange =
                    "MISC: Sets pitch-bend range in semitones up and down.";
                constexpr const char* kUnison =
                    "MISC: Enables Master Unison (can override Patch keyboard mode).";
                constexpr const char* kVolumeInvert =
                    "MISC: Inverts volume pedal response when enabled.";
                constexpr const char* kBankLock =
                    "MISC: Locks bank changes on the hardware.";
                constexpr const char* kMemoryProtect =
                    "MISC: Protects synth memory from being overwritten.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "DCO 1: Resets this module to its init values.";
                constexpr const char* kCopy =
                    "DCO 1: Copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "DCO 1: Pastes compatible clipboard data into this module.";
                constexpr const char* kFrequency =
                    "DCO 1: Sets DCO 1 pitch in semitones.";
                constexpr const char* kFrequencyModByLfo1 =
                    "DCO 1: How much LFO 1 modulates DCO 1 frequency.";
                constexpr const char* kSync =
                    "DCO 1: Hard-syncs DCO 1 to DCO 2 when enabled.";
                constexpr const char* kPulseWidth =
                    "DCO 1: Sets pulse width for pulse / combination waves.";
                constexpr const char* kPulseWidthModByLfo2 =
                    "DCO 1: How much LFO 2 modulates pulse width.";
                constexpr const char* kWaveShape =
                    "DCO 1: Mixes wave components for the selected wave family.";
                constexpr const char* kWaveSelect =
                    "DCO 1: Chooses the DCO 1 waveform family.";
                constexpr const char* kLevers =
                    "DCO 1: Enables lever modulation for this DCO.";
                constexpr const char* kKeyboardPortamento =
                    "DCO 1: Tracks keyboard / portamento for this DCO.";
                constexpr const char* kKeyClick =
                    "DCO 1: Adds a short click on key-on for this DCO.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "DCO 2: Resets this module to its init values.";
                constexpr const char* kCopy =
                    "DCO 2: Copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "DCO 2: Pastes compatible clipboard data into this module.";
                constexpr const char* kFrequency =
                    "DCO 2: Sets DCO 2 pitch in semitones.";
                constexpr const char* kFrequencyModByLfo1 =
                    "DCO 2: How much LFO 1 modulates DCO 2 frequency.";
                constexpr const char* kDetune =
                    "DCO 2: Fine detune of DCO 2 relative to DCO 1.";
                constexpr const char* kPulseWidth =
                    "DCO 2: Sets pulse width for pulse / combination waves.";
                constexpr const char* kPulseWidthModByLfo2 =
                    "DCO 2: How much LFO 2 modulates pulse width.";
                constexpr const char* kWaveShape =
                    "DCO 2: Mixes wave components for the selected wave family.";
                constexpr const char* kWaveSelect =
                    "DCO 2: Chooses the DCO 2 waveform family (includes Noise).";
                constexpr const char* kLevers =
                    "DCO 2: Enables lever modulation for this DCO.";
                constexpr const char* kKeyboardPortamento =
                    "DCO 2: Tracks keyboard / portamento for this DCO.";
                constexpr const char* kKeyClick =
                    "DCO 2: Adds a short click on key-on for this DCO.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "VCF/VCA: Resets this module to its init values.";
                constexpr const char* kBalance =
                    "VCF/VCA: Balances DCO 1 and DCO 2 into the filter stage.";
                constexpr const char* kFrequency =
                    "VCF/VCA: Sets the filter cutoff frequency.";
                constexpr const char* kFrequencyModByEnv1 =
                    "VCF/VCA: How much ENV 1 modulates filter cutoff frequency.";
                constexpr const char* kFrequencyModByPressure =
                    "VCF/VCA: How much pressure modulates filter cutoff frequency.";
                constexpr const char* kResonance =
                    "VCF/VCA: Sets filter resonance (emphasis at cutoff frequency).";
                constexpr const char* kVca1Volume =
                    "VCF/VCA: Sets VCA 1 overall volume.";
                constexpr const char* kVca1ModByVelocity =
                    "VCF/VCA: How much velocity modulates VCA 1.";
                constexpr const char* kVca2ModByEnv2 =
                    "VCF/VCA: How much ENV 2 modulates VCA 2.";
                constexpr const char* kLevers =
                    "VCF/VCA: Enables lever modulation for filter / amplifier.";
                constexpr const char* kKeyboardPortamento =
                    "VCF/VCA: Tracks keyboard / portamento for the filter.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "FM/TRACK: Resets this module to its init values.";
                constexpr const char* kFmAmount =
                    "FM/TRACK: Sets FM amount into the VCF from DCO 2.";
                constexpr const char* kFmModByEnv3 =
                    "FM/TRACK: How much ENV 3 modulates FM amount.";
                constexpr const char* kFmModByPressure =
                    "FM/TRACK: How much pressure modulates FM amount.";
                constexpr const char* kTrackPoint1 =
                    "FM/TRACK: Sets Track Generator point 1 level.";
                constexpr const char* kTrackPoint2 =
                    "FM/TRACK: Sets Track Generator point 2 level.";
                constexpr const char* kTrackPoint3 =
                    "FM/TRACK: Sets Track Generator point 3 level.";
                constexpr const char* kTrackPoint4 =
                    "FM/TRACK: Sets Track Generator point 4 level.";
                constexpr const char* kTrackPoint5 =
                    "FM/TRACK: Sets Track Generator point 5 level.";
                constexpr const char* kTrackInput =
                    "FM/TRACK: Selects the source that drives the Track Generator.";
                constexpr const char* kTrackGeneratorDisplay =
                    "FM/TRACK: Drag the Track Generator curve points for tracking shape.";
            }
        }

        namespace RampPortamentoModule
        {
            constexpr const char* kName = "RAMP/PORTAMENTO";
            constexpr const char* kLegatoPortaUnisonBlockedFooter =
                "LEGATO PORTA is only available when KEYBOARD MODE is UNISON.";
            constexpr const char* kMasterUnisonOverrideFooter =
                "MASTER UNISON is overriding KEYBOARD MODE.";
            constexpr const char* kStrigRequiresUnisonFooter =
                "STRIG is only available when KEYBOARD MODE is UNISON.";

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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "RAMP/PORTAMENTO: Resets this module to its init values.";
                constexpr const char* kRamp1Rate =
                    "RAMP/PORTAMENTO: Sets Ramp 1 rise time.";
                constexpr const char* kRamp1Trigger =
                    "RAMP/PORTAMENTO: Chooses what starts Ramp 1.";
                constexpr const char* kRamp2Rate =
                    "RAMP/PORTAMENTO: Sets Ramp 2 rise time.";
                constexpr const char* kRamp2Trigger =
                    "RAMP/PORTAMENTO: Chooses what starts Ramp 2.";
                constexpr const char* kPortamentoRate =
                    "RAMP/PORTAMENTO: Sets glide time between notes.";
                constexpr const char* kPortamentoModByVelocity =
                    "RAMP/PORTAMENTO: How much velocity scales portamento rate.";
                constexpr const char* kPortamentoMode =
                    "RAMP/PORTAMENTO: Chooses how portamento glides between notes.";
                constexpr const char* kPortamentoLegato =
                    "RAMP/PORTAMENTO: Enables legato-only portamento (Unison keyboard modes).";
                constexpr const char* kPortamentoKeyboardMode =
                    "RAMP/PORTAMENTO: Chooses poly / unison / related keyboard response.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "ENV 1: Resets this module to its init values.";
                constexpr const char* kCopy =
                    "ENV 1: Copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "ENV 1: Pastes compatible clipboard data into this module.";
                constexpr const char* kDelay =
                    "ENV 1: Delay before the envelope attack starts.";
                constexpr const char* kAttack =
                    "ENV 1: Time to rise from zero to peak.";
                constexpr const char* kDecay =
                    "ENV 1: Time to fall from peak to sustain.";
                constexpr const char* kSustain =
                    "ENV 1: Level held while the key is down.";
                constexpr const char* kRelease =
                    "ENV 1: Time to fall to zero after key-up.";
                constexpr const char* kAmplitude =
                    "ENV 1: Overall envelope depth.";
                constexpr const char* kAmplitudeModByVelocity =
                    "ENV 1: How much velocity scales envelope amplitude.";
                constexpr const char* kTriggerMode =
                    "ENV 1: Chooses how the envelope is triggered.";
                constexpr const char* kEnvelopeMode =
                    "ENV 1: Chooses envelope contour behaviour.";
                constexpr const char* kLfo1Trigger =
                    "ENV 1: Lets LFO 1 retrigger this envelope when enabled.";
                constexpr const char* kEnvelopeDisplay =
                    "ENV 1: Drag Delay, Attack, Decay, Sustain, and Release on the curve.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "ENV 2: Resets this module to its init values.";
                constexpr const char* kCopy =
                    "ENV 2: Copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "ENV 2: Pastes compatible clipboard data into this module.";
                constexpr const char* kDelay =
                    "ENV 2: Delay before the envelope attack starts.";
                constexpr const char* kAttack =
                    "ENV 2: Time to rise from zero to peak.";
                constexpr const char* kDecay =
                    "ENV 2: Time to fall from peak to sustain.";
                constexpr const char* kSustain =
                    "ENV 2: Level held while the key is down.";
                constexpr const char* kRelease =
                    "ENV 2: Time to fall to zero after key-up.";
                constexpr const char* kAmplitude =
                    "ENV 2: Overall envelope depth.";
                constexpr const char* kAmplitudeModByVelocity =
                    "ENV 2: How much velocity scales envelope amplitude.";
                constexpr const char* kTriggerMode =
                    "ENV 2: Chooses how the envelope is triggered.";
                constexpr const char* kEnvelopeMode =
                    "ENV 2: Chooses envelope contour behaviour.";
                constexpr const char* kLfo1Trigger =
                    "ENV 2: Lets LFO 1 retrigger this envelope when enabled.";
                constexpr const char* kEnvelopeDisplay =
                    "ENV 2: Drag Delay, Attack, Decay, Sustain, and Release on the curve.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "ENV 3: Resets this module to its init values.";
                constexpr const char* kCopy =
                    "ENV 3: Copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "ENV 3: Pastes compatible clipboard data into this module.";
                constexpr const char* kDelay =
                    "ENV 3: Delay before the envelope attack starts.";
                constexpr const char* kAttack =
                    "ENV 3: Time to rise from zero to peak.";
                constexpr const char* kDecay =
                    "ENV 3: Time to fall from peak to sustain.";
                constexpr const char* kSustain =
                    "ENV 3: Level held while the key is down.";
                constexpr const char* kRelease =
                    "ENV 3: Time to fall to zero after key-up.";
                constexpr const char* kAmplitude =
                    "ENV 3: Overall envelope depth.";
                constexpr const char* kAmplitudeModByVelocity =
                    "ENV 3: How much velocity scales envelope amplitude.";
                constexpr const char* kTriggerMode =
                    "ENV 3: Chooses how the envelope is triggered.";
                constexpr const char* kEnvelopeMode =
                    "ENV 3: Chooses envelope contour behaviour.";
                constexpr const char* kLfo1Trigger =
                    "ENV 3: Lets LFO 1 retrigger this envelope when enabled.";
                constexpr const char* kEnvelopeDisplay =
                    "ENV 3: Drag Delay, Attack, Decay, Sustain, and Release on the curve.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "LFO 1: Resets this module to its init values.";
                constexpr const char* kCopy =
                    "LFO 1: Copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "LFO 1: Pastes compatible clipboard data into this module.";
                constexpr const char* kSpeed =
                    "LFO 1: Sets LFO 1 rate.";
                constexpr const char* kSpeedModByPressure =
                    "LFO 1: How much pressure modulates LFO 1 speed.";
                constexpr const char* kRetriggerPoint =
                    "LFO 1: Phase point used when the LFO retriggers.";
                constexpr const char* kAmplitude =
                    "LFO 1: Sets LFO 1 depth.";
                constexpr const char* kAmplitudeModByRamp1 =
                    "LFO 1: How much Ramp 1 scales LFO 1 amplitude.";
                constexpr const char* kWaveform =
                    "LFO 1: Chooses the LFO 1 waveform.";
                constexpr const char* kTriggerMode =
                    "LFO 1: Chooses how LFO 1 starts and retriggers.";
                constexpr const char* kLag =
                    "LFO 1: Smooths LFO 1 output changes.";
                constexpr const char* kSampleInput =
                    "LFO 1: Chooses which modulation source the LFO samples when WAVEFORM is SAMPLED.";
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

            namespace ContextualHelp
            {
                constexpr const char* kInit =
                    "LFO 2: Resets this module to its init values.";
                constexpr const char* kCopy =
                    "LFO 2: Copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "LFO 2: Pastes compatible clipboard data into this module.";
                constexpr const char* kSpeed =
                    "LFO 2: Sets LFO 2 rate.";
                constexpr const char* kSpeedModByKeyboard =
                    "LFO 2: How much keyboard tracking modulates LFO 2 speed.";
                constexpr const char* kRetriggerPoint =
                    "LFO 2: Phase point used when the LFO retriggers.";
                constexpr const char* kAmplitude =
                    "LFO 2: Sets LFO 2 depth.";
                constexpr const char* kAmplitudeModByRamp2 =
                    "LFO 2: How much Ramp 2 scales LFO 2 amplitude.";
                constexpr const char* kWaveform =
                    "LFO 2: Chooses the LFO 2 waveform.";
                constexpr const char* kTriggerMode =
                    "LFO 2: Chooses how LFO 2 starts and retriggers.";
                constexpr const char* kLag =
                    "LFO 2: Smooths LFO 2 output changes.";
                constexpr const char* kSampleInput =
                    "LFO 2: Chooses which modulation source the LFO samples when WAVEFORM is SAMPLED.";
            }
        }

        namespace PatchNameModule
        {
            constexpr const char* kName = "PATCH NAME";

            namespace StandaloneWidgets
            {
                // Patch names are 8 characters long in the Oberheim Matrix-1000 :
                constexpr const char* kDefaultPatchName = "--------";
                // Runtime sentinel after Internal Patches INIT (8 chars). Not Matrix-legal for
                // Computer Patches Save / Save As — user must rename first. Distinct from
                // session-idle kDefaultPatchName and from Mutator Compare secondary "INITIAL".
                constexpr const char* kInitPatchName = "* INIT *";
                // Secondary line literal while Compare is active — distinct from the Mxx / Mxx-Ryy
                // Mutator history labels used the rest of the time.
                constexpr const char* kCompareSecondaryLabel = "INITIAL";
            }

            namespace ContextualHelp
            {
                constexpr const char* kPatchNameDisplay =
                    "PATCH NAME: Shows the 8-character patch name - double-click to rename when editable.";
            }

            // Footer copy for the inline rename custom-caret editor. English only.
            namespace Messages
            {
                constexpr const char* kInvalidCharacterFooter = "Invalid character for patch name";
            }

            // Drag-drop .syx overlay on PATCH NAME (primary fixed; secondary blinks).
            namespace DragDropOverlay
            {
                // Eight asterisks — distinct from startup "--------"; ASCII-safe for the display font.
                constexpr const char* kBadPrimary = "********";
                // Multi / folder drag accept — ASCII dots, 10 chars fixed (no counter).
                constexpr const char* kPatchesEllipsis = "PATCHES...";
                constexpr const char* kDropToLoad = "DROP TO LOAD";
                constexpr const char* kBadFile = "BAD FILE";
                // Plural secondary when the drag selection has two or more unloadable items.
                constexpr const char* kBadFiles = "BAD FILES";
            }

            // Name-required presentation (STORE-after-INIT gate UI): empty L1 + caret, blinking L2.
            // Distinct from DragDropOverlay and from Mutator Compare secondary "INITIAL".
            namespace NameRequired
            {
                constexpr const char* kSecondaryLabel = "NAME REQUIRED";
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
                "Clipboard contents are not compatible with MATRIX MODULATION paste.";

            namespace StandaloneWidgets
            {
                constexpr const char* kInit  = ShortLabels::kInit;
                constexpr const char* kCopy  = ShortLabels::kCopy;
                constexpr const char* kPaste = ShortLabels::kPaste;
            }
        }

        namespace ContextualHelp
        {
            constexpr const char* kSectionInit =
                "MATRIX MODULATION: Resets all modulation buses to init.";
            constexpr const char* kSectionCopy =
                "MATRIX MODULATION: Copies all modulation buses to the clipboard.";
            constexpr const char* kSectionPaste =
                "MATRIX MODULATION: Pastes compatible clipboard data into all modulation buses.";
            constexpr const char* kBusHandle =
                "MATRIX MODULATION: Drag to reorder this modulation bus.";
            constexpr const char* kBusInit =
                "MATRIX MODULATION: Resets this bus source, amount, and destination.";
            constexpr const char* kSource =
                "MATRIX MODULATION: Selects the modulation source for this bus.";
            constexpr const char* kAmount =
                "MATRIX MODULATION: Sets how strongly this bus modulates its destination.";
            constexpr const char* kDestination =
                "MATRIX MODULATION: Selects what this bus modulates.";
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

            namespace ContextualHelp
            {
                constexpr const char* kBank0 =
                    "BANK UTILITY: Selects Matrix-1000 bank 0 (RAM) and sets it as the copy/paste/import/export target.";
                constexpr const char* kBank1 =
                    "BANK UTILITY: Selects Matrix-1000 bank 1 (RAM) and sets it as the copy/paste/import/export target.";
                constexpr const char* kBank2 =
                    "BANK UTILITY: Selects Matrix-1000 bank 2 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kBank3 =
                    "BANK UTILITY: Selects Matrix-1000 bank 3 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kBank4 =
                    "BANK UTILITY: Selects Matrix-1000 bank 4 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kBank5 =
                    "BANK UTILITY: Selects Matrix-1000 bank 5 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kBank6 =
                    "BANK UTILITY: Selects Matrix-1000 bank 6 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kBank7 =
                    "BANK UTILITY: Selects Matrix-1000 bank 7 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kBank8 =
                    "BANK UTILITY: Selects Matrix-1000 bank 8 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kBank9 =
                    "BANK UTILITY: Selects Matrix-1000 bank 9 (ROM) and sets it as the copy/export target (paste/import need RAM 0-1).";
                constexpr const char* kCopy =
                    "BANK UTILITY: Copies the selected bank from the synthesizer to the clipboard.";
                constexpr const char* kPaste =
                    "BANK UTILITY: Pastes the clipboard bank into the selected destination bank.";
                constexpr const char* kImport =
                    "BANK UTILITY: Imports patch files into the selected RAM bank.";
                constexpr const char* kExport =
                    "BANK UTILITY: Exports the selected bank as SysEx files on disk.";
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

            namespace ContextualHelp
            {
                constexpr const char* kTitle =
                    "INTERNAL PATCHES: Reloads / focuses the internal patch browser for the connected synthesizer.";
                constexpr const char* kBrowser =
                    "INTERNAL PATCHES: Shows the internal patch browser list.";
                constexpr const char* kMemory =
                    "INTERNAL PATCHES: Shows memory-oriented patch controls.";
                constexpr const char* kPrevious =
                    "INTERNAL PATCHES: Loads the previous patch in the current bank.";
                constexpr const char* kNext =
                    "INTERNAL PATCHES: Loads the next patch in the current bank.";
                constexpr const char* kCurrentBank =
                    "INTERNAL PATCHES: Selects the current synthesizer bank.";
                constexpr const char* kCurrentPatch =
                    "INTERNAL PATCHES: Selects the current patch number in the bank.";
                constexpr const char* kInit =
                    "INTERNAL PATCHES: Loads the init patch template into the editor and sends it to the synthesizer.";
                constexpr const char* kCopy =
                    "INTERNAL PATCHES: Copies the current patch to the clipboard.";
                constexpr const char* kPaste =
                    "INTERNAL PATCHES: Pastes a clipboard patch into the editor and sends it to the synthesizer.";
                constexpr const char* kStore =
                    "INTERNAL PATCHES: Stores the current patch into the selected synthesizer memory slot.";
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

                // Full absolute path; FooterPanel middle-truncates Loaded/Saved with ASCII "...".
                inline juce::String formatReadablePatchLocation(const juce::File& file)
                {
                    const auto fullPath = file.getFullPathName();
                    return fullPath.isNotEmpty() ? fullPath : file.getFileName();
                }

                inline juce::String formatSaveSuccess(const juce::String& location)
                {
                    return "Saved " + location;
                }

                inline juce::String formatLoadSuccess(const juce::String& location)
                {
                    return "Loaded " + location;
                }

                // Prefix for the first successful load after OPEN / drop scan (not Prev/Next).
                inline juce::String formatScanLoadPrefix(int validCount, int invalidCount)
                {
                    auto summary = "Patch files: " + juce::String(validCount) + " valid";
                    if (invalidCount > 0)
                        summary += " / " + juce::String(invalidCount) + " invalid";
                    return summary;
                }

                inline juce::String formatFirstLoadAfterScan(int validCount,
                                                            int invalidCount,
                                                            const juce::String& location)
                {
                    return formatScanLoadPrefix(validCount, invalidCount) + " - "
                           + formatLoadSuccess(location);
                }

                inline juce::String formatFirstLoadAfterScanMessage(int validCount,
                                                                   int invalidCount,
                                                                   const juce::String& loadedMessage)
                {
                    return formatScanLoadPrefix(validCount, invalidCount) + " - " + loadedMessage;
                }

                inline juce::String formatReconciliationNotice(const juce::String& location,
                                                               bool usedFilename)
                {
                    if (usedFilename)
                        return "Loaded " + location + " (filename used)";

                    return "Loaded " + location + " (internal name used)";
                }

                constexpr const char* kEmptyFolder = "0 files in folder";
                constexpr const char* kFolderNotFound = "Folder not found";
                constexpr const char* kLoadSelectionStale = "Selection out of date";
                constexpr const char* kPatchFileNotFound = "Patch file not found";
                constexpr const char* kInvalidSaveStem =
                    "Invalid patch file name (use A-Z, 0-9, space, -, _; max 8)";

                // Drag-drop onto the editor (footer only on drop — never during drag).
                constexpr const char* kDropRejectedNoValid =
                    "Drop rejected: no valid Matrix patch .syx";
                constexpr const char* kDropRejectedNotSyx =
                    "Drop rejected: not a Matrix patch .syx";
                constexpr const char* kDropRejectedBankOrMulti =
                    "Drop rejected: bank or multi-message dump";
                constexpr const char* kDropRejectedInvalid =
                    "Drop rejected: invalid patch file";
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

            namespace ContextualHelp
            {
                constexpr const char* kTitle =
                    "COMPUTER PATCHES: Reloads / focuses computer .syx patch browsing on disk.";
                constexpr const char* kBrowser =
                    "COMPUTER PATCHES: Shows the computer patch browser list.";
                constexpr const char* kStorage =
                    "COMPUTER PATCHES: Shows save / storage controls for .syx files.";
                constexpr const char* kPrevious =
                    "COMPUTER PATCHES: Loads the previous patch file in the folder.";
                constexpr const char* kNext =
                    "COMPUTER PATCHES: Loads the next patch file in the folder.";
                constexpr const char* kSelectPatch =
                    "COMPUTER PATCHES: Chooses which .syx patch file is selected.";
                constexpr const char* kOpen =
                    "COMPUTER PATCHES: Opens a folder of .syx patch files.";
                constexpr const char* kSaveAs =
                    "COMPUTER PATCHES: Saves the current patch as a new .syx file.";
                constexpr const char* kSave =
                    "COMPUTER PATCHES: Saves over the current .syx file.";
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

            // Furtive left-footer help while hovering / focusing Mutator controls (display-only).
            // Furtive left-footer help while hovering / focusing Mutator controls (display-only).
            // Sticky Messages and HELP both use "PATCH MUTATOR: " (GUI casing).
            namespace ContextualHelp
            {
                constexpr const char* kMode =
                    "PATCH MUTATOR: Sets how far mutations stray - Kindred, Drift, Warp, or Wild.";
                constexpr const char* kPitch =
                    "PATCH MUTATOR: Controls how DCO pitch may move - Keep, Consonant, Dissonant, or Free.";
                constexpr const char* kHistory =
                    "PATCH MUTATOR: Recalls a mutation or retry from this session.";
                constexpr const char* kMutate =
                    "PATCH MUTATOR: Creates a new variation from the current recipe and sends it to the synthesizer.";
                constexpr const char* kRetry =
                    "PATCH MUTATOR: Rolls again from the same mutation root.";
                constexpr const char* kHistoryPrevious =
                    "PATCH MUTATOR: Steps backward through session history.";
                constexpr const char* kHistoryNext =
                    "PATCH MUTATOR: Steps forward through session history.";
                constexpr const char* kCompare =
                    "PATCH MUTATOR: Compares with the origin patch and locks editing until you click [C] button again.";
                constexpr const char* kDelete =
                    "PATCH MUTATOR: Deletes the selected history entry.";
                constexpr const char* kFlush =
                    "PATCH MUTATOR: Flushes the whole session mutation history.";
                constexpr const char* kExport =
                    "PATCH MUTATOR: Exports the session mutations as SysEx files.";
                constexpr const char* kEnableDco1 =
                    "PATCH MUTATOR: Include DCO 1 module in the mutation recipe.";
                constexpr const char* kEnableDco2 =
                    "PATCH MUTATOR: Include DCO 2 module in the mutation recipe.";
                constexpr const char* kEnableVcfVca =
                    "PATCH MUTATOR: Include VCF/VCA module in the recipe.";
                constexpr const char* kEnableFmTrack =
                    "PATCH MUTATOR: Include FM/TRACK module in the recipe.";
                constexpr const char* kEnableRampPortamento =
                    "PATCH MUTATOR: Include RAMP/PORTAMENTO module in the recipe.";
                constexpr const char* kEnableEnvelope1 =
                    "PATCH MUTATOR: Include ENV 1 module in the recipe.";
                constexpr const char* kEnableEnvelope2 =
                    "PATCH MUTATOR: Include ENV 2 module in the recipe.";
                constexpr const char* kEnableEnvelope3 =
                    "PATCH MUTATOR: Include ENV 3 module in the recipe.";
                constexpr const char* kEnableLfo1 =
                    "PATCH MUTATOR: Include LFO 1 module in the recipe.";
                constexpr const char* kEnableLfo2 =
                    "PATCH MUTATOR: Include LFO 2 module in the recipe.";
                constexpr const char* kEnableMatrixMod =
                    "PATCH MUTATOR: Include MATRIX MODULATION module in the recipe.";
            }

            // Footer + dialog copy for the session-load / Compare features. English only.
            // Sticky footers use "PATCH MUTATOR: " prefix; dialog-only strings stay unprefixed.
            namespace Messages
            {
                constexpr const char* kCompareLockedFooter =
                    "PATCH MUTATOR: Compare mode - editing and patch/bank changes are locked. "
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

                constexpr const char* kExportCancelledFooter = "PATCH MUTATOR: Export cancelled.";

                // Stem for Export success sticky + FooterPanel path-style middle truncate.
                constexpr const char* kExportCompleteFooterStem = "PATCH MUTATOR: Exported ";

                constexpr const char* kDeviceDumpFailedFooter =
                    "PATCH MUTATOR: Could not read the patch from the synth. Keeping the current editor buffer. "
                    "Check that MIDI FROM is the synth MIDI OUT.";
                constexpr const char* kDeviceDumpAbortedEditedFooter =
                    "PATCH MUTATOR: Synth patch load cancelled because the editor changed while waiting. "
                    "Bank and patch numbers were restored; your edits were kept.";
            }
        }
    } 
}