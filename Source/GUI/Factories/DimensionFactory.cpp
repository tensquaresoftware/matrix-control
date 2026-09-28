#include "DimensionFactory.h"

#include <utility>

#include "DimensionFactoryBuilders.h"
#include "GUI/Layout/Design/Design.h"

namespace
{
    using namespace TSS::Design;
    using namespace DimensionFactoryBuilders;

    PatchManagerPanelDimensions buildPatchManagerPanelDimensions(
        const ModuleHeaderDimensions& moduleHeader, const ButtonDimensions& buttons)
    {
        namespace PMS = Panels::Body::PatchManagerSection;
        const int moduleWidth = PMS::kWidth;

        return {
            .width = PMS::kWidth,
            .height = PMS::kHeight,
            .sectionHeaderWidth = PanelWidgets::Widths::SectionHeader::kPatchManager,
            .sectionHeaderHeight = Atoms::Heights::kSectionHeader,
            .moduleStackGap = PMS::kModuleStackGap,
            .bankUtilityHeight = PMS::BankUtilityModule::kHeight,
            .internalPatchesHeight = PMS::InternalPatchesModule::kHeight,
            .computerPatchesHeight = PMS::ComputerPatchesModule::kHeight,
            .patchMutatorHeight = PMS::PatchMutatorModule::kHeight,
            .bankUtility = buildBankUtilityPanelDimensions(
                moduleWidth, PMS::BankUtilityModule::kHeight, moduleHeader, buttons),
            .internalPatches = buildInternalPatchesPanelDimensions(
                moduleWidth, PMS::InternalPatchesModule::kHeight, moduleHeader, buttons),
            .computerPatches = buildComputerPatchesPanelDimensions(
                moduleWidth, PMS::ComputerPatchesModule::kHeight, moduleHeader, buttons),
            .patchMutator = buildPatchMutatorPanelDimensions(
                moduleWidth, PMS::PatchMutatorModule::kHeight, moduleHeader, buttons),
        };
    }

    MatrixModulationPanelDimensions buildMatrixModulationPanelDimensions(const ButtonDimensions& buttons)
    {
        namespace MMS = Panels::Body::MatrixModulationSection;

        return {
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
            .busCell = buildModulationBusCellDimensions(buttons),
        };
    }

    SharedPanelDimensions buildSharedPanelDimensions(
        const ModuleHeaderDimensions& moduleHeader, const ButtonDimensions& buttons)
    {
        namespace MMS = Panels::Body::MatrixModulationSection;
        namespace PMS = Panels::Body::PatchManagerSection;

        return {
            .width = Panels::Body::SharedColumn::kPanelWidth,
            .height = Panels::Body::SharedColumn::kPanelHeight,
            .padding = Panels::Body::kColumnPadding,
            .verticalStackGap = Panels::Body::SharedColumn::kVerticalStackGap,
            .matrixModulationHeight = MMS::kHeight,
            .patchManagerHeight = PMS::kHeight,
            .matrixModulation = buildMatrixModulationPanelDimensions(buttons),
            .patchManager = buildPatchManagerPanelDimensions(moduleHeader, buttons),
        };
    }

    PatchEditDisplaysPanelDimensions buildPatchEditDisplaysPanelDimensions(
        const ModuleHeaderDimensions& moduleHeader)
    {
        namespace PES = Panels::Body::PatchEditSection;
        const int childBandWidth = PES::MiddleModules::ChildModules::kWidth;
        const int childBandHeight = PES::MiddleModules::ChildModules::kHeight;

        return {
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
            .moduleHeader = moduleHeader,
            .interModuleGap = PES::kInterModuleGap,
        };
    }

