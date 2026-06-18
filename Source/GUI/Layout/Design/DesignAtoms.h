#pragma once

// DesignAtoms.h
// Fixed spacing tokens and atomic widget dimensions at UI display scale 1.0 (÷4 grid).
// No composed panel totals or cross-atom sums — see DesignRecipes.h for Lego formulas.

namespace TSS::Design
{
    namespace Spacing
    {
        inline constexpr int kStandard = 4;
        inline constexpr int kMedium = 8;
        inline constexpr int kLarge = 12;
    }

    namespace Atoms
    {
        namespace Heights
        {
            inline constexpr int kLabel = 16;
            inline constexpr int kSlider = 16;
            inline constexpr int kComboBox = 16;
            inline constexpr int kHorizontalSeparator = 8;

            inline constexpr int kButton = 20;
            inline constexpr int kToggle = 20;
            inline constexpr int kNumberBox = 20;
            inline constexpr int kGroupLabel = 24;

            inline constexpr int kSectionHeader = 32;
            inline constexpr int kModuleHeader = 32;
            inline constexpr int kModulationBusHeader = 32;

            inline constexpr int kEnvelopeDisplay = 128;
            inline constexpr int kTrackGeneratorDisplay = 128;
            inline constexpr int kPatchNameDisplay = 72;
        }

        namespace Widths
        {
            namespace ParameterCell
            {
                inline constexpr int kLabel = 92;
                inline constexpr int kControl = 60;
            }

            namespace Button
            {
                inline constexpr int kHeaderPanelTheme = 68;

                inline constexpr int kInit = 20;
                inline constexpr int kCopy = 20;
                inline constexpr int kPaste = 20;

                inline constexpr int kPatchManagerBankSelect = 36;
                inline constexpr int kPatchManagerLockBank = 68;

                inline constexpr int kInternalPatchesInit = 36;
                inline constexpr int kInternalPatchesCopy = 36;
                inline constexpr int kInternalPatchesPaste = 36;
                inline constexpr int kInternalPatchesStore = 36;

                inline constexpr int kComputerPatchesLoad = 36;
                inline constexpr int kComputerPatchesSaveAs = 44;
                inline constexpr int kComputerPatchesSave = 36;

                inline constexpr int kPatchMutatorMutate = 52;
                inline constexpr int kPatchMutatorRetry = 52;
                inline constexpr int kPatchMutatorCompare = 52;
                inline constexpr int kPatchMutatorDelete = 20;
                inline constexpr int kPatchMutatorClear = 44;
                inline constexpr int kPatchMutatorExport = 44;
            }

            namespace Toggle
            {
                inline constexpr int kPatchMutator = 20;
            }

            namespace Slider
            {
                inline constexpr int kPatchMutator = 48;
            }

            namespace ComboBox
            {
                inline constexpr int kMatrixModulationSource = 60;
                inline constexpr int kMatrixModulationDestination = 104;
                inline constexpr int kPatchManagerComputerPatches = 88;
                inline constexpr int kPatchMutatorHistory = 48;
            }

            namespace NumberBox
            {
                inline constexpr int kPatchManagerBankNumber = 24;
                inline constexpr int kPatchManagerPatchNumber = 28;
            }

            namespace Label
            {
                inline constexpr int kModulationBusNumber = 8;
                inline constexpr int kPatchManagerSelectBank = 68;
                inline constexpr int kPatchMutator = 40;
            }

            namespace GroupLabel
            {
                inline constexpr int kInternalPatchesBrowser = 104;
                inline constexpr int kInternalPatchesMemory = 156;
                inline constexpr int kComputerPatchesBrowser = 136;
                inline constexpr int kComputerPatchesStorage = 124;
            }

            namespace ModulationBus
            {
                inline constexpr int kInterControlGap = 4;
            }

            namespace ModulationBusCell
            {
                inline constexpr int kReorderDragThreshold = 4;
            }

            namespace ModulationBusHeader
            {
                inline constexpr int kBusNumberTextWidth = 8;
                inline constexpr int kBusSourceTextWidth = 60;
                inline constexpr int kBusAmountTextWidth = 60;
                inline constexpr int kBusDestinationTextWidth = 68;
            }

            namespace PopupMenu
            {
                inline constexpr int kItemHeight = 20;
                inline constexpr int kVerticalMargin = Spacing::kStandard;
                inline constexpr int kScrollbarWidth = 8;
                inline constexpr int kMinThumbHeight = 20;
                inline constexpr int kMaxScrollHeight = 300;
                inline constexpr int kBorderThickness = 1;
                inline constexpr int kTextLeftPadding = 3;
            }

            namespace DisplayBand
            {
                inline constexpr int kBorderThickness = 2;
                inline constexpr int kPaddingTop = 12;
                inline constexpr int kPaddingBottom = 12;
                inline constexpr int kInnerHeight = 104;
                inline constexpr int kTriangleBase = 10;
                inline constexpr int kCurvePadding = 5;
                inline constexpr int kCurvePointRadius = 3;
                inline constexpr int kCurveLineThickness = 1;
                inline constexpr int kMinCurveSegmentWidth = 1;
                inline constexpr int kPointHitZoneRadius = 10;
                inline constexpr int kSustainSegmentHitZone = 10;
            }

            namespace ModuleHeader
            {
                inline constexpr int kTextLeftPadding = 0;
                inline constexpr int kTextAreaHeight = 20;
                inline constexpr int kLineThickness = 4;
            }

            namespace VerticalSeparator
            {
                inline constexpr int kStandard = 24;
                inline constexpr int kLineWidth = 4;
                inline constexpr int kTopPadding = 4;
                inline constexpr int kBottomPadding = 2;
            }

            namespace Indicator
            {
                inline constexpr int kLedSize = 12;
            }
        }
    }
}
