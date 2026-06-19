#include "DimensionFactory.h"

#include <utility>

#include "GUI/Layout/Design/Design.h"

namespace
{
    using namespace TSS::Design;
    namespace BW = Atoms::Widths::Button;

    ButtonDimensions buildButtonDimensions()
    {
        return {
            .initWidth = BW::kInit,
            .copyWidth = BW::kCopy,
            .pasteWidth = BW::kPaste,
            .height = Atoms::Heights::kButton,
            .patchManagerBankSelectWidth = BW::kPatchManagerBankSelect,
            .patchManagerUnlockBankWidth = BW::kPatchManagerUnlockBank,
            .internalPatchesInitWidth = BW::kInternalPatchesInit,
            .internalPatchesCopyWidth = BW::kInternalPatchesCopy,
            .internalPatchesPasteWidth = BW::kInternalPatchesPaste,
            .internalPatchesStoreWidth = BW::kInternalPatchesStore,
            .computerPatchesLoadWidth = BW::kComputerPatchesLoad,
            .computerPatchesSaveWidth = BW::kComputerPatchesSave,
            .computerPatchesSaveAsWidth = BW::kComputerPatchesSaveAs,
            .patchMutatorMutateWidth = BW::kPatchMutatorMutate,
            .patchMutatorRetryWidth = BW::kPatchMutatorRetry,
            .patchMutatorCompareWidth = BW::kPatchMutatorCompare,
            .patchMutatorDeleteWidth = BW::kPatchMutatorDelete,
            .patchMutatorClearWidth = BW::kPatchMutatorClear,
            .patchMutatorExportWidth = BW::kPatchMutatorExport,
        };
    }

    ParameterCellDimensions buildPatchEditParameterCellDimensions()
    {
        return {
            .rowHeight = Recipes::ParameterCell::kHeight,
            .labelWidth = Recipes::Label::kPatchEditModule,
            .controlWidth = Recipes::ComboBox::kPatchEditModule,
            .separatorWidth = PanelWidgets::Widths::HorizontalSeparator::kPatchEditModule,
            .labelHeight = Atoms::Heights::kLabel,
            .sliderHeight = Atoms::Heights::kSlider,
            .comboBoxHeight = Atoms::Heights::kComboBox,
            .horizontalSeparatorHeight = Atoms::Heights::kHorizontalSeparator,
        };
    }

    ParameterCellDimensions buildMasterEditParameterCellDimensions()
    {
        return {
            .rowHeight = Recipes::ParameterCell::kHeight,
            .labelWidth = Recipes::Label::kMasterEditModule,
            .controlWidth = Recipes::ComboBox::kMasterEditModule,
            .separatorWidth = PanelWidgets::Widths::HorizontalSeparator::kMasterEditModule,
            .labelHeight = Atoms::Heights::kLabel,
            .sliderHeight = Atoms::Heights::kSlider,
            .comboBoxHeight = Atoms::Heights::kComboBox,
            .horizontalSeparatorHeight = Atoms::Heights::kHorizontalSeparator,
        };
    }

    PopupMenuLayoutDimensions buildPopupMenuLayoutDimensions()
    {
        return {
            .itemHeight = Atoms::Widths::PopupMenu::kItemHeight,
            .verticalMargin = Atoms::Widths::PopupMenu::kVerticalMargin,
            .scrollbarWidth = Atoms::Widths::PopupMenu::kScrollbarWidth,
            .minThumbHeight = Atoms::Widths::PopupMenu::kMinThumbHeight,
            .maxScrollHeight = Atoms::Widths::PopupMenu::kMaxScrollHeight,
            .borderThickness = Atoms::Widths::PopupMenu::kBorderThickness,
            .textLeftPadding = Atoms::Widths::PopupMenu::kTextLeftPadding,
        };
    }

    ModuleHeaderDimensions buildPatchEditModuleHeaderDimensions(const ButtonDimensions& buttons)
    {
        return {
            .height = Atoms::Heights::kModuleHeader,
            .buttonHeight = buttons.height,
            .patchEditTitleBandWidth = PanelWidgets::Widths::ModuleHeader::kPatchEditModule,
            .masterEditTitleBandWidth = PanelWidgets::Widths::ModuleHeader::kMasterEditModule,
            .patchManagerTitleBandWidth = PanelWidgets::Widths::ModuleHeader::kPatchManagerModule,
            .initWidth = buttons.initWidth,
            .copyWidth = buttons.copyWidth,
            .pasteWidth = buttons.pasteWidth,
            .textLeftPadding = Atoms::Widths::ModuleHeader::kTextLeftPadding,
            .textAreaHeight = Atoms::Widths::ModuleHeader::kTextAreaHeight,
            .lineThickness = Atoms::Widths::ModuleHeader::kLineThickness,
        };
    }

