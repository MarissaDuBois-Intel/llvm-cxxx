# C+++ (cppp / CXXX)

C+++ is an experimental, opt-in C++ language variant in this tree. Select it
with `-x c+++`. The equivalent spellings are `C+++`, `cppp`,
`c-plus-plus-plus`, `cxxx`, and `CXXX`.
These aliases are handled by the compiler driver. The internal `-cc1`
frontend accepts only the canonical `c+++` spelling, corresponding to
`Language::CXXX`.

Source files keep their `.c` or `.cpp` extensions; included headers keep `.h`
or `.hpp`. Extensions alone do **not** enable C+++: normal C and C++ compilation
is unchanged. Use `-x cppp-header` to create a precompiled header and
`-x cppp-cpp-output` to compile preprocessed input. The same suffixes work with
the other aliases.

C+++ reuses Clang's C++ parser, ABI, standard library support, and implemented
C++ features. Its default is this tree's newest C++ working-draft mode,
currently `-std=c++2d`. Explicit C++ standards from C++11 onwards also work,
for example `-std=c++23`. `__cplusplus` retains its normal meaning;
`__cxxx__` is additionally defined to `1`.

C code that is also valid C++ can be compiled directly in this mode. This is
not a claim that every C program is valid C++: C-only constructs and existing
C/C++ semantic differences are unchanged. Compile those translation units in
normal C mode and use the usual `extern "C"` declarations to link them.
Likewise, C+++ reserves the keywords `trine`, `Trine`, `other`, and `otherwise`; code using
those names as identifiers must be renamed or kept in its
original language mode.

## The trine type

`trine` (also spelled `Trine`) is a **fundamental signed integral type** in
Clang's AST, distinct from both `bool` and `_BitInt(2)`. It has a two-bit value
representation and is lowered to LLVM `i2`. No wrapper class, header, or runtime
library is required. Clang's `__is_fundamental`, `__is_scalar`, and
`__is_integral` type traits recognize it.

| State | Signed value | Two-bit representation |
|---|---:|---|
| `false` | 0 | `00` |
| `true` | 1 | `01` |
| `other` | -1 | `11` |

The fourth bit pattern, `10` (-2), is not a valid trine value and cannot be
produced by its supported conversions. The sign uses two's-complement
representation.
As with `bool`, an independently addressable object occupies at least a byte;
the two-bit value does not imply packed arrays or a fractional-byte `sizeof`.
Loads and stores use a sign-extended byte representation. Trine bit-fields
require at least two bits; adjacent two-bit fields can share a byte.
Target calling conventions may widen scalar arguments without changing the
two-bit value representation.

The existing `true` and `false` literals remain `bool`. They, and other `bool`
expressions, implicitly initialize or assign a trine. `other` is a literal of
type `trine`. Value-initialization (`trine{}`) produces `false`. As with other
fundamental types, a local declaration without an initializer (`trine state;`)
does not initialize its value. Copying preserves all three states.

```cpp
trine state = false;
state = true;
state = other;
Trine copy = state;

if (state == other) {
  int value = static_cast<int>(state); // -1
}
```

Use `==` or `!=` to inspect a state, or explicitly convert it to `int` to read
0, 1, or -1. Integer and pointer construction is rejected rather than silently
converting arbitrary values to `bool`. There is no implicit conversion back
to `bool` or `int`, so `if (state)` cannot accidentally collapse `other` into
`true`. Arithmetic and logical operators on trines remain unsupported.

The old resource-header class implementation has been removed. Rebuild
applications and precompiled headers made with that prototype: the native
scalar type has different mangling and calling conventions.

### Configuring `other` equality

`-ftrine-other-equal` is the default: `other == other` is `true`.
Use `-fno-trine-other-equal` to make that comparison `false`. `!=` is always
the complement of `==`, so `other != other` becomes `true` in the latter mode.
The last of these flags on the command line wins.

The flag affects both runtime and constant-expression comparisons, including
comparisons between variables holding `other`. Comparisons of `true` and
`false`, and ordinary `bool` comparisons, are unchanged. In non-reflexive mode,
use `static_cast<int>(state) == -1` to detect the third state; `state == other`
will never be true.

Use the same setting when building and consuming a precompiled header; a
mismatch is diagnosed. The flag does not change the type's representation.

