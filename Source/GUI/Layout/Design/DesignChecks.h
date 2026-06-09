#pragma once

#include "DesignPanels.h"

// DesignChecks.h
// Compile-time cross-checks — include last in the Design* chain.

namespace TSS::Design
{
    static_assert(Recipes::ParameterCell::kWidth == Recipes::Label::kPatchEditModule + Recipes::ComboBox::kPatchEditModule,
        "Parameter cell width");

    static_assert(Recipes::ModulationBusCell::kWidth == 268, "Modulation bus cell width");

    static_assert(Recipes::BankUtilityModule::kHeight == 76, "Bank utility module height");
    static_assert(Recipes::InternalPatchesModule::kHeight == 76, "Internal patches module height");
    static_assert(Recipes::ComputerPatchesModule::kHeight == 76, "Computer patches module height");
    static_assert(Recipes::PatchMutatorModule::kHeight == 100, "Patch mutator module height");
    static_assert(Panels::Body::PatchManagerSection::kHeight == 384, "Patch manager section height");
    static_assert(Panels::Body::MatrixModulationSection::kHeight == 304, "Matrix modulation section height");

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
        Panels::Body::PatchEditSection::MiddleModules::PatchNameModule::kTopPadding
                + Atoms::Heights::kModuleHeader
                + Panels::Body::PatchEditSection::MiddleModules::PatchNameModule::kModuleHeaderToDisplayGap
                + Atoms::Heights::kPatchNameDisplay
                + Panels::Body::PatchEditSection::MiddleModules::PatchNameModule::kBottomPadding
            == Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Patch name module vertical stack");

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

    static_assert(Panels::Body::SharedColumn::kHeight == 700, "Shared column height");
    static_assert(Panels::Body::kEffectiveHeight - Panels::Body::SharedColumn::kHeight == 4,
        "Shared column 4 px shorter than patch/master (Mutator omits trailing separator)");

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
}