    ModulationBusHeaderDimensions buildModulationBusHeaderDimensions()
    {
        return {
            .busNumberTextWidth = Atoms::Widths::ModulationBusHeader::kBusNumberTextWidth,
            .busSourceTextWidth = Atoms::Widths::ModulationBusHeader::kBusSourceTextWidth,
            .busAmountTextWidth = Atoms::Widths::ModulationBusHeader::kBusAmountTextWidth,
            .busDestinationTextWidth = Atoms::Widths::ModulationBusHeader::kBusDestinationTextWidth,
            .interControlGap = Atoms::Widths::ModulationBus::kInterControlGap,
        };
    }

    DisplayBandDimensions buildDisplayBandDimensions(int width, int height, bool includeSustainHitZone)
    {
        return {
            .width = width,
            .height = height,
            .borderThickness = Atoms::Widths::DisplayBand::kBorderThickness,
            .paddingTop = Atoms::Widths::DisplayBand::kPaddingTop,
            .paddingBottom = Atoms::Widths::DisplayBand::kPaddingBottom,
            .triangleBase = Atoms::Widths::DisplayBand::kTriangleBase,
            .curvePadding = Atoms::Widths::DisplayBand::kCurvePadding,
            .curvePointRadius = Atoms::Widths::DisplayBand::kCurvePointRadius,
            .curveLineThickness = Atoms::Widths::DisplayBand::kCurveLineThickness,
            .minCurveSegmentWidth = Atoms::Widths::DisplayBand::kMinCurveSegmentWidth,
            .pointHitZoneRadius = Atoms::Widths::DisplayBand::kPointHitZoneRadius,
            .sustainSegmentHitZone = includeSustainHitZone ? Atoms::Widths::DisplayBand::kSustainSegmentHitZone : 0,
        };
    }

    ModulationBusCellDimensions buildModulationBusCellDimensions(const ButtonDimensions& buttons)
    {
        return {
            .panelWidth = Recipes::ModulationBusCell::kWidth,
            .panelHeight = Recipes::ModulationBusCell::kHeight,
            .busNumberLabelWidth = Atoms::Widths::Label::kModulationBusNumber,
            .busNumberLabelHeight = Atoms::Heights::kLabel,
            .sourceComboBoxWidth = Atoms::Widths::ComboBox::kMatrixModulationSource,
            .sourceComboBoxHeight = Atoms::Heights::kComboBox,
            .amountSliderWidth = Atoms::Widths::ParameterCell::kControl,
            .amountSliderHeight = Atoms::Heights::kSlider,
            .destinationComboBoxWidth = Atoms::Widths::ComboBox::kMatrixModulationDestination,
            .destinationComboBoxHeight = Atoms::Heights::kComboBox,
            .initButtonWidth = buttons.initWidth,
            .initButtonHeight = Atoms::Heights::kLabel,
            .separatorWidth = Recipes::ModulationBusCell::kWidth,
            .separatorHeight = Atoms::Heights::kHorizontalSeparator,
            .interControlGap = Atoms::Widths::ModulationBus::kInterControlGap,
            .reorderDragThreshold = Atoms::Widths::ModulationBusCell::kReorderDragThreshold,
        };
    }

    PatchManagerModuleLayoutDimensions buildPatchManagerModuleLayout()
    {
        return {
            .contentRowHeight = Recipes::PatchManagerModule::kContentRowHeight,
            .interControlGap = Recipes::PatchManagerModule::kInterControlGap,
            .columnGap = Recipes::PatchManagerModule::kColumnGap,
        };
    }

    BankUtilityPanelDimensions buildBankUtilityPanelDimensions(
        int width, int height, const ModuleHeaderDimensions& moduleHeader, const ButtonDimensions& buttons)
    {
        return {
            .width = width,
            .height = height,
            .moduleHeader = moduleHeader,
            .bankSelectorLabel = {
                .modulationBusNumberWidth = Atoms::Widths::Label::kModulationBusNumber,
                .patchManagerSelectBankWidth = Atoms::Widths::Label::kPatchManagerSelectBank,
                .patchMutatorWidth = Atoms::Widths::Label::kPatchMutator,
                .height = Atoms::Heights::kLabel,
            },
            .buttons = buttons,
            .layout = buildPatchManagerModuleLayout(),
        };
    }