    PatchEditPanelDimensions buildPatchEditPanelDimensions(const ModuleHeaderDimensions& moduleHeader)
    {
        namespace PES = Panels::Body::PatchEditSection;
        const PatchEditModulesRowDimensions modulesRow {
            .childModuleWidth = PES::TopModules::ChildModules::kWidth,
            .childModuleHeight = PES::TopModules::ChildModules::kHeight,
            .interModuleGap = PES::kInterModuleGap,
        };

        return {
            .width = PES::kPanelWidth,
            .height = PES::kPanelHeight,
            .padding = Panels::Body::kColumnPadding,
            .topHeight = PES::TopModules::kHeight,
            .middleHeight = PES::MiddleModules::kHeight,
            .bottomHeight = PES::BottomModules::kHeight,
            .sectionHeaderWidth = PanelWidgets::Widths::SectionHeader::kPatchEdit,
            .sectionHeaderHeight = Atoms::Heights::kSectionHeader,
            .topModules = modulesRow,
            .bottomModules = modulesRow,
            .displays = buildPatchEditDisplaysPanelDimensions(moduleHeader),
        };
    }

    MasterEditPanelDimensions buildMasterEditPanelDimensions(
        const ParameterCellDimensions& parameterCell, const ModuleHeaderDimensions& moduleHeader)
    {
        namespace MES = Panels::Body::MasterEditSection;

        return {
            .width = MES::kPanelWidth,
            .height = MES::kPanelHeight,
            .padding = Panels::Body::kColumnPadding,
            .childModuleWidth = MES::ChildModules::kWidth,
            .interModuleGap = MES::kInterModuleGap,
            .midiPanelHeight = MES::MidiModule::kHeight,
            .vibratoPanelHeight = MES::VibratoModule::kHeight,
            .miscPanelHeight = MES::MiscModule::kHeight,
            .sectionHeaderWidth = PanelWidgets::Widths::SectionHeader::kMasterEdit,
            .sectionHeaderHeight = Atoms::Heights::kSectionHeader,
            .parameterCell = parameterCell,
            .moduleHeader = moduleHeader,
        };
    }

    BodyPanelDimensions buildBodyPanelDimensions(
        PatchEditPanelDimensions patchEdit,
        SharedPanelDimensions shared,
        MasterEditPanelDimensions masterEdit)
    {
        namespace PES = Panels::Body::PatchEditSection;
        namespace MES = Panels::Body::MasterEditSection;

        return {
            .width = Panels::Body::kWidth,
            .height = Panels::Body::kHeight,
            .interColumnGap = Panels::Body::kInterColumnGap,
            .patchEditWidth = PES::kPanelWidth,
            .patchEditHeight = PES::kPanelHeight,
            .masterEditWidth = MES::kPanelWidth,
            .masterEditHeight = MES::kPanelHeight,
            .sharedColumnWidth = Panels::Body::SharedColumn::kPanelWidth,
            .sharedColumnHeight = Panels::Body::SharedColumn::kPanelHeight,
            .patchEdit = std::move(patchEdit),
            .shared = std::move(shared),
            .masterEdit = std::move(masterEdit),
        };
    }

