# Circuit input format

Circuit descriptions are plain text `.isc` files. Fields within a branch record are separated by underscores. Node `0` is ground, and numbered circuit nodes lie in `1..N`. The file ends with `END`.

The wrapper accepts blank lines and `#` or `//` comments and normalizes them before invoking the selected engine. Keep the input connection first and the output connection second after the header.

## Continuous-time circuits

The first line gives the number of circuit nodes:

```text
2
VI_1_1
2_1_VO
1_G1_2
2_C1_0
END
```

This describes the bundled first-order RC low-pass example: input at node 1, conductance `G1` between nodes 1 and 2, and capacitor `C1` from node 2 to ground. Output is node 2.

## Multiphase and transistor circuits

The first line gives the number of phases and the second line gives the number of circuit nodes:

```text
2
5
VI_1_1
5_1_VO
3_OP-_4
2_C1_0
4_C2_3
1_S1_2
4_S1_5
2_S2_3
END
```

`S1` is closed in phase 1 and `S2` is closed in phase 2. A single-phase transistor circuit starts with `1`, then its node count.

## Branch records

```text
SOURCE_ELEMENT_DESTINATION
```

| Element | Meaning |
|---|---|
| `G1`, `G2`, … | Symbolic conductances |
| `C1`, `C2`, … | Symbolic capacitances |
| `OP+`, `OP-` | Op-amp branches |
| `S1`, `S2`, … | Switches associated with numbered phases |
| `1` | Unit-weight input/output connection |

Use `VI_1_<node>` to attach the input and `<node>_1_VO` to select the output. Switch suffixes must lie within the declared phase count. Component labels carry symbolic names; numeric device values are not part of this format.

## Transistor records

```text
DEVICE_GATE_DRAIN_SOURCE
```

For example:

```text
M0_8_4_7
P1_4_4_5
```

`M` and `P` prefixes identify the transistor types used by the original implementation. Terminal order is **gate, drain, source**. There is no bulk-terminal or SPICE model field. Transistor contributions are represented by symbolic terms such as `gmM0S1`, `gdsM0S1`, and `s.CgsM0S1`.

## Engine selection

| Input | Automatic engine |
|---|---|
| Contains transistor records | `latest` |
| Phase-count header, no transistor records | `sc` |
| Node-count-only header | `ct` |

The wrapper normalizes the header for an explicitly selected engine. With automatic selection, use the node-count-only format for continuous-time examples and the phase-count format for switched-capacitor examples.

## Internal graph node names

The continuous-time engine uses numeric circuit node IDs and negative auxiliary IDs. The phase-based engines use powers of two for circuit node IDs, combine IDs during switching, and append a phase suffix such as `_S1`. For example, original node 3 is represented as `8.000000_S1` in phase 1. Graph images shorten this to `8_S1`; exported data retains the original identifier.
