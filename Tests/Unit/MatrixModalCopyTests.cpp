#include <juce_core/juce_core.h>

#include "Shared/Definitions/PluginDisplayNames.h"

class MatrixModalCopyTests : public juce::UnitTest
{
public:
    MatrixModalCopyTests() : juce::UnitTest("MatrixModalCopy") {}

    void runTest() override
    {
        testMutatorBodies();
        testDeleteInitBodies();
        testMasterBodies();
        testBankAndUnsavedBodies();
        testPatchNameMismatchCopy();
    }

private:
    void expectApprovedAscii(const juce::String& actual, const juce::String& approved)
    {
        expectEquals(actual, approved);
        expect(actual.containsOnly(getAsciiCharacters()), "body must be ASCII only");
        expect(! actual.contains(" ?") && ! actual.contains(" !") && ! actual.contains(" :"),
               "no spaces before ? ! :");
    }

    static juce::String getAsciiCharacters()
    {
        juce::String ascii;
        for (int c = 0x09; c < 0x7f; ++c)
            ascii += juce::String::charToString(static_cast<juce::juce_wchar>(c));
        return ascii;
    }

    void testMutatorBodies()
    {
        beginTest("mutator confirms - UPPERCASE button citations and line breaks");

        namespace Dialog = PluginDisplayNames::Dialogs;
        expectApprovedAscii(Dialog::MutatorFlushConfirm::kBody,
                            "This clears the Patch Mutator history for this session.\n\n"
                            "The initial patch snapshot is kept.\n\n"
                            "CONTINUE to flush, or CANCEL to keep the history.");
        expectApprovedAscii(Dialog::MutatorDeleteConfirm::kBody,
                            "This removes the selected mutation or retry from Patch Mutator history.\n\n"
                            "Deleting a root mutation also removes all of its retries.\n\n"
                            "DELETE to remove it, or CANCEL to keep the history.");
        expectApprovedAscii(Dialog::MutatorHistoryDefrag::kBody,
                            "Defrag will compact mutation history and preserve the current selection.\n\n"
                            "DEFRAG to compact it, or CANCEL to keep the history as is.");
    }

    void testDeleteInitBodies()
    {
        beginTest("delete init template - line break between sentences, UPPERCASE citations");

        namespace Dialog = PluginDisplayNames::Dialogs::DeleteInitTemplateConfirm;
        expectApprovedAscii(Dialog::kBodyPatch,
                            "This removes the system Patch init template (PatchInit.syx).\n\n"
                            "The next Patch INIT will use the built-in defaults.\n\n"
                            "DELETE to remove it, or CANCEL to keep the file.");
        expectApprovedAscii(Dialog::kBodyMaster,
                            "This removes the system Master init template (MasterInit.syx).\n\n"
                            "The next Master INIT will use the built-in defaults.\n\n"
                            "DELETE to remove it, or CANCEL to keep the file.");
    }

    void testMasterBodies()
    {
        beginTest("m1km load choice - cites option labels and CANCEL");

        expectApprovedAscii(PluginDisplayNames::Dialogs::MasterM1kmLoadChoice::kBody,
                            "This .m1km Master file may include Groups and cascade data. "
                            "Matrix-Control does not edit those yet, but the Matrix-1000 still uses them.\n\n"
                            "Choose whether to load MASTER SETTINGS ONLY (Groups/cascade reset) or the "
                            "FULL MASTER (Groups/cascade kept).\n\n"
                            "CANCEL leaves the current Master unchanged.");
    }

    void testBankAndUnsavedBodies()
    {
        beginTest("bank confirm and unsaved bodies - UPPERCASE citations");

        namespace Dialog = PluginDisplayNames::Dialogs;
        expectApprovedAscii(Dialog::BankImportConfirm::kBody,
                            "This overwrites patches on the device with files from the selected folder.\n\n"
                            "CONTINUE to import, or CANCEL to keep the device unchanged.");
        expectApprovedAscii(Dialog::BankPasteConfirm::formatBody(1, 2),
                            "This overwrites all 100 patches in bank 2 with the copied bank 1.\n\n"
                            "CONTINUE to paste, or CANCEL to keep the device unchanged.");
        expectApprovedAscii(Dialog::UnsavedEditConfirm::kBodyStore,
                            "This patch is not stored in the synth's current RAM slot yet "
                            "(edits and/or an INIT that was never stored).\n\n"
                            "STORE writes it to the current RAM location. DISCARD abandons it and continues. "
                            "CANCEL keeps editing.");
        expectApprovedAscii(Dialog::UnsavedEditConfirm::kBodySaveAs,
                            "This patch has changes that were not saved as a .syx file.\n\n"
                            "SAVE AS writes a new file. DISCARD abandons the changes and continues. "
                            "CANCEL keeps editing.");
    }

    void testPatchNameMismatchCopy()
    {
        beginTest("patch name mismatch - separate row labels and question");

        namespace Dialog = PluginDisplayNames::Dialogs::PatchNameReconciliation;
        expectEquals(juce::String(Dialog::kInternalNameLabel), juce::String("Internal name:"));
        expectEquals(juce::String(Dialog::kFilenameLabel), juce::String("Filename:"));
        expectEquals(juce::String(Dialog::kBody), juce::String("Which name should be used for this load?"));
    }
};

static MatrixModalCopyTests matrixModalCopyTests;
