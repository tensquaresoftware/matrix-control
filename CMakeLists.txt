# JUCE audio plugin project (CMake). Do not remove cmake_minimum_required / project() order.
cmake_minimum_required(VERSION 3.22)

# IDE: compile_commands.json (Ninja / Makefile generators). Set before project() so generation picks it up.
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Platform-specific configuration
if(APPLE)
    # Real host arch (uname -m): reliable on Apple Silicon even when CMake runs under Rosetta
    execute_process(COMMAND uname -m OUTPUT_VARIABLE HOST_ARCH OUTPUT_STRIP_TRAILING_WHITESPACE)
    # Architecture is set per-preset in CMakeUserPresets.json (arm64 for Apple Silicon, x86_64 for Intel)
    if(NOT CMAKE_OSX_ARCHITECTURES)
        set(CMAKE_OSX_ARCHITECTURES "arm64" CACHE STRING "Build architectures for macOS (arm64 or x86_64)")
    endif()
    # Preset vs host consistency: avoid wrong preset for the current machine
    # - Apple Silicon: reject "macOS Intel" (native); use "macOS Intel-Rosetta"
    # - Mac Intel: reject "macOS Intel-Rosetta" and "macOS Apple Silicon" (ARM); use "macOS Intel" (native)
    if(HOST_ARCH STREQUAL "arm64")
        # Rosetta builds use .../Intel-Rosetta, so they do not match .../Intel/?$ at end of path.
        if(CMAKE_BINARY_DIR MATCHES "Builds/macOS/Intel/?$")
            message(FATAL_ERROR
                "On Apple Silicon, do not use the 'macOS Intel' preset (default-macos-x86_64; meant for native Mac Intel). Use 'macOS Intel-Rosetta' instead (preset default-macos-x86_64-rosetta): in your IDE select that preset. To list all presets: cmake --list-presets.")
        endif()
    elseif(HOST_ARCH STREQUAL "x86_64" OR HOST_ARCH STREQUAL "i386")
        if(CMAKE_BINARY_DIR MATCHES "Intel-Rosetta")
            message(FATAL_ERROR
                "On Mac Intel, do not use the 'macOS Intel-Rosetta' preset (default-macos-x86_64-rosetta; meant for Apple Silicon). Use 'macOS Intel' instead (preset default-macos-x86_64): in your IDE select that preset. To list all presets: cmake --list-presets.")
        endif()
        if(CMAKE_BINARY_DIR MATCHES "Builds/macOS/ARM/?$")
            message(FATAL_ERROR
                "On Mac Intel, do not use the 'macOS Apple Silicon' preset (default-macos-arm64). Use 'macOS Intel' instead (preset default-macos-x86_64): in your IDE select that preset. To list all presets: cmake --list-presets.")
        endif()
    endif()
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

# Plugin copy configuration (edit project-configuration.cmake to customize)
include(project-configuration.cmake)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Reference JUCE: priority cache (preset/-D), then ENV, then platform default
# Portable logic - works on any platform regardless of where the project was generated
set(JUCE_DIR_CANDIDATE "")
if(DEFINED JUCE_DIR AND NOT JUCE_DIR STREQUAL "" AND EXISTS "${JUCE_DIR}")
    set(JUCE_DIR_CANDIDATE "${JUCE_DIR}")
elseif(DEFINED ENV{JUCE_DIR} AND NOT "$ENV{JUCE_DIR}" STREQUAL "")
    set(JUCE_DIR_CANDIDATE "$ENV{JUCE_DIR}")
elseif(APPLE AND EXISTS "/Applications/JUCE")
    set(JUCE_DIR_CANDIDATE "/Applications/JUCE")
elseif(WIN32 AND EXISTS "C:/Program Files/JUCE")
    set(JUCE_DIR_CANDIDATE "C:/Program Files/JUCE")
elseif(UNIX AND EXISTS "/usr/local/JUCE")
    set(JUCE_DIR_CANDIDATE "/usr/local/JUCE")
endif()

if(JUCE_DIR_CANDIDATE STREQUAL "")
    message(FATAL_ERROR "JUCE not found. Set JUCE_DIR (e.g. export JUCE_DIR=/Applications/JUCE) or use -DJUCE_DIR=/path/to/JUCE")