### Comparisons and ordering

Trines support `==`, `!=`, `<`, `<=`, `>`, `>=`, and `<=>`, including comparisons
with `bool` and integer operands in either order. The first six return `bool`.
`<=>` requires C++20 or later and `<compare>`; it returns
`std::partial_ordering` in **both** ordering modes.

The default, `-fno-trine-other-ordered`, makes `other` unordered relative to
every Boolean or integer value. Known trine values compare numerically as
0 and 1, using ordinary integer conversions. For example:

```cpp
#include <compare>

trine f = false, t = true;
bool a = f < t;      // true
bool b = t == 1;     // true
bool c = 2 > t;      // true
bool d = other < t;  // false in unordered mode
bool e = other >= t; // also false: >= does not mean !(<)
bool n = other != t; // true

auto cmp = other <=> t;
bool unordered = cmp != 0 && !(cmp < 0) && !(cmp > 0);
```

With default self-equality, `other <= other` and `other >= other` are true,
and `other <=> other` is equivalent. With `-fno-trine-other-equal`, that
three-way comparison is unordered: only `other != other` is true.

Use **`-ftrine-other-ordered`** to place `other` before **every** Boolean or
integer value, including negative integers. Among trines:
`other < false < true`. Integer `-1` remains distinct from `other`:

| Expression | Unordered (default) | Ordered |
|---|---|---|
| `other == -1` | false | false |
| `other < -1` | false | true |
| `-1 > other` | false | true |
| `other <=> -1` | unordered | less |
| `static_cast<int>(other) == -1` | true | true |

The explicit cast reads the representation and opts into ordinary integer
semantics. Comparison interoperability does **not** allow integer construction
of trines or implicit conversion of trines to integers. Comparisons without a
trine operand keep their existing C++ behavior.

Ordered mode requires self-equality. Combining `-ftrine-other-ordered` with
`-fno-trine-other-equal` is an error. The last flag within each positive/negative
pair wins. Use matching equality and ordering settings across an application,
including its precompiled headers; PCH mismatches are diagnosed. Both constexpr
evaluators and runtime code implement the same rules.

Unordered mode is not suitable as the default comparator for sorting or ordered
containers. Choose ordered mode or provide an explicit ordering policy instead.

### Three-way `if` and `else if` chains

Ordinary C+++ `if` statements can test a trine directly:

```cpp
if (state) {
  handle_true();
} else {
  handle_false();
} otherwise {
  handle_other();
}
```

The condition is evaluated exactly once. `true` selects the first body, `false`
selects `else`, and `other` selects `otherwise`. Both trailing branches are
optional: an omitted branch does nothing. In particular, `if (state) A(); else
B();` executes **neither** body for `other`. This does not introduce an implicit
conversion to `bool`.

One terminal `otherwise` handles the whole `else if` chain:

```cpp
if (first) {
  handle_first();
} else if (second) {
  handle_second();
} else {
  handle_all_false();
} otherwise {
  handle_other();
}
```

Conditions are evaluated left to right. A true condition executes its body and
finishes; a false condition advances to the next condition; `other` immediately
selects the shared handler, skipping all remaining conditions. If no handler is
present, `other` still stops the chain and execution continues after it.

`otherwise` must appear last, at most once per chain. Bodies may be braced or
single statements. As with dangling `else`, an unbraced nested `if` binds the
trailing handler to the innermost chain; use braces to make nesting explicit.
An independent nested chain never propagates its `other` to an outer handler.

Boolean and trine conditions can be mixed. Other condition types retain their
C++ contextual Boolean conversion; integer `-1` is true, not `other`. A
Boolean-only chain may have an unreachable `otherwise`, which is useful in
templates. Comparisons still return `bool`, so `if (state == true)` and
`if (state == other)` need no handler. The latter comparison obeys the
self-equality option; **direct state branching is independent of both trine
comparison options**.

C++ condition declarations and init-statements retain their scopes. The shared
handler can use variables declared by the first `if`, but not variables declared
only in later `else if` conditions. Root condition variables remain alive through
the handler; later condition scopes are destroyed before entering it.

`-Wtrine-missing-otherwise` enables an optional warning for direct trine conditions
without a handler. It is off by default. Use
`-Werror=trine-missing-otherwise` to require explicit handling, or provide an
empty `otherwise {}` to explicitly ignore the third state. These diagnostics do
not affect Boolean comparisons or change program behavior.

