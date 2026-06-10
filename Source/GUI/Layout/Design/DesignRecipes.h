#pragma once

#include "DesignAtoms.h"
#include "Shared/Definitions/Matrix1000Limits.h"

// DesignRecipes.h
// Lego assembly formulas on atoms.

namespace TSS::Design
{
    namespace Recipes
    {
        // Figma-only ParameterCore (no C++ class): Label + Slider/Combo on one row (152 x 16).
        // ParameterCell = ParameterCore + HorizontalSeparator (152 x 24).
        namespace ParameterCell
        {
            inline constexpr int kWidth = Atoms::Widths::ParameterCell::kLabel + Atoms::Widths::ParameterCell::kControl;
            inline constexpr int kHeight = Atoms::Heights::kLabel + Atoms::Heights::kHorizontalSeparator;
        }

        // PatchNameModule: ModuleHeader "PATCH NAME" + PatchNameDisplay.
        namespace PatchNameModule
        {
            inline constexpr int kTopPadding = Spacing::kMedium;
            inline constexpr int kModuleHeaderToDisplayGap = Spacing::kStandard;
            inline constexpr int kBottomPadding = Spacing::kLarge;
            inline constexpr int kWidth = ParameterCell::kWidth;
            inline constexpr int kHeight = kTopPadding + Atoms::Heights::kModuleHeader + kModuleHeaderToDisplayGap
                + Atoms::Heights::kPatchNameDisplay + kBottomPadding;
        }

        // Figma-only ModulationBusCore (no C++ class): Label.W8 + combos + slider + Init + 4 x gap (268 x 16).
        // ModulationBusCell = ModulationBusCore + HorizontalSeparator (268 x 24).
        namespace ModulationBusCell
        {
            inline constexpr int kWidth = Atoms::Widths::Label::kModulationBusNumber
                + Atoms::Widths::ComboBox::kMatrixModulationSource + Atoms::Widths::ParameterCell::kControl
                + Atoms::Widths::ComboBox::kMatrixModulationDestination + Atoms::Widths::Button::kInit
                + 4 * Atoms::Widths::ModulationBus::kInterControlGap;
            inline constexpr int kHeight = Atoms::Heights::kLabel + Atoms::Heights::kHorizontalSeparator;
        }

        namespace PatchEditModule
        {
            inline constexpr int kWidth = ParameterCell::kWidth;
            inline constexpr int kHeight = Atoms::Heights::kModuleHeader
                + Matrix1000Limits::kParameterCellCount * ParameterCell::kHeight;
        }

        namespace Slider
        {
            inline constexpr int kStandard = Atoms::Widths::ParameterCell::kControl;
        }

        namespace ComboBox
        {
            inline constexpr int kMasterEditModule = Atoms::Widths::ParameterCell::kControl;
            inline constexpr int kPatchEditModule = Atoms::Widths::ParameterCell::kControl;
        }

        namespace Label
        {
            inline constexpr int kPatchEditModule = Atoms::Widths::ParameterCell::kLabel;
            inline constexpr int kMasterEditModule = Atoms::Widths::ParameterCell::kLabel;
        }

        namespace PatchManagerModule
        {
            inline constexpr int kContentRowHeight = Atoms::Heights::kButton;
            inline constexpr int kInterControlGap = Spacing::kStandard;
            inline constexpr int kColumnGap = Spacing::kMedium;
            inline constexpr int kShortControlVerticalInset = (kContentRowHeight - Atoms::Heights::kLabel) / 2;
        }

        namespace BankUtilityModule
        {
            inline constexpr int kContentRowCount = 2;
            inline constexpr int kHeight = Atoms::Heights::kModuleHeader + kContentRowCount * PatchManagerModule::kContentRowHeight
                + (kContentRowCount - 1) * PatchManagerModule::kInterControlGap;
        }

        namespace InternalPatchesModule
        {
            inline constexpr int kHeight = Atoms::Heights::kModuleHeader + Atoms::Heights::kGroupLabel
                + PatchManagerModule::kContentRowHeight;
        }

        namespace ComputerPatchesModule
        {
            inline constexpr int kHeight = Atoms::Heights::kModuleHeader + Atoms::Heights::kGroupLabel
                + PatchManagerModule::kContentRowHeight;
        }

        // No HorizontalSeparator after the last content row (Figma — saves 4 px vs Patch/Master columns).
        namespace PatchMutatorModule
        {
            inline constexpr int kContentRowCount = 3;
            inline constexpr int kHeight = Atoms::Heights::kModuleHeader + kContentRowCount * PatchManagerModule::kContentRowHeight
                + (kContentRowCount - 1) * PatchManagerModule::kInterControlGap;
        }
    }
}
