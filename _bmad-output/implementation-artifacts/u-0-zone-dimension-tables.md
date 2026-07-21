---
organization: Ten Square Software
project: Matrix-Control
title: U-0 Zone Dimension Tables
author: BMad Agent
status: final
sources:
  - Source/GUI/Layout/Design/DesignAtoms.h
  - Source/GUI/Layout/Design/DesignRecipes.h
  - Source/GUI/Layout/Design/DesignPanels.h
  - Source/GUI/Layout/Design/DesignChecks.h
created: 2026-06-09
updated: 2026-07-21
---

# U-0 Zone Dimension Tables

Valeurs de référence @ 100 % (grille ÷4). Chemins de symboles : namespace `TSS::Design`.

## Shell (Body, MainComponent, séparateurs)

| px @ 100 % | Symbole Design* | Story UAT |
|------------|-------------------|-----------|
| 1300 × 792 | `GUI::kWidth` × `GUI::kHeight` | U-5 |
| 1276 × 728 | `GUI::kBodyInnerWidth` × `GUI::kBodyHeight` | U-5 |
| 12 | `Spacing::kLarge` / `Panels::Body::kPadding` | U-5 |
| 704 | `Panels::Body::kEffectiveHeight` | U-5 |
| 700 | `Panels::Body::SharedColumn::kHeight` | U-5, U-8 |
| 24 × 704 | `Atoms::Widths::VerticalSeparator::kStandard` × `PanelWidgets::Heights::kVerticalSeparator` | U-5 |

## Header / Footer

| px @ 100 % | Symbole Design* | Story UAT |
|------------|-------------------|-----------|
| 1300 × 40 | `Panels::Header::kWidth` × `Panels::Header::kHeight` | U-3 |
| 1300 × 32 | `Panels::Footer::kWidth` × `Panels::Footer::kHeight` | U-4 |

### FR-41 layout slots (header)

Logo is the left-side anchor. Skin / UI Scale live in the logo popup columns (`kLogoPopupColumnWidth`); UI Elements (Debug) is logo Shift+Ctrl. Implemented in Stories 7.8 / 7.10 — no on-bar SCALE/SKIN/UI Elements cluster.

## Patch Edit (haut / milieu / bas)

| px @ 100 % | Symbole Design* | Story UAT |
|------------|-------------------|-----------|
| 808 × 704 | `Panels::Body::PatchEditSection::kWidth` × `kHeight` | U-6 |
| 808 × 272 | `PatchEditSection::TopModules::kWidth` × `kHeight` | U-6 |
| 808 × 128 | `PatchEditSection::MiddleModules::kWidth` × `kHeight` | U-6 |
| 808 × 272 | `PatchEditSection::BottomModules::kWidth` × `kHeight` | U-6 |
| 152 × 272 | `Recipes::PatchEditModule::kWidth` × `kHeight` | U-6 |
| 12 | `PatchEditSection::kInterModuleGap` | U-6 |
| 152 × 24 | `Recipes::ParameterCell::kWidth` × `kHeight` | U-2, U-6 |
| 152 × 32 | `Recipes::ParameterCell::kWidth` × `Atoms::Heights::kModuleHeader` | U-2 |

## Matrix Mod

| px @ 100 % | Symbole Design* | Story UAT |
|------------|-------------------|-----------|
| 268 × 304 | `Panels::Body::MatrixModulationSection::kWidth` × `kHeight` | U-7 |
| 268 × 24 | `Recipes::ModulationBusCell::kWidth` × `kHeight` | U-2, U-7 |
| 268 × 32 | `PanelWidgets::Widths::ModulationBusHeader::kStandard` × `Atoms::Heights::kModulationBusHeader` | U-7 |
| 4 | `Atoms::Widths::ModulationBusCell::kInterControlGap` | U-7 |

## Patch Manager (4 modules)

| px @ 100 % | Symbole Design* | Story UAT |
|------------|-------------------|-----------|
| 268 × 384 | `Panels::Body::PatchManagerSection::kWidth` × `kHeight` | U-8 |
| 268 × 76 | `Recipes::BankUtilityModule::kHeight` | U-8 |
| 268 × 76 | `Recipes::InternalPatchesModule::kHeight` | U-8 |
| 268 × 76 | `Recipes::ComputerPatchesModule::kHeight` | U-8 |
| 268 × 100 | `Recipes::PatchMutatorModule::kHeight` | U-8 |
| 8 | `PatchManagerSection::kModuleStackGap` (entre modules) | U-8 |
| 12 | `SharedColumn::kVerticalStackGap` (Matrix Mod ↔ Patch Manager) | U-8 |

## Master Edit

| px @ 100 % | Symbole Design* | Story UAT |
|------------|-------------------|-----------|
| 152 × 704 | `Panels::Body::MasterEditSection::kWidth` × `kHeight` | U-9 |
| 152 × 224 | `MasterEditSection::MidiModule::kHeight` | U-9 |
| 152 × 200 | `MasterEditSection::VibratoModule::kHeight` | U-9 |
| 152 × 224 | `MasterEditSection::MiscModule::kHeight` | U-9 |
| 12 | `MasterEditSection::kInterModuleGap` | U-9 |

## Widgets transversaux (U-2)

| px @ 100 % | Symbole Design* | Story UAT |
|------------|-------------------|-----------|
| 92 × 16 | `Atoms::Widths::ParameterCell::kLabel` × `Atoms::Heights::kLabel` | U-2 |
| 60 × 16 | `Atoms::Widths::ParameterCell::kControl` × `Atoms::Heights::kSlider` | U-2 |
| 20 × 20 | `Atoms::Widths::Button::kInit` × `Atoms::Heights::kButton` | U-2 |
| 8 | `Atoms::Heights::kHorizontalSeparator` | U-2 |
| 4 / 8 / 12 | `Spacing::kStandard` / `kMedium` / `kLarge` | U-2 |
