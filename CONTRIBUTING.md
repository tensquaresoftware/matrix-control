# Contributing to Matrix-Control

Thank you for your interest in Matrix-Control! Whether you are a JUCE developer, a Matrix-1000 owner, or simply someone who appreciates clean code and vintage synthesizers, your input is welcome.

------

## Table of Contents

- [Ways to Contribute](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#ways-to-contribute)
- [Reporting Bugs](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#reporting-bugs)
- [Suggesting Features](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#suggesting-features)
- [Contributing Code](https://claude.ai/chat/16b5619a-506c-4e52-9a86-10a1574ec048#contributing-code)
- [Continuous Integration](#continuous-integration)
- [Releasing](#releasing)
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

Every **push** to `main` and every **pull request** targeting `main` runs the [Build and Test](https://github.com/tensquaresoftware/matrix-control/actions/workflows/build-and-test.yml) workflow on GitHub Actions.

### Solo maintainer workflow (default)

Matrix-Control is primarily maintained by a **solo developer on macOS**. The default loop prioritises velocity:

1. **Local gate (before every commit):** configure and build with the macOS Debug preset, run `Matrix-Control_Tests` (see [Reproduce CI locally](#reproduce-ci-locally) below).
2. **Push directly to `main`** — no PR required for routine story work.
3. **CI runs asynchronously** on `main` (full 3-OS matrix, ~12 min). Results are **informational** — they do not block pushes or merges.
4. **Fix cross-platform breaks** when CI reports them (batch fixes are fine between stories or before a release).
5. **Release gate (unchanged):** tag and publish only from a commit where CI is green on all three OS legs.

`main` may temporarily fail CI on Windows or Linux while macOS development continues. That is acceptable pre-release for a solo workflow; JUCE cross-platform abstractions catch most issues, but MSVC/GCC-specific compile errors still happen occasionally.

### Optional PR path

Use a PR when it helps — external contributions, large refactors, or CMake/toolchain changes you want isolated:

| Trigger | Jobs | Typical duration |
|---------|------|------------------|
| **Draft PR** — `opened`, `synchronize`, `reopened` while draft | `release-script-tests` + macOS Debug build/tests | ~5 min |
| **Ready for review** — non-draft PR, `ready_for_review`, or label **`ci-full`** | `release-script-tests` + full 3-OS matrix | ~12 min |
| **Push to `main`** | Always full 3-OS matrix | ~12 min |

Open PRs as **draft** during active review to use the fast tier. Mark **ready for review** or add the **`ci-full`** label when you want the full cross-platform matrix on a PR.

The aggregate job **`ci-success`** (and the three matrix leg names) are informational. They are **not** required status checks on `main`.

### Re-enable merge gate (team mode)

If the project adopts a multi-contributor workflow, restore required checks:

```bash
gh api repos/tensquaresoftware/matrix-control/branches/main/protection/required_status_checks \
  -X PATCH \
  --input - <<'EOF'
{
  "strict": true,
  "contexts": ["release-script-tests", "ci-success"]
}
EOF
```

### Matrix (full tier)

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

### GitHub Actions maintenance

Workflows target the **Node.js 24** runtime on GitHub-hosted runners. GitHub deprecated Node 20 for JavaScript actions in mid-2026; runners remove it in fall 2026. Pin first-party actions to their Node-24 majors (commit `8ac1354`, 2026-07-14):

| Action | Pinned version | Workflow(s) |
|--------|----------------|-------------|
| `actions/checkout` | `@v5` | Build and Test, Release |
| `actions/cache` | `@v5` | Build and Test, Release |
| `actions/upload-artifact` | `@v6` | Release |
| `actions/download-artifact` | `@v7` | Release |

**Upgrade policy:** When GitHub shows Node deprecation annotations, bump to the latest major of each action that declares `node24` in its `action.yml`. Prefer `@v7` over `@v8` for `download-artifact` until you explicitly need v8 behaviour (stricter digest checks, ESM migration).

**Third-party actions:** `sudara/basic-macos-keychain-action@v1` (Release macOS signing) is a **composite** action (shell only) — not affected by the Node runtime migration.

**Self-hosted runners:** Not used today. If added later, require Actions Runner **≥ 2.327.1** for Node 24 actions.

Correct-course archive: `Documentation/Development/Plans/2026/07/2026-07-14-Correct-Course-GitHub-Actions-Node-24-Migration.md` (decision **D-047-T**).

------

## Releasing

Public downloads are published via **GitHub Releases**, triggered by pushing a semver **git tag** (not by merging to `main`).

### Version source of truth

1. Bump `project(Matrix-Control VERSION X.Y.Z)` in `CMakeLists.txt`.
2. Set `MATRIX_CONTROL_PRERELEASE_SUFFIX`:
   - **Stable release:** empty string (`""`) — required for `v1.0.0`.
   - **Pre-release:** suffix matching the tag (e.g. `rc1` for `v1.0.0-rc1`, `alpha` for `v0.2.0-alpha`).
3. Commit, then create an **annotated tag** with a **`v` prefix** matching the version + suffix:
   - Stable: `git tag -a v1.0.0 -m "Matrix-Control 1.0.0"`
   - RC: `git tag -a v1.0.0-rc1 -m "Matrix-Control 1.0.0-rc1"`
4. Push the tag: `git push origin v1.0.0-rc1`

The [Release](https://github.com/tensquaresoftware/matrix-control/actions/workflows/release.yml) workflow validates the tag against `CMakeLists.txt`, builds **Release** binaries on macOS / Windows / Linux, runs unit tests, packages per-OS zips, and publishes a GitHub Release.

### Recommended flow (RC → stable)

1. Clear blockers (e.g. Epic U-10 release gate — no `TestComponent` in Release builds).
2. Bump version + set suffix (e.g. `rc1`), tag `vX.Y.Z-rc1`, push tag.
3. Download assets from the GitHub Release; smoke-test on each OS (DAW + standalone).
4. For stable: set `MATRIX_CONTROL_PRERELEASE_SUFFIX=""`, commit, tag `vX.Y.Z`, push tag.

### Required GitHub secrets (macOS signing)

Configure in **Settings → Secrets and variables → Actions** (names only — never commit values):

| Secret | Purpose |
|--------|---------|
| `DEV_ID_APP_CERT` | Base64-encoded `.p12` Developer ID Application certificate |
| `DEV_ID_APP_PASSWORD` | Password for the `.p12` |
| `DEVELOPER_ID_APPLICATION` | Codesign identity name (e.g. `Developer ID Application: …`) |
| `NOTARIZATION_USERNAME` | Apple ID for notarytool |
| `NOTARIZATION_PASSWORD` | App-specific password for notarytool |
| `APPLE_TEAM_ID` | Apple Developer Team ID |

The macOS release leg **fails** if any of these are missing — there is no silent skip of signing/notarization.

**Windows Authenticode** signing is optional for v1 (unsigned VST3/Standalone is acceptable for MIT open-source distribution).

### Local packaging (fallback)

If CI is unavailable, build Release locally and pack with:

```bash
cmake --preset macos-release-arm64 \
  -DMATRIX_BUILD_TESTS=ON \
  -DUSER_COPY_TO_SYSTEM_FOLDERS=OFF \
  -DUSER_COPY_TO_ARTEFACTS_DIR=OFF \
  -DMATRIX_CONTROL_PRERELEASE_SUFFIX=""
cmake --build --preset macos-release-arm64 --target Matrix-Control
python3 Scripts/release/prepare_release.py --tag v1.0.0 pack macos --arch arm64
```

Release script unit tests:

```bash
python3 -m pip install -r Scripts/release/requirements-test.txt
python3 -m pytest Scripts/release/tests/ -q
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