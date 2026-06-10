#pragma once

#include "WidgetDimensions.h"

struct PluginEditorDimensions
{
    int width;
    int height;
    int headerHeight;
    int bodyHeight;
    int footerHeight;
    int bodyPadding;
};

struct HeaderPanelDimensions
{
    int width;
    int height;
};

struct FooterPanelDimensions
{
    int width;
    int height;
};

struct PatchEditModulesRowDimensions
{
    int childModuleWidth;
    int childModuleHeight;
    int interModuleGap;
};

struct PatchEditDisplaysPanelDimensions
{
    int width;
    int height;
    DisplayBandDimensions childBand;
    DisplayBandDimensions trackGeneratorBand;
    PatchNameDisplayDimensions patchName;
    ModuleHeaderDimensions moduleHeader;
    int interModuleGap;
};

struct PatchEditPanelDimensions
{
    int width;
    int height;
    int topHeight;
    int middleHeight;
    int bottomHeight;
    int sectionHeaderWidth;
    int sectionHeaderHeight;
    PatchEditModulesRowDimensions topModules;
    PatchEditModulesRowDimensions bottomModules;
    PatchEditDisplaysPanelDimensions displays;
};

struct MatrixModulationPanelDimensions
{
    int width;
    int height;
    int modulationBusRowHeight;
    int sectionHeaderWidth;
    int sectionHeaderHeight;
    int busHeaderWidth;
    int busHeaderHeight;
    int initAllButtonWidth;
    int initAllButtonHeight;
    ModulationBusHeaderDimensions busHeader;
    ModulationBusCellDimensions busCell;
};

struct BankUtilityPanelDimensions
{
    int width;
    int height;
    ModuleHeaderDimensions moduleHeader;
    LabelDimensions bankSelectorLabel;
    ButtonDimensions buttons;
    PatchManagerModuleLayoutDimensions layout;
};

struct InternalPatchesPanelDimensions
{
    int width;
    int height;
    ModuleHeaderDimensions moduleHeader;
    GroupLabelDimensions groupLabels;
    NumberBoxDimensions numberBoxes;
    ButtonDimensions buttons;
    PatchManagerModuleLayoutDimensions layout;
};

struct ComputerPatchesPanelDimensions
{
    int width;
    int height;
    ModuleHeaderDimensions moduleHeader;
    GroupLabelDimensions groupLabels;
    ComboBoxDimensions comboBoxes;
    ButtonDimensions buttons;
    PatchManagerModuleLayoutDimensions layout;
};

struct PatchMutatorPanelDimensions
{
    int width;
    int height;
    ModuleHeaderDimensions moduleHeader;
    LabelDimensions labels;
    SliderDimensions sliders;
    ToggleDimensions toggles;
    ComboBoxDimensions comboBoxes;
    ButtonDimensions buttons;
    PatchManagerModuleLayoutDimensions layout;
};

struct PatchManagerPanelDimensions
{
    int width;
    int height;
    int sectionHeaderWidth;
    int sectionHeaderHeight;
    int moduleStackGap;
    int bankUtilityHeight;
    int internalPatchesHeight;
    int computerPatchesHeight;
    int patchMutatorHeight;
    BankUtilityPanelDimensions bankUtility;
    InternalPatchesPanelDimensions internalPatches;
    ComputerPatchesPanelDimensions computerPatches;
    PatchMutatorPanelDimensions patchMutator;
};

struct SharedPanelDimensions
{
    int width;
    int height;
    int verticalStackGap;
    int matrixModulationHeight;
    int patchManagerHeight;
    MatrixModulationPanelDimensions matrixModulation;
    PatchManagerPanelDimensions patchManager;
};

struct MasterEditPanelDimensions
{
    int width;
    int height;
    int childModuleWidth;
    int interModuleGap;
    int midiPanelHeight;
    int vibratoPanelHeight;
    int miscPanelHeight;
    int sectionHeaderWidth;
    int sectionHeaderHeight;
    ParameterCellDimensions parameterCell;
    ModuleHeaderDimensions moduleHeader;
};

struct BodyPanelDimensions
{
    int width;
    int height;
    int padding;
    int patchEditWidth;
    int patchEditHeight;
    int masterEditWidth;
    int masterEditHeight;
    int sharedColumnWidth;
    int sharedColumnHeight;
    SeparatorDimensions separators;
    PatchEditPanelDimensions patchEdit;
    SharedPanelDimensions shared;
    MasterEditPanelDimensions masterEdit;
};

struct GuiLayoutDimensions
{
    PluginEditorDimensions editor;
    HeaderPanelDimensions header;
    FooterPanelDimensions footer;
    BodyPanelDimensions body;
    ParameterCellDimensions patchEditParameterCell;
    ParameterCellDimensions masterEditParameterCell;
    ModuleHeaderDimensions patchEditModuleHeader;
    ModuleHeaderDimensions masterEditModuleHeader;
    PopupMenuLayoutDimensions popupMenu;
    IndicatorDimensions activityLed;
};