    InternalPatchesPanelDimensions buildInternalPatchesPanelDimensions(
        int width, int height, const ModuleHeaderDimensions& moduleHeader, const ButtonDimensions& buttons)
    {
        return {
            .width = width,
            .height = height,
            .moduleHeader = moduleHeader,
            .groupLabels = {
                .internalPatchesBrowserWidth = Atoms::Widths::GroupLabel::kInternalPatchesBrowser,
                .internalPatchesMemoryWidth = Atoms::Widths::GroupLabel::kInternalPatchesMemory,
                .computerPatchesBrowserWidth = Atoms::Widths::GroupLabel::kComputerPatchesBrowser,
                .computerPatchesStorageWidth = Atoms::Widths::GroupLabel::kComputerPatchesStorage,
                .height = Atoms::Heights::kGroupLabel,
            },
            .numberBoxes = {
                .bankNumberWidth = Atoms::Widths::NumberBox::kPatchManagerBankNumber,
                .patchNumberWidth = Atoms::Widths::NumberBox::kPatchManagerPatchNumber,
                .height = Atoms::Heights::kNumberBox,
            },
            .buttons = buttons,
            .layout = buildPatchManagerModuleLayout(),
        };
    }

    ComputerPatchesPanelDimensions buildComputerPatchesPanelDimensions(
        int width, int height, const ModuleHeaderDimensions& moduleHeader, const ButtonDimensions& buttons)
    {
        return {
            .width = width,
            .height = height,
            .moduleHeader = moduleHeader,
            .groupLabels = {
                .internalPatchesBrowserWidth = Atoms::Widths::GroupLabel::kInternalPatchesBrowser,
                .internalPatchesMemoryWidth = Atoms::Widths::GroupLabel::kInternalPatchesMemory,
                .computerPatchesBrowserWidth = Atoms::Widths::GroupLabel::kComputerPatchesBrowser,
                .computerPatchesStorageWidth = Atoms::Widths::GroupLabel::kComputerPatchesStorage,
                .height = Atoms::Heights::kGroupLabel,
            },
            .comboBoxes = {
                .standardHeight = Atoms::Heights::kComboBox,
                .matrixModulationSourceWidth = Atoms::Widths::ComboBox::kMatrixModulationSource,
                .matrixModulationDestinationWidth = Atoms::Widths::ComboBox::kMatrixModulationDestination,
                .patchManagerComputerPatchesWidth = Atoms::Widths::ComboBox::kPatchManagerComputerPatches,
                .patchMutatorHistoryMWidth = Atoms::Widths::ComboBox::kPatchMutatorHistoryM,
                .patchMutatorHistoryRWidth = Atoms::Widths::ComboBox::kPatchMutatorHistoryR,
                .popup = buildPopupMenuLayoutDimensions(),
            },
            .buttons = buttons,
            .layout = buildPatchManagerModuleLayout(),
        };
    }

    PatchMutatorPanelDimensions buildPatchMutatorPanelDimensions(
        int width, int height, const ModuleHeaderDimensions& moduleHeader, const ButtonDimensions& buttons)
    {
        return {
            .width = width,
            .height = height,
            .moduleHeader = moduleHeader,
            .labels = {
                .modulationBusNumberWidth = Atoms::Widths::Label::kModulationBusNumber,
                .patchManagerSelectBankWidth = Atoms::Widths::Label::kPatchManagerSelectBank,
                .patchMutatorWidth = Atoms::Widths::Label::kPatchMutator,
                .height = Atoms::Heights::kLabel,
            },
            .sliders = {
                .standardWidth = Recipes::Slider::kStandard,
                .standardHeight = Atoms::Heights::kSlider,
                .patchMutatorWidth = Atoms::Widths::Slider::kPatchMutator,
            },
            .toggles = {
                .patchMutatorWidth = Atoms::Widths::Toggle::kPatchMutator,
                .height = Atoms::Heights::kToggle,
            },
            .comboBoxes = {
                .standardHeight = Atoms::Heights::kComboBox,
                .matrixModulationSourceWidth = Atoms::Widths::ComboBox::kMatrixModulationSource,
                .matrixModulationDestinationWidth = Atoms::Widths::ComboBox::kMatrixModulationDestination,
                .patchManagerComputerPatchesWidth = Atoms::Widths::ComboBox::kPatchManagerComputerPatches,
                .patchMutatorHistoryMWidth = Atoms::Widths::ComboBox::kPatchMutatorHistoryM,
                .patchMutatorHistoryRWidth = Atoms::Widths::ComboBox::kPatchMutatorHistoryR,
                .popup = buildPopupMenuLayoutDimensions(),
            },
            .buttons = buttons,
            .layout = buildPatchManagerModuleLayout(),
        };
    }
}

