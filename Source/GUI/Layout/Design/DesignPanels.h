#pragma once

#include "DesignRecipes.h"
#include "Shared/Definitions/Matrix1000Limits.h"

// DesignPanels.h
// Panel and GUI root aggregations at UI display scale 1.0.

namespace TSS::Design
{
    namespace Panels
    {
        namespace Body
        {
            // Column panel inset (former Body edge padding) — not applied on Body itself.
            inline constexpr int kColumnPadding = Spacing::kLarge;
            inline constexpr int kInterColumnGap = Spacing::kStandard;
            inline constexpr int kInterColumnGapCount = 2;

            namespace MasterEditSection
            {
                inline constexpr int kWidth = Recipes::ParameterCell::kWidth;
                inline constexpr int kPanelWidth = kWidth + 2 * Body::kColumnPadding;
                inline constexpr int kInterModuleGap = Spacing::kLarge;
                inline constexpr int kChildModuleCount = 3;
                inline constexpr int kInterModuleGapCount = kChildModuleCount - 1;

                namespace ChildModules
                {
                    inline constexpr int kWidth = MasterEditSection::kWidth;
                }

                namespace MidiModule
                {
                    inline constexpr int kParameterCellCount = 8;
                    inline constexpr int kHeight = Atoms::Heights::kModuleHeader
                        + kParameterCellCount * Recipes::ParameterCell::kHeight;
                }

                namespace VibratoModule
                {
                    inline constexpr int kParameterCellCount = 7;
                    inline constexpr int kHeight = Atoms::Heights::kModuleHeader
                        + kParameterCellCount * Recipes::ParameterCell::kHeight;
                }

                namespace MiscModule
                {
                    inline constexpr int kParameterCellCount = 8;
                    inline constexpr int kHeight = Atoms::Heights::kModuleHeader
                        + kParameterCellCount * Recipes::ParameterCell::kHeight;
                }

                inline constexpr int kHeight = Atoms::Heights::kSectionHeader + MidiModule::kHeight
                    + VibratoModule::kHeight + MiscModule::kHeight + kInterModuleGapCount * kInterModuleGap;
                inline constexpr int kPanelHeight = kHeight + 2 * Body::kColumnPadding;
            }

            namespace SharedColumn
            {
                inline constexpr int kWidth = Recipes::ModulationBusCell::kWidth;
                inline constexpr int kPanelWidth = kWidth + 2 * Body::kColumnPadding;
                inline constexpr int kVerticalStackGap = Spacing::kLarge; // Matrix Modulation ↔ Patch Manager gap (figma-mockup.md)
            }

            namespace PatchEditSection
            {
                inline constexpr int kModuleWidth = Recipes::ParameterCell::kWidth;
                inline constexpr int kInterModuleGap = Spacing::kLarge;
                inline constexpr int kModuleCountPerRow = 5;
                inline constexpr int kInterModuleGapCountPerRow = kModuleCountPerRow - 1;
                inline constexpr int kTopBottomBandCount = 2;
                inline constexpr int kWidth = kModuleCountPerRow * kModuleWidth + kInterModuleGapCountPerRow * kInterModuleGap;
                inline constexpr int kPanelWidth = kWidth + 2 * Body::kColumnPadding;
                inline constexpr int kPatchEditModuleHeight = Recipes::PatchEditModule::kHeight;
                inline constexpr int kTopBottomPanelHeight = kPatchEditModuleHeight;

                namespace TopModules
                {
                    inline constexpr int kWidth = PatchEditSection::kWidth;
                    inline constexpr int kHeight = PatchEditSection::kTopBottomPanelHeight;

                    namespace ChildModules
                    {
                        inline constexpr int kWidth = PatchEditSection::kModuleWidth;
                        inline constexpr int kHeight = TopModules::kHeight;
                    }
                }

                namespace MiddleModules
                {
                    namespace PatchNameModule
                    {
                        inline constexpr int kTopPadding = Recipes::PatchNameModule::kTopPadding;
                        inline constexpr int kModuleHeaderToDisplayGap = Recipes::PatchNameModule::kModuleHeaderToDisplayGap;
                        inline constexpr int kBottomPadding = Recipes::PatchNameModule::kBottomPadding;
                        inline constexpr int kHeight = Recipes::PatchNameModule::kHeight;
                    }

