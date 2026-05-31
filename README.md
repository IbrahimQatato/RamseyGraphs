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
./build/ramsey                                  # exhaustive search — prompts for s and t
./build/ramsey --verbose                        # step through every coloring, press Enter to advance
./build/ramsey --strategy random                # probabilistic search (faster, not a proof)
./build/ramsey --strategy random --samples 5000 # random search with 5000 samples per K_n
```

Enter s and t when prompted:
```
Enter s and t for R(s,t): 3 3
```

Results are saved automatically to `data/R_s_t.txt`. Running again with the same s and t resumes from where it left off. The strategy is inferred from the save file — no need to pass `--strategy` on resume.

### Check mode
```bash
./build/ramsey --check data/R_3_3.txt
```

Loads a saved search and lets you browse results interactively:
```
list <n>                       — list counterexample indices for K_n
show <n> <index>               — display counterexample graph and adjacency matrix
export <n> <index> <file.dot>  — export counterexample as Graphviz DOT file
quit                           — exit
```

### Run tests
```bash
./build/tests
```

## Interrupting

Press Ctrl+C at any time — the search saves its state before exiting.

## Generating Docs

Requires Doxygen (`sudo apt install doxygen` / `brew install doxygen`).

```bash
doxygen Doxyfile
```

Then open `docs/html/index.html` in a browser.

## Exporting
in check mode you can export the a dot file and see it in Graphviz
```
dot -[Tpng/Tsvg/Tpdf] <out.dot> -o <out.png>
```


