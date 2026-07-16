# Personal Instructions

**Author:** Guillaume DUPONT
**Organization:** Ten Square Software
**Context:** VST/AU plugin development with JUCE (Cursor + Claude Code)
**Revision date:** 2026-06-19

---

## Purpose of this document

This file defines my personal instructions for the AI coding agent. It guides the agent in code generation, documentation, advice, and responses by establishing a set of rules, standards, and conventions to follow throughout our development sessions.

---

## 1. Communication

### 1.1 Interactions with me

- My name is Guillaume, you can call me by my first name
- Always use informal tone ("tu" en français) in all our exchanges
- Reply to me exclusively in French in our conversations
- **Plain French, no anglicisms or management jargon** — Avoid "trade-off", "ledger", "workflow", "pain point", "scope", "deliverable", etc. Prefer French terms: "compromis", "fichier source de vérité", "découplage", "formule", "garde-fou à la compilation", "déroulement", "point de friction"
- **Complete sentences** — No telegraphic prose or chat abbreviations (fn, impl, req/res, etc.)
- Keep **English only for project identifiers**: class names, file names, symbols, APIs, error messages (e.g. `Factory`, `static_assert`, `removeFromTop`)
- Be frank, honest, and factual, even if it means contradicting me
- Absolute priority: help me find the most relevant solution!

### 1.2 Structure of responses

- Goal: help me progress with clear, proportionate answers — not essays
- Ensure longevity and maintainability of generated code
- Optimal solutions compliant with professional standards
- **Default tone: plain, natural French** — Questions and explanations without unnecessary jargon. Technical terms only when truly needed; then keep them short or add a brief plain-language gloss.
- When introducing a new or abstract concept, provide a short concrete example to make it easier to understand
- **No redundant repetition** — One well-written explanation or question per turn is enough. Do not restate the same point as intro + detailed body + summary + closing recap. Do not rephrase the same idea under multiple angles in one message. No filler openers ("Bien sûr !", "Excellente question !") or forced follow-up lists. Respect the token budget.
- **Response length proportional to the question** — A simple question deserves a short answer
- **DO NOT** paste full code blocks in the chat when modifying/generating
- Prefer summaries and conclusions in natural language only
- The diffs are already shown by the tool, no need to repeat the code in the chat

---

## 2. Process & Workflow

> ⚠️ **MANDATORY BEFORE ANY CODING**

### 2.1 Three-phase workflow

**STOP!** Always follow this workflow rigorously:

#### Phase 1: DESIGN (mandatory before touching code)

