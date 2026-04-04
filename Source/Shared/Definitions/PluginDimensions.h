#pragma once

// PluginDimensions.h
// Design dimensions at UI display scale 1.0. Values are multiples of 4 for clean scaling at
// presets 50% / 75% / 100% / 125% / 150% / 200%.
// Reading order: widget primitives, column geometry, panels, GUI root.

namespace PluginDimensions
{
    namespace Widgets
    {
        namespace Heights
        {
            inline constexpr int kSectionHeader = 24;
            inline constexpr int kModuleHeader = 24;
            inline constexpr int kModulationBusHeader = 24;
            inline constexpr int kGroupLabel = 24;
            inline constexpr int kLabel = 20;
            inline constexpr int kButton = 20;
            inline constexpr int kToggle = 20;
            inline constexpr int kSlider = 20;
            inline constexpr int kComboBox = 20;
            inline constexpr int kNumberBox = 20;
            inline constexpr int kPatchNameDisplay = 72;
            inline constexpr int kHorizontalSeparator = 4;
        }

        namespace Widths
        {
            namespace VerticalSeparator
            {
                inline constexpr int kStandard = 32;
            }

            namespace GroupLabel
            {
                inline constexpr int kInternalPatchesBrowser = 108;
                inline constexpr int kInternalPatchesMemory = 156;
                inline constexpr int kComputerPatchesBrowser = 136;
                inline constexpr int kComputerPatchesStorage = 128;
            }

            namespace Label
            {
                inline constexpr int kMasterEditModule = 100;
                inline constexpr int kPatchEditModule = 88;
                inline constexpr int kModulationBusNumber = 16;
                inline constexpr int kPatchManagerBankSelector = 76;
                inline constexpr int kPatchMutator = 44;
            }

            namespace Button
            {
                inline constexpr int kHeaderPanelTheme = 68;
                inline constexpr int kInit = 20;
                inline constexpr int kCopy = 20;
                inline constexpr int kPaste = 20;
                inline constexpr int kPatchManagerBankSelect = 36;
                inline constexpr int kPatchManagerUnlockBank = 76;
                inline constexpr int kInternalPatchesMemory = 36;
                inline constexpr int kComputerPatchesStorage = 36;
                inline constexpr int kComputerPatchesSaveAs = 48;
                inline constexpr int kPatchMutatorMutate = 48;
                inline constexpr int kPatchMutatorCompare = 48;
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
                inline constexpr int kStandard = 60;
                inline constexpr int kPatchMutator = 44;
            }

            namespace ComboBox
            {
                inline constexpr int kMasterEditModule = 60;
                inline constexpr int kPatchEditModule = 60;
                inline constexpr int kMatrixModulationSource = 60;
                inline constexpr int kMatrixModulationDestination = 104;
                inline constexpr int kPatchManagerComputerPatches = 84;
                inline constexpr int kPatchMutatorHistory = 44;
            }

            namespace NumberBox
            {
                inline constexpr int kPatchManagerBankNumber = 24;
                inline constexpr int kPatchManagerPatchNumber = 28;
            }
        }
    }

    namespace GUI
    {
        inline constexpr int kBodyInnerWidth =
            800
            + Widgets::Widths::VerticalSeparator::kStandard
            + 264
            + Widgets::Widths::VerticalSeparator::kStandard
            + 160;

        inline constexpr int kPadding = 12;
        inline constexpr int kWidth = kBodyInnerWidth + 2 * kPadding;
        inline constexpr int kHeaderHeight = 24;
        inline constexpr int kFooterHeight = 24;

        inline constexpr int kBodyEffectiveHeight = 320 + 400;
        inline constexpr int kBodyHeight = kBodyEffectiveHeight + 2 * kPadding;
        inline constexpr int kHeight = kHeaderHeight + kBodyHeight + kFooterHeight;
    }

    namespace Panels
    {
        namespace Header
        {
            inline constexpr int kWidth = GUI::kWidth;
            inline constexpr int kHeight = GUI::kHeaderHeight;
        }

        namespace Body
        {
            inline constexpr int kWidth = GUI::kWidth;
            inline constexpr int kHeight = GUI::kBodyHeight;
            inline constexpr int kPadding = GUI::kPadding;
            inline constexpr int kEffectiveHeight = GUI::kBodyEffectiveHeight;

            namespace MasterEditSection
            {
                inline constexpr int kWidth = 160;
                inline constexpr int kHeight = Body::kEffectiveHeight;

                namespace ChildModules
                {
                    inline constexpr int kWidth = MasterEditSection::kWidth;
                }

