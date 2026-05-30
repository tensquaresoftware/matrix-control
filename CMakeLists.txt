cmake_minimum_required(VERSION 3.22)

# Platform-specific configuration
if(APPLE)
    # Force arm64 for Apple Silicon (macOS only)
    set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for macOS")
    # Use Ninja generator on macOS if not specified
    if(NOT CMAKE_GENERATOR)
        find_program(NINJA_EXE ninja)
        if(NINJA_EXE)
            set(CMAKE_GENERATOR "Ninja" CACHE INTERNAL "")
        endif()
    endif()
elseif(WIN32)
    # Use Visual Studio generator on Windows if not specified
    if(NOT CMAKE_GENERATOR)
        set(CMAKE_GENERATOR "Visual Studio 17 2022" CACHE INTERNAL "")
    endif()
endif()

project(Matrix-Control VERSION 0.0.18)

# C++ Standard configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Generate compile_commands.json for IDE support (clangd, etc.)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Reference JUCE from environment variable
if(DEFINED ENV{JUCE_DIR})
    add_subdirectory($ENV{JUCE_DIR} "${CMAKE_BINARY_DIR}/JUCE")
else()
    message(FATAL_ERROR "JUCE_DIR environment variable not set. Please set it to your JUCE installation path.")
endif()

# ============================================================================
# BINARY DATA (Fonts)
# ============================================================================

juce_add_binary_data(PluginFonts
    HEADER_NAME PluginFontsData.h
    NAMESPACE PluginFontsData
    SOURCES
        assets/Fonts/PTSansNarrow-Regular.ttf
        assets/Fonts/PTSansNarrow-Bold.ttf
)

# ============================================================================
# SOURCE FILES
# Add your new .cpp files here (keep them sorted alphabetically for clarity)
# ============================================================================

set(PLUGIN_SOURCES
    # Core
    src/Core/PluginProcessor.cpp
    src/Core/Factories/ApvtsFactory.cpp
    src/Core/Factories/ApvtsValidator.cpp
    src/Core/Factories/ApvtsLayoutBuilder.cpp
    src/Shared/Definitions/PluginHelpers.cpp
    src/Shared/Definitions/PluginDescriptors.cpp
    src/Shared/Definitions/PluginDescriptorsMasterEdit.cpp
    src/Shared/Definitions/PluginDescriptorsPatchEdit.cpp
    src/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp
    src/Shared/Definitions/PluginDescriptorsPatchManager.cpp
    src/GUI/PluginEditor.cpp
    src/GUI/Factories/WidgetFactory.cpp
    src/GUI/Factories/WidgetFactoryValidator.cpp
    
    # Core - MIDI Communication Layer
    src/Core/MIDI/Ports/MidiInputPort.cpp
    src/Core/MIDI/Ports/MidiOutputPort.cpp
    src/Core/MIDI/Transport/MidiSender.cpp
    src/Core/MIDI/Transport/MidiReceiver.cpp
    src/Core/MIDI/MidiManager.cpp
    src/Core/MIDI/SysEx/SysExParser.cpp
    src/Core/MIDI/SysEx/SysExDecoder.cpp
    src/Core/MIDI/SysEx/SysExEncoder.cpp
    
    # GUI - Skins
    src/GUI/Skins/Skin.cpp
    
    # GUI - Looks
    src/GUI/Looks/LookBuilders.cpp
    src/GUI/Looks/TypographyStyles.cpp
    
    # GUI - Main Component
    src/GUI/MainComponent.cpp
    src/GUI/Tests/TestButtons.cpp
    src/GUI/Tests/TestComponent.cpp
    src/GUI/Tests/TestSliders.cpp
    
    # GUI - Panels
    src/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp
    src/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/MasterEditPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/Modules/MidiPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/Modules/MiscPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/Modules/VibratoPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/PatchEditBottomModulesPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env1Panel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env2Panel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env3Panel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Lfo1Panel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Lfo2Panel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/InteractiveDisplayApvtsSync.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/PatchEditTopModulesPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/Dco1Panel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/Dco2Panel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/FmTrackPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/RampPortamentoPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/VcfVcaPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
    src/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.cpp
    
    # GUI - Panels - Reusable
    src/GUI/Panels/Reusable/BaseModulePanel.cpp
    
    # GUI - Widgets
    src/GUI/Widgets/Button.cpp
    src/GUI/Widgets/ComboBox.cpp
    src/GUI/Widgets/Toggle.cpp
    src/GUI/Widgets/EnvelopeDisplay.cpp
    src/GUI/Widgets/GroupLabel.cpp
    src/GUI/Widgets/HorizontalSeparator.cpp
    src/GUI/Widgets/Label.cpp
    src/GUI/Widgets/ModulationBusCell.cpp
    src/GUI/Widgets/ModulationBusHeader.cpp
    src/GUI/Widgets/ModuleHeader.cpp
    src/GUI/Widgets/MultiColumnPopupMenu.cpp
    src/GUI/Widgets/NumberBox.cpp
    src/GUI/Widgets/ParameterCell.cpp
    src/GUI/Widgets/PatchNameDisplay.cpp
    src/GUI/Widgets/PopupMenuBase.cpp
    src/GUI/Widgets/PopupMenuPositioner.cpp
    src/GUI/Widgets/PopupMenuRenderer.cpp
    src/GUI/Widgets/ScrollablePopupMenu.cpp
    src/GUI/Widgets/SectionHeader.cpp
    src/GUI/Widgets/Slider.cpp
    src/GUI/Widgets/TrackGeneratorDisplay.cpp
    src/GUI/Widgets/VerticalSeparator.cpp
    
    # Loggers
    src/Core/Loggers/MidiLogger.cpp
    src/Core/Loggers/ApvtsLogger.cpp

    # Unit Tests
    tests/unit/MidiManagerTests.cpp
    tests/unit/SysExEncoderTests.cpp
    tests/unit/SysExParserTests.cpp
)

