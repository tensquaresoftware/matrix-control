# Contributing to Matrix-Control

Thank you for your interest in Matrix-Control! Whether you are a JUCE developer, a Matrix-1000 owner, or simply someone who appreciates clean code and vintage synthesizers, your input is welcome.

------

## Table of Contents

- [Ways to Contribute](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#ways-to-contribute)
- [Reporting Bugs](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#reporting-bugs)
- [Suggesting Features](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#suggesting-features)
- [Contributing Code](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#contributing-code)
- [Continuous Integration](#continuous-integration)
- [Code Style Guidelines](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#code-style-guidelines)
- [Commit Message Convention](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#commit-message-convention)
- [Pull Request Process](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#pull-request-process)
- [Supporting the Project](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#supporting-the-project)

------

## Ways to Contribute

You do not need to write code to contribute. Here are several ways you can help:

- **Report a bug** — unexpected behaviour, crashes, or incorrect MIDI output
- **Suggest a feature** — something that would make Matrix-Control more useful or enjoyable
- **Improve documentation** — typos, unclear explanations, missing build instructions
- **Test on hardware** — if you own a Matrix-1000 (or a Matrix-6 / Matrix-6R), your feedback on real-hardware behaviour is invaluable
- **Contribute code** — bug fixes, new features, performance improvements
- **Sponsor the project** — see [Supporting the Project](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#supporting-the-project)

------

## Reporting Bugs

Before opening a bug report, please check the [existing issues](https://github.com/tensquaresoftware/matrix-control/issues) to avoid duplicates.

When filing a bug, please include:

- **Your environment** — macOS version, plugin format (AU / VST3 / Standalone), DAW name and version if applicable
- **Steps to reproduce** — the minimal sequence of actions that triggers the bug
- **Expected behaviour** — what you expected to happen
- **Actual behaviour** — what actually happened
- **Logs** — if relevant, attach any output from the plugin's debug logging (see `Logs/` in the project)
- **MIDI captures** — if the bug involves incorrect SysEx or MIDI behaviour, a MIDI Monitor capture is extremely helpful

Use the **Bug Report** issue template when available.

------

## Suggesting Features

Feature suggestions are welcome via [GitHub Issues](https://github.com/tensquaresoftware/matrix-control/issues) using the **Feature Request** template.

Please describe:

- The problem you are trying to solve, or the workflow you have in mind
- How you imagine the feature working in the context of the plugin
- Any relevant reference (e.g. behaviour of another editor, Matrix-1000 SysEx spec detail)

Note that Matrix-Control is primarily designed for the **Matrix-1000**. Features specific to the Matrix-6 or Matrix-6R may be considered if they are architecturally compatible.

------

## Contributing Code

### Prerequisites

Please make sure you are comfortable with the project's build setup before contributing. See the [Build Instructions](https://claude.ai/chat/README.md#build-instructions) in the README.

Familiarity with the following is recommended:

- **C++17**
- **JUCE 8** framework (particularly `AudioProcessor`, `AudioProcessorValueTreeState`, MIDI I/O)
- **CMake** as a build system
- Basic knowledge of **MIDI SysEx** and the Matrix-1000 parameter model is a plus

### Getting Started

1. **Fork** the repository on GitHub

2. **Clone** your fork locally

3. Create a **feature branch** from `main`:

   ```bash
   git checkout -b feature/my-feature-name
   ```

4. Make your changes, following the [Code Style Guidelines](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#code-style-guidelines)

5. **Test** your changes (standalone mode is the fastest feedback loop; test against real hardware if possible)

6. **Commit** using the [commit convention](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#commit-message-convention)

7. **Push** your branch and open a **Pull Request**

------

## Continuous Integration

Every **push** and **pull request** targeting `main` runs the [Build and Test](https://github.com/tensquaresoftware/matrix-control/actions/workflows/build-and-test.yml) workflow on GitHub Actions.

### Matrix

| Runner | CMake preset | Toolchain |
|--------|--------------|-----------|
| `macos-latest` (Apple Silicon) | `macos-debug-arm64` | Ninja |
| `windows-latest` | `windows-debug` | Visual Studio 2026 (x64) |
| `ubuntu-latest` | `linux-debug` | Ninja |

Each leg:

1. Checks out JUCE **8.0.12** (cached between runs)
2. Configures with `MATRIX_BUILD_TESTS=ON` and plugin copy disabled (`USER_COPY_TO_*=OFF`)
3. Builds the `Matrix-Control` plugin target and `Matrix-Control_Tests`
4. Runs the `Matrix-Control_Tests` console binary (headless Core unit tests — no MIDI hardware)

The matrix uses `fail-fast: false` so all three OS results appear in one run.

### Reproduce CI locally

Set `JUCE_DIR` to your JUCE 8.0.12 install, then configure with the preset for your platform and the same CI flags:

**macOS (Apple Silicon):**

```bash
export JUCE_DIR=/Applications/JUCE
cmake --preset macos-debug-arm64 \
  -DMATRIX_BUILD_TESTS=ON \
  -DUSER_COPY_TO_SYSTEM_FOLDERS=OFF \
  -DUSER_COPY_TO_ARTEFACTS_DIR=OFF
cmake --build --preset macos-debug-arm64 --target Matrix-Control Matrix-Control_Tests
"Builds/macOS/ARM/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests"
```

**Windows (PowerShell):**

```powershell
$env:JUCE_DIR = "C:\JUCE"
cmake --preset windows-debug `
  -DMATRIX_BUILD_TESTS=ON `
  -DUSER_COPY_TO_SYSTEM_FOLDERS=OFF `
  -DUSER_COPY_TO_ARTEFACTS_DIR=OFF
cmake --build --preset windows-debug --target Matrix-Control Matrix-Control_Tests --config Debug
& "Builds/Windows/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests.exe"
```

**Linux (Ubuntu/Debian):**

Install the same packages as the workflow (`build-essential`, `ninja-build`, ALSA/JACK, FreeType, X11/Mesa dev libs — see `.github/workflows/build-and-test.yml`), then:

```bash
export JUCE_DIR=/path/to/JUCE
cmake --preset linux-debug \
  -DMATRIX_BUILD_TESTS=ON \
  -DUSER_COPY_TO_SYSTEM_FOLDERS=OFF \
  -DUSER_COPY_TO_ARTEFACTS_DIR=OFF
cmake --build --preset linux-debug --target Matrix-Control Matrix-Control_Tests
"Builds/Linux/Debug/Matrix-Control_Tests_artefacts/Debug/Matrix-Control_Tests"
```

------

## Code Style Guidelines

Matrix-Control follows the principles of **Clean Code** (Robert C. Martin) and **Clean Architecture**. If you are not familiar with these, the short version is: write code that reads like well-structured prose, with clear intent and no surprises.

Concretely:

- **Naming** — use descriptive, intention-revealing names. Avoid abbreviations unless they are universally understood (e.g. `MIDI`, `SysEx`, `AU`).
- **Functions** — keep them short and focused on a single responsibility.
- **Comments** — prefer self-documenting code over comments. When a comment is necessary, explain *why*, not *what*.
- **SOLID principles** — apply where relevant, particularly single responsibility and dependency inversion.
- **No magic numbers** — use named constants, especially for MIDI parameter indices and SysEx byte positions.
- **Formatting** — follow the existing indentation and brace style of the surrounding code. A `.clang-format` file will be added to the project in a future update.

When in doubt, look at the existing `Source/Core` code as a reference.

------

## Commit Message Convention

Matrix-Control uses a structured commit message format inspired by [Conventional Commits](https://www.conventionalcommits.org/):

```
<type>(<scope>): <short description>
```

**Types:**

| Type       | When to use                                             |
| ---------- | ------------------------------------------------------- |
| `feat`     | A new feature                                           |
| `fix`      | A bug fix                                               |
| `refactor` | Code change that neither fixes a bug nor adds a feature |
| `docs`     | Documentation changes only                              |
| `test`     | Adding or updating tests or profiling data              |
| `chore`    | Build system, CI, dependency updates                    |
| `style`    | Formatting, whitespace — no logic change                |

**Scopes** (examples): `core`, `gui`, `midi`, `sysex`, `apvts`, `build`, `docs`

**Examples:**

```
feat(sysex): add support for Matrix-1000 single patch dump request
fix(gui): correct slider range for oscillator 2 detune parameter
refactor(core): extract SysEx validation into dedicated class
docs(readme): update Windows build instructions for JUCE 8.0.8
```

Keep the short description under 72 characters. Use the imperative mood ("add", "fix", "extract" — not "added", "fixed", "extracted").

------

## Pull Request Process

1. Ensure your branch is up to date with `main` before opening the PR
2. Fill in the PR template (description, type of change, testing notes)
3. Link any related issue(s) using `Closes #<issue-number>` in the PR description
4. Keep PRs focused — one feature or fix per PR makes review much easier
5. Be patient — this project is maintained in spare time. PRs will be reviewed as promptly as possible.

------

## Supporting the Project

Matrix-Control is developed in my free time. If you find it useful or want to help sustain its development, you can sponsor the project via [GitHub Sponsors](https://github.com/sponsors/tensquaresoftware).

Every contribution, large or small, is genuinely appreciated and helps keep the project moving forward.

------

*Guillaume DUPONT — Montpellier, France*