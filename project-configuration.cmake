# =============================================================================
# PROJECT CONFIGURATION - Plugin copy settings
# =============================================================================
#
# Edit the USER OPTIONS section below. Do not modify the CODE section.
# Override at configure: cmake .. -DUSER_COPY_TO_ARTEFACTS_DIR=ON
#
# =============================================================================
# USER OPTIONS - Edit these values only
# =============================================================================
#
# COPY_TO_SYSTEM_FOLDERS: ON/OFF
#   When ON: copies plugins to the standard locations where DAWs scan.
#   - Windows: VST3 → C:\Program Files\Common Files\VST3\ (UAC prompt at build; click Yes to copy)
#   - macOS: AU → ~/Library/Audio/Plug-Ins/Components/, VST3 → ~/Library/Audio/Plug-Ins/VST3/
#   - Linux: VST3 → ~/.vst3/
#
# COPY_TO_ARTEFACTS_DIR: ON/OFF
#   When ON: copies build outputs (AU, VST3, Standalone) to a central custom folder
#   (ARTEFACTS_DIR_WINDOWS/MACOS/LINUX below).
#   Organized by platform and architecture (macOS: ARM/Intel/Intel-Rosetta/Universal).
#
# =============================================================================

set(USER_COPY_TO_SYSTEM_FOLDERS ON)
set(USER_COPY_TO_ARTEFACTS_DIR ON)

# =============================================================================
# CODE - Do not edit below
# =============================================================================
set(COPY_TO_SYSTEM_FOLDERS ${USER_COPY_TO_SYSTEM_FOLDERS} CACHE BOOL "Copy plugins to system folders after build (all OS)")
set(COPY_TO_ARTEFACTS_DIR ${USER_COPY_TO_ARTEFACTS_DIR} CACHE BOOL "Copy build outputs to central artefacts folder (organized by platform/architecture)")

set(ARTEFACTS_DIR_WINDOWS "C:/Users/Guillaume/Dev/SDKs/JUCE/Artefacts")
set(ARTEFACTS_DIR_MACOS   "/Volumes/Guillaume/Dev/SDKs/JUCE/Artefacts")
set(ARTEFACTS_DIR_LINUX   "/home/guillaume/Dev/SDKs/JUCE/Artefacts")