- Identify responsibilities (Single Responsibility Principle)
- Detect potential duplication (Don't Repeat Yourself — see §3.1 and §6.16 for WET nuance)
- Choose appropriate abstractions (interfaces, base classes) — **only what the current need requires (YAGNI)**
- Prefer the **simplest design that works (KISS)** over speculative generality
- Ask: **Does this choice make future changes easier (ETC — Easy to Change)?**
- Plan classes/functions with explicit, intention-revealing names
- Estimate method sizes (target: < 15 lines)
- Ask myself: "Would Uncle Bob be proud of this design?"
- Mentally sketch the architecture (dependencies, hierarchy)

#### Phase 2: IMPLEMENTATION

- Write code strictly following the Phase 1 design
- Respect quantifiable limits (see dedicated section below)
- If a method exceeds 15 lines → **STOP** and extract helpers IMMEDIATELY
- If I detect duplication (even 3 lines) → evaluate: is it **stable and meaningful**? If yes → extract; if first occurrence → **WET is OK** (see §6.16)
- If a class exceeds 200 lines → **STOP** and re-evaluate the design (SRP likely violated)
- Name each function to reveal its intent (no vague `doStuff()` or `process()`)

#### Phase 3: AUTO-REVIEW (before presenting code to Guillaume)

- Reread with a critical "Uncle Bob" eye
- Verify SOLID for each class (full checklist below)
- Verify that each function does ONE thing only
- Verify there is NO duplication
- Verify names: are they all explicit and reveal intent?
- If review fails → return to Phase 1 (do not present non-Clean code)

### 2.2 Exception: Rapid prototyping

If Guillaume explicitly requests a "rapid prototype", "POC", or "spike", I may skip Phase 1 but I MUST propose a Clean refactoring after validation.

### 2.3 Document metadata

When I modify a file (documentation, code, configuration, etc.) that contains a **revision date** or **version number** in its header or metadata, I MUST update them accordingly.

### 2.4 BMad — titres des conversations Cursor Agents

When Guillaume starts a conversation with a **BMad skill** (attached skill or slash command), the agent MUST **rename the current chat immediately** in the Cursor Agents sidebar, before any other work.

Use the MCP tool `rename_chat` (`cursor-app-control`). Cursor rule: `.cursor/rules/bmad-agent-chat-titles.mdc` (authoritative detail).

**No date in the title.** The Agents sidebar already shows when each conversation took place.

**Format A — story triad** (ID only, no colon):

| Command | Sidebar title |
|---|---|
| `/bmad-create-story 4-5` | `BMad — Create Story 4-5` |
| `/bmad-dev-story 4-5` | `BMad — Dev Story 4-5` |
| `/bmad-code-review 4-5` | `BMad — Code Review 4-5` |

**Format B — all other BMad commands:** `BMad — {Workflow Label} : {Topic}`

- Workflow labels stay in English (`Quick Dev`, `Correct Course`, …).
- Topic after the colon is **French** (short, 2–5 words, no filler) — not English Title Case. Product/UI terms may stay as-is inside the French phrase (`Look`, `Patch Mutator`, `MIDI`).
- Omit the colon and topic when none is useful (e.g. `/bmad-help` → `BMad — Help`).

| Command (examples) | Sidebar title |
|---|---|
| `/bmad-quick-dev` (disabled controls look) | `BMad — Quick Dev : Look contrôles inactifs` |
| `/bmad-quick-dev` (mutator work) | `BMad — Quick Dev : Patch Mutator` |
| `/bmad-correct-course` (bank unlock) | `BMad — Correct Course : Unlock banque` |
| `/bmad-sprint-status` | `BMad — Sprint Status` |

**Duplicates:** if the base title is already used, append ` (n)` with the next free positive integer (`(1)`, `(2)`, …). See the Cursor rule for the read-only DB check before `rename_chat`.

Story IDs include epic-story numbers (`4-5`, `7-3b`) and utility stories (`U-0`, `U-11`).

**Persistence:** only `rename_chat` writes the title durably. The bulk script `_local/rename-bmad-agent-chats.py` updates the sidebar cache only — opening a conversation restores the old auto-generated title from the checkpoint. To fix existing conversations, open each one and send any message; the agent will rename via `rename_chat` on first reply.

---

## 3. Quantifiable Limits

> 🚫 **Strict, non-negotiable rules**

### 3.1 Code metrics - MAXIMUM limits

These limits are **HARD LIMITS**, not suggestions:

#### Function/Method: 15 lines MAXIMUM (ideal: 5-10)

- **Exception:** `show()` or orchestration method may go up to 20 lines IF it delegates properly
- **Beyond that:** extract immediately into helpers with explicit names

#### Class: 200 lines MAXIMUM (ideal: < 150)

- **If exceeded:** probable SRP violation → split into multiple classes

#### Function parameters: 3 MAXIMUM

- **If > 3:** create a struct/class of parameters with explicit names

#### Cyclomatic complexity: < 5 per function (ideal: 1-3)

- **Too many `if`/`switch`** → extract into separate functions or use polymorphism

#### Indentation levels: 2 MAXIMUM

- No `if` nested in `for` nested in `if`
- **Solution:** early returns, function extraction, guard clauses

#### Code duplication: extract when stable — WET before premature abstraction

- **First occurrence (WET):** duplicating once is acceptable — do not abstract prematurely
- **Second stable occurrence:** evaluate whether extraction improves clarity (ETC)
- **Third similar occurrence** or confirmed stable duplication → **mandatory extraction** into a common function
- Use helpers, templates, or utility functions — but only when duplication is real, not imagined

### 3.2 Procedure when I exceed a limit

1. **DO NOT** continue writing code
2. **STOP** immediately
3. Ask me: "Why is this limit exceeded?"
4. Answer honestly: "Because this function/class does several things"
5. Apply SRP: extract responsibilities into new functions/classes
6. Start over with the correct design

### 3.3 Warning signs - Indicators of bad design

- I tell myself "this function is a bit long but it's fine" → ❌ **NO**, refactor
- I see similar code elsewhere → evaluate stability; factorize when duplication is confirmed, not on first copy
- I can't find a good name for the function → Sign it does too many things
- I need a comment to explain → Code is not clear enough, rename/refactor
- I add a comment like "// Part 1", "// Part 2" → Each part = separate function

---

## 4. Self-Critique

> ✅ **Mandatory checklist before presenting code**

Before presenting code to Guillaume, I MUST systematically verify:

### 4.1 SOLID Check ✓

- [ ] **S**ingle Responsibility: Does each class have ONE well-defined responsibility?
  - Test: can I describe the class in one simple sentence without "and"?
- [ ] **O**pen/Closed: Is the code extensible without modifying existing classes?
  - Use of abstractions, interfaces, polymorphism?
- [ ] **L**iskov Substitution: Are subclasses substitutable without breaking behavior?
  - Do derived classes respect base class contracts?
- [ ] **I**nterface Segregation: No "fat" interfaces forcing implementation of useless methods?
  - Small, focused interfaces?
- [ ] **D**ependency Inversion: Dependencies toward abstractions, not concrete implementations?
  - Do high-level modules not depend on low-level details?

### 4.2 Clean Code Check ✓

- [ ] All functions < 15 lines? (ideally < 10)
  - If NO: extract immediately
- [ ] All names are explicit and reveal intent?
  - `calculateTotalPrice()` not `calc()`, `isUserLoggedIn()` not `check()`
- [ ] No code duplication beyond WET tolerance (see §3.1)?
  - If stable duplication exists: factorize into common function
- [ ] Single level of abstraction per function?
  - No mix of high-level calls + low-level manipulations
- [ ] No explanatory comments needed?
  - Code must be self-documenting via names
- [ ] No magic numbers?
  - All constants are named: `kMaxRetries` not `3`
- [ ] No dead or commented code?
  - Remove, versioning (git) keeps history

### 4.3 Architecture & Design Check ✓

- [ ] Dependencies point in the right direction?
  - GUI → Core, never Core → GUI
  - High-level modules → abstractions ← low-level modules
- [ ] No tight coupling between modules?
  - Use of interfaces to decouple
- [ ] Responsibilities clearly separated?
  - Rendering, business logic, state, coordination: each in its own class
- [ ] Easily testable?
  - No hidden dependencies, dependency injection

### 4.4 Readability Check ✓

- [ ] Can a developer discovering the code understand it without explanation?
- [ ] Do classes and functions have names that document their role?
- [ ] Is the structure logical and predictable?
- [ ] Are there no "surprises" or hidden side effects?

### 4.5 Design Principles Check ✓

- [ ] **KISS:** Is this the simplest design that satisfies the current requirement?
- [ ] **YAGNI:** Did I avoid features, abstractions, or parameters not needed now?
- [ ] **ETC:** Does this design make the next likely change easier, not harder?
- [ ] **Boy Scout Rule:** Is the touched code at least slightly cleaner than before?
- [ ] **CQS:** Do mutating functions avoid returning queried state (and vice versa)?

> Full reference: `Documentation/Development/software-development-quality-principles.md` and §6.16

### 4.6 Golden rule

> ❌ **If ONE ✗ → I do NOT present the code**
>
> - I refactor first until ALL ✓ are validated
> - Only then do I present the result to Guillaume

---

## 5. Development Environment

### 5.1 System & Tools

- **Platform:** MacBook Pro M5 with macOS Tahoe
- **IDE / AI agent:** Cursor Pro+ (project rules in `.cursor/rules/`)
- **Compiler:** Xcode 26
- **Build system:** CMake
- **Build directory:** `Builds/` (subfolders `macOS/`, `Windows/`, `Linux/`) — do not use `build/` at root
- **Audio framework:** JUCE 8.0.12

### 5.2 JUCE reference

- **JUCE installation:** `/Applications/JUCE`
- **Official documentation:** https://docs.juce.com
- **Modules:** `/Applications/JUCE/modules/`
- **CMake:** `/Applications/JUCE/CMake/`

### 5.3 Verification of generated code

- Always refer to the most up-to-date JUCE documentation
- Verify that each API, class, and method exists in JUCE 8
- Avoid any obsolete syntax (JUCE 7, JUCE 6, etc.)
- Explicitly report if unsure about an API

---

## 6. C++ Standards & Quality

### 6.1 C++ generalities

- **Standard:** C++17 minimum, compatible with Xcode 26
- **Source code language:** English only
  - Variable, function, class names
  - All code comments
  - No accents, no French in code

### 6.2 Git

#### Commits

- Git Commits: Summary/Description in English only
- **Create commits only on explicit request**
- Never commit proactively or automatically, even after successful refactoring
- Always detail the commit with a main summary line (title) followed by a bullet list of the most important points (same format as annotated tags and releases)

**Format:**

```
Short summary line (imperative mood)

- Bullet point describing the first significant change
- Bullet point describing the second significant change
- etc.
```

- Focus on architectural and significant changes, not minor implementation details

#### GitHub Issues

- GitHub Issues: title and description in English only
- The repository must remain ready for collaboration with other developers (unless special mention otherwise)

#### Annotated tags

When creating an annotated Git tag, use the following format:

- A main description sentence (tag title)
- A short bullet list of the most important points that were reworked in the code

**Example:**

```
Refactor Slider class and improve focus management

- Refactor Slider: simplify drawing code by removing unnecessary constants and helper methods
- Use reduced() for bounds calculations directly in paint() method
- Improve focus management: add ability to remove focus by clicking on background
```

- Do not include implementation details or temporary test widgets
- Focus on architectural improvements and significant code changes

### 6.3 Naming conventions

#### Variables & Methods: lowerCamelCase

- Examples: `audioBuffer`, `processMidiEvents()`, `getSampleRate()`

#### Public variables and constants

- No prefix or suffix
- Examples: `pluginFactory`, `kMaxBufferSize`

#### Private variables and constants

- Underscore suffix (Google Style)
- Examples: `apvts_`, `kWidth_`

#### Classes: PascalCase

- Examples: `PluginProcessor`, `PluginEditor`, `AudioAnalyzer`

#### Enums

- PascalCase for type, `k` prefix for values
- Examples: `enum class ParameterType { kDco, kEnv, kLfo };`

#### Namespaces: PascalCase

- Examples: `namespace Core { }`, `namespace GUI { }`

#### General rule

- Avoid `underscore_case`, except in very rare cases

### 6.4 Magic Numbers - FORBIDDEN

- **NEVER** use magic numbers in code!
- Always define explicit constants with meaningful names

#### Examples to avoid

- ❌ `if (value == -1)` → ✅ `if (value == kNoSysExId)`
- ❌ `for (int i = 0; i < 10; ++i)` → ✅ `for (int i = 0; i < kModulationBusCount; ++i)`
- ✅ `return nullptr;` → OK (nullptr is explicit, not a magic number)

#### Acceptable exceptions

- Obvious self-explanatory literal values: `0`, `1`, `nullptr`, `true`, `false`
- Values in unit tests where the exact value is the subject of the test

### 6.5 Include organization

1. C++ system headers (e.g. `<vector>`, `<memory>`, etc. then blank line)
2. JUCE headers (e.g. `<juce_core/juce_core.h>`, etc. then blank line)
3. Project headers (e.g. `"Core/PatchModel.h"`, etc.)

### 6.6 Include paths

- Use relative paths from project root (configured in CMake)
- **NEVER** use relative paths with multiple `../`
- **Format:** `"GUI/Panels/MainComponent.h"` rather than `"../../../../../GUI/Panels/MainComponent.h"`
- Paths must be clear, readable, and maintainable
- ✅ Correct example: `#include "GUI/Widgets/Slider.h"`
- ❌ Example to avoid: `#include "../../../GUI/Widgets/Slider.h"`

### 6.7 Include Guards

- Always use `#pragma once` as first line of each header
- Do not use old include guards (`#ifndef`/`#define`/`#endif`)
- Example: `#pragma once` as first line, before any other code

### 6.8 Forward Declarations

- Use forward declarations in headers to reduce dependencies
- Include full headers only in `.cpp` when possible
- Example: `class MidiManager;` in header, then `#include "MidiManager.h"` in `.cpp`

### 6.9 .h/.cpp separation

Always separate declaration (`.h`) and definition (`.cpp`) **except for**:

- Templates (definitions in header)
- Explicit inline functions (marked `inline`)
- Small trivial functions (simple getters/setters)

Prefer inline methods in header only if they are short (< 5 lines)

- ✅ Inline example: `int getValue() const { return value_; }` in header
- ✅ Separated example: declaration in `.h`, full definition in `.cpp`

### 6.10 Class organization

- Group members and methods logically in `.h` (e.g. getter/setter pairs)
- Member variables (almost always private) after public/protected methods
- Private methods at the end, after member variables
- Use `JUCE_DECLARE_NON_COPYABLE` if class is not copyable (last element)
- Mark `explicit` for single-parameter constructors
- In `.cpp`, group methods logically (called near their callers)
- Virtual destructors if class is meant to be inherited (polymorphism)
- **Rule of 3/5/0:** if defining destructor/copy/move, define or delete the others explicitly
- Use `= default` for generated default constructors/destructors (more explicit)
- Use `= delete` to explicitly disable copy/move/default constructor
- Examples: `MyClass() = default;` or `MyClass(const MyClass&) = delete;`

### 6.11 SOLID & Clean Code principles

> Reference: Robert C. Martin / Uncle Bob - Clean Code & Clean Architecture
> Simplicity and pragmatism: see §6.16 (KISS, YAGNI, WET/DRY, ETC, Boy Scout, CQS)

- Readable and human-understandable code
- Names ALWAYS explicit (no cryptic abbreviations)
- Minimize comments: use only when necessary
- Refactor blocks > 15 lines into short functions with explicit names rather than adding comments (which can become obsolete)
- Single level of abstraction per function
- Single responsibility (Single Responsibility Principle)
- No hidden effects, no modified global state
- DRY (Don't Repeat Yourself)
- Respect Law of Demeter
- Prefer delegation over inheritance
- Exceptions > null pointers
- `std::optional` > null
- Handle errors at the appropriate level

### 6.12 Error handling

- Use exceptions for exceptional errors (not for normal flow)
- Use `std::optional` for optional values (avoid `nullptr`)
- Use `std::expected` (C++23) or Result types for operations that can fail
- Never silently ignore errors
- Log errors with appropriate context
- Example: `std::optional<PatchModel> loadPatch(const juce::File& file);`

### 6.13 RAII (Resource Acquisition Is Initialization)

- Always acquire resources in constructors
- Always release resources in destructors
- Use smart pointers for automatic memory management
- Objects must always be in valid state
- Example: `std::unique_ptr<juce::MidiInput> midiInput_;` in class, automatically released

### 6.14 Smart Pointers

- Prefer `unique_ptr` for exclusive ownership
- Use `shared_ptr` only when necessary (shared ownership)
- Use `weak_ptr` to break reference cycles
- Avoid raw pointers except for observing (non-owning)
- Example: `std::unique_ptr<juce::MidiInput> midiInput_;` for ownership, `MidiInput* input` for observer

### 6.15 Move Semantics

- Use `std::move()` to transfer ownership of heavy objects
- Prefer move constructors and move assignment operators
- Mark `noexcept` when appropriate (optimizations, move constraints)
- Example: `void setData(juce::MemoryBlock&& data) { data_ = std::move(data); }`

### 6.16 Design principles (agent priorities)

> Full human reference: `Documentation/Development/software-development-quality-principles.md`
> These principles complement SOLID and Clean Code — they resolve tensions between purity and pragmatism.

#### Simplicity & change (apply in Phase 1 DESIGN)

| Principle | Rule for the agent |
|---|---|
| **KISS** | Prefer the simplest design that meets the current requirement and respects §3 limits |
| **YAGNI** | Do not implement features, abstractions, hooks, or parameters for hypothetical future needs |
| **ETC** (Easy to Change) | Every design choice should make the next likely change easier, not harder |
| **Boy Scout Rule** | Leave every touched file slightly cleaner than found (without scope creep) |

#### Duplication (balance with §3.1)

| Principle | Rule for the agent |
|---|---|
| **DRY** | Each piece of knowledge has one authoritative representation — but only after duplication is confirmed |
| **WET** | Duplicating once is acceptable; abstract only when duplication is **stable**, **meaningful**, and **confirmed** |

#### Structure (already partially covered — explicit names)

| Principle | Rule for the agent |
|---|---|
| **SoC** | Separation of Concerns — GUI, Core, MIDI, state, rendering: distinct modules |
| **LoD** | Law of Demeter — no `a.getB().getC().doX()` chains; talk to immediate collaborators |
| **HC / LC** | High Cohesion within a class; Loose Coupling between modules |
| **PI** | Persistence Ignorance — `Source/Core/` must not know GUI or persistence details |
| **Intention-revealing names** | Names reveal *why*, not *how* — already mandatory elsewhere |

#### Functions & contracts

| Principle | Rule for the agent |
|---|---|
| **CQS** | Command Query Separation — a function either mutates state or returns information, not both |
| **Fail-fast** | Detect invalid preconditions early (`jassert`, guards, explicit errors) — never fail silently |
| **DbC** (lightweight) | Document and enforce preconditions for SysEx, patch ranges, checksums, MIDI contracts |

#### Priority when principles conflict

1. **Correctness & thread-safety** (audio thread, SysEx reliability)
2. **KISS + YAGNI + ETC** over speculative abstraction
3. **SOLID + §3 limits** over cleverness
4. **DRY** after confirmed duplication — not before

---

## 7. Formatting & JUCE Style

### 7.1 Formatting and spacing

#### Indentation

- **No tab characters!** Use 4 spaces for indentation

#### Allman style for braces

Opening brace on new line:

```cpp
if (x == 0)        // Yes!
{
    foobar();
    return "zero";
}
```

#### Operators

- Always put space before and after binary operators: `x = 1 + y - 2 * z / 3;`
- Operator `!` must NOT have space after: `if (!foo)` (JUCE standard style)
- Operator `~` must be preceded by space, but not followed
- Operators `++` and `--` have no space between operator and operand: `++i`, `--j`

#### General spacing

- Never space before comma, always space after: `foo(x, y);`
- Always space before opening parenthesis containing text: `foo(123);`
- Never space before empty parenthesis pair: `foo();`
- No space before opening bracket used as array index: `foo[1]`

#### Blank lines

- Blank line before `if`, `for`, `while`, `do` when preceded by another statement
- Blank line after closing brace `}` (except if next line is just another closing brace)

#### `if` statements

Do not write `if` on a single line... **except** for series of similar `if` aligned vertically to show a pattern:

```cpp
if (x == 1) return "one";
if (x == 2) return "two";
if (x == 3) return "three";
```

- Omit braces for trivially simple one-line `if` statements
- In `if-else` with multiple branches, all branches must be formatted the same way (all with or all without braces)

#### NEVER put `else` after `return`

```cpp
// ✅ Good
if (foobar())
    return doSomething();
doSomethingElse();

// ❌ Bad
if (foobar())
    return doSomething();
else
    doSomethingElse();
```

### 7.2 Pointer and reference declarations

- Always put space after type, never before: `SomeObject* myObject`, `SomeObject& myObject`
- Never declare multiple pointers/references of same type in a single declaration
- Prefer smart pointers or typedef to avoid multiple asterisks

### 7.3 const modifier

- Put `const` before type name: `const Thing& t;` (not `Thing const& t;`)
- Mark methods that do not modify state as `const`
- Use `const&` for read-only parameters
- Prefer `const_iterator` for read-only iterations
- Const pointers: `const Type*` (pointer to constant) vs `Type* const` (constant pointer)

### 7.4 Multi-line operators and expressions

When splitting an expression containing operators across lines, each new line must start with the operator:

```cpp
auto xyz = foo + bar
         + func(123)
         - def + 4321;
```

Same principle for dot operator (chained methods):

```cpp
auto t = AffineTransform::translation(x, y)
                   .scaled(2.0f)
                   .rotated(0.5f);
```

### 7.5 Lambdas

Preferred style:

```cpp
auto myLambda = [] { return 123; };
auto myLambda = [this, &x](int z) -> float { return x + z; };
auto longerLambda = [](int x, int y) -> int
{
    // ...multiple lines of stuff...
};
```

- Prefer explicit captures over `[=]` or `[&]` (clearer, avoids accidental captures)
- Avoid capturing large objects by value (prefer `const&` or `&` as appropriate)
- Example: `[this, &x, &y]` rather than `[&]` if only x and y are needed

### 7.6 Null values and pointers

- **NEVER** use `NULL`, `null`, or `0` for null pointer! Always use `nullptr`
- Limit scope of possibly null pointers:

```cpp
if (auto* f = getFoo())
    f->doSomething();
// f is out of scope here, impossible to use null pointer by mistake
```

### 7.7 Parameter passing

- For small POD objects, always pass by value, not by reference
- Use `const Foo&` only for complex objects (Array, String, etc.)
- JUCE classes to always pass by value: `Point`, `Time`, `RelativeTime`, `Colour`, `Identifier`, `ModifierKeys`, `JustificationType`, `Range`, `PixelRGB`, `PixelARGB`, `Rectangle`

### 7.8 Increment operators

- Always prefer pre-increment over post-increment: `for (int i = 0; i < 10; ++i)`

### 7.9 Standard library

- Always prefer std versions of functions over old C equivalents
- Use `std::abs`, `std::sqrt`, `std::sin`, `std::cos`, `std::pow` rather than `fabs`, `sqrtf`, `powf`, etc.

### 7.10 Types

- Never use `unsigned` alone - always write `unsigned int`
- Use JUCE types: `int8`, `uint8`, `int16`, `uint16`, `int32`, `uint32`, `int64`, `uint64`
- Prefer range-based for loops over raw for loops to iterate over containers

### 7.11 Auto

"Almost-always-auto" style but avoid in some cases:

- ❌ `auto x = 0;` → not OK (not obvious it's a signed int)
- ✅ `for (int i = 0; i < someNumber; ++i)` → OK (clear it's a signed int)
- ✅ `bool someCondition = false;` → OK (clearer than auto)
- ✅ `auto someResult = thisReturnsABool();` → OK (use auto if RHS is an expression)

### 7.12 Comments

- For short one- or two-line comments, prefer `//` over `/* */`
- Always leave space before text in `//` comment: `// yes!` not `//no!`
- Code names must be self-documenting (avoid comments)
- Use comments only to explain "why", not "how"
- For complex public APIs, consider Doxygen/Javadoc style comments

### 7.13 Macros

- **Do not use macros!** Treat macros as last resort
- Never use a macro just to hold a constant value or to perform a function that could be done as a real inline function
- If macros are used, give them names that won't conflict with other code
- `#undef` macros after use when possible

### 7.14 Templates

- Template parameters must follow their type without space: `vector<int>`
- In template declaration, leave space before opening bracket: `template <typename Type1, typename Type2>`

### 7.15 Dependencies

- Avoid circular dependencies between modules
- Use abstract interfaces to decouple (Dependency Inversion Principle)
- Respect dependency hierarchy (GUI depends on Core, not the reverse)

### 7.16 Enums

- Prefer `enum class` over `enum` for type safety and to avoid collisions
- Name enums in singular (except collections): `enum class ParameterType { kDco, kEnv, kLfo };`
- Prefix values with `k`: `ParameterType::kDco`
- Example: `enum class ClipboardType { kEmpty, kDco, kEnv, kLfo, kPatch };`

### 7.17 Initialization

- Prefer uniform initialization `{}`: `int value {0};` rather than `int value = 0;`
- Initialize members in constructor initializer list
- Initialization order = member declaration order (not list order)
- Example: `MyClass(int x) : member1_ {x}, member2_ {0} {}`

### 7.18 Namespaces

- Use namespaces to organize code by module
- Avoid `using namespace` in headers (global pollution)
- OK in `.cpp` to simplify: `using juce::String;`
- Example: `namespace Core { class MidiManager; }`

### 7.19 Static Members

- Prefer static free functions over static methods when possible
- Use `static constexpr` for class constants
- Example: `static constexpr int kMaxBufferSize {1024};`

### 7.20 Constexpr

- Use `constexpr` for functions computable at compile time
- Use `constexpr` for constant variables computable at compile time
- Prefer `constexpr` over `const` for compile-time computable constants
- `constexpr` implies `const`, but allows compile-time evaluation
- Examples:

```cpp
constexpr int calculateSize(int count) { return count * 4; }
constexpr int kBufferSize {calculateSize(256)};
```

### 7.21 Virtual Functions

- Use `override` explicitly (not `virtual` in derived classes)
- Use `final` to prevent further derivation if appropriate
- Prefer composition over inheritance (SOLID: favor delegation)
- Example: `void paint(Graphics& g) override;`

### 7.22 Assertions

- Use `jassert` (JUCE) or `assert` for development invariants
- Do not use assertions for user error handling (use exceptions)
- Assertions can be disabled in release: do not put critical logic in them
- Example: `jassert(bufferSize > 0);` to verify a precondition

### 7.23 Logging

- Use `juce::Logger` for structured logging
- Appropriate levels: `DBG()`, `Logger::writeToLog()` for debug, critical errors
- Avoid excessive logging in critical loops
- Log with context: include relevant information (file, line, values)

---

## 8. JUCE-Specific Rules

### 8.1 Plugin class naming

- **Processor class:** `PluginProcessor` (files: `PluginProcessor.h`/`.cpp`)
- **UI editor class:** `PluginEditor` (files: `PluginEditor.h`/`.cpp`)

### 8.2 Deprecated JUCE APIs - AVOID

- ~~`getCurrentPlaybackSampleRate()`~~ → `getSampleRate()`
- ~~`setPlayConfigDetails()`~~ → `setBusesLayout()`
- ~~`Timer` with callbacks~~ → `juce::HighResolutionTimer`
- ~~`AudioProcessorValueTreeState`~~ → modern `ValueTree`
- ~~Legacy parameter handling~~ → `juce::AudioProcessorParameter`

> List is non-exhaustive, take time to complete as needed.

### 8.3 Thread-Safety

- Always consider thread-safety in audio code (`processBlock()` called from audio thread)
- Use `juce::MessageManager::callAsync()` to call GUI thread from other threads
- Use `juce::AbstractFifo` or `juce::LockFreeFifo` for inter-thread communication (lock-free)
- APVTS listeners are thread-safe, but GUI updates must be done from message thread
- **NEVER** call GUI methods from audio thread
- Example: `juce::MessageManager::callAsync([this] { updateGUI(); });` from audio thread

### 8.4 Performance

- Avoid memory allocations in `processBlock()` (audio thread)
- Use pre-allocated buffers
- Avoid system calls in critical loops
- Profile before optimizing
- Prefer in-place algorithms when possible
- Avoid unnecessary copies: use `const&` or move semantics

### 8.5 Unit Tests

> Target: make Matrix-Control a reference repo — Core business logic must be test-covered.
> Full testing rules: §8.5 and `.cursor/rules/core-testing.mdc`.

#### Scope

- **Write tests for Core business classes** — SysEx, patch model, MIDI routing, parameter mapping
- **Do NOT write unit tests for GUI components** — manual / harness validation instead
- **No tests in audio thread or `paint()` paths**

#### Method — TDD for Core (preferred)

When implementing or changing `Source/Core/` logic:

1. **Red** — write or update a failing test that expresses the requirement
2. **Green** — implement the minimum code to pass
3. **Refactor** — apply §3 limits and §6.16 principles while keeping tests green

If Guillaume requests a spike or urgent fix, tests may follow immediately after — but never skip them for stable Core logic.

#### Test structure — AAA (mandatory)

Every unit test follows **Arrange / Act / Assert**:

```cpp
// Arrange — setup inputs and dependencies
// Act     — call the function under test
// Assert  — verify outcome (expect / ASSERT)
```

Keep each section visible; one logical assertion focus per test case when possible.

#### Test quality — F.I.R.S.T.

| Letter | Requirement |
|---|---|
| **F**ast | Unit tests run in milliseconds — no hardware, no sleep, no I/O |
| **I**ndependent | No shared mutable state between tests; order must not matter |
| **R**epeatable | Same result on every run, every machine |
| **S**elf-validating | Pass or fail automatically — no manual inspection |
| **T**imely | Written with or before the production code (TDD on Core) |

#### Test pyramid

| Layer | Volume | Matrix-Control focus |
|---|---|---|
| **Unit** | Many | `Tests/Unit/` — parsers, models, dispatchers, mappers |
| **Integration** | Some | MIDI queue + dispatcher chains, APVTS mappers (when needed) |
| **E2E / GUI** | Few | Manual via Standalone / `TestComponent` — no automated GUI suite yet |

Prefer many fast unit tests over a few slow integration tests.

#### Naming & organization

- One test class or `TEST_CASE` = one behavior under test
- Test names state **expected behavior**: `validateStructure_rejectsIncompletePatchMessage`
- Register new tests in `Tests/CMakeLists.txt`
- Use fixtures under `Tests/Fixtures/` for SysEx binary files — do not embed large blobs inline

#### Example

`TEST_CASE("SysExParser validates checksum correctly")` — see `Tests/Unit/SysExParserTests.cpp`

### 8.6 Complexity

- Maintain cyclomatic complexity < 5 per function (ideal: 1-3)
- If too complex, refactor into sub-functions
- Avoid > 3 parameters: consider a struct/class of parameters
- Prefer early returns to reduce nesting

---

## 9. Markdown Documentation (plans, refactorings)

### 9.1 Location of planning Markdown files

When I create Markdown files for refactoring plans, optimizations, or other planned operations, I store them **directly** in:

`Documentation/Development/Plans/`

The **Plans** folder must be **compartmentalized into subfolders by year**: `2025`, `2026`, etc. Inside each year, create **subfolders by month**: `01`, `02`, `03`, …, `12`. Each plan file is placed in the year then month subfolder corresponding to the plan date (e.g. a plan dated 2026-02-23 → `Documentation/Development/Plans/2026/02/`).

**⚠️ Important procedure:**
- Any plan or design document I produce must be saved under `Documentation/Development/Plans/<year>/<month>/` with the naming convention below
- Keep the archived plan up to date: when all of its TODOs are marked `completed`, update the saved file so it reflects the final state

### 9.2 Naming convention

- **Date** at the beginning: `YYYY-MM-DD-`
- **Names in English** only
- **One capital letter at the start of each word**
- **Separator:** hyphen `-`
- **No stop words** (de, la, l', the, of, etc.)
- **Explicit names** reflecting the document content

**Example:** `Documentation/Development/Plans/2026/02/2026-02-03-Theme-To-Skin-Migration-Phase-Six-Summary.md`

---

*End of my personal instructions.*