PluginEditorDimensions DimensionFactory::buildPluginEditorDimensions()
{
    using namespace TSS::Design;
    return {
        .width = GUI::kWidth,
        .height = GUI::kHeight,
        .headerHeight = Panels::Header::kHeight,
        .bodyHeight = Panels::Body::kHeight,
        .footerHeight = Panels::Footer::kHeight,
        .bodyPadding = Panels::Body::kPadding,
    };
}

GuiLayoutDimensions DimensionFactory::buildGuiLayoutDimensions()
{
    using namespace TSS::Design;
    namespace PES = Panels::Body::PatchEditSection;
    namespace MMS = Panels::Body::MatrixModulationSection;
    namespace PMS = Panels::Body::PatchManagerSection;
    namespace MES = Panels::Body::MasterEditSection;

    const auto buttons = buildButtonDimensions();
    const auto patchEditParameterCell = buildPatchEditParameterCellDimensions();
    const auto masterEditParameterCell = buildMasterEditParameterCellDimensions();
    const auto patchEditModuleHeader = buildPatchEditModuleHeaderDimensions(buttons);
    const auto masterEditModuleHeader = patchEditModuleHeader;

    const int patchManagerModuleWidth = PMS::kWidth;

    BankUtilityPanelDimensions bankUtility = buildBankUtilityPanelDimensions(
        patchManagerModuleWidth, PMS::BankUtilityModule::kHeight, patchEditModuleHeader, buttons);
    InternalPatchesPanelDimensions internalPatches = buildInternalPatchesPanelDimensions(
        patchManagerModuleWidth, PMS::InternalPatchesModule::kHeight, patchEditModuleHeader, buttons);
    ComputerPatchesPanelDimensions computerPatches = buildComputerPatchesPanelDimensions(
        patchManagerModuleWidth, PMS::ComputerPatchesModule::kHeight, patchEditModuleHeader, buttons);
    PatchMutatorPanelDimensions patchMutator = buildPatchMutatorPanelDimensions(
        patchManagerModuleWidth, PMS::PatchMutatorModule::kHeight, patchEditModuleHeader, buttons);

    PatchManagerPanelDimensions patchManager {
        .width = PMS::kWidth,
        .height = PMS::kHeight,
        .sectionHeaderWidth = PanelWidgets::Widths::SectionHeader::kPatchManager,
        .sectionHeaderHeight = Atoms::Heights::kSectionHeader,
        .moduleStackGap = PMS::kModuleStackGap,
        .bankUtilityHeight = PMS::BankUtilityModule::kHeight,
        .internalPatchesHeight = PMS::InternalPatchesModule::kHeight,
        .computerPatchesHeight = PMS::ComputerPatchesModule::kHeight,
        .patchMutatorHeight = PMS::PatchMutatorModule::kHeight,
        .bankUtility = std::move(bankUtility),
        .internalPatches = std::move(internalPatches),
        .computerPatches = std::move(computerPatches),
        .patchMutator = std::move(patchMutator),
    };

    const auto busCell = buildModulationBusCellDimensions(buttons);
    MatrixModulationPanelDimensions matrixModulation {
        .width = MMS::kWidth,
        .height = MMS::kHeight,
        .modulationBusRowHeight = Atoms::Heights::kLabel + Atoms::Heights::kHorizontalSeparator,
        .sectionHeaderWidth = PanelWidgets::Widths::SectionHeader::kMatrixModulation,
        .sectionHeaderHeight = Atoms::Heights::kSectionHeader,
        .busHeaderWidth = PanelWidgets::Widths::ModulationBusHeader::kStandard,
        .busHeaderHeight = Atoms::Heights::kModulationBusHeader,
        .initWidth = buttons.initWidth,
        .copyWidth = buttons.copyWidth,
        .pasteWidth = buttons.pasteWidth,
        .buttonHeight = buttons.height,
        .busHeader = buildModulationBusHeaderDimensions(),
        .busCell = busCell,
    };

    SharedPanelDimensions shared {
        .width = Panels::Body::SharedColumn::kWidth,
        .height = Panels::Body::SharedColumn::kSharedPanelHeight,
        .verticalStackGap = Panels::Body::SharedColumn::kVerticalStackGap,
        .matrixModulationHeight = MMS::kHeight,
        .patchManagerHeight = PMS::kHeight,
        .matrixModulation = std::move(matrixModulation),
        .patchManager = std::move(patchManager),
    };

    const int childBandWidth = PES::MiddleModules::ChildModules::kWidth;
    const int childBandHeight = PES::MiddleModules::ChildModules::kHeight;
    PatchEditDisplaysPanelDimensions displays {
        .width = PES::MiddleModules::kWidth,
        .height = PES::MiddleModules::kHeight,
        .childBand = buildDisplayBandDimensions(childBandWidth, childBandHeight, true),
        .trackGeneratorBand = buildDisplayBandDimensions(childBandWidth, childBandHeight, false),
        .patchName = {
            .width = PES::MiddleModules::ChildModules::kWidth,
            .height = Atoms::Heights::kPatchNameDisplay,
            .topPadding = Recipes::PatchNameModule::kTopPadding,
            .moduleHeaderToDisplayGap = Recipes::PatchNameModule::kModuleHeaderToDisplayGap,
            .bottomPadding = Recipes::PatchNameModule::kBottomPadding,
        },
        .moduleHeader = patchEditModuleHeader,
        .interModuleGap = PES::kInterModuleGap,
    };

    PatchEditModulesRowDimensions modulesRow {
        .childModuleWidth = PES::TopModules::ChildModules::kWidth,
        .childModuleHeight = PES::TopModules::ChildModules::kHeight,
        .interModuleGap = PES::kInterModuleGap,
    };

    PatchEditPanelDimensions patchEdit {
        .width = PES::kWidth,
        .height = PES::kHeight,
        .topHeight = PES::TopModules::kHeight,
        .middleHeight = PES::MiddleModules::kHeight,
        .bottomHeight = PES::BottomModules::kHeight,
        .sectionHeaderWidth = PanelWidgets::Widths::SectionHeader::kPatchEdit,
        .sectionHeaderHeight = Atoms::Heights::kSectionHeader,
        .topModules = modulesRow,
        .bottomModules = modulesRow,
        .displays = displays,
    };

    MasterEditPanelDimensions masterEdit {
        .width = MES::kWidth,
        .height = MES::kHeight,
        .childModuleWidth = MES::ChildModules::kWidth,
        .interModuleGap = MES::kInterModuleGap,
        .midiPanelHeight = MES::MidiModule::kHeight,
        .vibratoPanelHeight = MES::VibratoModule::kHeight,
        .miscPanelHeight = MES::MiscModule::kHeight,
        .sectionHeaderWidth = PanelWidgets::Widths::SectionHeader::kMasterEdit,
        .sectionHeaderHeight = Atoms::Heights::kSectionHeader,
        .parameterCell = masterEditParameterCell,
        .moduleHeader = masterEditModuleHeader,
    };

    BodyPanelDimensions body {
        .width = Panels::Body::kWidth,
        .height = Panels::Body::kHeight,
        .padding = Panels::Body::kPadding,
        .patchEditWidth = PES::kWidth,
        .patchEditHeight = PES::kHeight,
        .masterEditWidth = MES::kWidth,
        .masterEditHeight = MES::kHeight,
        .sharedColumnWidth = Panels::Body::SharedColumn::kWidth,
        .sharedColumnHeight = Panels::Body::SharedColumn::kSharedPanelHeight,
        .separators = {
            .verticalStandardWidth = Atoms::Widths::VerticalSeparator::kStandard,
            .verticalStandardHeight = PanelWidgets::Heights::kVerticalSeparator,
            .horizontalHeight = Atoms::Heights::kHorizontalSeparator,
            .verticalLineWidth = Atoms::Widths::VerticalSeparator::kLineWidth,
            .verticalTopPadding = Atoms::Widths::VerticalSeparator::kTopPadding,
            .verticalBottomPadding = Atoms::Widths::VerticalSeparator::kBottomPadding,
        },
        .patchEdit = std::move(patchEdit),
        .shared = std::move(shared),
        .masterEdit = std::move(masterEdit),
    };

    return {
        .editor = buildPluginEditorDimensions(),
        .header = { .width = Panels::Header::kWidth, .height = Panels::Header::kHeight },
        .footer = { .width = Panels::Footer::kWidth, .height = Panels::Footer::kHeight },
        .body = std::move(body),
        .patchEditParameterCell = patchEditParameterCell,
        .masterEditParameterCell = masterEditParameterCell,
        .patchEditModuleHeader = patchEditModuleHeader,
        .masterEditModuleHeader = masterEditModuleHeader,
        .popupMenu = buildPopupMenuLayoutDimensions(),
        .activityLed = {
            .size = Atoms::Widths::Indicator::kLedSize,
            .borderThicknessDesign = 2.0f,
        },
    };
}
