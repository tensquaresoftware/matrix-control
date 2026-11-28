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
        // Monitoring labels (header) — Settings cabling uses KEYBOARD FROM / SYNTH FROM / SYNTH TO.
        constexpr const char* kFromKeyboardLabel   = "FROM KEYBOARD";
        constexpr const char* kFromSynthLabel      = "FROM SYNTH";
        constexpr const char* kToSynthLabel        = "TO SYNTH";
        constexpr const char* kMidiCartoucheLabel  = "MIDI";
        constexpr const char* kAudioCartoucheLabel = "AUDIO";
        constexpr const char* kEditCartoucheLabel  = "EDIT";
        constexpr const char* kInputGainLabel      = "INPUT GAIN";
        constexpr const char* kNoInputSentinel     = "NO INPUT";
        constexpr const char* kNoOutputSentinel    = "NO OUTPUT";
        constexpr const char* kSettingsButton      = "SETTINGS...";
        constexpr const char* kAboutButton         = "ABOUT...";
        constexpr const char* kLogoSkinSection     = "SKIN";
        constexpr const char* kLogoUiScaleSection  = "UI SCALE";
        constexpr const char* kPanic               = "PANIC";
        constexpr const char* kUndo                = "UNDO";
        constexpr const char* kRedo                = "REDO";

        namespace ContextualHelp
        {
            // Prefixes follow on-screen cartouches: EDIT = UNDO/REDO; MIDI = LEDs + labels + PANIC;
            // AUDIO = INPUT GAIN + peak; logo + logo menu (outside cartouches) use MATRIX-CONTROL:.
            constexpr const char* kEditBadge =
                "EDIT: Undo and redo the last PATCH or MASTER edit.";
            constexpr const char* kMidiBadge =
                "MIDI: Activity lights for MIDI keyboard (or host in Plugin), synthesizer MIDI in and out, plus PANIC.";
            constexpr const char* kAudioBadge =
                "AUDIO: Monitors the selected audio input level and sets INPUT GAIN (Standalone).";
            constexpr const char* kInputGain =
                "AUDIO: INPUT GAIN sets monitoring level for the selected audio input. Choose the listen source in"
                " SETTINGS > AUDIO > AUDIO FROM.";
            constexpr const char* kUndo =
                "EDIT: UNDO undoes the last PATCH or MASTER edit in this session.";
            constexpr const char* kRedo =
                "EDIT: REDO redoes the last undone PATCH or MASTER edit.";
            constexpr const char* kPanic =
                "MIDI: PANIC sends Note Offs for held notes, plus All Notes Off and Reset Controllers.";
            constexpr const char* kLogo =
                "MATRIX-CONTROL: LOGO opens the logo menu for SETTINGS, About, Skin, and UI Scale.";
            constexpr const char* kSettings =
                "MATRIX-CONTROL: SETTINGS opens SETTINGS (MIDI ports, standalone audio, paths, warnings, master"
                " utility, shortcuts).";
            constexpr const char* kAbout =
                "MATRIX-CONTROL: ABOUT shows product version, links, and credits.";
            constexpr const char* kSkin =
                "MATRIX-CONTROL: SKIN chooses the visual skin (BLACK, CREAM, or TONES family sub-menus)."
                " Cmd/Ctrl+arrows cycle within the active family; Cmd/Ctrl+Shift+arrows cycle all skins.";
            constexpr const char* kUiScale =
                "MATRIX-CONTROL: UI SCALE sets the user interface scale.";
            // Shared by each LED and its face label (same cartouche lane).
            constexpr const char* kFromKeyboardActivityLed =
                "MIDI: FROM KEYBOARD lights when MIDI activity arrives from the MIDI keyboard (or host in Plugin).";
            constexpr const char* kFromSynthActivityLed =
                "MIDI: FROM SYNTH lights when MIDI activity arrives from the synthesizer.";
            constexpr const char* kToSynthActivityLed =
                "MIDI: TO SYNTH lights when MIDI activity is sent to the synthesizer.";
            constexpr const char* kAudioPeakIndicator =
                "AUDIO: AUDIO PEAK shows the peak level of the monitored audio input.";
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
            constexpr const char* kCentreBand =
                "MIDI: Shows a red alert when the MIDI send queue is backed up - click PANIC button to clear stuck notes.";
            constexpr const char* kDevice =
                "DEVICE: Shows the connected synthesizer model and EPROM version, or connection status.";
        }

        // Left-zone guidance while FR-2 device lock is active (D-038 — footer only, no modal).
        constexpr const char* kDeviceLockGuidance =
            "No synth detected - check MIDI cables, Settings > MIDI & DEVICE, and power-cycle your Matrix.";

        // Presence inquiry timed out while still detected — synth likely overloaded, not unplugged.
        constexpr const char* kDeviceUnresponsiveGuidance =
            "Synth not responding (often after heavy SysEx) - stop editing, power-cycle the Matrix, then click PANIC if needed.";

        // Connected Oberheim Matrix-family device whose member is not Matrix-1000 / Matrix-6/6R.
        constexpr const char* kUnsupportedMatrixDeviceFooter =
            "Connected Matrix device is not supported - editing is locked. Change ports in Settings > MIDI & DEVICE.";

        // Standalone: SYNTH FROM and KEYBOARD FROM must open distinct input devices.
        constexpr const char* kMidiFromKeyboardFromConflictFooter =
            "SYNTH FROM and KEYBOARD FROM must use different MIDI input devices.";
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
        constexpr const char* kBmadCreditPrefix      = "Yet another project successfully completed with ";
        constexpr const char* kBmadCreditLinkDisplay = "BMad";
        constexpr const char* kBmadCreditSuffix      = "!";
        constexpr const char* kBmadCreditUrl         = "https://github.com/bmad-code-org/bmad-method";
    }

    namespace Settings
    {
        const juce::String kWindowTitle                = "SETTINGS";
        constexpr const char* kSkinLabel               = "SKIN:";
        constexpr const char* kUiScaleLabel            = "UI SCALE:";
        // User Interface row labels (no colon — peers of INFO MESSAGE / CONTEXTUAL HELP).
        constexpr const char* kUiScaleRowLabel         = "UI SCALE";
        constexpr const char* kSkinRowLabel            = "SKIN";
        constexpr const char* kGettingStartedLabel     = "GETTING STARTED";
        constexpr const char* kSetupWizardLabel        = "SETUP WIZARD";
        constexpr const char* kShowWhenIncomplete      = "SHOW WHEN INCOMPLETE";
        constexpr const char* kNeverAtLaunch           = "NEVER SHOW AT LAUNCH";
        constexpr const char* kRunSetupAgainButton     = "RUN SETUP AGAIN";
        constexpr const char* kHardwareLatencyLabel    = "HARDWARE LATENCY";
        constexpr const char* kInputGainLabel          = "INPUT GAIN:";
        constexpr const char* kGettingStartedTab       = "GETTING STARTED";
        constexpr const char* kUserInterfaceTab        = "USER INTERFACE";
        constexpr const char* kDeviceSection           = "DEVICE";
        constexpr const char* kMidiTab                 = "MIDI";
        constexpr const char* kMidiAndDeviceTab        = "MIDI & DEVICE";
        constexpr const char* kAudioTab                = "AUDIO";
        constexpr const char* kPatchSection            = "PATCH";
        constexpr const char* kPatchMutatorSection     = "PATCH MUTATOR";
        constexpr const char* kMasterSection           = "MASTER";
        constexpr const char* kSupportTab              = "SUPPORT";
        constexpr const char* kDiagnosticLoggingLabel  = "DIAGNOSTIC LOGGING";
        constexpr const char* kOpenLogFolderButton     = "OPEN LOG FOLDER";
        constexpr const char* kOff                     = "OFF";
        constexpr const char* kOn                      = "ON";
        constexpr const char* kDriverTypeLabel         = "DRIVER TYPE";
        constexpr const char* kInputDeviceLabel        = "INPUT DEVICE";
        constexpr const char* kOutputDeviceLabel       = "OUTPUT DEVICE";
        constexpr const char* kNoDeviceSentinel        = "NONE";
        constexpr const char* kSampleRateLabel         = "SAMPLE RATE";
        constexpr const char* kBufferSizeLabel         = "BUFFER SIZE";
        constexpr const char* kInputChannelsLabel      = "INPUT CHANNELS";
        constexpr const char* kOutputChannelsLabel     = "OUTPUT CHANNELS";
        constexpr const char* kKeyboardFromLabel       = "KEYBOARD FROM";
        constexpr const char* kSynthFromLabel          = "SYNTH FROM";
        constexpr const char* kSynthToLabel            = "SYNTH TO";
        constexpr const char* kAudioFromLabel          = "AUDIO FROM";
        constexpr const char* kPlayTestToneButton      = "PLAY TEST TONE";
        constexpr const char* kInfoMessageLabel        = "INFO MESSAGE";
        constexpr const char* kContextualHelpLabel     = "CONTEXTUAL HELP";
        constexpr const char* kKeep                    = "KEEP";
        constexpr const char* kAutoClear               = "AUTO CLEAR";
        constexpr const char* kPresenceCheckLabel      = "PRESENCE CHECK";
        constexpr const char* kPresenceCheckOnceOnConnect = "ONCE ON CONNECT";
        constexpr const char* kPresenceCheckEvery5s    = "EVERY 5 SECONDS";
        constexpr const char* kPresenceCheckEvery15s   = "EVERY 15 SECONDS";
        constexpr const char* kPresenceCheckEvery30s   = "EVERY 30 SECONDS";
        constexpr const char* kEpromTypeLabel          = "EPROM TYPE";
        constexpr const char* kMatrix1000PatchesLabel  = "MATRIX-1000 PATCHES";
        constexpr const char* kComputerPatchesLabel    = "COMPUTER PATCHES";
        constexpr const char* kUnsavedStateLabel       = "UNSAVED STATE";
        constexpr const char* kDeleteWarningLabel      = "DELETE WARNING";
        constexpr const char* kShow                    = "SHOW";
        constexpr const char* kHide                    = "HIDE";
        constexpr const char* kDefragHistoryLabel      = "MUTATION HISTORY";
        constexpr const char* kDefragButton            = "DEFRAG";
        constexpr const char* kInitTemplateLabel       = "INIT TEMPLATE";
        constexpr const char* kUtilityLabel            = "UTILITY";
        constexpr const char* kSaveAsInitButton        = "SAVE AS INIT";
        constexpr const char* kDeleteButton            = "DELETE";
        constexpr const char* kLoadButton              = "LOAD";
        constexpr const char* kSaveAsButton            = "SAVE AS";
        constexpr const char* kInitButton              = "INIT";
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

        namespace ContextualHelp
        {
            constexpr const char* kPresenceCheck =
                "Periodically re-checks that the synth still answers. A Matrix-6/6R may return a full Master "
                "on each check. ONCE ON CONNECT stops re-checking after the first detection.";
        }

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
            constexpr const char* kDropRejectedNotMaster =
                "Drop rejected: not a Matrix Master file";
            constexpr const char* kDropRejectedMasterMulti =
                "Drop rejected: drop one Master file at a time";
            constexpr const char* kMasterPullFailed =
                "MASTER: Could not read Master parameters from the synth. Kept current Master Edit settings. "
                "Check SYNTH FROM.";
            constexpr const char* kRenameBeforeSave =
                "Rename patch (replace * INIT *) before SAVE / SAVE AS";
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
                "STORE writes it to the current RAM location. DISCARD abandons it and continues. "
                "CANCEL keeps editing.";
            constexpr const char* kBodySave =
                "This patch has changes that were not saved to a .syx file.\n\n"
                "SAVE writes/updates the .syx file (same name). DISCARD abandons the changes and continues. "
                "CANCEL keeps editing.";
            constexpr const char* kBodySaveAs =
                "This patch has changes that were not saved as a .syx file.\n\n"
                "SAVE AS writes a new file. DISCARD abandons the changes and continues. "
                "CANCEL keeps editing.";
            constexpr const char* kCancel  = "CANCEL";
            constexpr const char* kDiscard = "DISCARD";
            constexpr const char* kStore   = "STORE";
            constexpr const char* kSave    = "SAVE";
            constexpr const char* kSaveAs  = "SAVE AS";
        }

        namespace PatchNameReconciliation
        {
            constexpr const char* kTitle = "Patch name mismatch";
            // Internal/filename rows are drawn as label/value columns; kBody follows after a blank line.
            constexpr const char* kInternalNameLabel = "Internal name:";
            constexpr const char* kFilenameLabel = "Filename:";
            constexpr const char* kBody = "Which name should be used for this load?";
            constexpr const char* kInternal = "INTERNAL";
            constexpr const char* kFilename = "FILENAME";
            constexpr const char* kCancel   = "CANCEL";
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
            constexpr const char* kConfirm = "RESET";
            constexpr const char* kCancel  = "CANCEL";
        }

        namespace EpromTypePrompt
        {
            constexpr const char* kTitle = "DEVICE SETUP";
            constexpr const char* kBody =
                "Welcome to Matrix-Control - a modern SysEx MIDI editor for the Oberheim "
                "Matrix-1000/6/6R synthesizers.\n\n"
                "A quick setup is needed so you can use "
                "Matrix-Control optimally with your synth. Select MIDI ports and the EPROM "
                "type installed in your synth. This affects MIDI timing and future features.";
            constexpr const char* kBodySuggestionSuffix =
                "\n\nA suggestion is preselected from the reported firmware version when possible.";
            constexpr const char* kMidiFromLabel = "MIDI FROM";
            constexpr const char* kMidiToLabel = "MIDI TO";
            constexpr const char* kSearching = "SEARCHING";
            constexpr const char* kEpromTypeLabel = "EPROM TYPE";
            constexpr const char* kConfirm = "CONFIRM";
            constexpr const char* kSpecifyLater = "SPECIFY LATER";
        }

        // GETTING STARTED wizard (frozen copy: _bmad-output/specs/spec-getting-started/ui-copy.md).
        // ASCII only: dash and colon follow _bmad/custom/ascii-display-strings.md.
        namespace GettingStarted
        {
            constexpr const char* kTitleIntro = "GETTING STARTED";
            constexpr const char* kTitleUserInterface = "GETTING STARTED | STEP 1: USER INTERFACE";
            constexpr const char* kTitleSynthCommunication =
                "GETTING STARTED | STEP 2: SYNTH COMMUNICATION";
            constexpr const char* kTitleMidiKeyboard = "GETTING STARTED | STEP 3: MIDI KEYBOARD";
            constexpr const char* kTitleAudio = "GETTING STARTED | STEP 4: AUDIO";

            constexpr const char* kBodyIntro =
                "Welcome to Matrix-Control, a modern SysEx editor for the Oberheim "
                "Matrix-1000, 6, and 6R synthesizers.\n\n"
                "During this setup, we'll set appearance, MIDI connection, optional keyboard "
                "input, and audio monitoring (standalone application only) so you can edit, "
                "play, and hear your synth.\n\n"
                "Continue, or choose Configure later and finish in Settings.";
            constexpr const char* kBodyUserInterface =
                "Start with UI scale and skin so the next steps stay readable on your screen. "
                "You can change these anytime from the logo menu or Settings.";
            constexpr const char* kBodySynthCommunication =
                "Select the MIDI ports wired to your synth and the EPROM type installed in it. "
                "Wait until the device is recognized when possible - this unlocks reliable "
                "editing and timing. "
                "Matrix-Control re-checks the link periodically; you can change that later in "
                "Settings > MIDI & DEVICE > PRESENCE CHECK.";
            constexpr const char* kBodySynthCommunicationSuggestionSuffix =
                "\n\nA suggestion is preselected from the reported firmware version when possible.";
            constexpr const char* kBodyMidiKeyboardStandalone =
                "If you use a separate MIDI keyboard, choose it here. Matrix-6 owners who play "
                "the built-in keys can skip this step.";
            constexpr const char* kBodyMidiKeyboardPlugin =
                "When Matrix-Control runs as a plugin, MIDI notes come from the host. Route your "
                "master keyboard on a DAW track (or MIDI input) to Matrix-Control - not in this "
                "window. See the user manual for host examples.";
            constexpr const char* kBodyAudioFirstPass =
                "Choose the audio interface, channels, and AUDIO FROM so you can hear your synth in "
                "Matrix-Control. These are the same audio settings as in Settings.";
            constexpr const char* kBodyAudioResume =
                "MIDI is already set. One more step: finish audio routing so the standalone "
                "application can monitor your synth.";

            constexpr const char* kConfigureLater = "CONFIGURE LATER";
            constexpr const char* kContinue = "CONTINUE";
            constexpr const char* kPrevious = "PREVIOUS";
            constexpr const char* kNext = "NEXT";
            constexpr const char* kSkip = "SKIP";
            constexpr const char* kFinish = "FINISH";
        }

        namespace MasterGlobalInitConfirm
        {
            constexpr const char* kTitle = "RESET ALL MASTER MODULES?";
            constexpr const char* kBody =
                "This will reset MIDI, VIBRATO, and MISC to the Master init template "
                "(or built-in defaults) and send a full master SysEx to the synth.";
            constexpr const char* kConfirm = "RESET";
            constexpr const char* kCancel  = "CANCEL";
        }

        namespace MutatorHistoryDefrag
        {
            constexpr const char* kTitle = "Defrag mutation history?";
            constexpr const char* kBody =
                "Defrag will compact mutation history and preserve the current selection.\n\n"
                "DEFRAG to compact it, or CANCEL to keep the history as is.";
            constexpr const char* kConfirm = "DEFRAG";
            constexpr const char* kCancel  = "CANCEL";
        }

        namespace MutatorFlushConfirm
        {
            constexpr const char* kTitle = "Flush mutation history?";
            constexpr const char* kBody =
                "This clears the Patch Mutator history for this session.\n\n"
                "The initial patch snapshot is kept.\n\n"
                "CONTINUE to flush, or CANCEL to keep the history.";
            constexpr const char* kCancel   = "CANCEL";
            constexpr const char* kContinue = "CONTINUE";
        }

        namespace MutatorDeleteConfirm
        {
            constexpr const char* kTitle = "Delete mutation?";
            constexpr const char* kBody =
                "This removes the selected mutation or retry from Patch Mutator history.\n\n"
                "Deleting a root mutation also removes all of its retries.\n\n"
                "DELETE to remove it, or CANCEL to keep the history.";
            constexpr const char* kCancel        = "CANCEL";
            constexpr const char* kDelete        = "DELETE";
            constexpr const char* kDontAskAgain  = "Don't ask again";
        }

        namespace DeleteInitTemplateConfirm
        {
            constexpr const char* kTitle = "Delete init template?";
            constexpr const char* kBodyPatch =
                "This removes the system Patch init template (PatchInit.syx).\n\n"
                "The next Patch INIT will use the built-in defaults.\n\n"
                "DELETE to remove it, or CANCEL to keep the file.";
            constexpr const char* kBodyMaster =
                "This removes the system Master init template (MasterInit.syx).\n\n"
                "The next Master INIT will use the built-in defaults.\n\n"
                "DELETE to remove it, or CANCEL to keep the file.";
            constexpr const char* kCancel = "CANCEL";
            constexpr const char* kDelete = "DELETE";
        }

        namespace BankImportConfirm
        {
            constexpr const char* kTitle = "Import bank?";
            constexpr const char* kBody =
                "This overwrites patches on the device with files from the selected folder.\n\n"
                "CONTINUE to import, or CANCEL to keep the device unchanged.";
            constexpr const char* kCancel   = "CANCEL";
            constexpr const char* kContinue = "CONTINUE";
        }

        namespace BankPasteConfirm
        {
            constexpr const char* kTitle = "Paste bank?";
            constexpr const char* kCancel   = "CANCEL";
            constexpr const char* kContinue = "CONTINUE";

            inline juce::String formatBody(int sourceBank, int targetBank)
            {
                return "This overwrites all 100 patches in bank " + juce::String(targetBank)
                    + " with the copied bank " + juce::String(sourceBank) + ".\n\n"
                    + "CONTINUE to paste, or CANCEL to keep the device unchanged.";
            }
        }

        namespace BankExportOverwriteConfirm
        {
            constexpr const char* kTitle = "Replace export folder?";
            constexpr const char* kBody =
                "An export folder already exists at this location.\n\n"
                "CONTINUE will delete that folder completely, recreate it empty, "
                "then export a fresh copy of the bank.\n\n"
                "CANCEL leaves the existing folder untouched.";
            constexpr const char* kCancel   = "CANCEL";
            constexpr const char* kContinue = "CONTINUE";
        }

        // SAVE after loading a .m1kp when a same-stem .syx already exists in the folder.
        namespace M1kpSiblingSyxOverwriteConfirm
        {
            constexpr const char* kTitle = "Overwrite existing .syx?";
            constexpr const char* kCancel   = "CANCEL";
            constexpr const char* kContinue = "CONTINUE";

            inline juce::String formatBody(const juce::String& existingSyxFileName)
            {
                return "You loaded a .m1kp patch. SAVE will write "
                    + existingSyxFileName
                    + ", which already exists in this folder.\n\n"
                    + "CONTINUE replaces that .syx file. The original .m1kp stays unchanged.\n\n"
                    + "CANCEL keeps editing without saving.";
            }
        }

        // Settings LOAD of a legacy .m1km master — Groups/cascade policy before commit.
        namespace MasterM1kmLoadChoice
        {
            constexpr const char* kTitle = "Load .m1km Master?";
            constexpr const char* kBody =
                "This .m1km Master file may include Groups and cascade data. "
                "Matrix-Control does not edit those yet, but the Matrix-1000 still uses them.\n\n"
                "Choose whether to load MASTER SETTINGS ONLY (Groups/cascade reset) or the "
                "FULL MASTER (Groups/cascade kept).\n\n"
                "CANCEL leaves the current Master unchanged.";
            constexpr const char* kMasterSettingsOnly = "MASTER SETTINGS ONLY";
            constexpr const char* kFullMaster = "FULL MASTER";
            constexpr const char* kCancel = "CANCEL";
        }

        namespace BankTransferProgress
        {
            constexpr const char* kExportTitle = "EXPORTING BANK TO DISK";
            constexpr const char* kImportTitle = "IMPORTING BANK";
            constexpr const char* kCopyTitle   = "COPYING BANK";
            constexpr const char* kPasteTitle  = "PASTING BANK";
            constexpr const char* kCancel      = "CANCEL";
            constexpr const char* kDestinationFolderLabel = "Destination folder:";
            constexpr const char* kSourceFolderLabel = "Source folder:";
            constexpr const char* kDestinationLabel = "Destination:";
            constexpr const char* kSourceLabel = "Source:";
            constexpr const char* kClipboardLabel = "Clipboard";

            inline juce::String formatExportProgressMessage(int bank)
            {
                return "Exporting bank " + juce::String(bank) + ":";
            }

            inline juce::String formatExportProgressMessageNoBank()
            {
                return "Exporting patches:";
            }

            inline juce::String formatCopyProgressMessage(int bank)
            {
                return "Reading source bank " + juce::String(bank) + ":";
            }

            inline juce::String formatPasteSafetyCopyMessage(int destinationBank)
            {
                return "Saving safety copy of destination bank " + juce::String(destinationBank) + ":";
            }

            inline juce::String formatPasteWritingMessage(int destinationBank)
            {
                return "Writing clipboard into destination bank " + juce::String(destinationBank) + ":";
            }

            inline juce::String formatPasteRestoringMessage(int destinationBank)
            {
                return "Cancelling - restoring destination bank " + juce::String(destinationBank) + ":";
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
            constexpr const char* kBlack = "BLACK/GREEN";
            constexpr const char* kBlackChroma = "BLACK/CHROMA";
            constexpr const char* kBlackMinimal = "BLACK/MINIMAL";
            constexpr const char* kCream = "CREAM/GREEN";
            constexpr const char* kCreamChroma = "CREAM/CHROMA";
            constexpr const char* kCreamMinimal = "CREAM/MINIMAL";
            constexpr const char* kTonesCherry = "TONES/CHERRY";
            constexpr const char* kTonesCoral = "TONES/CORAL";
            constexpr const char* kTonesSaffron = "TONES/SAFFRON";
            constexpr const char* kTonesOchre = "TONES/OCHRE";
            constexpr const char* kTonesGold = "TONES/GOLD";
            constexpr const char* kTonesOlive = "TONES/OLIVE";
            constexpr const char* kTonesLime = "TONES/LIME";
            constexpr const char* kTonesEmerald = "TONES/EMERALD";
            constexpr const char* kTonesKigee = "TONES/KIGEE";
            constexpr const char* kTonesPetrol = "TONES/PETROL";
            constexpr const char* kTonesAzur = "TONES/AZUR";
            constexpr const char* kTonesMarine = "TONES/MARINE";
            constexpr const char* kTonesPurple = "TONES/PURPLE";
            constexpr const char* kTonesIndigo = "TONES/INDIGO";
            constexpr const char* kTonesFuchsia = "TONES/FUCHSIA";
        }

        namespace SkinFamilies
        {
            constexpr const char* kBlack = "BLACK";
            constexpr const char* kCream = "CREAM";
            constexpr const char* kTones = "TONES";
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
            constexpr const char* kNormal = "NORMAL";
            constexpr const char* kLfo1   = "LFO 1";
            constexpr const char* kGLfo1  = "G-LFO 1";
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

        namespace ContextualHelp
        {
            constexpr const char* kSectionTitle =
                "MASTER EDIT: Edits global settings for all patches (Matrix-1000 only).";
        }

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
                constexpr const char* kTitle =
                    "MIDI: Global MIDI channel and related controls for the synthesizer.";
                constexpr const char* kInit =
                    "MIDI: I resets this module to its init values.";
                constexpr const char* kChannel =
                    "MIDI: CHANNEL sets the synthesizer MIDI channel.";
                constexpr const char* kMidiEcho =
                    "MIDI: MIDI ECHO is the echoes incoming MIDI back out when enabled.";
                constexpr const char* kControllers =
                    "MIDI: CONTROLLERS enables or disables MIDI controller reception.";
                constexpr const char* kPatchChanges =
                    "MIDI: PATCH CHANGES enables or disables MIDI program changes.";
                constexpr const char* kPedal1Select =
                    "MIDI: PEDAL 1 SELECT is the assigns the function for pedal 1.";
                constexpr const char* kPedal2Select =
                    "MIDI: PEDAL 2 SELECT is the assigns the function for pedal 2.";
                constexpr const char* kLever2Select =
                    "MIDI: LEVER 2 SELECT is the assigns the function for lever 2.";
                constexpr const char* kLever3Select =
                    "MIDI: LEVER 3 SELECT is the assigns the function for lever 3.";
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
                constexpr const char* kTitle =
                    "VIBRATO: Global vibrato controls for the synthesizer.";
                constexpr const char* kInit =
                    "VIBRATO: I resets this module to its init values.";
                constexpr const char* kSpeed =
                    "VIBRATO: SPEED sets global vibrato rate.";
                constexpr const char* kWaveform =
                    "VIBRATO: WAVEFORM chooses the vibrato waveform.";
                constexpr const char* kAmplitude =
                    "VIBRATO: AMPLITUDE sets global vibrato depth.";
                constexpr const char* kSpeedModSource =
                    "VIBRATO: SPEED MOD SOURCE selects what modulates vibrato speed.";
                constexpr const char* kSpeedModAmount =
                    "VIBRATO: SPEED MOD AMOUNT sets how much the speed mod source affects vibrato rate.";
                constexpr const char* kAmpModSource =
                    "VIBRATO: AMP MOD SOURCE selects what modulates vibrato depth.";
                constexpr const char* kAmpModAmount =
                    "VIBRATO: AMP MOD AMOUNT sets how much the amp mod source affects vibrato depth.";
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
                constexpr const char* kUnisonDetune        = "UNISON DETUNE";
                constexpr const char* kVolumeInvertEnable  = "VOLUME INVERT";
                constexpr const char* kBankLockEnable      = "BANK LOCK";
                constexpr const char* kMemoryProtectEnable = "MEMORY PROTECT";
            }

            constexpr const char* kUnisonDetuneOptimisedEpromFooter =
                "UNISON DETUNE requires an optimised EPROM (GLIGLI, TAUNTEK, or UNTERGEEK).";

            namespace ContextualHelp
            {
                constexpr const char* kTitle =
                    "MISC: Global bend, pressure, tuning, memory protect, plus global Unison and Unison Detune.";
                constexpr const char* kInit =
                    "MISC: I resets this module to its init values.";
                constexpr const char* kMasterTune =
                    "MISC: MASTER TUNE fine-tunes overall instrument pitch.";
                constexpr const char* kMasterTranspose =
                    "MISC: MASTER TRANSPOSE transposes the instrument in semitones.";
                constexpr const char* kBendRange =
                    "MISC: BEND RANGE (+/-) sets pitch-bend range in semitones up and down.";
                constexpr const char* kUnison =
                    "MISC: UNISON enables Master Unison (can override Patch keyboard mode).";
                constexpr const char* kUnisonDetune =
                    "MISC: UNISON DETUNE sets Unison Detune depth (optimised EPROM / MIDI CC 94).";
                constexpr const char* kVolumeInvert =
                    "MISC: VOLUME INVERT inverts volume pedal response when enabled.";
                constexpr const char* kBankLock =
                    "MISC: BANK LOCK locks bank changes on the hardware.";
                constexpr const char* kMemoryProtect =
                    "MISC: MEMORY PROTECT protects synth memory from being overwritten.";
            }
        }
    }

    namespace PatchEditSection
    {
        constexpr const char* kName = "PATCH EDIT";

        namespace ContextualHelp
        {
            constexpr const char* kSectionTitle =
                "PATCH EDIT: Edits the current patch and shows envelopes, Track Generator, and patch name.";
        }

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
                constexpr const char* kTitle =
                    "DCO 1: Oscillator 1 controls for the current patch.";
                constexpr const char* kInit =
                    "DCO 1: I resets this module to its init values.";
                constexpr const char* kCopy =
                    "DCO 1: C copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "DCO 1: P pastes compatible clipboard data into this module.";
                constexpr const char* kFrequency =
                    "DCO 1: FREQUENCY sets DCO 1 pitch in semitones.";
                constexpr const char* kFrequencyModByLfo1 =
                    "DCO 1: FREQ < LFO 1 sets how much LFO 1 modulates DCO 1 frequency.";
                constexpr const char* kSync =
                    "DCO 1: SYNC hard-syncs DCO 1 to DCO 2 when enabled.";
                constexpr const char* kPulseWidth =
                    "DCO 1: PULSE WIDTH sets pulse width for pulse / combination waves.";
                constexpr const char* kPulseWidthModByLfo2 =
                    "DCO 1: PW < LFO 2 sets how much LFO 2 modulates pulse width.";
                constexpr const char* kWaveShape =
                    "DCO 1: WAVE SHAPE mixes wave components for the selected wave family.";
                constexpr const char* kWaveSelect =
                    "DCO 1: WAVE SELECT chooses the DCO 1 waveform family.";
                constexpr const char* kLevers =
                    "DCO 1: LEVERS enables lever modulation for this DCO.";
                constexpr const char* kKeyboardPortamento =
                    "DCO 1: KEYBD/PORTA tracks keyboard / portamento for this DCO.";
                constexpr const char* kKeyClick =
                    "DCO 1: KEY CLICK adds a short click on key-on for this DCO.";
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
                constexpr const char* kTitle =
                    "DCO 2: Oscillator 2 controls for the current patch.";
                constexpr const char* kInit =
                    "DCO 2: I resets this module to its init values.";
                constexpr const char* kCopy =
                    "DCO 2: C copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "DCO 2: P pastes compatible clipboard data into this module.";
                constexpr const char* kFrequency =
                    "DCO 2: FREQUENCY sets DCO 2 pitch in semitones.";
                constexpr const char* kFrequencyModByLfo1 =
                    "DCO 2: FREQ < LFO 1 sets how much LFO 1 modulates DCO 2 frequency.";
                constexpr const char* kDetune =
                    "DCO 2: DETUNE fine-tunes DCO 2 relative to DCO 1.";
                constexpr const char* kPulseWidth =
                    "DCO 2: PULSE WIDTH sets pulse width for pulse / combination waves.";
                constexpr const char* kPulseWidthModByLfo2 =
                    "DCO 2: PW < LFO 2 sets how much LFO 2 modulates pulse width.";
                constexpr const char* kWaveShape =
                    "DCO 2: WAVE SHAPE mixes wave components for the selected wave family.";
                constexpr const char* kWaveSelect =
                    "DCO 2: WAVE SELECT chooses the DCO 2 waveform family (includes Noise).";
                constexpr const char* kLevers =
                    "DCO 2: LEVERS enables lever modulation for this DCO.";
                constexpr const char* kKeyboardPortamento =
                    "DCO 2: KEYBD/PORTA tracks keyboard / portamento for this DCO.";
                constexpr const char* kKeyClick =
                    "DCO 2: KEY CLICK adds a short click on key-on for this DCO.";
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
                constexpr const char* kTitle =
                    "VCF/VCA: Filter and amplifier controls for the current patch.";
                constexpr const char* kInit =
                    "VCF/VCA: I resets this module to its init values.";
                constexpr const char* kBalance =
                    "VCF/VCA: DCO 2 | DCO 1 MIX balances DCO 1 and DCO 2 into the filter stage.";
                constexpr const char* kFrequency =
                    "VCF/VCA: FREQUENCY sets the filter cutoff frequency.";
                constexpr const char* kFrequencyModByEnv1 =
                    "VCF/VCA: FREQ < ENV 1 sets how much ENV 1 modulates filter cutoff frequency.";
                constexpr const char* kFrequencyModByPressure =
                    "VCF/VCA: FREQ < PRESSURE sets how much pressure modulates filter cutoff frequency.";
                constexpr const char* kResonance =
                    "VCF/VCA: RESONANCE sets filter resonance (emphasis at cutoff frequency).";
                constexpr const char* kVca1Volume =
                    "VCF/VCA: VCA 1 VOLUME sets VCA 1 overall volume.";
                constexpr const char* kVca1ModByVelocity =
                    "VCF/VCA: VCA 1 < VELOCITY sets how much velocity modulates VCA 1.";
                constexpr const char* kVca2ModByEnv2 =
                    "VCF/VCA: VCA 2 < ENV 2 sets how much ENV 2 modulates VCA 2.";
                constexpr const char* kLevers =
                    "VCF/VCA: LEVERS enables lever modulation for filter / amplifier.";
                constexpr const char* kKeyboardPortamento =
                    "VCF/VCA: KEYBD/PORTA tracks keyboard / portamento for the filter.";
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
                constexpr const char* kTitle =
                    "FM/TRACK: Frequency modulation and Track Generator controls for the current patch.";
                constexpr const char* kInit =
                    "FM/TRACK: I resets this module to its init values.";
                constexpr const char* kFmAmount =
                    "FM/TRACK: VCF FM AMOUNT sets FM amount into the VCF from DCO 2.";
                constexpr const char* kFmModByEnv3 =
                    "FM/TRACK: FM < ENV 3 sets how much ENV 3 modulates FM amount.";
                constexpr const char* kFmModByPressure =
                    "FM/TRACK: FM < PRESSURE sets how much pressure modulates FM amount.";
                constexpr const char* kTrackPoint1 =
                    "FM/TRACK: TRACK POINT 1 sets Track Generator point 1 level.";
                constexpr const char* kTrackPoint2 =
                    "FM/TRACK: TRACK POINT 2 sets Track Generator point 2 level.";
                constexpr const char* kTrackPoint3 =
                    "FM/TRACK: TRACK POINT 3 sets Track Generator point 3 level.";
                constexpr const char* kTrackPoint4 =
                    "FM/TRACK: TRACK POINT 4 sets Track Generator point 4 level.";
                constexpr const char* kTrackPoint5 =
                    "FM/TRACK: TRACK POINT 5 sets Track Generator point 5 level.";
                constexpr const char* kTrackInput =
                    "FM/TRACK: TRACK INPUT selects the source that drives the Track Generator.";
                constexpr const char* kTrackGeneratorDisplay =
                    "FM/TRACK: TRACK GENERATOR lets you drag the Track Generator curve points for tracking shape.";
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
                constexpr const char* kTitle =
                    "RAMP/PORTAMENTO: Ramp generator and portamento controls for the current patch.";
                constexpr const char* kInit =
                    "RAMP/PORTAMENTO: I resets this module to its init values.";
                constexpr const char* kRamp1Rate =
                    "RAMP/PORTAMENTO: RAMP 1 RATE sets Ramp 1 rise time.";
                constexpr const char* kRamp1Trigger =
                    "RAMP/PORTAMENTO: RAMP 1 TRIGGER chooses what starts Ramp 1.";
                constexpr const char* kRamp2Rate =
                    "RAMP/PORTAMENTO: RAMP 2 RATE sets Ramp 2 rise time.";
                constexpr const char* kRamp2Trigger =
                    "RAMP/PORTAMENTO: RAMP 2 TRIGGER chooses what starts Ramp 2.";
                constexpr const char* kPortamentoRate =
                    "RAMP/PORTAMENTO: PORTAMENTO RATE sets glide time between notes.";
                constexpr const char* kPortamentoModByVelocity =
                    "RAMP/PORTAMENTO: PORTA < VELOCITY sets how much velocity scales portamento rate.";
                constexpr const char* kPortamentoMode =
                    "RAMP/PORTAMENTO: PORTA MODE chooses how portamento glides between notes.";
                constexpr const char* kPortamentoLegato =
                    "RAMP/PORTAMENTO: LEGATO PORTA enables legato-only portamento (Unison keyboard modes).";
                constexpr const char* kPortamentoKeyboardMode =
                    "RAMP/PORTAMENTO: KEYBOARD MODE chooses poly / unison / related keyboard response.";
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
                constexpr const char* kTitle =
                    "ENV 1: Envelope 1 controls for the current patch.";
                constexpr const char* kInit =
                    "ENV 1: I resets this module to its init values.";
                constexpr const char* kCopy =
                    "ENV 1: C copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "ENV 1: P pastes compatible clipboard data into this module.";
                constexpr const char* kDelay =
                    "ENV 1: DELAY sets the delay before the envelope attack starts.";
                constexpr const char* kAttack =
                    "ENV 1: ATTACK sets the time to rise from zero to peak.";
                constexpr const char* kDecay =
                    "ENV 1: DECAY sets the time to fall from peak to sustain.";
                constexpr const char* kSustain =
                    "ENV 1: SUSTAIN sets the level held while the key is down.";
                constexpr const char* kRelease =
                    "ENV 1: RELEASE sets the time to fall to zero after key-up.";
                constexpr const char* kAmplitude =
                    "ENV 1: AMPLITUDE sets overall envelope depth.";
                constexpr const char* kAmplitudeModByVelocity =
                    "ENV 1: AMP < VELOCITY sets how much velocity scales envelope amplitude.";
                constexpr const char* kTriggerMode =
                    "ENV 1: TRIGGER MODE chooses how the envelope is triggered.";
                constexpr const char* kEnvelopeMode =
                    "ENV 1: ENVELOPE MODE chooses envelope contour behaviour.";
                constexpr const char* kLfo1Trigger =
                    "ENV 1: LFO 1 TRIGGER lets LFO 1 retrigger this envelope when enabled.";
                constexpr const char* kEnvelopeDisplay =
                    "ENV 1: ENVELOPE lets you drag Delay, Attack, Decay, Sustain, and Release on the curve.";
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
                constexpr const char* kTitle =
                    "ENV 2: Envelope 2 controls for the current patch.";
                constexpr const char* kInit =
                    "ENV 2: I resets this module to its init values.";
                constexpr const char* kCopy =
                    "ENV 2: C copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "ENV 2: P pastes compatible clipboard data into this module.";
                constexpr const char* kDelay =
                    "ENV 2: DELAY sets the delay before the envelope attack starts.";
                constexpr const char* kAttack =
                    "ENV 2: ATTACK sets the time to rise from zero to peak.";
                constexpr const char* kDecay =
                    "ENV 2: DECAY sets the time to fall from peak to sustain.";
                constexpr const char* kSustain =
                    "ENV 2: SUSTAIN sets the level held while the key is down.";
                constexpr const char* kRelease =
                    "ENV 2: RELEASE sets the time to fall to zero after key-up.";
                constexpr const char* kAmplitude =
                    "ENV 2: AMPLITUDE sets overall envelope depth.";
                constexpr const char* kAmplitudeModByVelocity =
                    "ENV 2: AMP < VELOCITY sets how much velocity scales envelope amplitude.";
                constexpr const char* kTriggerMode =
                    "ENV 2: TRIGGER MODE chooses how the envelope is triggered.";
                constexpr const char* kEnvelopeMode =
                    "ENV 2: ENVELOPE MODE chooses envelope contour behaviour.";
                constexpr const char* kLfo1Trigger =
                    "ENV 2: LFO 1 TRIGGER lets LFO 1 retrigger this envelope when enabled.";
                constexpr const char* kEnvelopeDisplay =
                    "ENV 2: ENVELOPE lets you drag Delay, Attack, Decay, Sustain, and Release on the curve.";
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
                constexpr const char* kTitle =
                    "ENV 3: Envelope 3 controls for the current patch.";
                constexpr const char* kInit =
                    "ENV 3: I resets this module to its init values.";
                constexpr const char* kCopy =
                    "ENV 3: C copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "ENV 3: P pastes compatible clipboard data into this module.";
                constexpr const char* kDelay =
                    "ENV 3: DELAY sets the delay before the envelope attack starts.";
                constexpr const char* kAttack =
                    "ENV 3: ATTACK sets the time to rise from zero to peak.";
                constexpr const char* kDecay =
                    "ENV 3: DECAY sets the time to fall from peak to sustain.";
                constexpr const char* kSustain =
                    "ENV 3: SUSTAIN sets the level held while the key is down.";
                constexpr const char* kRelease =
                    "ENV 3: RELEASE sets the time to fall to zero after key-up.";
                constexpr const char* kAmplitude =
                    "ENV 3: AMPLITUDE sets overall envelope depth.";
                constexpr const char* kAmplitudeModByVelocity =
                    "ENV 3: AMP < VELOCITY sets how much velocity scales envelope amplitude.";
                constexpr const char* kTriggerMode =
                    "ENV 3: TRIGGER MODE chooses how the envelope is triggered.";
                constexpr const char* kEnvelopeMode =
                    "ENV 3: ENVELOPE MODE chooses envelope contour behaviour.";
                constexpr const char* kLfo1Trigger =
                    "ENV 3: LFO 1 TRIGGER lets LFO 1 retrigger this envelope when enabled.";
                constexpr const char* kEnvelopeDisplay =
                    "ENV 3: ENVELOPE lets you drag Delay, Attack, Decay, Sustain, and Release on the curve.";
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
                constexpr const char* kTitle =
                    "LFO 1: Low-frequency oscillator 1 controls for the current patch.";
                constexpr const char* kInit =
                    "LFO 1: I resets this module to its init values.";
                constexpr const char* kCopy =
                    "LFO 1: C copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "LFO 1: P pastes compatible clipboard data into this module.";
                constexpr const char* kSpeed =
                    "LFO 1: SPEED sets LFO 1 rate.";
                constexpr const char* kSpeedModByPressure =
                    "LFO 1: SPEED < PRESSURE sets how much pressure modulates LFO 1 speed.";
                constexpr const char* kRetriggerPoint =
                    "LFO 1: RETRIGGER POINT sets the phase point used when the LFO retriggers.";
                constexpr const char* kAmplitude =
                    "LFO 1: AMPLITUDE sets LFO 1 depth.";
                constexpr const char* kAmplitudeModByRamp1 =
                    "LFO 1: AMP < RAMP 1 sets how much Ramp 1 scales LFO 1 amplitude.";
                constexpr const char* kWaveform =
                    "LFO 1: WAVEFORM chooses the LFO 1 waveform.";
                constexpr const char* kTriggerMode =
                    "LFO 1: TRIGGER MODE chooses how LFO 1 starts and retriggers.";
                constexpr const char* kLag =
                    "LFO 1: LAG smooths LFO 1 output changes.";
                constexpr const char* kSampleInput =
                    "LFO 1: SAMPLE INPUT chooses which modulation source the LFO samples when WAVEFORM is SAMPLED.";
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
                constexpr const char* kTitle =
                    "LFO 2: Low-frequency oscillator 2 controls for the current patch.";
                constexpr const char* kInit =
                    "LFO 2: I resets this module to its init values.";
                constexpr const char* kCopy =
                    "LFO 2: C copies this module to the clipboard.";
                constexpr const char* kPaste =
                    "LFO 2: P pastes compatible clipboard data into this module.";
                constexpr const char* kSpeed =
                    "LFO 2: SPEED sets LFO 2 rate.";
                constexpr const char* kSpeedModByKeyboard =
                    "LFO 2: SPEED < KEYBD sets how much keyboard tracking modulates LFO 2 speed.";
                constexpr const char* kRetriggerPoint =
                    "LFO 2: RETRIGGER POINT sets the phase point used when the LFO retriggers.";
                constexpr const char* kAmplitude =
                    "LFO 2: AMPLITUDE sets LFO 2 depth.";
                constexpr const char* kAmplitudeModByRamp2 =
                    "LFO 2: AMP < RAMP 2 sets how much Ramp 2 scales LFO 2 amplitude.";
                constexpr const char* kWaveform =
                    "LFO 2: WAVEFORM chooses the LFO 2 waveform.";
                constexpr const char* kTriggerMode =
                    "LFO 2: TRIGGER MODE chooses how LFO 2 starts and retriggers.";
                constexpr const char* kLag =
                    "LFO 2: LAG smooths LFO 2 output changes.";
                constexpr const char* kSampleInput =
                    "LFO 2: SAMPLE INPUT chooses which modulation source the LFO samples when WAVEFORM is SAMPLED.";
            }
        }

        namespace PatchNameModule
        {
            constexpr const char* kName = "PATCH NAME";
            // Temporary header title while a valid Patch file or selection is dragged over the editor.
            constexpr const char* kLoadPatchName = "LOAD PATCH";
            // Temporary header title while a valid Master file is dragged over the editor.
            constexpr const char* kLoadMasterName = "LOAD MASTER";

            namespace StandaloneWidgets
            {
                // Patch names are 8 characters long in the Oberheim Matrix-1000:
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
                constexpr const char* kTitle = kPatchNameDisplay;
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
                // Fixed Master drag primary — Master has no 8-char name field.
                constexpr const char* kMasterPrimary = "MASTER";
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
            constexpr const char* kSectionTitle =
                "MATRIX MODULATION: Routes modulation sources to destinations across ten buses.";
            constexpr const char* kColumnHeader =
                "MATRIX MODULATION: # / SOURCE / AMOUNT / DESTINATION is the column guide for bus number, source,"
                " amount, and destination. Drag a bus number to reorder.";
            constexpr const char* kSectionInit =
                "MATRIX MODULATION: I resets all modulation buses to init.";
            constexpr const char* kSectionCopy =
                "MATRIX MODULATION: C copies all modulation buses to the clipboard.";
            constexpr const char* kSectionPaste =
                "MATRIX MODULATION: P pastes compatible clipboard data into all modulation buses.";
            constexpr const char* kBusHandle =
                "MATRIX MODULATION: # lets you drag to reorder this modulation bus.";
            constexpr const char* kBusInit =
                "MATRIX MODULATION: I resets this bus source, amount, and destination.";
            constexpr const char* kSource =
                "MATRIX MODULATION: SOURCE selects the modulation source for this bus.";
            constexpr const char* kAmount =
                "MATRIX MODULATION: AMOUNT sets how strongly this bus modulates its destination.";
            constexpr const char* kDestination =
                "MATRIX MODULATION: DESTINATION selects what this bus modulates.";
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

        namespace ContextualHelp
        {
            constexpr const char* kSectionTitle =
                "PATCH MANAGER: Navigates banks and patches, stores to memory or disk, imports or exports banks, and inits, copies, or pastes patches.";
        }

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
            constexpr const char* kImportingReadingMessage = "Reading bank safety copy from device:";
            constexpr const char* kImportingWritingMessage = "Writing patches to the device:";
            constexpr const char* kImportingRestoringMessage = "Cancelling - restoring device:";
            constexpr const char* kPastingWritingMessage = "Writing clipboard into destination bank:";

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
                        return "Bank " + juce::String(bank) + " exported successfully: " + savedClause;

                    return "Patches exported successfully: " + savedClause;
                }

                inline juce::String formatImportSuccess(int found, int valid, int imported)
                {
                    return "Import complete - found " + juce::String(found)
                        + ", valid " + juce::String(valid)
                        + ", imported " + juce::String(imported);
                }

                inline juce::String formatImportNoValidFiles(int found)
                {
                    return "No valid patch files to import (found " + juce::String(found) + ")";
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
                constexpr const char* kTitle =
                    "BANK UTILITY: Selects a bank and copies, pastes, imports, or exports it (Matrix-1000 only).";
                constexpr const char* kBank0 =
                    "BANK UTILITY: 0 selects Matrix-1000 bank 0 (RAM) and sets it as the copy/paste/import/export target.";
                constexpr const char* kBank1 =
                    "BANK UTILITY: 1 selects Matrix-1000 bank 1 (RAM) and sets it as the copy/paste/import/export target.";
                constexpr const char* kBank2 =
                    "BANK UTILITY: 2 selects Matrix-1000 bank 2 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kBank3 =
                    "BANK UTILITY: 3 selects Matrix-1000 bank 3 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kBank4 =
                    "BANK UTILITY: 4 selects Matrix-1000 bank 4 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kBank5 =
                    "BANK UTILITY: 5 selects Matrix-1000 bank 5 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kBank6 =
                    "BANK UTILITY: 6 selects Matrix-1000 bank 6 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kBank7 =
                    "BANK UTILITY: 7 selects Matrix-1000 bank 7 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kBank8 =
                    "BANK UTILITY: 8 selects Matrix-1000 bank 8 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kBank9 =
                    "BANK UTILITY: 9 selects Matrix-1000 bank 9 (ROM) and sets it as the copy/export target"
                    " (paste/import need RAM 0-1).";
                constexpr const char* kCopy =
                    "BANK UTILITY: COPY copies the selected bank from the synthesizer to the clipboard.";
                constexpr const char* kPaste =
                    "BANK UTILITY: PASTE pastes the clipboard bank into the selected destination bank.";
                constexpr const char* kImport =
                    "BANK UTILITY: IMPORT imports patch files into the selected RAM bank.";
                constexpr const char* kExport =
                    "BANK UTILITY: EXPORT exports the selected bank as SysEx files on disk.";
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
                    "INTERNAL PATCHES: Click the title to focus this module and navigate synthesizer memory (banks and patches).";
                constexpr const char* kBrowser =
                    "INTERNAL PATCHES: BROWSER is the bank and patch number browser for synthesizer memory.";
                constexpr const char* kMemory =
                    "INTERNAL PATCHES: MEMORY groups init, copy, paste, and store for the selected memory slot.";
                constexpr const char* kPrevious =
                    "INTERNAL PATCHES: < loads the previous patch in the current bank.";
                constexpr const char* kNext =
                    "INTERNAL PATCHES: > loads the next patch in the current bank.";
                constexpr const char* kCurrentBank =
                    "INTERNAL PATCHES: CURRENT BANK selects the current synthesizer bank.";
                constexpr const char* kCurrentPatch =
                    "INTERNAL PATCHES: CURRENT PATCH selects the current patch number in the bank.";
                constexpr const char* kInit =
                    "INTERNAL PATCHES: INIT loads the init patch template into the editor and sends it to the"
                    " synthesizer.";
                constexpr const char* kCopy =
                    "INTERNAL PATCHES: COPY copies the current patch to the clipboard.";
                constexpr const char* kPaste =
                    "INTERNAL PATCHES: PASTE pastes a clipboard patch into the editor and sends it to the synthesizer.";
                constexpr const char* kStore =
                    "INTERNAL PATCHES: STORE stores the current patch into the selected synthesizer memory slot.";
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
                                                               bool usedFilename,
                                                               bool m1kpForcedFilename = false)
                {
                    if (usedFilename && m1kpForcedFilename)
                        return "Loaded " + location + " (.m1kp always uses filename)";

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
                    "Drop rejected: no valid Matrix patch file";
                constexpr const char* kDropRejectedNotSyx =
                    "Drop rejected: not a Matrix patch file";
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
                    "COMPUTER PATCHES: Click the title to focus this module and navigate .syx / .m1kp patch files on disk.";
                constexpr const char* kBrowser =
                    "COMPUTER PATCHES: BROWSER is the folder browser for patch files on disk.";
                constexpr const char* kStorage =
                    "COMPUTER PATCHES: STORAGE opens a folder and saves the current patch as a .syx file.";
                constexpr const char* kPrevious =
                    "COMPUTER PATCHES: < loads the previous patch file in the folder.";
                constexpr const char* kNext =
                    "COMPUTER PATCHES: > loads the next patch file in the folder.";
                constexpr const char* kSelectPatch =
                    "COMPUTER PATCHES: SELECT A PATCH chooses which patch file is selected.";
                constexpr const char* kOpen =
                    "COMPUTER PATCHES: OPEN opens a folder of .syx / .m1kp patch files.";
                constexpr const char* kSaveAs =
                    "COMPUTER PATCHES: SAVE AS saves the current patch as a new .syx file.";
                constexpr const char* kSave =
                    "COMPUTER PATCHES: SAVE is the saves/updates the .syx file (same name).";
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
                constexpr const char* kTitle =
                    "PATCH MUTATOR: Mutates the current patch and browses mutation history.";
                constexpr const char* kMode =
                    "PATCH MUTATOR: MODE sets how far mutations stray - Kindred, Drift, Warp, or Wild.";
                constexpr const char* kPitch =
                    "PATCH MUTATOR: PITCH controls how DCO pitch may move - Keep, Consonant, Dissonant, or Free.";
                constexpr const char* kHistory =
                    "PATCH MUTATOR: HISTORY recalls a mutation or retry from this session.";
                constexpr const char* kMutate =
                    "PATCH MUTATOR: MUTATE creates a new variation from the current recipe and sends it to the"
                    " synthesizer.";
                constexpr const char* kRetry =
                    "PATCH MUTATOR: RETRY rolls again from the same mutation root.";
                constexpr const char* kHistoryPrevious =
                    "PATCH MUTATOR: < steps backward through session history.";
                constexpr const char* kHistoryNext =
                    "PATCH MUTATOR: > steps forward through session history.";
                constexpr const char* kCompare =
                    "PATCH MUTATOR: C compares with the origin patch and locks editing until you click [C] button again.";
                constexpr const char* kDelete =
                    "PATCH MUTATOR: D deletes the selected history entry.";
                constexpr const char* kFlush =
                    "PATCH MUTATOR: F flushes the whole session mutation history.";
                constexpr const char* kExport =
                    "PATCH MUTATOR: E exports the session mutations as SysEx files.";
                constexpr const char* kEnableDco1 =
                    "PATCH MUTATOR: D1 includes DCO 1 module in the mutation recipe.";
                constexpr const char* kEnableDco2 =
                    "PATCH MUTATOR: D2 includes DCO 2 module in the mutation recipe.";
                constexpr const char* kEnableVcfVca =
                    "PATCH MUTATOR: F/A includes VCF/VCA module in the recipe.";
                constexpr const char* kEnableFmTrack =
                    "PATCH MUTATOR: F/T includes FM/TRACK module in the recipe.";
                constexpr const char* kEnableRampPortamento =
                    "PATCH MUTATOR: R/P includes RAMP/PORTAMENTO module in the recipe.";
                constexpr const char* kEnableEnvelope1 =
                    "PATCH MUTATOR: E1 includes ENV 1 module in the recipe.";
                constexpr const char* kEnableEnvelope2 =
                    "PATCH MUTATOR: E2 includes ENV 2 module in the recipe.";
                constexpr const char* kEnableEnvelope3 =
                    "PATCH MUTATOR: E3 includes ENV 3 module in the recipe.";
                constexpr const char* kEnableLfo1 =
                    "PATCH MUTATOR: L1 includes LFO 1 module in the recipe.";
                constexpr const char* kEnableLfo2 =
                    "PATCH MUTATOR: L2 includes LFO 2 module in the recipe.";
                constexpr const char* kEnableMatrixMod =
                    "PATCH MUTATOR: MM includes MATRIX MODULATION module in the recipe.";
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
                constexpr const char* kHistoryGateExport = "EXPORT";
                constexpr const char* kHistoryGateCancel = "CANCEL";
                constexpr const char* kHistoryGateDiscard = "DISCARD";

                constexpr const char* kExportCollisionTitle = "Export folder exists";
                constexpr const char* kExportCollisionMessage =
                    "A session folder with this name already exists.";
                constexpr const char* kExportCollisionOverwrite = "OVERWRITE";
                constexpr const char* kExportCollisionKeep = "KEEP BOTH";
                constexpr const char* kExportCollisionCancel = "CANCEL";

                constexpr const char* kExportCancelledFooter = "PATCH MUTATOR: Export cancelled.";

                // Stem for Export success sticky + FooterPanel path-style middle truncate.
                constexpr const char* kExportCompleteFooterStem = "PATCH MUTATOR: Exported ";

                inline juce::String formatDefragCompleteFooterMessage(int mutationsUsed,
                                                                      int mutationsAvailable,
                                                                      int retriesUsed,
                                                                      int retriesAvailable)
                {
                    return "PATCH MUTATOR: Mutation history renumbered. Mutations: "
                        + juce::String(mutationsUsed) + " used / " + juce::String(mutationsAvailable)
                        + " available. Retries: " + juce::String(retriesUsed) + " used / "
                        + juce::String(retriesAvailable) + " left under them.";
                }

                constexpr const char* kDeviceDumpFailedFooter =
                    "PATCH MUTATOR: Could not read the patch from the synth. Kept bank/patch numbers and editor "
                    "buffer. Check SYNTH FROM.";
                constexpr const char* kDeviceDumpAbortedEditedFooter =
                    "PATCH MUTATOR: Synth load cancelled - editor changed while waiting. Kept bank/patch numbers "
                    "and your edits.";
            }
        }
    } 

    // =================================================================================================================
    // Host parameter names (unique DAW / APVTS leaf names; UI keeps ParameterWidgets short labels)
    // SSOT list: _bmad-output/implementation-artifacts/inventory-host-parameter-names-draft.md
    // =================================================================================================================

    namespace HostParameterNames
    {
        namespace MasterEditSection
        {
            namespace MidiModule
            {
                constexpr const char* kChannel = "MIDI Channel";
                constexpr const char* kControllers = "MIDI Controllers";
                constexpr const char* kLever2Select = "MIDI Lever 2 Select";
                constexpr const char* kLever3Select = "MIDI Lever 3 Select";
                constexpr const char* kMidiEcho = "MIDI Echo";
                constexpr const char* kPatchChanges = "MIDI Patch Changes";
                constexpr const char* kPedal1Select = "MIDI Pedal 1 Select";
                constexpr const char* kPedal2Select = "MIDI Pedal 2 Select";
            }

            namespace MiscModule
            {
                constexpr const char* kBankLockEnable = "Misc Bank Lock";
                constexpr const char* kBendRange = "Misc Bend Range (+/-)";
                constexpr const char* kMasterTranspose = "Misc Master Transpose";
                constexpr const char* kMasterTune = "Misc Master Tune";
                constexpr const char* kMemoryProtectEnable = "Misc Memory Protect";
                constexpr const char* kUnisonDetune = "Misc Unison Detune";
                constexpr const char* kUnisonEnable = "Misc Unison";
                constexpr const char* kVolumeInvertEnable = "Misc Volume Invert";
            }

            namespace VibratoModule
            {
                constexpr const char* kAmpModAmount = "Vibrato Amplitude Modulation Amount";
                constexpr const char* kAmpModSource = "Vibrato Amplitude Modulation Source";
                constexpr const char* kAmplitude = "Vibrato Amplitude";
                constexpr const char* kSpeed = "Vibrato Speed";
                constexpr const char* kSpeedModAmount = "Vibrato Speed Modulation Amount";
                constexpr const char* kSpeedModSource = "Vibrato Speed Modulation Source";
                constexpr const char* kWaveform = "Vibrato Waveform";
            }

        }

        namespace MatrixModulationSection
        {
            namespace ModulationBus
            {
                constexpr const char* kBus0Amount = "Bus 0 Amount";
                constexpr const char* kBus0Destination = "Bus 0 Destination";
                constexpr const char* kBus0Source = "Bus 0 Source";
                constexpr const char* kBus1Amount = "Bus 1 Amount";
                constexpr const char* kBus1Destination = "Bus 1 Destination";
                constexpr const char* kBus1Source = "Bus 1 Source";
                constexpr const char* kBus2Amount = "Bus 2 Amount";
                constexpr const char* kBus2Destination = "Bus 2 Destination";
                constexpr const char* kBus2Source = "Bus 2 Source";
                constexpr const char* kBus3Amount = "Bus 3 Amount";
                constexpr const char* kBus3Destination = "Bus 3 Destination";
                constexpr const char* kBus3Source = "Bus 3 Source";
                constexpr const char* kBus4Amount = "Bus 4 Amount";
                constexpr const char* kBus4Destination = "Bus 4 Destination";
                constexpr const char* kBus4Source = "Bus 4 Source";
                constexpr const char* kBus5Amount = "Bus 5 Amount";
                constexpr const char* kBus5Destination = "Bus 5 Destination";
                constexpr const char* kBus5Source = "Bus 5 Source";
                constexpr const char* kBus6Amount = "Bus 6 Amount";
                constexpr const char* kBus6Destination = "Bus 6 Destination";
                constexpr const char* kBus6Source = "Bus 6 Source";
                constexpr const char* kBus7Amount = "Bus 7 Amount";
                constexpr const char* kBus7Destination = "Bus 7 Destination";
                constexpr const char* kBus7Source = "Bus 7 Source";
                constexpr const char* kBus8Amount = "Bus 8 Amount";
                constexpr const char* kBus8Destination = "Bus 8 Destination";
                constexpr const char* kBus8Source = "Bus 8 Source";
                constexpr const char* kBus9Amount = "Bus 9 Amount";
                constexpr const char* kBus9Destination = "Bus 9 Destination";
                constexpr const char* kBus9Source = "Bus 9 Source";
            }

        }

        namespace PatchEditSection
        {
            namespace Dco1Module
            {
                constexpr const char* kFrequency = "DCO 1 Frequency";
                constexpr const char* kFrequencyModByLfo1 = "DCO 1 Frequency < LFO 1";
                constexpr const char* kKeyClick = "DCO 1 Key Click";
                constexpr const char* kKeyboardPortamento = "DCO 1 Keyboard/Portamento";
                constexpr const char* kLevers = "DCO 1 Levers";
                constexpr const char* kPulseWidth = "DCO 1 Pulse Width";
                constexpr const char* kPulseWidthModByLfo2 = "DCO 1 Pulse Width < LFO 2";
                constexpr const char* kSync = "DCO 1 Sync";
                constexpr const char* kWaveSelect = "DCO 1 Wave Select";
                constexpr const char* kWaveShape = "DCO 1 Wave Shape";
            }

            namespace Dco2Module
            {
                constexpr const char* kDetune = "DCO 2 Detune";
                constexpr const char* kFrequency = "DCO 2 Frequency";
                constexpr const char* kFrequencyModByLfo1 = "DCO 2 Frequency < LFO 1";
                constexpr const char* kKeyClick = "DCO 2 Key Click";
                constexpr const char* kKeyboardPortamento = "DCO 2 Keyboard/Portamento";
                constexpr const char* kLevers = "DCO 2 Levers";
                constexpr const char* kPulseWidth = "DCO 2 Pulse Width";
                constexpr const char* kPulseWidthModByLfo2 = "DCO 2 Pulse Width < LFO 2";
                constexpr const char* kWaveSelect = "DCO 2 Wave Select";
                constexpr const char* kWaveShape = "DCO 2 Wave Shape";
            }

            namespace Envelope1Module
            {
                constexpr const char* kAmplitude = "ENV 1 Amplitude";
                constexpr const char* kAmplitudeModByVelocity = "ENV 1 Amplitude < Velocity";
                constexpr const char* kAttack = "ENV 1 Attack";
                constexpr const char* kDecay = "ENV 1 Decay";
                constexpr const char* kDelay = "ENV 1 Delay";
                constexpr const char* kEnvelopeMode = "ENV 1 Envelope Mode";
                constexpr const char* kLfo1Trigger = "ENV 1 LFO 1 Trigger";
                constexpr const char* kRelease = "ENV 1 Release";
                constexpr const char* kSustain = "ENV 1 Sustain";
                constexpr const char* kTriggerMode = "ENV 1 Trigger Mode";
            }

            namespace Envelope2Module
            {
                constexpr const char* kAmplitude = "ENV 2 Amplitude";
                constexpr const char* kAmplitudeModByVelocity = "ENV 2 Amplitude < Velocity";
                constexpr const char* kAttack = "ENV 2 Attack";
                constexpr const char* kDecay = "ENV 2 Decay";
                constexpr const char* kDelay = "ENV 2 Delay";
                constexpr const char* kEnvelopeMode = "ENV 2 Envelope Mode";
                constexpr const char* kLfo1Trigger = "ENV 2 LFO 1 Trigger";
                constexpr const char* kRelease = "ENV 2 Release";
                constexpr const char* kSustain = "ENV 2 Sustain";
                constexpr const char* kTriggerMode = "ENV 2 Trigger Mode";
            }

            namespace Envelope3Module
            {
                constexpr const char* kAmplitude = "ENV 3 Amplitude";
                constexpr const char* kAmplitudeModByVelocity = "ENV 3 Amplitude < Velocity";
                constexpr const char* kAttack = "ENV 3 Attack";
                constexpr const char* kDecay = "ENV 3 Decay";
                constexpr const char* kDelay = "ENV 3 Delay";
                constexpr const char* kEnvelopeMode = "ENV 3 Envelope Mode";
                constexpr const char* kLfo1Trigger = "ENV 3 LFO 1 Trigger";
                constexpr const char* kRelease = "ENV 3 Release";
                constexpr const char* kSustain = "ENV 3 Sustain";
                constexpr const char* kTriggerMode = "ENV 3 Trigger Mode";
            }

            namespace FmTrackModule
            {
                constexpr const char* kFmAmount = "FM/Track VCF FM Amount";
                constexpr const char* kFmModByEnv3 = "FM/Track FM < ENV 3";
                constexpr const char* kFmModByPressure = "FM/Track FM < Pressure";
                constexpr const char* kTrackInput = "FM/Track Track Input";
                constexpr const char* kTrackPoint1 = "FM/Track Track Point 1";
                constexpr const char* kTrackPoint2 = "FM/Track Track Point 2";
                constexpr const char* kTrackPoint3 = "FM/Track Track Point 3";
                constexpr const char* kTrackPoint4 = "FM/Track Track Point 4";
                constexpr const char* kTrackPoint5 = "FM/Track Track Point 5";
            }

            namespace Lfo1Module
            {
                constexpr const char* kAmplitude = "LFO 1 Amplitude";
                constexpr const char* kAmplitudeModByRamp1 = "LFO 1 Amplitude < Ramp 1";
                constexpr const char* kLag = "LFO 1 Lag";
                constexpr const char* kRetriggerPoint = "LFO 1 Retrigger Point";
                constexpr const char* kSampleInput = "LFO 1 Sample Input";
                constexpr const char* kSpeed = "LFO 1 Speed";
                constexpr const char* kSpeedModByPressure = "LFO 1 Speed < Pressure";
                constexpr const char* kTriggerMode = "LFO 1 Trigger Mode";
                constexpr const char* kWaveform = "LFO 1 Waveform";
            }

            namespace Lfo2Module
            {
                constexpr const char* kAmplitude = "LFO 2 Amplitude";
                constexpr const char* kAmplitudeModByRamp2 = "LFO 2 Amplitude < Ramp 2";
                constexpr const char* kLag = "LFO 2 Lag";
                constexpr const char* kRetriggerPoint = "LFO 2 Retrigger Point";
                constexpr const char* kSampleInput = "LFO 2 Sample Input";
                constexpr const char* kSpeed = "LFO 2 Speed";
                constexpr const char* kSpeedModByKeyboard = "LFO 2 Speed < Keyboard";
                constexpr const char* kTriggerMode = "LFO 2 Trigger Mode";
                constexpr const char* kWaveform = "LFO 2 Waveform";
            }

            namespace RampPortamentoModule
            {
                constexpr const char* kPortamentoKeyboardMode = "Ramp/Portamento Keyboard Mode";
                constexpr const char* kPortamentoLegato = "Ramp/Portamento Legato Portamento";
                constexpr const char* kPortamentoModByVelocity = "Ramp/Portamento Portamento < Velocity";
                constexpr const char* kPortamentoMode = "Ramp/Portamento Portamento Mode";
                constexpr const char* kPortamentoRate = "Ramp/Portamento Portamento Rate";
                constexpr const char* kRamp1Rate = "Ramp/Portamento Ramp 1 Rate";
                constexpr const char* kRamp1Trigger = "Ramp/Portamento Ramp 1 Trigger";
                constexpr const char* kRamp2Rate = "Ramp/Portamento Ramp 2 Rate";
                constexpr const char* kRamp2Trigger = "Ramp/Portamento Ramp 2 Trigger";
            }

            namespace VcfVcaModule
            {
                constexpr const char* kBalance = "VCF/VCA DCO 2 | DCO 1 Mix";
                constexpr const char* kFrequency = "VCF/VCA Frequency";
                constexpr const char* kFrequencyModByEnv1 = "VCF/VCA Frequency < ENV 1";
                constexpr const char* kFrequencyModByPressure = "VCF/VCA Frequency < Pressure";
                constexpr const char* kKeyboardPortamento = "VCF/VCA Keyboard/Portamento";
                constexpr const char* kLevers = "VCF/VCA Levers";
                constexpr const char* kResonance = "VCF/VCA Resonance";
                constexpr const char* kVca1ModByVelocity = "VCF/VCA VCA 1 < Velocity";
                constexpr const char* kVca1Volume = "VCF/VCA VCA 1 Volume";
                constexpr const char* kVca2ModByEnv2 = "VCF/VCA VCA 2 < ENV 2";
            }

        }

        namespace PatchManagerSection
        {
            namespace PatchMutatorModule
            {
                constexpr const char* kAmount = "Patch Mutator Amount";
                constexpr const char* kRandom = "Patch Mutator Random";
            }

        }

    }

}