endif()

set(JUCE_DIR "${JUCE_DIR_CANDIDATE}" CACHE PATH "Path to JUCE installation" FORCE)

if(NOT EXISTS "${JUCE_DIR}")
    message(FATAL_ERROR "JUCE not found at ${JUCE_DIR}. Check JUCE_DIR path.")
endif()

add_subdirectory(${JUCE_DIR} "${CMAKE_BINARY_DIR}/JUCE")

# ============================================================================
# BINARY DATA (Fonts)
# ============================================================================

juce_add_binary_data(PluginFonts
    HEADER_NAME PluginFontsData.h
    NAMESPACE PluginFontsData
    SOURCES
        Assets/Fonts/PTSansNarrow-Regular.ttf
        Assets/Fonts/PTSansNarrow-Bold.ttf
)

# ============================================================================
# SOURCE FILES
# Add your new .cpp files here (keep them sorted alphabetically for clarity)
# ============================================================================

set(PLUGIN_SOURCES
    # Core
    Source/Core/PluginProcessor.cpp
    Source/Core/Audio/InstrumentMidiForwarder.cpp
    Source/Core/Factories/ApvtsFactory.cpp
    Source/Core/Models/ApvtsMasterMapper.cpp
    Source/Core/Models/ApvtsPatchMapper.cpp
    Source/Core/Models/MasterModel.cpp
    Source/Core/Models/PatchNameSyncer.cpp
    Source/Core/Models/PackedFieldCodec.cpp
    Source/Core/Models/PatchModel.cpp
    Source/Core/Factories/ApvtsValidator.cpp
    Source/Core/Factories/ApvtsLayoutBuilder.cpp
    Source/Shared/Definitions/PluginHelpers.cpp
    Source/Shared/Definitions/PluginDescriptors.cpp
    Source/Shared/Definitions/PluginDescriptorsMasterEdit.cpp
    Source/Shared/Definitions/PluginDescriptorsPatchEdit.cpp
    Source/Shared/Definitions/PluginDescriptorsMatrixModulation.cpp
    Source/Shared/Definitions/PluginDescriptorsPatchManager.cpp
    Source/GUI/PluginEditor.cpp
    Source/GUI/Factories/WidgetFactory.cpp
    Source/GUI/Factories/WidgetFactoryValidator.cpp
    
    # Core - MIDI Communication Layer
    Source/Core/MIDI/Ports/MidiInputPort.cpp
    Source/Core/MIDI/Ports/MidiOutputPort.cpp
    Source/Core/MIDI/Transport/MidiSender.cpp
    Source/Core/MIDI/Transport/MidiReceiver.cpp
    Source/Core/MIDI/EditorPath.cpp
    Source/Core/MIDI/MasterParameterSysExDispatcher.cpp
    Source/Core/MIDI/PatchParameterSysExDispatcher.cpp
    Source/Core/MIDI/MidiManager.cpp
    Source/Core/MIDI/Queue/MidiOutboundQueue.cpp
    Source/Core/MIDI/Queue/SysExDelayProfile.cpp
    Source/Core/MIDI/Queue/SysExInterMessageDelay.cpp
    Source/Core/MIDI/SysEx/SysExParser.cpp
    Source/Core/MIDI/SysEx/SysExDecoder.cpp
    Source/Core/MIDI/SysEx/SysExEncoder.cpp
    
    # GUI - Skins
    Source/GUI/Skins/Skin.cpp
    
    # GUI - Looks
    Source/GUI/Looks/LookBuilders.cpp
    Source/GUI/Looks/TypographyStyles.cpp
    
    # GUI - Main Component
    Source/GUI/MainComponent.cpp
    Source/GUI/Tests/TestButtons.cpp
    Source/GUI/Tests/TestComponent.cpp
    Source/GUI/Tests/TestSliders.cpp
    
    # GUI - Panels
    Source/GUI/Panels/MainComponent/FooterPanel/FooterPanel.cpp
    Source/GUI/Panels/MainComponent/HeaderPanel/HeaderPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/BodyPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/MasterEditPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/Modules/MidiPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/Modules/MiscPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/MasterEditPanel/Modules/VibratoPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/MatrixModulationPanel/MatrixModulationPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/PatchEditBottomModulesPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env1Panel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env2Panel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Env3Panel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Lfo1Panel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditBottomModulesPanel/Modules/Lfo2Panel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/InteractiveDisplayApvtsSync.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditDisplaysPanel/PatchEditDisplaysPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/PatchEditTopModulesPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/Dco1Panel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/Dco2Panel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/FmTrackPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/RampPortamentoPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/PatchEditPanel/PatchEditTopModulesPanel/Modules/VcfVcaPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/PatchManagerPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/BankUtilityPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/ComputerPatchesPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/InternalPatchesPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/PatchManagerPanel/Modules/PatchMutatorPanel.cpp
    Source/GUI/Panels/MainComponent/BodyPanel/SharedPanel/SharedPanel.cpp
    
    # GUI - Panels - Reusable
    Source/GUI/Panels/Reusable/BaseModulePanel.cpp
    
    # GUI - Widgets
    Source/GUI/Widgets/Button.cpp
    Source/GUI/Widgets/ComboBox.cpp
    Source/GUI/Widgets/Toggle.cpp
    Source/GUI/Widgets/EnvelopeDisplay.cpp
    Source/GUI/Widgets/GroupLabel.cpp
    Source/GUI/Widgets/HorizontalSeparator.cpp
    Source/GUI/Widgets/Label.cpp
    Source/GUI/Widgets/ModulationBusCell.cpp
    Source/GUI/Widgets/ModulationBusHeader.cpp
    Source/GUI/Widgets/ModuleHeader.cpp
    Source/GUI/Widgets/MultiColumnPopupMenu.cpp
    Source/GUI/Widgets/NumberBox.cpp
    Source/GUI/Widgets/ParameterCell.cpp
    Source/GUI/Widgets/PatchNameDisplay.cpp
    Source/GUI/Widgets/PopupMenuBase.cpp
    Source/GUI/Widgets/PopupMenuPositioner.cpp
    Source/GUI/Widgets/PopupMenuRenderer.cpp
    Source/GUI/Widgets/ScrollablePopupMenu.cpp
    Source/GUI/Widgets/SectionHeader.cpp
    Source/GUI/Widgets/Slider.cpp
    Source/GUI/Widgets/TrackGeneratorDisplay.cpp
    Source/GUI/Widgets/VerticalSeparator.cpp
    
    # Shared
    Source/Shared/ProjectPaths.cpp

    # Loggers
    Source/Core/Loggers/MidiLogger.cpp
    Source/Core/Loggers/ApvtsLogger.cpp
)

