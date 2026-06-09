#pragma once

// Runtime widget dimension structs at UI display scale 1.0 (design px).
// Populated by DimensionFactory from Design* headers — panels/widgets consume these only.

struct ButtonDimensions
{
    int initWidth;
    int copyWidth;
    int pasteWidth;
    int height;

    int patchManagerBankSelectWidth;
    int patchManagerLockBankWidth;

    int internalPatchesInitWidth;
    int internalPatchesCopyWidth;
    int internalPatchesPasteWidth;
    int internalPatchesStoreWidth;

    int computerPatchesLoadWidth;
    int computerPatchesSaveWidth;
    int computerPatchesSaveAsWidth;

    int patchMutatorMutateWidth;
    int patchMutatorCompareWidth;
    int patchMutatorDeleteWidth;
    int patchMutatorClearWidth;
    int patchMutatorExportWidth;
};

struct SliderDimensions
{
    int standardWidth;
    int standardHeight;
    int patchMutatorWidth;
};

struct ComboBoxDimensions
{
    int standardHeight;
    int matrixModulationSourceWidth;
    int matrixModulationDestinationWidth;
    int patchManagerComputerPatchesWidth;
    int patchMutatorHistoryWidth;
};

struct ParameterCellDimensions
{
    int rowHeight;
    int labelWidth;
    int controlWidth;
    int separatorWidth;
    int labelHeight;
    int sliderHeight;
    int comboBoxHeight;
    int horizontalSeparatorHeight;
};

struct ModuleHeaderDimensions
{
    int height;
    int buttonHeight;
    int patchEditTitleBandWidth;
    int masterEditTitleBandWidth;
    int patchManagerTitleBandWidth;
    int initWidth;
    int copyWidth;
    int pasteWidth;
};

struct ModulationBusCellDimensions
{
    int panelWidth;
    int panelHeight;
    int busNumberLabelWidth;
    int busNumberLabelHeight;
    int sourceComboBoxWidth;
    int sourceComboBoxHeight;
    int amountSliderWidth;
    int amountSliderHeight;
    int destinationComboBoxWidth;
    int destinationComboBoxHeight;
    int initButtonWidth;
    int initButtonHeight;
    int separatorWidth;
    int separatorHeight;
};

struct SeparatorDimensions
{
    int verticalStandardWidth;
    int verticalStandardHeight;
    int horizontalHeight;
};

struct DisplayBandDimensions
{
    int width;
    int height;
};

struct PatchNameDisplayDimensions
{
    int width;
    int height;
    int topPadding;
    int moduleHeaderToDisplayGap;
};

struct GroupLabelDimensions
{
    int internalPatchesBrowserWidth;
    int internalPatchesMemoryWidth;
    int computerPatchesBrowserWidth;
    int computerPatchesStorageWidth;
    int height;
};

struct NumberBoxDimensions
{
    int bankNumberWidth;
    int patchNumberWidth;
    int height;
};

struct LabelDimensions
{
    int modulationBusNumberWidth;
    int patchManagerBankSelectorWidth;
    int patchMutatorWidth;
    int height;
};

struct ToggleDimensions
{
    int patchMutatorWidth;
    int height;
};

struct PatchManagerModuleLayoutDimensions
{
    int contentRowHeight;
    int rowGap;
};