This extension applies only to ordinary `if` statements, including those
evaluated inside constexpr functions. It does not extend `if constexpr`,
`if consteval`, loops, `switch`, logical operators, or `?:`.

## Build on Windows

Install Visual Studio's C++ build tools and Windows SDK, CMake, Ninja, and
Python. Open a **Developer PowerShell for VS** with the x64 build environment,
and run these commands from the repository root:

```powershell
cmake -S llvm -B build-cppp -G Ninja `
  -DCMAKE_BUILD_TYPE=Release `
  '-DLLVM_ENABLE_PROJECTS=clang;lld' `
  -DLLVM_TARGETS_TO_BUILD=X86 `
  -DCLANG_ENABLE_CIR=OFF `
  -DLLVM_ENABLE_ASSERTIONS=ON `
  -DLLVM_PARALLEL_COMPILE_JOBS=6 `
  -DLLVM_PARALLEL_LINK_JOBS=1
cmake --build build-cppp --target clang lld llvm-config FileCheck not count split-file --parallel 6
```

This builds native x86/x64 LLVM support, Clang, LLD, and the focused test tools,
not every optional LLVM subproject or backend. ClangIR is unnecessary for
C+++ and is disabled to avoid building its MLIR dependency.

Compile and run the [example](../examples/CPPP/trine.cpp) in the same shell:

```powershell
.\build-cppp\bin\clang++.exe -x c+++ .\clang\examples\CPPP\trine.cpp `
  -o .\build-cppp\trine-demo.exe
.\build-cppp\trine-demo.exe
```

The output begins with:

```text
false = 0
true = 1
other = -1
The third state survives assignment and copying.
other == other: true
other ordering: unordered
```

The demo then prints every comparison operator for known trines, `other`,
Booleans, and integers, and shows how to detect an unordered result. It also
demonstrates all three branches, short-circuiting chains, omitted handlers, and
ordinary Boolean comparisons. The three chains select branches `2`, `-1`, and
`0` after evaluating `2`, `2`, and `3` conditions respectively, in every
comparison mode. Press
**Enter** to exit (exit status 0).

Build the same examples in ordered mode:

```powershell
.\build-cppp\bin\clang++.exe -x c+++ -ftrine-other-ordered `
  .\clang\examples\CPPP\trine.cpp -o .\build-cppp\trine-demo.exe
.\build-cppp\trine-demo.exe
```

The `other / true` and `other / integer -1` rows now report `<` and `<=` as
true and `<=> less`, rather than unordered. Use `-fno-trine-other-ordered` to
restore the default. Use `-fno-trine-other-equal` with unordered mode to
demonstrate non-reflexive equality.

Use `clang++` when linking C++/C+++ applications so the platform's C++ runtime
is selected. Clang still needs the host SDK and standard library; LLD does not
replace those.

### Visual Studio solution

The example has a standalone CMake project that invokes the C+++ compiler
instead of MSVC. After building Clang, generate the solution from the repository
root:

```powershell
cmake -S .\clang\examples\CPPP -B .\build-cppp\trine-vs `
  -G "Visual Studio 18 2026" -A x64
cmake --build .\build-cppp\trine-vs --config Debug --target trine-demo
```

Open `build-cppp/trine-vs/Trine.slnx` in Visual Studio, select **Debug / x64**,
and use **trine-demo** as the startup project. **F5** builds and debugs;
**Ctrl+F5** runs without debugging. Set a breakpoint in `trine.cpp` to pause
before the console program exits. Debug builds disable optimization and emit
CodeView/PDB debug information.

To make `other` unequal to itself in this project, reconfigure with
`-DCXXX_TRINE_OTHER_EQUAL=OFF` and rebuild. Use `ON` to restore the default.
The example reports `other == other: false` and an unordered self-comparison.

To enable ordering, configure and rebuild:

```powershell
cmake -S .\clang\examples\CPPP -B .\build-cppp\trine-vs `
  -DCXXX_TRINE_OTHER_ORDERED=ON -DCXXX_TRINE_OTHER_EQUAL=ON