# ============================================================================
# UNIT TEST RUNNER (console app)
# Unit tests use juce::UnitTest and must NOT be linked into AU/VST3/Standalone
# (static UnitTest registrars crash some hosts). They build into a dedicated
# console target instead. Enable with -DMATRIX_BUILD_TESTS=ON, then:
#   cmake --build <buildDir> --target Matrix-Control_Tests
#   "<buildDir>/Matrix-Control_Tests_artefacts/<config>/Matrix-Control Tests"
# Only Core sources under test are compiled here — no GUI, no plugin target.
# ============================================================================
option(MATRIX_BUILD_TESTS "Build the Matrix-Control unit test runner" OFF)

if(MATRIX_BUILD_TESTS)
    juce_add_console_app(Matrix-Control_Tests PRODUCT_NAME "Matrix-Control Tests")

    target_sources(Matrix-Control_Tests PRIVATE
        Tests/TestMain.cpp
        Tests/Unit/ApvtsMasterMapperTests.cpp
        Tests/Unit/ApvtsPatchMapperTests.cpp
        Tests/Unit/PatchNameSyncerTests.cpp
        Tests/Unit/MasterModelTests.cpp
        Tests/Unit/MidiManagerTests.cpp
        Tests/Unit/PatchModelTests.cpp
        Tests/Unit/SysExEncoderTests.cpp
        Tests/Unit/MasterParameterSysExDispatcherTests.cpp
        Tests/Unit/PatchParameterSysExDispatcherTests.cpp
        Tests/Unit/EditorPathTests.cpp
        Tests/Unit/InstrumentMidiForwarderTests.cpp
        Tests/Unit/MidiOutboundQueueTests.cpp
        Tests/Unit/SysExDelayProfileTests.cpp
        Tests/Unit/SysExInterMessageDelayTests.cpp
        Tests/Unit/SysExParserTests.cpp

        Source/Core/Models/ApvtsMasterMapper.cpp
        Source/Core/Models/ApvtsPatchMapper.cpp
        Source/Core/Models/MasterModel.cpp
        Source/Core/Models/PatchModel.cpp
        Source/Core/Models/PatchNameSyncer.cpp
        Source/Core/Models/PackedFieldCodec.cpp
        Source/Core/Audio/InstrumentMidiForwarder.cpp
        Source/Core/MIDI/EditorPath.cpp
        Source/Core/MIDI/MasterParameterSysExDispatcher.cpp
        Source/Core/MIDI/PatchParameterSysExDispatcher.cpp
        Source/Core/MIDI/Queue/MidiOutboundQueue.cpp
        Source/Core/MIDI/Queue/SysExDelayProfile.cpp
        Source/Core/MIDI/Queue/SysExInterMessageDelay.cpp
        Source/Core/MIDI/SysEx/SysExParser.cpp
        Source/Core/MIDI/SysEx/SysExDecoder.cpp
        Source/Core/MIDI/SysEx/SysExEncoder.cpp
        Source/Core/Loggers/MidiLogger.cpp
        Source/Shared/Definitions/PluginDescriptors.cpp
        Source/Shared/Definitions/PluginDescriptorsPatchEdit.cpp
        Source/Shared/Definitions/PluginHelpers.cpp
        Source/Shared/ProjectPaths.cpp
    )

    target_include_directories(Matrix-Control_Tests PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
    )

    target_compile_definitions(Matrix-Control_Tests PRIVATE
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_STANDALONE_APPLICATION=1
        MATRIX_TEST_FIXTURES_DIR="${CMAKE_CURRENT_SOURCE_DIR}/Tests/Fixtures"
    )

    target_link_libraries(Matrix-Control_Tests PRIVATE
        juce::juce_audio_processors
        juce::juce_core
        juce::juce_recommended_config_flags
        juce::juce_recommended_warning_flags
    )