# ============================================================================
# PLUGIN CONFIGURATION
# ============================================================================

# Platform-specific plugin formats (filter out AU on Windows)
set(PLUGIN_FORMATS_LIST AU VST3 Standalone)
if(NOT APPLE)
    # Remove AU from formats on non-Apple platforms
    string(REPLACE "AU " "" PLUGIN_FORMATS_LIST "${PLUGIN_FORMATS_LIST}")
    string(REPLACE " AU" "" PLUGIN_FORMATS_LIST "${PLUGIN_FORMATS_LIST}")
    string(REPLACE "AU" "" PLUGIN_FORMATS_LIST "${PLUGIN_FORMATS_LIST}")
    string(STRIP "${PLUGIN_FORMATS_LIST}" PLUGIN_FORMATS_LIST)
endif()

# Platform-specific COPY_PLUGIN_AFTER_BUILD setting
if(APPLE)
    set(COPY_PLUGIN_AFTER_BUILD_VALUE TRUE)
else()
    set(COPY_PLUGIN_AFTER_BUILD_VALUE FALSE)
endif()

juce_add_plugin(Matrix-Control
    COMPANY_NAME "Ten Square Software"
    BUNDLE_ID "com.TenSquareSoftware.Matrix-Control"
    PLUGIN_MANUFACTURER_CODE Tssf
    PLUGIN_CODE Mcpi
    FORMATS ${PLUGIN_FORMATS_LIST}
    PRODUCT_NAME "Matrix-Control"
	PLUGIN_NAME "Matrix-Control"
    IS_SYNTH FALSE
    NEEDS_MIDI_INPUT TRUE
    NEEDS_MIDI_OUTPUT TRUE
    IS_MIDI_EFFECT FALSE
    EDITOR_WANTS_KEYBOARD_FOCUS FALSE
    COPY_PLUGIN_AFTER_BUILD ${COPY_PLUGIN_AFTER_BUILD_VALUE}
    VST3_CATEGORIES Fx
    AU_MAIN_TYPE kAudioUnitType_Effect
)

target_sources(Matrix-Control PRIVATE ${PLUGIN_SOURCES})

# Add src directory as include directory to eliminate relative paths
target_include_directories(Matrix-Control
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

target_link_libraries(Matrix-Control
    PRIVATE
        PluginFonts
        juce::juce_audio_utils
        juce::juce_dsp
        juce::juce_gui_basics
    PUBLIC
        juce::juce_recommended_config_flags
        juce::juce_recommended_lto_flags
        juce::juce_recommended_warning_flags
)

target_compile_definitions(Matrix-Control
    PRIVATE
        MATRIX_CONTROL_PROJECT_ROOT="${CMAKE_CURRENT_SOURCE_DIR}"
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_REPORT_APP_USAGE=0
)

# ============================================================================
# CUSTOM VST3 INSTALLATION (Windows)
# Copy VST3 plugin to custom folder after build for easy testing
# ============================================================================
#
# CONFIGURATION:
# ------------
# To customize the VST3 installation folder, modify the default value below
# or override it when configuring CMake:
#   cmake .. -DCUSTOM_VST3_FOLDER="your/custom/path"
#
# Examples:
#   - Personal folder: "C:/Users/YourName/VST3"
#   - Custom location: "D:/MyPlugins/VST3"
#   - Standard location (requires admin): "C:/Program Files/Common Files/VST3"
#
# Note: Using a personal folder avoids requiring administrator privileges.
#       Configure your DAW to scan this custom folder for VST3 plugins.
#
# ============================================================================

if(WIN32)
    # Define custom VST3 folder (can be overridden via CMake cache)
    if(NOT DEFINED CUSTOM_VST3_FOLDER)
        set(CUSTOM_VST3_FOLDER "C:/Users/YourName/VST3" CACHE PATH "Custom VST3 installation folder")
    endif()
    
    # VST3 target name (JUCE appends _VST3)
    set(VST3_TARGET_NAME "Matrix-Control_VST3")
    
    # Path where JUCE generates the VST3 bundle (using generator expression for CONFIG)
    set(VST3_SOURCE_DIR "${CMAKE_BINARY_DIR}/Matrix-Control_artefacts/$<CONFIG>/VST3/Matrix-Control.vst3")
    set(VST3_DEST_DIR "${CUSTOM_VST3_FOLDER}/Matrix-Control.vst3")
    
    # Add custom command to copy VST3 plugin after build
    add_custom_command(TARGET ${VST3_TARGET_NAME} POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E echo "Copying VST3 plugin to custom folder: ${CUSTOM_VST3_FOLDER}"
        COMMAND ${CMAKE_COMMAND} -E make_directory "${CUSTOM_VST3_FOLDER}"
        COMMAND ${CMAKE_COMMAND} -E remove_directory "${VST3_DEST_DIR}"
        COMMAND ${CMAKE_COMMAND} -E copy_directory "${VST3_SOURCE_DIR}" "${VST3_DEST_DIR}"
        COMMENT "Installing VST3 plugin to custom folder"
        VERBATIM
    )
    
    message(STATUS "VST3 plugin will be copied to: ${CUSTOM_VST3_FOLDER}")
endif()