cmake --build .\build-cppp\trine-vs --config Debug --target trine-demo
```

Set `CXXX_TRINE_OTHER_ORDERED=OFF` and `CXXX_TRINE_OTHER_EQUAL=ON` to restore
the defaults. CMake rejects the incompatible ordered/non-reflexive combination.
Set `-DCXXX_WARN_MISSING_OTHERWISE=ON` to see the optional warnings on the demo's
intentionally omitted handlers. Leave it `OFF` for the default build.

If the compiler is in a different build directory, pass
`-DCXXX_CLANG=C:\path\to\clang++.exe` when configuring. The project builds only
the demonstration; rebuild Clang separately after changing the language
implementation. Visual Studio's built-in IntelliSense does not know C+++
keywords and may show editor diagnostics even when the custom compiler accepts
the program.

### Syntax highlighting in VS Code and Visual Studio

Local editor extensions highlight `trine`, `Trine`, `other`, and `otherwise`
without changing ordinary C/C++ files. The demo already has this comment on its
first line; add it to other C+++ source files and headers:

```cpp
// -*- mode: c+++ -*-
```

The comment selects editor highlighting only. Keep using `-x c+++` to compile
the file; `.c`, `.cpp`, `.h`, and `.hpp` names are unchanged.

Build the installers from the repository root:

```powershell
.\clang\examples\CPPP\editors\build.ps1
```

This requires Visual Studio's MSBuild, editor assemblies, and .NET Framework 4.8
targeting pack. The first build restores Microsoft's VSIX build tools from
NuGet. Use `-VisualStudioInstallDir` to select a non-default installation.
The build runs the Visual Studio keyword-scanner tests and writes both VSIX
packages to `build-cppp\editors`.

**VS Code:** install the package and use **Developer: Reload Window** if the
already-open demo has not switched to **C+++** language mode:

```powershell
code --install-extension .\build-cppp\editors\cxxx-vscode.vsix
```

Marked C/C++ documents switch automatically; unmarked documents keep their
existing language. You can also select **C+++** using **Change Language Mode**.
The extension reuses VS Code's C++ grammar and adds the new keywords. Semantic
coloring is disabled only in C+++ mode so an unmodified C++ parser cannot
override these scopes.

**Visual Studio:** save your work and close Visual Studio, then open
`build-cppp\editors\cxxx-visual-studio.vsix` and follow the VSIX installer.
Reopen the demo solution afterward. This extension supports x64 Visual Studio
2022 and 2026 and retains the C/C++ editor, adding theme-based keyword colors
only in marked files. The installer requires Visual Studio to be closed; do
not force-close the IDE to install it.

Both extensions leave comments and string/character literals alone. These are
**syntax-highlighting extensions, not C+++ language servers**: they do not add
IntelliSense, completion, or semantic diagnostics for the new constructs.
VS Code's C+++ mode is separate from C++, so extensions restricted to the
`cpp` language identifier may not run there. Visual Studio's existing C++
IntelliSense can still report errors for otherwise valid C+++ code.

Run the VS Code grammar and extension-host tests with:

```powershell
.\clang\examples\CPPP\editors\test-vscode.ps1
```

This opens an isolated VS Code test window, checks highlighting, marker
detection, and ordinary C++ isolation, then closes the test window.
Use `-CodeExecutable` if `code` is not on `PATH`.

### Compiler regression tests

Run the focused compiler tests:

```powershell
python .\build-cppp\bin\llvm-lit.py -v `
  .\clang\test\Driver\cppp.cpp `
  .\clang\test\SemaCXX\cppp-trine.cpp `
  .\clang\test\SemaCXX\cppp-trine-equality.cpp `
  .\clang\test\SemaCXX\cppp-trine-comparison.cpp `
  .\clang\test\SemaCXX\cppp-trine-if.cpp `
  .\clang\test\SemaCXX\cppp-trine-if-errors.cpp `
  .\clang\test\CodeGenCXX\cppp-trine.cpp `
  .\clang\test\CodeGenCXX\cppp-trine-equality.cpp `
  .\clang\test\CodeGenCXX\cppp-trine-comparison.cpp `
  .\clang\test\CodeGenCXX\cppp-trine-if.cpp `
  .\clang\test\PCH\cppp-trine.cpp `
  .\clang\test\PCH\cppp-trine-if.cpp
```