    HeaderPanelDimensions buildHeaderPanelDimensions()
    {
        return {
            .width = Panels::Header::kWidth,
            .height = Panels::Header::kHeight,
            .logoWidth = Panels::Header::kLogoWidth,
            .logoHeight = Panels::Header::kLogoHeight,
            .logoVerticalOffset = Panels::Header::kLogoVerticalOffset,
            .contentVerticalOffset = Panels::Header::kContentVerticalOffset,
            .logoGapAfter = Panels::Header::kLogoGapAfter,
            .logoPopupColumnWidth = Panels::Header::kLogoPopupColumnWidth,
            .leftPadding = Panels::Header::kLeftPadding,
            .controlHeight = Panels::Header::kControlHeight,
            .ledSize = Atoms::Widths::Indicator::kLedSize,
            .gap = Panels::Header::kGap,
            .packetExternalGap = Panels::Header::kPacketExternalGap,
            .labelToControlGap = Panels::Header::kLabelToControlGap,
            .keyboardFromLabelToComboGap = Panels::Header::kKeyboardFromLabelToComboGap,
            .audioFromLabelToComboGap = Panels::Header::kAudioFromLabelToComboGap,
            .editorMidiFromLabelWidth = Panels::Header::kEditorMidiFromLabelWidth,
            .midiToLabelWidth = Panels::Header::kMidiToLabelWidth,
            .keyboardFromLabelWidth = Panels::Header::kKeyboardFromLabelWidth,
            .audioFromLabelWidth = Panels::Header::kAudioFromLabelWidth,
            .inputGainLabelWidth = Panels::Header::kInputGainLabelWidth,
            .inputGainLabelToSliderGap = Panels::Header::kInputGainLabelToSliderGap,
            .portComboBoxWidth = Panels::Header::kPortComboBoxWidth,
            .inputGainSliderWidth = Panels::Header::kInputGainSliderWidth,
            .peakIndicatorWidth = Panels::Header::kPeakIndicatorWidth,
            .rightPadding = Panels::Header::kRightPadding,
            .panicButtonWidth = Panels::Header::kPanicButtonWidth,
            .undoButtonWidth = Panels::Header::kUndoButtonWidth,
            .redoButtonWidth = Panels::Header::kRedoButtonWidth,
            .redoToPanicGap = Panels::Header::kRedoToPanicGap,
        };
    }

    FooterPanelDimensions buildFooterPanelDimensions()
    {
        return {
            .width = Panels::Footer::kWidth,
            .height = Panels::Footer::kHeight,
            .padding = Panels::Footer::kPadding,
            .iconSize = Panels::Footer::kIconSize,
            .bandHeight = Panels::Footer::kBandHeight,
            .bandVerticalInset = Panels::Footer::kBandVerticalInset,
            .patchEditPanelWidth = Panels::Body::PatchEditSection::kPanelWidth,
            .sharedPanelWidth = Panels::Body::SharedColumn::kPanelWidth,
            .masterEditPanelWidth = Panels::Body::MasterEditSection::kPanelWidth,
            .interColumnGap = Panels::Body::kInterColumnGap,
            .severityBadgeHeight = Panels::Footer::kSeverityBadgeHeight,
            .severityBadgeHorizontalPadding = Panels::Footer::kSeverityBadgeHorizontalPadding,
            .severityBadgeToMessageGap = Panels::Footer::kSeverityBadgeToMessageGap,
        };
    }

    IndicatorDimensions buildActivityLedDimensions()
    {
        return {
            .size = Atoms::Widths::Indicator::kLedSize,
            .borderThicknessDesign = 2.0f,
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
    };
}

GuiLayoutDimensions DimensionFactory::buildGuiLayoutDimensions()
{
    using namespace DimensionFactoryBuilders;

    const auto buttons = buildButtonDimensions();
    const auto patchEditParameterCell = buildPatchEditParameterCellDimensions();
    const auto masterEditParameterCell = buildMasterEditParameterCellDimensions();
    const auto patchEditModuleHeader = buildPatchEditModuleHeaderDimensions(buttons);
    const auto masterEditModuleHeader = patchEditModuleHeader;

    return {
        .editor = buildPluginEditorDimensions(),
        .header = buildHeaderPanelDimensions(),
        .footer = buildFooterPanelDimensions(),
        .body = buildBodyPanelDimensions(
            buildPatchEditPanelDimensions(patchEditModuleHeader),
            buildSharedPanelDimensions(patchEditModuleHeader, buttons),
            buildMasterEditPanelDimensions(masterEditParameterCell, masterEditModuleHeader)),
        .patchEditParameterCell = patchEditParameterCell,
        .masterEditParameterCell = masterEditParameterCell,
        .patchEditModuleHeader = patchEditModuleHeader,
        .masterEditModuleHeader = masterEditModuleHeader,
        .popupMenu = buildPopupMenuLayoutDimensions(),
        .activityLed = buildActivityLedDimensions(),
    };
}
