#pragma once

// PluginDesignDimensions.h
// Design dimensions at UI display scale 1.0 (4x4 design grid; scale presets 50% … 200%).
// Reading order: atomic widgets → composite rows/cells → panels → GUI root.
// Composite sizes are sums of smaller named constants so layouts stay auditable.

namespace PluginDesignDimensions
{
    namespace Widgets
    {
        namespace Heights
        {
            inline constexpr int kHorizontalSeparator = 8;

            inline constexpr int kLabel = 16;
            inline constexpr int kSlider = 16;
            inline constexpr int kComboBox = 16;

            inline constexpr int kButton = 20;
            inline constexpr int kToggle = 20;
            inline constexpr int kGroupLabel = 20;
            inline constexpr int kNumberBox = 20;

            inline constexpr int kPatchNameDisplay = 72;

            inline constexpr int kSectionHeader = 32;
            inline constexpr int kModuleHeader = 32;
            inline constexpr int kModulationBusHeader = 32;

            inline constexpr int kParameterCell = kLabel + kHorizontalSeparator;

            inline constexpr int kEnvelopeDisplay = 128;
            inline constexpr int kTrackGeneratorDisplay = 128;
        }

        namespace Widths
        {
            namespace VerticalSeparator
            {
                inline constexpr int kStandard = 24;
            }

            namespace ParameterRow
            {
                inline constexpr int kLabel = 92;
                inline constexpr int kControl = 60;
                inline constexpr int kWidth = kLabel + kControl;
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
                inline constexpr int kStandard = ParameterRow::kControl;
                inline constexpr int kPatchMutator = 44;
            }

            namespace ComboBox
            {
                inline constexpr int kMasterEditModule = ParameterRow::kControl;
                inline constexpr int kPatchEditModule = ParameterRow::kControl;
                inline constexpr int kMatrixModulationSource = ParameterRow::kControl;
                inline constexpr int kMatrixModulationDestination = 104;
                inline constexpr int kPatchManagerComputerPatches = 84;
                inline constexpr int kPatchMutatorHistory = 44;
            }

            namespace NumberBox
            {
                inline constexpr int kPatchManagerBankNumber = 24;
                inline constexpr int kPatchManagerPatchNumber = 28;
            }

            namespace Label
            {
                inline constexpr int kPatchEditModule = ParameterRow::kLabel;
                inline constexpr int kMasterEditModule = ParameterRow::kLabel;
                inline constexpr int kModulationBusNumber = 8;
                inline constexpr int kPatchManagerBankSelector = 76;
                inline constexpr int kPatchMutator = 44;
            }

            namespace GroupLabel
            {
                inline constexpr int kInternalPatchesBrowser = 108;
                inline constexpr int kInternalPatchesMemory = 156;
                inline constexpr int kComputerPatchesBrowser = 136;
                inline constexpr int kComputerPatchesStorage = 128;
            }

            namespace ModulationBusRow
            {
                inline constexpr int kInterControlGap = 4;
                inline constexpr int kInitButton = Button::kInit;
                inline constexpr int kWidth = Label::kModulationBusNumber + ComboBox::kMatrixModulationSource
                    + Slider::kStandard + ComboBox::kMatrixModulationDestination + kInitButton
                    + 4 * kInterControlGap;
            }
        }
    }

    namespace Panels
    {
        namespace Body
        {
            inline constexpr int kPadding = 12;

            namespace MasterEditSection
            {
                inline constexpr int kWidth = Widgets::Widths::ParameterRow::kWidth;
                inline constexpr int kInterModuleGap = 12;

                namespace ChildModules
                {
                    inline constexpr int kWidth = MasterEditSection::kWidth;
                }

                namespace MidiModule
                {
                    inline constexpr int kHeight =
                        Widgets::Heights::kModuleHeader + 8 * Widgets::Heights::kParameterCell;
                }

                namespace VibratoModule
                {
                    inline constexpr int kHeight =
                        Widgets::Heights::kModuleHeader + 7 * Widgets::Heights::kParameterCell;
                }