                    inline constexpr int kWidth = PatchEditSection::kWidth;
                    inline constexpr int kHeight = Atoms::Heights::kEnvelopeDisplay;

                    namespace ChildModules
                    {
                        inline constexpr int kWidth = PatchEditSection::kModuleWidth;
                        inline constexpr int kHeight = MiddleModules::kHeight;
                    }
                }

                namespace BottomModules
                {
                    inline constexpr int kWidth = PatchEditSection::kWidth;
                    inline constexpr int kHeight = PatchEditSection::kTopBottomPanelHeight;

                    namespace ChildModules
                    {
                        inline constexpr int kWidth = PatchEditSection::kModuleWidth;
                        inline constexpr int kHeight = BottomModules::kHeight;
                    }
                }

                inline constexpr int kHeight = Atoms::Heights::kSectionHeader + kTopBottomBandCount * kTopBottomPanelHeight
                    + MiddleModules::kHeight;
                inline constexpr int kPanelHeight = kHeight + 2 * Body::kColumnPadding;
            }

            namespace MatrixModulationSection
            {
                inline constexpr int kWidth = Body::SharedColumn::kWidth;
                inline constexpr int kHeight = Atoms::Heights::kSectionHeader + Atoms::Heights::kModulationBusHeader
                    + Matrix1000Limits::kModulationBusCount * Recipes::ModulationBusCell::kHeight;
            }

            namespace PatchManagerSection
            {
                inline constexpr int kModuleCount = 4;
                inline constexpr int kModuleStackGapCount = kModuleCount - 1;
                inline constexpr int kWidth = Body::SharedColumn::kWidth;
                inline constexpr int kModuleStackGap = Spacing::kMedium; 

                namespace BankUtilityModule
                {
                    inline constexpr int kHeight = Recipes::BankUtilityModule::kHeight;
                }

                namespace InternalPatchesModule
                {
                    inline constexpr int kHeight = Recipes::InternalPatchesModule::kHeight;
                }

                namespace ComputerPatchesModule
                {
                    inline constexpr int kHeight = Recipes::ComputerPatchesModule::kHeight;
                }

                namespace PatchMutatorModule
                {
                    inline constexpr int kHeight = Recipes::PatchMutatorModule::kHeight;
                }

                inline constexpr int kModulesStackHeight = BankUtilityModule::kHeight + InternalPatchesModule::kHeight
                    + ComputerPatchesModule::kHeight + PatchMutatorModule::kHeight
                    + kModuleStackGapCount * kModuleStackGap;

                inline constexpr int kHeight = Atoms::Heights::kSectionHeader + kModulesStackHeight;
            }

            namespace SharedColumn
            {
                inline constexpr int kSharedPanelHeight = MatrixModulationSection::kHeight + kVerticalStackGap
                    + PatchManagerSection::kHeight;
                inline constexpr int kHeight = kSharedPanelHeight;
                // Outer panel matches Patch/Master column height; 4 px slack remains below content stack.
                inline constexpr int kPanelHeight = PatchEditSection::kPanelHeight;
            }

            inline constexpr int kEffectiveHeight = PatchEditSection::kHeight;
        }

