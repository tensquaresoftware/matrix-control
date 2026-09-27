#pragma once

#include "DesignPanels.h"

// DesignChecks.h
// Compile-time cross-checks — include last in the Design* chain.

namespace TSS::Design
{
    static_assert(GUI::kWidth == 1308, "MainComponent width");
    static_assert(GUI::kHeight == 800, "MainComponent height");
    static_assert(Panels::Header::kHeight == 40, "HeaderPanel height");
    static_assert(Panels::Header::kLogoWidth % 4 == 0, "Header logo width must be on the design ÷4 grid");
    static_assert(Panels::Header::kLogoHeight % 4 == 0, "Header logo height must be on the design ÷4 grid");
    static_assert(Panels::Header::kLogoFontHeight % 4 == 0, "Header logo font height must be on the design ÷4 grid");
    static_assert(Panels::Header::kLogoGapAfter % 4 == 0, "Header logo gap must be on the design ÷4 grid");
    static_assert(Panels::Header::kEditorMidiFromLabelWidth % 4 == 0,
                  "Header editor MIDI from label width must be on the design ÷4 grid");
    static_assert(Panels::Header::kInputGainLabelToSliderGap % 4 == 0,
                  "Header input gain label-to-slider gap must be on the design ÷4 grid");
    static_assert(Panels::Header::kRedoToPanicGap % 4 == 0,
                  "Header redo-to-panic gap must be on the design ÷4 grid");
    static_assert(Panels::Header::kPanicButtonWidth % 4 == 0,
                  "Header panic button width must be on the design ÷4 grid");
    static_assert(Panels::Header::kUndoButtonWidth % 4 == 0,
                  "Header undo button width must be on the design ÷4 grid");
    static_assert(Panels::Header::kRedoButtonWidth % 4 == 0,
                  "Header redo button width must be on the design ÷4 grid");
    static_assert(Panels::Body::kHeight == 728, "BodyPanel height");
    static_assert(Panels::Footer::kHeight == 32, "FooterPanel height");
    static_assert(Panels::Footer::kBandHeight == 24, "Footer column band height");
    static_assert(Panels::Footer::kBandHeight % 4 == 0, "Footer band height must be on the design ÷4 grid");
    static_assert(Panels::Footer::kBandVerticalInset == 4, "Footer band vertical inset");
    static_assert(Panels::Footer::kBandVerticalInset * 2 + Panels::Footer::kBandHeight
                      == Panels::Footer::kHeight,
                  "Footer height = band + vertical insets");
    static_assert(Panels::Footer::kPadding == 12, "Footer padding");
    static_assert(Panels::Footer::kPadding % 4 == 0, "Footer padding must be on the design ÷4 grid");
    static_assert(Panels::Footer::kPadding == Panels::Body::kColumnPadding,
                  "Footer left inset matches Body column / Patch Edit module inset");
    static_assert(Panels::Footer::kIconSize == 14,
                  "Footer legacy icon size intentional ÷4 exception");
    static_assert(Panels::Footer::kSeverityBadgeHeight == 16, "Footer severity badge height");
    static_assert(Panels::Footer::kSeverityBadgeHeight % 4 == 0,
                  "Footer severity badge height must be on the design ÷4 grid");
    static_assert(Panels::Footer::kSeverityBadgeIconInset == 2,
                  "Footer severity badge icon uniform inset");
    static_assert(Panels::Footer::kSeverityBadgeIconSide == 12,
                  "Footer sticky severity icon square is height minus uniform inset");
    static_assert(Panels::Footer::kSeverityBadgeIconSide
                      == Panels::Footer::kSeverityBadgeHeight
                         - 2 * Panels::Footer::kSeverityBadgeIconInset,
                  "Footer sticky severity icon square must match height - 2 * inset");
    static_assert(Panels::Footer::kSeverityBadgeHeight
                      > 2 * Panels::Footer::kSeverityBadgeIconInset,
                  "Footer severity badge height must leave room for icon inset");
    static_assert(Panels::Footer::kSeverityBadgeHorizontalPadding == 4,
                  "Footer severity badge horizontal padding");
    static_assert(Panels::Footer::kSeverityBadgeToMessageGap == 8,
                  "Footer severity badge to message gap");

    static_assert(Recipes::ParameterCell::kWidth == 152, "ParameterCell width");
    static_assert(Recipes::ParameterCell::kHeight == 24, "ParameterCell height");
    static_assert(Recipes::ParameterCell::kWidth == Recipes::Label::kPatchEditModule + Recipes::ComboBox::kPatchEditModule,
        "Parameter cell width");

