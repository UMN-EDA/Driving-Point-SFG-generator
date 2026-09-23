# Original source snapshots

These sources are preserved byte-for-byte from the original project:

- `readCkt3.cpp`: continuous-time generator.
- `readCkt3_SC_1.7.cpp`: switched-capacitor generator.
- `readCkt3_SC_1.8_NEW.cpp`: transistor-level generator.
- `ALGORITHM.original.txt`: original design notebook.

The entry point for the packaged tool is `../../sfg.py`. The adapter in `../driver.cpp` supplies filename selection, operation modes, and unified exports around these sources.

Source and example checksums are recorded in `../../docs/original_sha256.json`.