                namespace MidiModule
                {
                    inline constexpr int kHeight = 232;
                }

                namespace VibratoModule
                {
                    inline constexpr int kHeight = 232;
                }

                namespace MiscModule
                {
                    inline constexpr int kHeight = 232;
                }
            }

            namespace SharedColumn
            {
                inline constexpr int kWidth = 264;
            }

            namespace PatchEditSection
            {
                inline constexpr int kModuleWidth = 144;
                inline constexpr int kInterModuleGap = 20;
                inline constexpr int kWidth =
                    5 * kModuleWidth + 4 * kInterModuleGap;
                inline constexpr int kTopBottomPanelHeight = 284;
                inline constexpr int kHeight = Body::kEffectiveHeight;

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
                    inline constexpr int kWidth = PatchEditSection::kWidth;
                    inline constexpr int kHeight = 128;

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
            }

            namespace MatrixModulationSection
            {
                inline constexpr int kWidth = Body::SharedColumn::kWidth;
                inline constexpr int kHeight = 320;
            }

            namespace PatchManagerSection
            {
                inline constexpr int kWidth = Body::SharedColumn::kWidth;
                inline constexpr int kHeight = 400;

                namespace BankUtilityModule
                {
                    inline constexpr int kWidth = PatchManagerSection::kWidth;
                    inline constexpr int kHeight = 88;
                }

                namespace InternalPatchesModule
                {
                    inline constexpr int kWidth = PatchManagerSection::kWidth;
                    inline constexpr int kHeight = 84;
                }

                namespace ComputerPatchesModule
                {
                    inline constexpr int kWidth = PatchManagerSection::kWidth;
                    inline constexpr int kHeight = 84;
                }

                namespace PatchMutatorModule
                {
                    inline constexpr int kWidth = PatchManagerSection::kWidth;
                    inline constexpr int kHeight = 120;
                }
            }
        }

        namespace Footer
        {
            inline constexpr int kWidth = GUI::kWidth;
            inline constexpr int kHeight = GUI::kFooterHeight;
        }
    }

    namespace Widgets
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
            inline constexpr int kVerticalSeparator = Panels::Body::kEffectiveHeight;
        }
    }

    static_assert(Panels::Body::PatchEditSection::kWidth
                      == 5 * Panels::Body::PatchEditSection::kModuleWidth
                             + 4 * Panels::Body::PatchEditSection::kInterModuleGap,
                  "Patch edit row width");
    static_assert(
        GUI::kBodyInnerWidth
            == Panels::Body::PatchEditSection::kWidth
                   + Widgets::Widths::VerticalSeparator::kStandard
                   + Panels::Body::SharedColumn::kWidth
                   + Widgets::Widths::VerticalSeparator::kStandard
                   + Panels::Body::MasterEditSection::kWidth,
        "Inner body width");
    static_assert(GUI::kWidth == GUI::kBodyInnerWidth + 2 * GUI::kPadding, "GUI width");
    static_assert(
        Panels::Body::kEffectiveHeight
            == Panels::Body::MatrixModulationSection::kHeight
                   + Panels::Body::PatchManagerSection::kHeight,
        "Column stack height");
    static_assert(
        Widgets::Heights::kSectionHeader + Panels::Body::PatchEditSection::kTopBottomPanelHeight
                + Panels::Body::PatchEditSection::MiddleModules::kHeight
                + Panels::Body::PatchEditSection::kTopBottomPanelHeight
            == Panels::Body::PatchEditSection::kHeight,
        "Patch edit vertical bands");
    static_assert(
        Widgets::Heights::kSectionHeader + Panels::Body::MasterEditSection::MidiModule::kHeight
                + Panels::Body::MasterEditSection::VibratoModule::kHeight
                + Panels::Body::MasterEditSection::MiscModule::kHeight
            == Panels::Body::MasterEditSection::kHeight,
        "Master section height");
    static_assert(
        Widgets::Heights::kSectionHeader
                + Panels::Body::PatchManagerSection::BankUtilityModule::kHeight
                + Panels::Body::PatchManagerSection::InternalPatchesModule::kHeight
                + Panels::Body::PatchManagerSection::ComputerPatchesModule::kHeight
                + Panels::Body::PatchManagerSection::PatchMutatorModule::kHeight
            == Panels::Body::PatchManagerSection::kHeight,
        "Patch manager height");
    static_assert(
        GUI::kHeight == GUI::kHeaderHeight + GUI::kBodyHeight + GUI::kFooterHeight,
        "GUI height stack");
}
