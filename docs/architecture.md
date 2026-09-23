# Implementation

The package separates the original circuit algorithms from command-line handling, exported data, graph analysis, and visualization.

```text
.isc circuit
    │
    ▼
sfg.py: validate input, select engine, normalize header, build executable
    │
    ▼
src/driver.cpp + selected original C++ source
    │
    ├── Circuit parsing and graph construction
    ├── Optional original C++ path/loop and transfer-function routines
    └── graph.tsv / nodes.tsv
            │
            ├── analyze_graph.py: optional Python path/cycle enumeration
            └── plot_graph.py: PNG / SVG / DOT / HTML
```

## Preserved engines

| Engine | Source | Role |
|---|---|---|
| `ct` | `src/legacy/readCkt3.cpp` | Continuous-time circuits |
| `sc` | `src/legacy/readCkt3_SC_1.7.cpp` | Multiphase switched-capacitor circuits |
| `latest` | `src/legacy/readCkt3_SC_1.8_NEW.cpp` | Transistor-level circuits and phase-aware transistor graphs |

These files are exact copies of the original sources. `docs/original_sha256.json` records their checksums and those of the circuit examples. The parent-directory originals are not used at runtime: this package is self-contained.

## Adapter

`src/driver.cpp` includes one original source at compile time. A preprocessor rename retains its original `main()` under `legacy_main`; the adapter supplies a filename- and mode-driven entry point. `<cmath>` is included by the adapter for the original uses of `pow()`.

The adapter calls the original graph-construction functions in their original sequence, exposes optional analysis and transfer-function calls, and exports graph data. It reads graph adjacency lists directly for exports, without consuming the legacy edge-selection state. Each explicitly selected transfer-function calculation invokes its numerator and denominator functions once.

## Python analysis

The Python backend works on the exported directed graph. NetworkX enumerates simple node paths/cycles, and the analyzer expands those sequences across parallel edges. Every exported walk therefore has an edge-ID sequence as well as its node and weight sequences. The original C++ analysis remains selectable through `--analysis-backend legacy`.

Analysis products are represented as symbolic strings. The graph-generation algorithms and device equations remain in the original C++ files.

## Plotting

Graphviz, when installed, lays out the directed graph. Otherwise Matplotlib and NetworkX provide a headless layout. Both backends create PNG and SVG files. Graphs use compact edge labels; the HTML report provides the complete gain expressions and downloads.

## Run isolation

The wrapper executes the selected engine inside a new result directory. Original engine filenames such as `FullGraph.txt` therefore stay within that run. `input.original.isc` preserves the supplied input, and `run.json` records the source hash, engine, mode, backend, compiler, and output counts.