                namespace MiscModule
                {
                    inline constexpr int kHeight =
                        Widgets::Heights::kModuleHeader + 8 * Widgets::Heights::kParameterCell;
                }

                inline constexpr int kHeight = Widgets::Heights::kSectionHeader + MidiModule::kHeight
                    + kInterModuleGap + VibratoModule::kHeight + kInterModuleGap + MiscModule::kHeight;
            }

            namespace SharedColumn
            {
                inline constexpr int kWidth = Widgets::Widths::ModulationBusRow::kWidth;
                inline constexpr int kInterPanelGap = 24;
            }

            namespace PatchEditSection
            {
                inline constexpr int kModuleWidth = Widgets::Widths::ParameterRow::kWidth;
                inline constexpr int kInterModuleGap = 12;

                inline constexpr int kWidth = 5 * kModuleWidth + 4 * kInterModuleGap;

                inline constexpr int kPatchEditModuleHeight = Widgets::Heights::kModuleHeader
                    + 10 * Widgets::Heights::kParameterCell;

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
                    namespace PatchNameColumn
                    {
                        inline constexpr int kTopPadding = 8;
                        inline constexpr int kModuleHeaderToDisplayGap = 4;
                        inline constexpr int kBottomPadding = 12;

                        inline constexpr int kHeight = kTopPadding + Widgets::Heights::kModuleHeader
                            + kModuleHeaderToDisplayGap + Widgets::Heights::kPatchNameDisplay + kBottomPadding;
                    }

                    inline constexpr int kWidth = PatchEditSection::kWidth;
                    inline constexpr int kHeight = Widgets::Heights::kEnvelopeDisplay;

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

                inline constexpr int kHeight = Widgets::Heights::kSectionHeader + kTopBottomPanelHeight
                    + MiddleModules::kHeight + kTopBottomPanelHeight;
            }

            namespace MatrixModulationSection
            {
                inline constexpr int kWidth = Body::SharedColumn::kWidth;
                inline constexpr int kHeight = Widgets::Heights::kSectionHeader + Widgets::Heights::kModulationBusHeader
                    + 10 * Widgets::Heights::kParameterCell;
            }

            namespace PatchManagerSection
            {
                inline constexpr int kWidth = Body::SharedColumn::kWidth;

                namespace BankUtilityModule
                {
                    inline constexpr int kHeight = 84;
                }

                namespace InternalPatchesModule
                {
                    inline constexpr int kHeight = 84;
                }

                namespace ComputerPatchesModule
                {
                    inline constexpr int kHeight = 84;
                }

                namespace PatchMutatorModule
                {
                    inline constexpr int kContentDesignHeight = 100;
                    // +16 closes PatchManagerSection to 400px with section header + three 84px modules.
                    inline constexpr int kHeight = kContentDesignHeight + 16;
                }

                inline constexpr int kHeight = Widgets::Heights::kSectionHeader + BankUtilityModule::kHeight
                    + InternalPatchesModule::kHeight + ComputerPatchesModule::kHeight + PatchMutatorModule::kHeight;
            }

            inline constexpr int kEffectiveHeight = PatchEditSection::kHeight;
        }
    }

    namespace GUI
    {
        inline constexpr int kHeaderHeight = 32;
        inline constexpr int kFooterHeight = 32;

        inline constexpr int kBodyEffectiveHeight = Panels::Body::kEffectiveHeight;

        inline constexpr int kBodyInnerWidth = Panels::Body::PatchEditSection::kWidth
            + Widgets::Widths::VerticalSeparator::kStandard + Panels::Body::SharedColumn::kWidth
            + Widgets::Widths::VerticalSeparator::kStandard + Panels::Body::MasterEditSection::kWidth;

        inline constexpr int kPadding = Panels::Body::kPadding;
        inline constexpr int kWidth = kBodyInnerWidth + 2 * kPadding;
        inline constexpr int kBodyHeight = kBodyEffectiveHeight + 2 * kPadding;
        inline constexpr int kHeight = kHeaderHeight + kBodyHeight + kFooterHeight;
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
            inline constexpr int kHeight = GUI::kHeaderHeight;
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

