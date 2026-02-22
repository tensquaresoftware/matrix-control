#pragma once

// PluginDimensions.h
// Central file for all plugin widget and panel dimensions
// Contains ONLY constant dimension definitions - no code/logic
// Single source of truth for all plugin dimensions

namespace PluginDimensions
{
    // ============================================================================
    // GUI Dimensions
    // ============================================================================
    
    namespace GUI
    {
        inline constexpr int kWidth = 1335;
        inline constexpr int kHeight = 810;
    }

    // ============================================================================
    // Panels
    // ============================================================================
    
    namespace Panels
    {
        namespace HeaderPanel
        {
            inline constexpr int kWidth = 1335;
            inline constexpr int kHeight = 30;
        }
        
        namespace BodyPanel
        {
            inline constexpr int kWidth = 1335;
            inline constexpr int kHeight = 750;
            inline constexpr int kPadding = 15;
        }
        
        namespace FooterPanel
        {
            inline constexpr int kWidth = 1335;
            inline constexpr int kHeight = 30;
        }
        
        namespace MasterEditPanel
        {
            inline constexpr int kWidth = 160;
            inline constexpr int kHeight = 720;
            
            namespace ChildPanels
            {
                inline constexpr int kWidth = 160;
            }
            
            namespace MidiPanel
            {
                inline constexpr int kHeight = 242;
            }
            
            namespace VibratoPanel
            {
                inline constexpr int kHeight = 218;
            }
            
            namespace MiscPanel
            {
                inline constexpr int kHeight = 230;
            }
        }
        
        namespace PatchEditPanel
        {
            inline constexpr int kWidth = 810;
            inline constexpr int kHeight = 720;
            
            namespace TopPanel
            {
                inline constexpr int kWidth = 810;
                inline constexpr int kHeight = 280;
                
                namespace ChildPanels
                {
                    inline constexpr int kWidth = 150;
                    inline constexpr int kHeight = 280;
                }
            }
            
            namespace MiddlePanel
            {
                inline constexpr int kWidth = 810;
                inline constexpr int kHeight = 130;

                namespace ChildPanels
                {
                    inline constexpr int kWidth = 150;
                    inline constexpr int kHeight = 130;
                }

                namespace PatchNameSection
                {
                    inline constexpr int kWidth = 150;
                    inline constexpr int kHeight = 130;
                    inline constexpr int kPaddingTop = 8;
                    inline constexpr int kSpacing = 12;
                    inline constexpr int kPaddingBottom = 10;
                    inline constexpr int kPatchNameDisplayWidth = 150;
                    inline constexpr int kPatchNameDisplayHeight = 70;
                }
            }
            
            namespace BottomPanel
            {
                inline constexpr int kWidth = 810;
                inline constexpr int kHeight = 280;
                
                namespace ChildPanels
                {
                    inline constexpr int kWidth = 150;
                    inline constexpr int kHeight = 280;
                }
            }
        }
        
        namespace MatrixModulationPanel
        {
            inline constexpr int kWidth = 275;
            inline constexpr int kHeight = 320;
        }
        
        namespace PatchManagerPanel
        {
            inline constexpr int kWidth = 275;
            inline constexpr int kHeight = 400;
            
            namespace BankUtilityPanel
            {
                inline constexpr int kWidth = 275;
                inline constexpr int kHeight = 90;
            }
            
            namespace InternalPatchesPanel
            {
                inline constexpr int kWidth = 275;
                inline constexpr int kHeight = 85;
            }
            
            namespace ComputerPatchesPanel
            {
                inline constexpr int kWidth = 275;
                inline constexpr int kHeight = 85;
            }

            namespace PatchMutatorPanel
            {
                inline constexpr int kWidth = 275;
                inline constexpr int kHeight = 105;
            }
        }
    }

    // ============================================================================
    // Widgets
    // ============================================================================
    
    namespace Widgets
    {
        // ============================================================================
        // Widths
        // ============================================================================
        
        namespace Widths
        {
            namespace SectionHeader
            {
                inline constexpr int kMasterEdit = 160;
                inline constexpr int kPatchEdit = 810;
                inline constexpr int kMatrixModulation = 275;
                inline constexpr int kPatchManager = 275;
            }
            
            namespace ModuleHeader
            {
                inline constexpr int kPatchEditModule = 150;
                inline constexpr int kMasterEditModule = 160;
                inline constexpr int kPatchManagerModule = 275;
            }
            
            namespace ModulationBusHeader
            {
                inline constexpr int kStandard = 275;
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
            
            namespace Toggle
            {
                inline constexpr int kPatchMutator = 20;
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
                inline constexpr int kMasterEditModule = 160;
                inline constexpr int kPatchEditModule = 150;
                inline constexpr int kMatrixModulationBus = 275;
            }
            
            namespace VerticalSeparator
            {
                inline constexpr int kStandard = 30;
            }
        }
        
        // ============================================================================
        // Heights
        // ============================================================================
        
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
            inline constexpr int kHorizontalSeparator = 5;
            inline constexpr int kVerticalSeparator = 720;
        }
    }
} // namespace PluginDimensions