endif()

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

if(WIN32)
    set(COPY_PLUGIN_AFTER_BUILD_VALUE FALSE)
else()
    set(COPY_PLUGIN_AFTER_BUILD_VALUE ${COPY_TO_SYSTEM_FOLDERS})
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

target_include_directories(Matrix-Control
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/Source
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
    PUBLIC
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_REPORT_APP_USAGE=0
)

# =============================================================================
# COPY TO CENTRAL ARTEFACTS FOLDER
# When COPY_TO_ARTEFACTS_DIR is ON, copies build outputs to a central custom folder.
# Paths: ARTEFACTS_DIR_WINDOWS, ARTEFACTS_DIR_MACOS, ARTEFACTS_DIR_LINUX (injected from generator-configuration.py).
# Structure: {ARTEFACTS_DIR}/{OS}/{arch}/{format}/
# - macOS: ARM/, Intel/, Intel-Rosetta/, Universal/
# - Windows/Linux: no arch subfolder
# Formats: VST3/, AU/ (macOS only), Standalone/. CLAP/ (Linux, future).
# Intel = x86_64 native (Mac Intel). Intel-Rosetta = x86_64 cross-compiled on Apple Silicon.
# Only copies formats selected for the project.
# =============================================================================

if(COPY_TO_ARTEFACTS_DIR)
    # Determine base artefacts directory for current OS
    if(APPLE)
        set(ARTEFACTS_DIR_BASE "${ARTEFACTS_DIR_MACOS}")
    elseif(WIN32)
        set(ARTEFACTS_DIR_BASE "${ARTEFACTS_DIR_WINDOWS}")
    else()
        set(ARTEFACTS_DIR_BASE "${ARTEFACTS_DIR_LINUX}")
    endif()

    # Determine architecture subfolder (macOS only)
    if(APPLE)
        set(ARCH_LIST ${CMAKE_OSX_ARCHITECTURES})
        list(FIND ARCH_LIST "arm64" IDX_ARM64)
        list(FIND ARCH_LIST "x86_64" IDX_X86_64)
        if(NOT IDX_ARM64 EQUAL -1 AND NOT IDX_X86_64 EQUAL -1)
            set(PLUGIN_ARCH_SUBFOLDER "Universal")
        elseif(NOT IDX_ARM64 EQUAL -1)
            set(PLUGIN_ARCH_SUBFOLDER "ARM")
        elseif(NOT IDX_X86_64 EQUAL -1)
            if(HOST_ARCH STREQUAL "arm64")
                set(PLUGIN_ARCH_SUBFOLDER "Intel-Rosetta")
            else()
                set(PLUGIN_ARCH_SUBFOLDER "Intel")
            endif()
        else()
            set(PLUGIN_ARCH_SUBFOLDER "ARM")
        endif()
        set(ARTEFACTS_DEST "${ARTEFACTS_DIR_BASE}/macOS/${PLUGIN_ARCH_SUBFOLDER}")
    elseif(WIN32)
        set(ARTEFACTS_DEST "${ARTEFACTS_DIR_BASE}/Windows")
    else()
        set(ARTEFACTS_DEST "${ARTEFACTS_DIR_BASE}/Linux")
    endif()

    message(STATUS "Build artefacts will be copied to: ${ARTEFACTS_DEST}")