        namespace Header
        {
            inline constexpr int kHeight = 40;
            inline constexpr int kLogoWidth = 172;
            inline constexpr int kLogoHeight = 20;
            inline constexpr int kLogoVerticalOffset = -1;
            // Half of the former 4 px bottom edge border (removed): nudge content down to re-center.
            inline constexpr int kContentVerticalOffset = 2;
            inline constexpr int kLogoFontHeight = 20;
            inline constexpr int kLogoGapAfter = 12;
            inline constexpr int kLogoPopupColumnWidth = 100;
            inline constexpr int kLogoPopupActionColumnWidth = 100;
            inline constexpr int kLeftPadding = 12;
            inline constexpr int kRightPadding = 12;
            inline constexpr int kControlHeight = 20;
            inline constexpr int kGap = 4;
            inline constexpr int kPacketExternalGap = kGap * 4;
            inline constexpr int kLabelToControlGap = 0;
            inline constexpr int kKeyboardFromLabelToComboGap = kLabelToControlGap - kGap;
            inline constexpr int kAudioFromLabelToComboGap = kLabelToControlGap - kGap;
            inline constexpr int kEditorMidiFromLabelWidth = 48;
            inline constexpr int kInputGainLabelToSliderGap = kLabelToControlGap - kGap;
            inline constexpr int kMidiToLabelWidth = 40;
            inline constexpr int kKeyboardFromLabelWidth = 84;
            inline constexpr int kAudioFromLabelWidth = 64;
            inline constexpr int kInputGainLabelWidth = 60;
            inline constexpr int kPortComboBoxWidth = 112;
            inline constexpr int kInputGainSliderWidth = 60;
            inline constexpr int kPeakIndicatorWidth = 12;
            inline constexpr int kPanicButtonWidth = 44;
            inline constexpr int kUndoButtonWidth = 44;
            inline constexpr int kRedoButtonWidth = 44;
            inline constexpr int kRedoToPanicGap = 16;
        }

        namespace Footer
        {
            inline constexpr int kHeight = 32;
            inline constexpr int kBandHeight = 24;
            inline constexpr int kBandVerticalInset = (kHeight - kBandHeight) / 2; // 4 px above/below
            inline constexpr int kPadding = Spacing::kLarge; // 12 — align with Body column / Patch Edit inset
            // Intentional ÷4 exception: severity icon slot stays 14 (do not snap to 12/16).
            inline constexpr int kIconSize = 14;
            inline constexpr int kSeverityBadgeHeight = 16;
            inline constexpr int kSeverityBadgeHorizontalPadding = Spacing::kStandard;
            inline constexpr int kSeverityBadgeToMessageGap = Spacing::kMedium;
        }
    }

    namespace GUI
    {
        inline constexpr int kWidth = Panels::Body::PatchEditSection::kPanelWidth
            + Panels::Body::kInterColumnGapCount * Panels::Body::kInterColumnGap
            + Panels::Body::SharedColumn::kPanelWidth
            + Panels::Body::MasterEditSection::kPanelWidth;
        inline constexpr int kBodyHeight = Panels::Body::PatchEditSection::kPanelHeight;
        inline constexpr int kHeight = Panels::Header::kHeight + kBodyHeight + Panels::Footer::kHeight;
    }

    namespace Panels
    {
        namespace Body
        {
            inline constexpr int kWidth = GUI::kWidth;
            inline constexpr int kHeight = GUI::kBodyHeight;
        }

        namespace Header
        {
            inline constexpr int kWidth = GUI::kWidth;
        }

        namespace Footer
        {
            inline constexpr int kWidth = GUI::kWidth;
        }
    }

    namespace PanelWidgets
    {
        namespace Widths
        {
            namespace SectionHeader
            {
                inline constexpr int kMasterEdit = Panels::Body::MasterEditSection::kWidth;
                inline constexpr int kPatchEdit = Panels::Body::PatchEditSection::kWidth;
                inline constexpr int kMatrixModulation = Panels::Body::MatrixModulationSection::kWidth;
                inline constexpr int kPatchManager = Panels::Body::PatchManagerSection::kWidth;
            }

            namespace ModuleHeader
            {
                inline constexpr int kPatchEditModule = Panels::Body::PatchEditSection::kModuleWidth;
                inline constexpr int kMasterEditModule = Panels::Body::MasterEditSection::kWidth;
                inline constexpr int kPatchManagerModule = Panels::Body::SharedColumn::kWidth;
            }

            namespace ModulationBusHeader
            {
                inline constexpr int kStandard = Panels::Body::SharedColumn::kWidth;
            }

            namespace HorizontalSeparator
            {
                inline constexpr int kMasterEditModule = Panels::Body::MasterEditSection::kWidth;
                inline constexpr int kPatchEditModule = Panels::Body::PatchEditSection::kModuleWidth;
                inline constexpr int kMatrixModulationBus = Panels::Body::SharedColumn::kWidth;
            }
        }

    }
}
