#pragma once

// PluginDimensions.h
// Central file for all plugin widget and panel dimensions
// Contains ONLY constant dimension definitions - no code/logic
// Single source of truth for all plugin dimensions

namespace PluginDimensions
{
    // =================================================================================================================
    // GUI Dimensions
    // =================================================================================================================
    
    namespace GUI
    {
        inline constexpr int kWidth = 1335;
        inline constexpr int kHeight = 810;
    }

    // =================================================================================================================
    // Panels Dimensions
    // =================================================================================================================
    
    namespace Panels
    {
        namespace Header
        {
            inline constexpr int kWidth = GUI::kWidth;
            inline constexpr int kHeight = 30;
        }

        namespace Body
        {
            inline constexpr int kWidth = GUI::kWidth;
            inline constexpr int kHeight = 750;
            inline constexpr int kPadding = 15;
            inline constexpr int kEffectiveHeight = kHeight - 2 * kPadding;

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
                    inline constexpr int kHeight = 242;
                }

                namespace VibratoModule
                {
                    inline constexpr int kHeight = 218;
                }

                namespace MiscModule
                {
                    inline constexpr int kHeight = 230;
                }
            }

            namespace PatchEditSection
            {
                inline constexpr int kWidth = 810;
                inline constexpr int kHeight = Body::kEffectiveHeight;

                inline constexpr int kModuleWidth = 150;
                inline constexpr int kTopBottomPanelHeight = 280;

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
                    inline constexpr int kHeight = 130;

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

            namespace SharedColumn
            {
                inline constexpr int kWidth = 275;
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
                    inline constexpr int kHeight = 90;
                }

                namespace InternalPatchesModule
                {
                    inline constexpr int kWidth = PatchManagerSection::kWidth;
                    inline constexpr int kHeight = 85;
                }

                namespace ComputerPatchesModule
                {
                    inline constexpr int kWidth = PatchManagerSection::kWidth;
                    inline constexpr int kHeight = 85;
                }

                namespace PatchMutatorModule
                {
                    inline constexpr int kWidth = PatchManagerSection::kWidth;
                    inline constexpr int kHeight = 105;
                }
            }
        }

        namespace Footer
        {
            inline constexpr int kWidth = GUI::kWidth;
            inline constexpr int kHeight = 30;
        }
    }


    // =================================================================================================================
    // Widgets Dimensions
    // =================================================================================================================
    
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
            
            namespace GroupLabel
            {
                inline constexpr int kInternalPatchesBrowser = 110;
                inline constexpr int kInternalPatchesMemory = 155;
                inline constexpr int kComputerPatchesBrowser = 135;
                inline constexpr int kComputerPatchesStorage = 130;
            }
            
            namespace Label
            {
                inline constexpr int kMasterEditModule = 100;
                inline constexpr int kPatchEditModule = 90;
                inline constexpr int kModulationBusNumber = 15;
                inline constexpr int kPatchManagerBankSelector = 75;
                inline constexpr int kPatchMutator = 45;
            }

            namespace Button
            {
                inline constexpr int kHeaderPanelTheme = 70;
                
                inline constexpr int kInit = 20;
                inline constexpr int kCopy = 20;
                inline constexpr int kPaste = 20;
                inline constexpr int kPatchManagerBankSelect = 35;
                inline constexpr int kPatchManagerUnlockBank = 75;
                inline constexpr int kInternalPatchesMemory = 35;
                inline constexpr int kComputerPatchesStorage = 35;
                inline constexpr int kComputerPatchesSaveAs = 50;
                inline constexpr int kPatchMutatorMutate = 50;
                inline constexpr int kPatchMutatorCompare = 50;
                inline constexpr int kPatchMutatorDelete = 20;
                inline constexpr int kPatchMutatorClear = 45;
                inline constexpr int kPatchMutatorExport = 45;
            }

            namespace Toggle
            {
                inline constexpr int kPatchMutator = 20;
            }
            
            namespace Slider
            {
                inline constexpr int kStandard = 60;
                inline constexpr int kPatchMutator = 45;
            }
            
            namespace ComboBox
            {
                inline constexpr int kMasterEditModule = 60;
                inline constexpr int kPatchEditModule = 60;
                inline constexpr int kMatrixModulationSource = 60;
                inline constexpr int kMatrixModulationDestination = 105;
                inline constexpr int kPatchManagerComputerPatches = 85;
                inline constexpr int kPatchMutatorHistory = 45;
            }
            
            namespace NumberBox
            {
                inline constexpr int kPatchManagerBankNumber = 25;
                inline constexpr int kPatchManagerPatchNumber = 30;
            }
            
            namespace HorizontalSeparator
            {
                inline constexpr int kMasterEditModule = Panels::Body::MasterEditSection::kWidth;
                inline constexpr int kPatchEditModule = Panels::Body::PatchEditSection::kModuleWidth;
                inline constexpr int kMatrixModulationBus = Panels::Body::SharedColumn::kWidth;
            }
            
            namespace VerticalSeparator
            {
                inline constexpr int kStandard = 30;
            }
        }
        
        namespace Heights
        {
            inline constexpr int kSectionHeader = 30;
            inline constexpr int kModuleHeader = 30;
            inline constexpr int kModulationBusHeader = 30;
            inline constexpr int kGroupLabel = 25;
            inline constexpr int kLabel = 20;
            inline constexpr int kButton = 20;
            inline constexpr int kToggle = 20;
            inline constexpr int kSlider = 20;
            inline constexpr int kComboBox = 20;
            inline constexpr int kNumberBox = 20;
            inline constexpr int kPatchNameDisplay = 70;
            inline constexpr int kHorizontalSeparator = 5;
            inline constexpr int kVerticalSeparator = Panels::Body::kEffectiveHeight;
        }
    }
}