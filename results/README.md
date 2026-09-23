# Example results

This directory contains generated outputs for the bundled circuits.

| Directory | Contents |
|---|---|
| [graphs](graphs/index.html) | Graph data and PNG/SVG/HTML visualizations for all 17 examples |
| [analysis](analysis/index.html) | Graphs plus Python forward-path/cycle analysis for all 17 examples |
| [legacy_analysis](legacy_analysis/index.html) | Original C++ forward-path/loop results for all 17 examples |
| [transfer](transfer/README.md) | Original C++ symbolic transfer-function examples |

Each run contains its input, graph exports, and metadata. Open a gallery's `index.html` locally, or view individual PNG/SVG files directly.

To generate a new set of results from the package root:

```bash
python run_examples.py --mode analysis
```

New runs go under `runs/`; included reference results remain unchanged.
