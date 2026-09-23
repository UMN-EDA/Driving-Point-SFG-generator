# Circuit examples

Run any example by name from the package root:

```bash
python sfg.py FirstOrderSCFilter --mode analysis
```

| Example | Category | Phases | Engine | Results |
|---|---|---:|---|---|
| [1stRCLPF](continuous_time/1stRCLPF.isc) | continuous time | 1 | `ct` | [Report](../results/analysis/1stRCLPF/report.html) |
| [Boctor_LPNotch](continuous_time/Boctor_LPNotch.isc) | continuous time | 1 | `ct` | [Report](../results/analysis/Boctor_LPNotch/report.html) |
| [MultiFBBP](continuous_time/MultiFBBP.isc) | continuous time | 1 | `ct` | [Report](../results/analysis/MultiFBBP/report.html) |
| [Sallen_Key](continuous_time/Sallen_Key.isc) | continuous time | 1 | `ct` | [Report](../results/analysis/Sallen_Key/report.html) |
| [TowThomasBQ](continuous_time/TowThomasBQ.isc) | continuous time | 1 | `ct` | [Report](../results/analysis/TowThomasBQ/report.html) |
| [TwinTNotch](continuous_time/TwinTNotch.isc) | continuous time | 1 | `ct` | [Report](../results/analysis/TwinTNotch/report.html) |
| [NEW_TEST](experimental/NEW_TEST.isc) | experimental | 1 | `latest` | [Report](../results/analysis/NEW_TEST/report.html) |
| [FirstOrderSCFilter](switched_capacitor/FirstOrderSCFilter.isc) | switched capacitor | 2 | `sc` | [Report](../results/analysis/FirstOrderSCFilter/report.html) |
| [LowQSCBiquadFilter](switched_capacitor/LowQSCBiquadFilter.isc) | switched capacitor | 2 | `sc` | [Report](../results/analysis/LowQSCBiquadFilter/report.html) |
| [ParasiticSensitiveIntegrator](switched_capacitor/ParasiticSensitiveIntegrator.isc) | switched capacitor | 2 | `sc` | [Report](../results/analysis/ParasiticSensitiveIntegrator/report.html) |
| [SimulatedInductor](switched_capacitor/SimulatedInductor.isc) | switched capacitor | 4 | `sc` | [Report](../results/analysis/SimulatedInductor/report.html) |
| [StraySensitiveSCIntegrator](switched_capacitor/StraySensitiveSCIntegrator.isc) | switched capacitor | 2 | `sc` | [Report](../results/analysis/StraySensitiveSCIntegrator/report.html) |
| [2Stage_OTA](transistor/2Stage_OTA.isc) | transistor | 1 | `latest` | [Report](../results/analysis/2Stage_OTA/report.html) |
| [5T_OTA_Integrator_tran](transistor/5T_OTA_Integrator_tran.isc) | transistor | 1 | `latest` | [Report](../results/analysis/5T_OTA_Integrator_tran/report.html) |
| [5T_OTA_Single_Ended_Stack_Integrator](transistor/5T_OTA_Single_Ended_Stack_Integrator.isc) | transistor | 1 | `latest` | [Report](../results/analysis/5T_OTA_Single_Ended_Stack_Integrator/report.html) |
| [CM_OTA_Integrator_tran](transistor/CM_OTA_Integrator_tran.isc) | transistor | 1 | `latest` | [Report](../results/analysis/CM_OTA_Integrator_tran/report.html) |
| [ParasiticSensitiveIntegrator_tran](transistor/ParasiticSensitiveIntegrator_tran.isc) | transistor | 2 | `latest` | [Report](../results/analysis/ParasiticSensitiveIntegrator_tran/report.html) |


Input files are copied unchanged from the original project. See the [input format guide](../docs/input-format.md) to create a custom circuit.