    static_assert(Recipes::ModulationBusCell::kWidth == 268, "Modulation bus cell width");
    static_assert(Recipes::ModulationBusCell::kHeight == 24, "Modulation bus cell height");
    static_assert(Atoms::Widths::ModulationBusHeader::kBusDestinationTextWidth
            + 3 * Atoms::Widths::Button::kInit == 128,
        "Matrix mod section header action column (D-095)");

    static_assert(Recipes::BankUtilityModule::kHeight == 76, "Bank utility module height");
    static_assert(Recipes::BankUtilityModule::kSelectorWidth == 156, "Bank utility selector width");
    static_assert(Recipes::BankUtilityModule::kUtilityWidth == 96, "Bank utility utility width");
    static_assert(Recipes::BankUtilityModule::kSelectorToUtilityGap == 16, "Bank utility selector-utility gap");
    static_assert(Recipes::BankUtilityModule::kContentWidth == 268, "Bank utility content width");
    static_assert(Atoms::Widths::Button::kPatchManagerBankSelect == 28, "Bank select button width");
    static_assert(Atoms::Widths::Button::kPatchManagerCopyBank == 44, "Bank COPY button width");
    static_assert(Atoms::Widths::Button::kPatchManagerPasteBank == 44, "Bank PASTE button width");
    static_assert(Atoms::Widths::Button::kPatchManagerImportBank == 48, "Bank IMPORT button width");
    static_assert(Atoms::Widths::Button::kPatchManagerExportBank == 48, "Bank EXPORT button width");
    static_assert(Recipes::InternalPatchesModule::kHeight == 76, "Internal patches module height");
    static_assert(Recipes::ComputerPatchesModule::kHeight == 76, "Computer patches module height");
    static_assert(Recipes::PatchMutatorModule::kHeight == 100, "Patch mutator module height");
    static_assert(Recipes::PatchManagerModule::kShortControlVerticalInset == 2,
        "Patch manager short control vertical inset in button row");
    static_assert(Panels::Body::PatchManagerSection::kHeight == 384, "Patch manager section height");
    static_assert(Panels::Body::MatrixModulationSection::kHeight == 304, "Matrix modulation section height");

    static_assert(Panels::Body::kColumnPadding == 12, "Column panel padding");
    static_assert(Panels::Body::kInterColumnGap == 4, "Body inter-column gap");
    static_assert(Panels::Body::PatchEditSection::kWidth == 808, "PatchEdit content width");
    static_assert(Panels::Body::PatchEditSection::kHeight == 704, "PatchEdit content height");
    static_assert(Panels::Body::PatchEditSection::kPanelWidth == 832, "PatchEditPanel outer width");
    static_assert(Panels::Body::PatchEditSection::kPanelHeight == 728, "PatchEditPanel outer height");
    static_assert(Panels::Body::MasterEditSection::kWidth == 152, "MasterEdit content width");
    static_assert(Panels::Body::MasterEditSection::kHeight == 704, "MasterEdit content height");
    static_assert(Panels::Body::MasterEditSection::kPanelWidth == 176, "MasterEditPanel outer width");
    static_assert(Panels::Body::MasterEditSection::kPanelHeight == 728, "MasterEditPanel outer height");
    static_assert(Panels::Body::SharedColumn::kWidth == 268, "Shared content width");
    static_assert(Panels::Body::SharedColumn::kSharedPanelHeight == 700, "Shared content stack height");
    static_assert(Panels::Body::SharedColumn::kPanelWidth == 292, "SharedPanel outer width");
    static_assert(Panels::Body::SharedColumn::kPanelHeight == 728, "SharedPanel outer height");

    static_assert(Panels::Body::PatchEditSection::TopModules::kHeight == 272, "PatchEditTopModulesPanel height");
    static_assert(Panels::Body::PatchEditSection::MiddleModules::kHeight == 128, "PatchEditDisplaysPanel height");
    static_assert(Panels::Body::PatchEditSection::BottomModules::kHeight == 272, "PatchEditBottomModulesPanel height");
    static_assert(Panels::Body::PatchEditSection::TopModules::ChildModules::kWidth == 152, "Patch edit child module width");
    static_assert(Panels::Body::PatchEditSection::TopModules::ChildModules::kHeight == 272, "Patch edit child module height");

