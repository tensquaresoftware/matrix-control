# Matrix-Control

![Matrix-Control GUI](https://raw.githubusercontent.com/tensquaresoftware/Matrix-Control/main/Assets/Screenshots/Matrix-Control-GUI-WIP.png)

[![Status: Work in Progress](https://img.shields.io/badge/Status-Work%20in%20Progress-FFA500?style=for-the-badge)](https://github.com/tensquaresoftware/Matrix-Control)
[![Language: C++](https://img.shields.io/badge/Language-C%2B%2B-00599C?style=for-the-badge&logo=c%2B%2B&logoColor=white)](https://github.com/tensquaresoftware/Matrix-Control)
[![Framework: JUCE 8](https://img.shields.io/badge/Framework-JUCE%208-5765A4?style=for-the-badge)](https://juce.com)
[![Formats: AU / VST3 / Standalone](https://img.shields.io/badge/Formats-AU%20%7C%20VST3%20%7C%20Standalone-2E86AB?style=for-the-badge)](https://github.com/tensquaresoftware/Matrix-Control)
[![License: MIT](https://img.shields.io/badge/License-MIT-green?style=for-the-badge)](LICENSE)
[![Sponsor](https://img.shields.io/badge/Sponsor-Ten%20Square%20Software-ff69b4?style=for-the-badge&logo=github-sponsors&logoColor=white)](https://github.com/sponsors/tensquaresoftware)

> 💛 If you find this project useful or inspiring, consider [sponsoring its development](https://github.com/sponsors/tensquaresoftware) — every contribution helps keep the work going!

---

## What is Matrix-Control?

**Matrix-Control** is a cross-platform MIDI editor plugin for the **Oberheim Matrix-1000** synthesizer — one of the most iconic and underestimated rack synths of the 1980s.

The Matrix-1000 shares almost the same synthesis engine as the legendary Matrix-6 and Matrix-6R, but unlike its siblings, it has no front-panel knobs or dedicated patch editor — making it notoriously difficult to program. Matrix-Control gives you full, intuitive, real-time control over every parameter of your Matrix-1000, directly from your DAW or as a standalone application.

### Supported formats

| Platform | AU | VST3 | Standalone |
|---|---|---|---|
| macOS | ✅ | ✅ | ✅ |
| Windows | — | ✅ | ✅ |
| Linux | — | 🔜 | 🔜 |

---

## Why this project exists — a little story

Back in 2020, a friend lent me his Matrix-1000 for an album project. He had also bought a dedicated hardware programmer for it, but found it unintuitive. I suggested building a software MIDI editor instead — and so the **Matrix-1000 Editor**, a Max for Live device, was born.

That first editor turned out to be well received by the community. It is freely available on [maxforlive.com](https://www.maxforlive.com/library/device/6806/matrix-1000-editor), and was featured in a [tutorial by Cycling '74](https://cycling74.com/tutorials/building-a-synthesizer-editor-with-javascript-part-1) (written by Jeremy Bernstein, a developer at Cycling '74 and a fellow Matrix-1000 enthusiast). It was also covered by [Audiofanzine](https://fr.audiofanzine.com/editeur-midi/tensquare/matrix-1000-editor/news/a.play,n.50627.html).

Over time, users of the Max for Live editor requested features that were difficult to implement within that environment. This led to a natural evolution: **Matrix-Control** — a full rewrite as a proper JUCE plugin, designed from the ground up with clean architecture, comprehensive documentation, and cross-platform support.

---

## Project goals

Matrix-Control is not just a plugin — it is also an attempt to provide a well-architected, thoroughly documented JUCE project can look like in practice.

Concretely, this means:

- **Clean code** — following established principles such as Clean Code, Clean Architecture, SOLID, and DRY (with thanks to Uncle Bob for the inspiration 😄)
- **Thorough technical documentation** — including a detailed technical specification, software architecture diagrams, GUI modeling, and developer guides — all written in English
- **User documentation** — a clear and complete user guide for the plugin
- **A project structure that others can learn from** — if Matrix-Control can serve as a useful reference or case study for other developers building JUCE plugins, that would be a tremendous bonus

---

## Current status

The project is actively under development. As of early 2026:

- ✅ Project structure, build system (CMake), and CI pipeline are in place
- ✅ Core MIDI infrastructure (SysEx parsing, validation, sending) is functional
- ✅ GUI layout and visual design are well advanced
- 🔄 Full patch editing (read/write all Matrix-1000 parameters) — in progress
- 🔜 Patch management (save, load, tag, organize)
- 🔜 Randomization / mutation engine
- 🔜 Linux support
- 🔜 User guide and full technical documentation

---

## Supporting the project

Matrix-Control is developed in my free time, alongside a day job as an instructional designer for an [Ableton-certified training center](https://fastlane.fr) in Montpellier, France. The main tool powering my development workflow is [Claude Code](https://claude.com/claude-code), which has been instrumental in accelerating progress on this project — especially during the large-scale refactoring phases.

If you enjoy the Matrix-1000 Editor or are looking forward to Matrix-Control, sponsoring the project on [GitHub Sponsors](https://github.com/sponsors/tensquaresoftware) is the most direct way to help keep the development going. Every contribution, no matter the size, is genuinely appreciated.

---

## Build Instructions

### Prerequisites

#### macOS

- macOS Tahoe or later
- VS Code (with the CMake Tools and C/C++ extensions)
- CMake 3.22+
- Ninja build system
- JUCE 8 installed (set `JUCE_DIR` environment variable to your JUCE installation path, e.g., `/Applications/JUCE`)

#### Windows

- Windows 11 or later
- VS Code (with the CMake Tools and C/C++ extensions)
- CMake 3.22+ (add to system PATH during installation)
- Visual Studio 2022 with "Desktop development with C++" workload
- JUCE 8 installed (set `JUCE_DIR` environment variable to your JUCE installation path, e.g., `C:\JUCE`)

### Environment Setup

Set the `JUCE_DIR` environment variable to point to your JUCE installation:

**macOS:**

```bash
export JUCE_DIR=/Applications/JUCE
```

**Windows:**

```
# System environment variable (recommended)
# Set via: System Properties → Environment Variables → System variables
# Name: JUCE_DIR
# Value: C:\JUCE
```

### Build

#### macOS

```bash
# Configure (using Ninja)
cmake -B Builds/macOS -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build all formats
cmake --build Builds/macOS --config Debug

# Or build specific format
cmake --build Builds/macOS --target Matrix-Control_Standalone --config Debug
cmake --build Builds/macOS --target Matrix-Control_AU --config Debug
cmake --build Builds/macOS --target Matrix-Control_VST3 --config Debug
```

#### Windows

```powershell
# Configure (using Visual Studio 2022)
cmake -B Builds/Windows -G "Visual Studio 17 2022" -DCMAKE_BUILD_TYPE=Debug

# Build all formats
cmake --build Builds/Windows --config Debug

# Or build specific format
cmake --build Builds/Windows --target Matrix-Control_Standalone --config Debug
cmake --build Builds/Windows --target Matrix-Control_VST3 --config Debug
```

#### Linux

```bash
# Configure (using Ninja)
cmake -B Builds/Linux -G Ninja -DCMAKE_BUILD_TYPE=Debug

# Build all formats
cmake --build Builds/Linux --config Debug
```

> **Note:** Audio Unit (AU) format is only available on macOS. On Windows, only VST3 and Standalone formats are built.

### Using VS Code

1. Open the project folder in VS Code
2. CMake Tools extension will automatically detect the project
3. Select your build kit (CMake Tools will prompt you)
4. Use the build tasks:
   - `Ctrl+Shift+P` (or `Cmd+Shift+P` on macOS) → "Tasks: Run Task"
   - Select "Build: All" or a specific format

### Plugin Installation

#### macOS

- **AU**: Copy `.component` file to `~/Library/Audio/Plug-Ins/Components/`
- **VST3**: Copy `.vst3` bundle to `~/Library/Audio/Plug-Ins/VST3/`
- **Standalone**: Run the `.app` directly

#### Windows

- **VST3**: Copy `.vst3` folder to `C:\Program Files\Common Files\VST3\`
- **Standalone**: Run the `.exe` directly

### Debugging

Debug configurations are available in `.vscode/launch.json`:

- **macOS**: Standalone, AU in Logic Pro, VST3 in Reaper, AU in Ableton Live
- **Windows**: Standalone, VST3 in Reaper

Press `F5` in VS Code to start debugging.

---

## Fonts

Matrix-Control uses **PT Sans Narrow** (Regular and Bold) for its user interface. These fonts are licensed under the [SIL Open Font License (OFL) 1.1](https://openfontlicense.org).

- **Copyright:** © 2010, [ParaType Ltd.](https://www.paratype.com/public), with Reserved Font Names "PT Sans" and "ParaType".
- **License:** [OFL 1.1](https://scripts.sil.org/OFL) — free for use, modification, and redistribution. Full license text is available in `Assets/Fonts/OFL.txt`.

---

## About the author

Guillaume DUPONT — instructional designer, synthesizer enthusiast, and occasional developer. Based in Montpellier, France.

- [Matrix-1000 Editor (Max for Live)](https://www.maxforlive.com/library/device/6806/matrix-1000-editor)
- [Matrix-1000 Editor Presentation at Montpellier Ableton User Group / FASTLANE (YouTube)](https://www.youtube.com/watch?v=DLOBp0x1ykY)
- [FASTLANE (Instructional Designer)](https://fastlane.fr/notre-equipe/guillaume-dupont/)
- [LinkedIn](https://www.linkedin.com/in/guillaumedupontmontpellier/)