endif()

# Create custom target that copies all artefacts after build
if(COPY_TO_ARTEFACTS_DIR)
    set(ALL_PLUGIN_TARGETS)
    set(COPY_COMMANDS)
    
    # VST3
    if(PLUGIN_FORMATS_LIST MATCHES "VST3")
        list(APPEND ALL_PLUGIN_TARGETS "Matrix-Control_VST3")
        set(VST3_SOURCE_DIR "${CMAKE_BINARY_DIR}/Matrix-Control_artefacts/$<CONFIG>/VST3/Matrix-Control.vst3")
        set(VST3_DEST_DIR "${ARTEFACTS_DEST}/VST3/Matrix-Control.vst3")
        list(APPEND COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E echo "Copying VST3 to ${ARTEFACTS_DEST}/VST3/"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTEFACTS_DEST}/VST3"
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${VST3_DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${VST3_SOURCE_DIR}" "${VST3_DEST_DIR}"
        )
    endif()
    
    # Standalone
    if(PLUGIN_FORMATS_LIST MATCHES "Standalone")
        list(APPEND ALL_PLUGIN_TARGETS "Matrix-Control_Standalone")
        if(APPLE)
            set(STANDALONE_SOURCE "${CMAKE_BINARY_DIR}/Matrix-Control_artefacts/$<CONFIG>/Standalone/Matrix-Control.app")
            set(STANDALONE_DEST_DIR "${ARTEFACTS_DEST}/Standalone/Matrix-Control.app")
            list(APPEND COPY_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E echo "Copying Standalone to ${ARTEFACTS_DEST}/Standalone/"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTEFACTS_DEST}/Standalone"
                COMMAND ${CMAKE_COMMAND} -E remove_directory "${STANDALONE_DEST_DIR}"
                COMMAND ${CMAKE_COMMAND} -E copy_directory "${STANDALONE_SOURCE}" "${STANDALONE_DEST_DIR}"
            )
        elseif(WIN32)
            set(STANDALONE_SOURCE "${CMAKE_BINARY_DIR}/Matrix-Control_artefacts/$<CONFIG>/Standalone/Matrix-Control.exe")
            set(STANDALONE_DEST "${ARTEFACTS_DEST}/Standalone/Matrix-Control.exe")
            list(APPEND COPY_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E echo "Copying Standalone to ${ARTEFACTS_DEST}/Standalone/"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTEFACTS_DEST}/Standalone"
                COMMAND ${CMAKE_COMMAND} -E copy "${STANDALONE_SOURCE}" "${STANDALONE_DEST}"
            )
        else()
            set(STANDALONE_SOURCE "${CMAKE_BINARY_DIR}/Matrix-Control_artefacts/$<CONFIG>/Standalone/Matrix-Control")
            set(STANDALONE_DEST "${ARTEFACTS_DEST}/Standalone/Matrix-Control")
            list(APPEND COPY_COMMANDS
                COMMAND ${CMAKE_COMMAND} -E echo "Copying Standalone to ${ARTEFACTS_DEST}/Standalone/"
                COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTEFACTS_DEST}/Standalone"
                COMMAND ${CMAKE_COMMAND} -E copy "${STANDALONE_SOURCE}" "${STANDALONE_DEST}"
            )
        endif()
    endif()
    
    # AU (macOS only)
    if(APPLE AND PLUGIN_FORMATS_LIST MATCHES "AU")
        list(APPEND ALL_PLUGIN_TARGETS "Matrix-Control_AU")
        set(AU_SOURCE_DIR "${CMAKE_BINARY_DIR}/Matrix-Control_artefacts/$<CONFIG>/AU/Matrix-Control.component")
        set(AU_DEST_DIR "${ARTEFACTS_DEST}/AU/Matrix-Control.component")
        list(APPEND COPY_COMMANDS
            COMMAND ${CMAKE_COMMAND} -E echo "Copying AU to ${ARTEFACTS_DEST}/AU/"
            COMMAND ${CMAKE_COMMAND} -E make_directory "${ARTEFACTS_DEST}/AU"
            COMMAND ${CMAKE_COMMAND} -E remove_directory "${AU_DEST_DIR}"
            COMMAND ${CMAKE_COMMAND} -E copy_directory "${AU_SOURCE_DIR}" "${AU_DEST_DIR}"
        )
    endif()
    
    # Create the custom target with all copy commands
    add_custom_target(CopyToArtefactsDir ALL
        ${COPY_COMMANDS}
        DEPENDS ${ALL_PLUGIN_TARGETS}
        COMMENT "Copying all plugin artefacts to ${ARTEFACTS_DEST}"
        VERBATIM
    )
