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
    int patchManagerUnlockBankWidth;

    int internalPatchesInitWidth;
    int internalPatchesCopyWidth;
    int internalPatchesPasteWidth;
    int internalPatchesStoreWidth;

    int computerPatchesLoadWidth;
    int computerPatchesSaveWidth;
    int computerPatchesSaveAsWidth;

    int patchMutatorMutateWidth;
    int patchMutatorRetryWidth;
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

struct PopupMenuLayoutDimensions
{
    int itemHeight;
    int verticalMargin;
    int scrollbarWidth;
    int minThumbHeight;
    int maxScrollHeight;
    int borderThickness;
    int textLeftPadding;
};

struct ComboBoxDimensions
{
    int standardHeight;
    int matrixModulationSourceWidth;
    int matrixModulationDestinationWidth;
    int patchManagerComputerPatchesWidth;
    int patchMutatorHistoryMWidth;
    int patchMutatorHistoryRWidth;
    int patchMutatorHistoryWidth;
    PopupMenuLayoutDimensions popup;
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
    int textLeftPadding;
    int textAreaHeight;
    int lineThickness;
};

struct ModulationBusHeaderDimensions
{
    int busNumberTextWidth;
    int busSourceTextWidth;
    int busAmountTextWidth;
    int busDestinationTextWidth;
    int interControlGap;
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
    int interControlGap;
    int reorderDragThreshold;
};

struct SeparatorDimensions
{
    int verticalStandardWidth;
    int verticalStandardHeight;
    int horizontalHeight;
    int verticalLineWidth;
    int verticalTopPadding;
    int verticalBottomPadding;
};

struct DisplayBandDimensions
{
    int width;
    int height;
    int borderThickness;
    int paddingTop;
    int paddingBottom;
    int triangleBase;
    int curvePadding;
    int curvePointRadius;
    int curveLineThickness;
    int minCurveSegmentWidth;
    int pointHitZoneRadius;
    int sustainSegmentHitZone;
};

struct PatchNameDisplayDimensions
{
    int width;
    int height;
    int topPadding;
    int moduleHeaderToDisplayGap;
    int bottomPadding;
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
    int patchManagerSelectBankWidth;
    int patchMutatorWidth;
    int height;
};

struct ToggleDimensions
{
    int patchMutatorWidth;
    int height;
};

struct IndicatorDimensions
{
    int size;
    float borderThicknessDesign;
};

struct PatchManagerModuleLayoutDimensions
{
    int contentRowHeight;
    int interControlGap;
    int columnGap;
};
