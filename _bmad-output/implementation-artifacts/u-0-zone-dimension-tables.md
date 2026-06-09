---
organization: Ten Square Software
project: Matrix-Control
title: U-0 Zone Dimension Tables
author: BMad Agent
status: final
sources:
  - implementation-artifacts/u-0-figma-intake-and-design-reconciliation.md
  - _local/Documents/Notes/figma-mockup.md
  - Source/GUI/Layout/Design/DesignPanels.h
created: 2026-06-09
updated: 2026-06-09
---

# U-0 Zone Dimension Tables

Reference values @ 100 % UI scale (÷4 grid). Symbol paths use `tss::design` namespace.

## Shell (Body, MainComponent, separators)

| Design px @ 100 % | Design* symbol path | UAT story |
|-------------------|---------------------|-----------|
| 1300 × 792 | `GUI::kWidth` × `GUI::kHeight` | U-5 |
| 1300 × 728 | `GUI::kWidth` × `GUI::kBodyHeight` | U-5 |
| 12 | `Panels::Body::kPadding` / `GUI::kPadding` | U-5 |
| 1276 | `GUI::kBodyInnerWidth` | U-5 |
| 704 | `Panels::Body::kEffectiveHeight` | U-5 |
| 24 × 704 | `Atoms::Widths::VerticalSeparator::kStandard` × `PanelWidgets::Heights::kVerticalSeparator` | U-5 |

## Header / Footer

| Design px @ 100 % | Design* symbol path | UAT story |
|-------------------|---------------------|-----------|
| 1300 × 32 | `Panels::Header::kWidth` × `Panels::Header::kHeight` | U-3 |
| 1300 × 32 | `Panels::Footer::kWidth` × `Panels::Footer::kHeight` | U-4 |
| 68 | `Atoms::Widths::Button::kHeaderPanelTheme` | U-3 |

## Patch Edit (top / middle / bottom)

| Design px @ 100 % | Design* symbol path | UAT story |
|-------------------|---------------------|-----------|
| 808 × 704 | `Panels::Body::PatchEditSection::kWidth` × `kHeight` | U-6 |
| 808 × 272 | `PatchEditSection::TopModules::kWidth` × `kHeight` | U-6 |
| 808 × 128 | `PatchEditSection::MiddleModules::kWidth` × `kHeight` | U-6 |
| 808 × 272 | `PatchEditSection::BottomModules::kWidth` × `kHeight` | U-6 |
| 152 × 272 | `Recipes::PatchEditModule::kWidth` × `kHeight` | U-6 |
| 152 × 128 | `MiddleModules::ChildModules::kWidth` × `kHeight` | U-6 |
| 12 | `PatchEditSection::kInterModuleGap` | U-6 |
| 152 × 24 | `Recipes::ParameterCell::kWidth` × `kHeight` | U-2, U-6 |
| 152 × 32 | `Recipes::ParameterCore::kWidth` × `Atoms::Heights::kModuleHeader` | U-2 |

## Matrix Mod

| Design px @ 100 % | Design* symbol path | UAT story |
|-------------------|---------------------|-----------|
| 268 × 304 | `Panels::Body::MatrixModulationSection::kWidth` × `kHeight` | U-7 |
| 268 × 24 | `Recipes::ModulationBusCell::kWidth` × `kHeight` | U-2, U-7 |
| 268 | `Recipes::ModulationBusRow::kWidth` | U-7 |
| 268 × 32 | `PanelWidgets::Widths::ModulationBusHeader::kStandard` × `Atoms::Heights::kModulationBusHeader` | U-7 |

## Patch Manager (4 modules)

| Design px @ 100 % | Design* symbol path | UAT story |
|-------------------|---------------------|-----------|
| 268 × 400 | `Panels::Body::PatchManagerSection::kWidth` × `kHeight` | U-8 |
| 268 × 84 | `BankUtilityModule::kHeight` (width = section width) | U-8 |
| 268 × 84 | `InternalPatchesModule::kHeight` | U-8 |
| 268 × 84 | `ComputerPatchesModule::kHeight` | U-8 |
| 268 × 116 | `PatchMutatorModule::kHeight` (100 content + 16 closure) | U-8 |
| 24 | `Panels::Body::SharedColumn::kInterPanelGap` | U-8 |

## Master Edit

| Design px @ 100 % | Design* symbol path | UAT story |
|-------------------|---------------------|-----------|
| 152 × 704 | `Panels::Body::MasterEditSection::kWidth` × `kHeight` | U-9 |
| 152 × 224 | `MasterEditSection::MidiModule::kHeight` | U-9 |
| 152 × 200 | `MasterEditSection::VibratoModule::kHeight` | U-9 |
| 152 × 224 | `MasterEditSection::MiscModule::kHeight` | U-9 |
| 12 | `MasterEditSection::kInterModuleGap` | U-9 |

## Transversal widgets (U-2)

| Design px @ 100 % | Design* symbol path | UAT story |
|-------------------|---------------------|-----------|
| 92 × 16 | `Atoms::Widths::ParameterRow::kLabel` × `Atoms::Heights::kLabel` | U-2 |
| 60 × 16 | `Recipes::ParameterRow::kControl` × `Atoms::Heights::kSlider` | U-2 |
| 20 × 20 | `Atoms::Widths::Button::kInit` × `Atoms::Heights::kButton` | U-2 |
| 8 | `Atoms::Heights::kHorizontalSeparator` | U-2 |
| 4 / 8 / 12 | `Spacing::kStandard` / `kMedium` / `kLarge` | U-2 |