endif()

# =============================================================================
# COPY TO SYSTEM FOLDERS (Windows only - global folder, UAC elevation)
# On Windows, we copy to C:\Program Files\Common Files\VST3\ (Ableton/DAW standard).
# Copy runs elevated via PowerShell (UAC prompt); if user cancels, build still succeeds.
# macOS and Linux use JUCE's built-in copy.
# =============================================================================
if(WIN32 AND COPY_TO_SYSTEM_FOLDERS AND PLUGIN_FORMATS_LIST MATCHES "VST3")
    set(VST3_SYSTEM_DEST "C:/Program Files/Common Files/VST3")
    set(VST3_ELEVATED_SCRIPT "${CMAKE_SOURCE_DIR}/CMake/CopyVst3Elevated.ps1")
    set(VST3_SOURCE_PATH "${CMAKE_BINARY_DIR}/Matrix-Control_artefacts/$<CONFIG>/VST3/Matrix-Control.vst3")
    set(VST3_ARGS_FILE "${CMAKE_BINARY_DIR}/vst3_copy_args.txt")
    add_custom_target(CopyVST3ToSystemFolder ALL
        COMMAND ${CMAKE_COMMAND} -E echo "Copying VST3 to ${VST3_SYSTEM_DEST} (UAC may prompt for admin rights)..."
        COMMAND PowerShell -NoProfile -ExecutionPolicy Bypass -Command "Set-Content -LiteralPath '${VST3_ARGS_FILE}' -Value '${VST3_SOURCE_PATH}','${VST3_SYSTEM_DEST}' -Encoding UTF8"
        COMMAND PowerShell -NoProfile -ExecutionPolicy Bypass -Command "Start-Process PowerShell -Verb RunAs -Wait -ArgumentList '-NoProfile','-ExecutionPolicy','Bypass','-File','${VST3_ELEVATED_SCRIPT}','-ArgsFile','${VST3_ARGS_FILE}'; exit 0"
        DEPENDS Matrix-Control_VST3
        COMMENT "Copying VST3 to ${VST3_SYSTEM_DEST} (elevated)"
        VERBATIM
    )
endif()