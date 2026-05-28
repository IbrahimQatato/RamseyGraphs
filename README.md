# Ramsey Analyzer

A console application for exploring Ramsey numbers and graph theory through exhaustive 2-coloring search.

## Install & Build

Requires: CMake 3.20+, a C++23 compiler (GCC 13+, Clang 16+, MSVC 2022+).

```bash
./checker_build.sh
```

## Usage

### Search mode
```bash
./build/ramsey              # fresh search — prompts for s and t
./build/ramsey --verbose    # shows every coloring, waits for Enter
```

Enter s and t when prompted:
```
Enter s and t for R(s,t): 3 3
```

Results are saved automatically to `data/R_s_t.txt`. Running again with the same s and t resumes from where it left off.

### Check mode
```bash
./build/ramsey --check data/R_3_3.txt
```

Loads a saved search and lets you browse results interactively:
```
list <n>         — list counterexample indices for K_n
show <n> <idx>   — display counterexample graph
quit             — exit
```

### Run tests
```bash
./build/tests
```

## Interrupting

Press Ctrl+C at any time — the search saves its state before exiting.

## Architecture

| File | Description |
|------|-------------|
| `Color.h/cpp` | `enum class Color { Red, Blue }` with stream operators. |
| `RamseyChecker.h` | `hasClique` and `checkRamseyProperty` — clique detection via combination enumeration. |
| `ExhaustiveSearch.h/cpp` | Exhaustive 2-coloring search. Encodes colorings as bitmasks. Supports save/load/resume. |
| `options_module/Options.h/cpp` | Command line argument parsing. |
| `RamseyMain.cpp` | Entry point — CLI, signal handling, mode dispatch. |
| `tests/Tests.cpp` | Unit and integration tests. |

