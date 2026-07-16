#pragma once

#include "DesignPanels.h"

// DesignChecks.h
// Compile-time cross-checks — include last in the Design* chain.

namespace TSS::Design
{
    static_assert(GUI::kWidth == 1300, "MainComponent width");
    static_assert(GUI::kHeight == 800, "MainComponent height");
    static_assert(Panels::Header::kHeight == 40, "HeaderPanel height");
    static_assert(Panels::Header::kLogoWidth % 4 == 0, "Header logo width must be on the design ÷4 grid");
    static_assert(Panels::Header::kLogoHeight % 4 == 0, "Header logo height must be on the design ÷4 grid");
    static_assert(Panels::Header::kLogoFontHeight % 4 == 0, "Header logo font height must be on the design ÷4 grid");
    static_assert(Panels::Header::kLogoGapAfter % 4 == 0, "Header logo gap must be on the design ÷4 grid");
    static_assert(Panels::Body::kHeight == 728, "BodyPanel height");
    static_assert(Panels::Footer::kHeight == 32, "FooterPanel height");

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
    static_assert(Recipes::InternalPatchesModule::kHeight == 76, "Internal patches module height");
    static_assert(Recipes::ComputerPatchesModule::kHeight == 76, "Computer patches module height");
    static_assert(Recipes::PatchMutatorModule::kHeight == 100, "Patch mutator module height");
    static_assert(Recipes::PatchManagerModule::kShortControlVerticalInset == 2,
        "Patch manager short control vertical inset in button row");
    static_assert(Panels::Body::PatchManagerSection::kHeight == 384, "Patch manager section height");
    static_assert(Panels::Body::MatrixModulationSection::kHeight == 304, "Matrix modulation section height");

    static_assert(Panels::Body::PatchEditSection::kWidth == 808, "PatchEditPanel width");
    static_assert(Panels::Body::PatchEditSection::kHeight == 704, "PatchEditPanel height");
    static_assert(Panels::Body::MasterEditSection::kWidth == 152, "MasterEditPanel width");
    static_assert(Panels::Body::MasterEditSection::kHeight == 704, "MasterEditPanel height");
    static_assert(Panels::Body::SharedColumn::kWidth == 268, "SharedPanel width");
    static_assert(Panels::Body::SharedColumn::kSharedPanelHeight == 700, "SharedPanel height");

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
        "Shared panel 4 px shorter than patch/master (Mutator omits trailing separator)");

    static_assert(
        GUI::kBodyInnerWidth
            == Panels::Body::PatchEditSection::kWidth
                + Panels::Body::kVerticalSeparatorCount * Atoms::Widths::VerticalSeparator::kStandard
                + Panels::Body::SharedColumn::kWidth + Panels::Body::MasterEditSection::kWidth,
        "Body inner width");

    static_assert(GUI::kWidth == GUI::kBodyInnerWidth + Panels::Body::kPaddingSideCount * GUI::kPadding, "GUI width");

    static_assert(
        Atoms::Heights::kSectionHeader + Panels::Body::PatchManagerSection::kModulesStackHeight
            == Panels::Body::PatchManagerSection::kHeight,
        "Patch manager modules stack height");

    static_assert(GUI::kHeight == Panels::Header::kHeight + GUI::kBodyHeight + Panels::Footer::kHeight, "GUI height stack");

    static_assert(PanelWidgets::Heights::kVerticalSeparator == Panels::Body::kHeight,
        "Vertical separators span full Body height to join Header/Footer borders");
}
