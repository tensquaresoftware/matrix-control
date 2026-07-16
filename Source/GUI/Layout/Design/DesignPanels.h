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
            inline constexpr int kPadding = Spacing::kLarge;
            inline constexpr int kVerticalSeparatorCount = 2;
            inline constexpr int kPaddingSideCount = 2;

            namespace MasterEditSection
            {
                inline constexpr int kWidth = Recipes::ParameterCell::kWidth;
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
            }

            namespace SharedColumn
            {
                inline constexpr int kWidth = Recipes::ModulationBusCell::kWidth;
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
            }

            inline constexpr int kEffectiveHeight = PatchEditSection::kHeight;
        }

        inline constexpr int kPanelEdgeBorderThickness = 4;

        namespace Header
        {
            inline constexpr int kHeight = 40;
            inline constexpr int kLogoWidth = 172;
            inline constexpr int kLogoHeight = 20;
            inline constexpr int kLogoVerticalOffset = -1;
            inline constexpr int kLogoFontHeight = 20;
            inline constexpr int kLogoGapAfter = 32;
            inline constexpr int kLogoPopupColumnWidth = 80;
            inline constexpr int kSettingsButtonWidth = 72;
        }

        namespace Footer
        {
            inline constexpr int kHeight = 32;
            inline constexpr int kIdentityMinWidth = 160;
        }
    }

    namespace GUI
    {
        inline constexpr int kBodyInnerWidth = Panels::Body::PatchEditSection::kWidth
            + Panels::Body::kVerticalSeparatorCount * Atoms::Widths::VerticalSeparator::kStandard
            + Panels::Body::SharedColumn::kWidth + Panels::Body::MasterEditSection::kWidth;

        inline constexpr int kPadding = Panels::Body::kPadding;
        inline constexpr int kWidth = kBodyInnerWidth + Panels::Body::kPaddingSideCount * kPadding;
        inline constexpr int kBodyHeight = Panels::Body::kEffectiveHeight + Panels::Body::kPaddingSideCount * kPadding;
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

        namespace Heights
        {
            // Full Body height so separators join Header/Footer panel-edge borders.
            inline constexpr int kVerticalSeparator = Panels::Body::kHeight;
        }
    }
}