    static_assert(
        Panels::Body::PatchEditSection::kWidth
            == Panels::Body::PatchEditSection::kModuleCountPerRow * Panels::Body::PatchEditSection::kModuleWidth
                + Panels::Body::PatchEditSection::kInterModuleGapCountPerRow * Panels::Body::PatchEditSection::kInterModuleGap,
        "Patch edit row width");

    static_assert(
        Panels::Body::PatchEditSection::MiddleModules::PatchNameModule::kHeight
            == Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Middle row: patch name module matches envelope/track band height");

    static_assert(
        Atoms::Heights::kTrackGeneratorDisplay == Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Middle row: track generator band height");

    static_assert(
        Atoms::Widths::DisplayBand::kPaddingTop + Atoms::Widths::DisplayBand::kInnerHeight
            + Atoms::Widths::DisplayBand::kPaddingBottom == Atoms::Heights::kEnvelopeDisplay,
        "Envelope/track display band inner stack");
    static_assert(Recipes::PatchNameModule::kTopPadding == 8, "Patch name top padding");
    static_assert(Recipes::PatchNameModule::kModuleHeaderToDisplayGap == 4, "Patch name header-to-display gap");
    static_assert(Recipes::PatchNameModule::kBottomPadding == 12, "Patch name bottom padding");
    static_assert(
        Panels::Body::PatchEditSection::MiddleModules::PatchNameModule::kTopPadding
                + Atoms::Heights::kModuleHeader
                + Panels::Body::PatchEditSection::MiddleModules::PatchNameModule::kModuleHeaderToDisplayGap
                + Atoms::Heights::kPatchNameDisplay
                + Panels::Body::PatchEditSection::MiddleModules::PatchNameModule::kBottomPadding
            == Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Patch name module vertical stack");
    static_assert(Panels::Body::MasterEditSection::kInterModuleGap == 12, "Master edit inter-module gap");

    static_assert(
        Panels::Body::PatchEditSection::kHeight
            == Atoms::Heights::kSectionHeader
                + Panels::Body::PatchEditSection::kTopBottomBandCount * Panels::Body::PatchEditSection::kTopBottomPanelHeight
                + Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Patch edit vertical stack");

    static_assert(
        Panels::Body::MasterEditSection::kHeight
            == Atoms::Heights::kSectionHeader + Panels::Body::MasterEditSection::MidiModule::kHeight
                + Panels::Body::MasterEditSection::VibratoModule::kHeight
                + Panels::Body::MasterEditSection::MiscModule::kHeight
                + Panels::Body::MasterEditSection::kInterModuleGapCount * Panels::Body::MasterEditSection::kInterModuleGap,
        "Master edit vertical stack");

    static_assert(
        Panels::Body::MasterEditSection::kHeight == Panels::Body::PatchEditSection::kHeight,
        "Master and patch edit column design heights match");

    static_assert(
        Panels::Body::SharedColumn::kHeight
            == Panels::Body::MatrixModulationSection::kHeight + Panels::Body::SharedColumn::kVerticalStackGap
                + Panels::Body::PatchManagerSection::kHeight,
        "Shared column stack height");

    static_assert(Panels::Body::kEffectiveHeight - Panels::Body::SharedColumn::kSharedPanelHeight == 4,
        "Shared content stack 4 px shorter than patch/master (Mutator omits trailing separator)");

    static_assert(
        GUI::kWidth
            == Panels::Body::PatchEditSection::kPanelWidth
                + Panels::Body::kInterColumnGapCount * Panels::Body::kInterColumnGap
                + Panels::Body::SharedColumn::kPanelWidth
                + Panels::Body::MasterEditSection::kPanelWidth,
        "GUI width from column outer sizes and gaps");

    static_assert(GUI::kBodyHeight == Panels::Body::PatchEditSection::kPanelHeight, "Body height equals column outer height");
    static_assert(
        Panels::Body::SharedColumn::kPanelHeight == Panels::Body::PatchEditSection::kPanelHeight
            && Panels::Body::MasterEditSection::kPanelHeight == Panels::Body::PatchEditSection::kPanelHeight,
        "Three Body columns share outer height");

    static_assert(
        Atoms::Heights::kSectionHeader + Panels::Body::PatchManagerSection::kModulesStackHeight
            == Panels::Body::PatchManagerSection::kHeight,
        "Patch manager modules stack height");

    static_assert(GUI::kHeight == Panels::Header::kHeight + GUI::kBodyHeight + Panels::Footer::kHeight, "GUI height stack");
}