    //========================================================================== Consistency checks
    static_assert(Widgets::Widths::ParameterRow::kWidth == Widgets::Widths::Label::kPatchEditModule
                       + Widgets::Widths::ComboBox::kPatchEditModule,
        "Parameter row width");

    static_assert(Widgets::Widths::ModulationBusRow::kWidth == 268, "Modulation bus row width");

    static_assert(
        Panels::Body::PatchEditSection::kWidth
            == 5 * Panels::Body::PatchEditSection::kModuleWidth
                + 4 * Panels::Body::PatchEditSection::kInterModuleGap,
        "Patch edit row width");

    static_assert(
        Panels::Body::PatchEditSection::MiddleModules::PatchNameColumn::kHeight
            == Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Middle row: patch name column matches envelope/track band height");

    static_assert(
        Widgets::Heights::kTrackGeneratorDisplay == Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Middle row: track generator band height");

    static_assert(
        Panels::Body::PatchEditSection::MiddleModules::PatchNameColumn::kTopPadding
                + Widgets::Heights::kModuleHeader
                + Panels::Body::PatchEditSection::MiddleModules::PatchNameColumn::kModuleHeaderToDisplayGap
                + Widgets::Heights::kPatchNameDisplay
                + Panels::Body::PatchEditSection::MiddleModules::PatchNameColumn::kBottomPadding
            == Panels::Body::PatchEditSection::MiddleModules::kHeight,
        "Patch name column vertical stack");

    static_assert(
        Panels::Body::PatchEditSection::kHeight
            == Widgets::Heights::kSectionHeader + Panels::Body::PatchEditSection::kTopBottomPanelHeight
                + Panels::Body::PatchEditSection::MiddleModules::kHeight + Panels::Body::PatchEditSection::kTopBottomPanelHeight,
        "Patch edit vertical stack");

    static_assert(
        Panels::Body::MasterEditSection::kHeight
            == Widgets::Heights::kSectionHeader + Panels::Body::MasterEditSection::MidiModule::kHeight
                + Panels::Body::MasterEditSection::kInterModuleGap
                + Panels::Body::MasterEditSection::VibratoModule::kHeight + Panels::Body::MasterEditSection::kInterModuleGap
                + Panels::Body::MasterEditSection::MiscModule::kHeight,
        "Master edit vertical stack");

    static_assert(
        Panels::Body::MasterEditSection::kHeight == Panels::Body::PatchEditSection::kHeight,
        "Master and patch edit column design heights match");

    static_assert(
        Panels::Body::kEffectiveHeight
            == Panels::Body::MatrixModulationSection::kHeight + Panels::Body::PatchManagerSection::kHeight,
        "Shared column stack height");

    static_assert(
        GUI::kBodyInnerWidth
            == Panels::Body::PatchEditSection::kWidth + Widgets::Widths::VerticalSeparator::kStandard
                + Panels::Body::SharedColumn::kWidth + Widgets::Widths::VerticalSeparator::kStandard
                + Panels::Body::MasterEditSection::kWidth,
        "Body inner width");

    static_assert(GUI::kWidth == GUI::kBodyInnerWidth + 2 * GUI::kPadding, "GUI width");

    static_assert(
        Widgets::Heights::kSectionHeader + Panels::Body::PatchManagerSection::BankUtilityModule::kHeight
            + Panels::Body::PatchManagerSection::InternalPatchesModule::kHeight
            + Panels::Body::PatchManagerSection::ComputerPatchesModule::kHeight
            + Panels::Body::PatchManagerSection::PatchMutatorModule::kHeight
            == Panels::Body::PatchManagerSection::kHeight,
        "Patch manager modules height");

    static_assert(GUI::kHeight == GUI::kHeaderHeight + GUI::kBodyHeight + GUI::kFooterHeight, "GUI height stack");
}
