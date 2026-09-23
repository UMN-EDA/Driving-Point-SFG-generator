# Commands and outputs

Run these commands from the package root after installing `requirements.txt`.

## Single example

```bash
python sfg.py 5T_OTA_Integrator_tran
```

Input can be an example name, an example filename, or a path to an `.isc` file. Example names work from any working directory when calling `sfg.py` by its full path.

```bash
python sfg.py Sallen_Key.isc
python sfg.py examples/switched_capacitor/FirstOrderSCFilter.isc
python sfg.py /path/to/custom.isc --output runs/custom
```

`--output` accepts a new or empty directory. Existing results are preserved. Without this option, the wrapper creates `runs/<circuit>-<timestamp>/`.

## Modes

| Mode | Outputs |
|---|---|
| `graph` | Graph nodes/edges, graph JSON, plots, HTML report; default |
| `paths` | Graph outputs plus forward paths and their products |
| `loops` | Graph outputs plus cycles and their products |
| `analysis` | Graph outputs plus both paths and cycles |
| `transfer` | Original C++ paths, loops, symbolic numerator, and symbolic denominator |

For `paths`, `loops`, and `analysis`, use `--analysis-backend python` (default) or `--analysis-backend legacy`.

Python analysis enumerates simple paths/cycles, expanding each sequence over its distinct parallel edges. Its products appear in `path_gains.txt` and `loop_gains.txt`. Legacy analysis uses the corresponding original C++ functions and their original loop-selection conventions. The backend is recorded in `run.json`.

`transfer` always selects the original C++ path/loop and transfer-function routines. `transfer_function.txt` stores the symbolic numerator and denominator strings using the original notation.

```bash
python sfg.py FirstOrderSCFilter --mode analysis
python sfg.py FirstOrderSCFilter --mode analysis --analysis-backend legacy
python sfg.py 1stRCLPF --mode transfer
```

## Options

| Option | Purpose |
|---|---|
| `--list-examples` | List bundled examples and their automatic engine selection |
| `--engine auto` | Select the engine from circuit contents and header; default |
| `--engine ct` | Use `readCkt3.cpp` |
| `--engine sc` | Use `readCkt3_SC_1.7.cpp` |
| `--engine latest` | Use `readCkt3_SC_1.8_NEW.cpp` |
| `--mode MODE` | Select the requested operation |
| `--analysis-backend python\|legacy` | Select path/loop analysis implementation |
| `--output DIRECTORY` | Write into a new or empty directory |
| `--no-plot` | Skip PNG/SVG/HTML generation |
| `--timeout SECONDS` | Time limit for each native or Python analysis process; default 60 |
| `--max-walks NUMBER` | Maximum total Python path/cycle exports; default 100000 |
| `--count TEXT` | Count substring occurrences in the combined path/cycle export |

```bash
python sfg.py 5T_OTA_Integrator_tran --mode analysis --count gmM0
python sfg.py LowQSCBiquadFilter --mode analysis --timeout 120 --max-walks 200000
python sfg.py --help
```

`--count` uses substring matching, as in the original `checkParam.py` utility.

## Files in a run directory

| File | Contents |
|---|---|
| `input.original.isc` | Exact input supplied to the wrapper |
| `input.isc` | Input with formatting/header normalization for the selected engine |
| `run.json` | Engine, mode, backend, build provenance, timing, counts, execution status |
| `generator.log` | Original C++ diagnostic output |
| `graph.tsv` | Header plus `source`, `weight`, `destination` columns |
| `nodes.tsv` | Node identifiers and the original node flag |
| `graph.json` | Nodes and directed edges in JSON |
| `graph.dot` | Graphviz graph source |
| `graph.png`, `graph.svg` | Graph images |
| `edge_legend.tsv` | Edge IDs and complete symbolic gains |
| `report.html` | Local graph viewer, gain table, and result links |
| `paths.tsv`, `loops.tsv` | Alternating node/edge/node sequences |
| `paths_nodes.tsv`, `loops_nodes.tsv` | Node sequences only |
| `paths_edges.tsv`, `loops_edges.tsv` | Edge expressions only |
| `paths_edge_ids.tsv`, `loops_edge_ids.tsv` | One-based graph-row IDs from Python analysis |
| `path_gains.txt`, `loop_gains.txt` | Symbolic path/loop products from the selected backend |
| `all_paths.txt` | Concatenated path and loop exports |
| `python_analysis.json`, `analysis.log` | Python analysis metadata and output, when selected |
| `transfer_function.txt` | Original C++ numerator and denominator strings in transfer mode |

The latest C++ engine also writes its original `FullGraph.txt`, `Forward*.txt`, and `LoopsNodesEdges.txt` files when the corresponding routines run. The unified `graph.tsv` export reads outgoing edges once and retains parallel branches.

Legacy node-sequence exports can use `alternatives[a | b]` where more than one graph edge joins the same pair of nodes. Exact gains returned by the legacy routines are stored separately in the gain files. Python analysis additionally exports edge IDs to distinguish those branches explicitly.

Long expressions are represented by short edge IDs in the graph image. The complete expressions are available in the report and edge legend.

## Replot existing results

```bash
python plot_graph.py results/analysis/5T_OTA_Integrator_tran
```

This updates plots and the HTML report without rerunning the C++ engine.

## Batch runs

```bash
python run_examples.py
python run_examples.py --mode analysis --output-root runs/all_analysis
python run_examples.py --mode analysis --analysis-backend legacy --output-root runs/all_legacy
```

Each example receives its own subdirectory. `index.html` and `index.json` summarize the batch. Use a fresh output root; `--skip-existing` explicitly reuses runs with matching mode/backend settings.

## Build control

The wrapper compiles each engine on first use and caches the executable using a hash of its source, adapter, compiler version, and flags. It uses C++11 and `-O0` to retain the original development build convention. Build output is in `build/build-<engine>.log`.

```bash
CXX=/path/to/g++ python sfg.py Sallen_Key
```

Graph generation with `--no-plot` uses only the Python standard library and a C++ compiler. Python analysis requires NetworkX; plotting requires the packages in `requirements.txt`.
