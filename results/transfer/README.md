# Symbolic transfer-function examples

These runs invoke the original C++ path/loop and numerator/denominator routines.

| Example | Expressions | Graph and analysis |
|---|---|---|
| First-order RC low-pass | [transfer_function.txt](1stRCLPF/transfer_function.txt) | [Report](1stRCLPF/report.html) |
| Transistor-level OTA | [transfer_function.txt](5T_OTA_Integrator_tran/transfer_function.txt) | [Report](5T_OTA_Integrator_tran/report.html) |

Reproduce them from the package root:

```bash
python sfg.py 1stRCLPF --mode transfer
python sfg.py 5T_OTA_Integrator_tran --mode transfer
```

The files retain the original symbolic string notation, with separate numerator and denominator sections.
