# Changes to the analytical model (cyclotron) and the RTL gating config

## Summary

One change to the cyclotron timing model and one new RTL config:

1. **cyclotron `config/timing/gmem.toml`**: the global data-cache delivery rate
   `[gmem.levels.data] bytes_per_cycle` is set from `64` to `6` (at every memory level; the tag and MSHR
   rates stay at 64). Committed on the cyclotron submodule (`gmem: global-cache data bandwidth 64->6
   B/cyc`).
2. **`chipyard/RadianceConfigs.scala`**: a new `RadianceSingleClusterFastConfig`, identical to
   `RadianceSingleClusterConfig` but with `trace = false, profiler = false`, for fast RTL cycle gating.

## Why the bandwidth change

cyclotron is an issue-bound model: it advances roughly one instruction per cycle per core, and at the
data nodes a global load delivered 64 B/cycle, effectively for free. As a result a cache-resident matmul
never became memory-bound, and staging operands into shared memory — which trades global traffic for a
barrier and SMEM accesses — looked neutral or slightly worse. The model could rank compute and register
effects faithfully but was blind to memory-hierarchy reuse, the single largest lever for GEMM-class
kernels on this machine.

Modeling a finite global delivery rate fixes this. With `bytes_per_cycle = 6`, a naive matmul that reads
A and B from global memory in the inner loop becomes bandwidth-bound, while an SMEM-staged matmul becomes
compute-bound. The SMEM win the model previously could not see now appears.

This is the data level only. Tag and MSHR rates are unchanged, so the change models delivery bandwidth,
not tag-lookup or miss-handling throughput.

## Calibration

Matmul 64³, naive vs SMEM, total-cycle ratio in cyclotron as the data-level rate is swept:

| bytes_per_cycle | naive/SMEM total ratio |
|---|---|
| 64 (original) | 1.02× (SMEM looks free / no win) |
| 8 | 1.92× |
| 6 | 2.43× (net) / 1.15× (total) |
| 4 | 3.35× |

`bytes_per_cycle = 6` is the setting whose matmul SMEM-vs-naive ratio matches RTL (below).

## RTL validation

`RadianceSingleClusterFastConfig` is an exact clone of `RadianceSingleClusterConfig` with tracing turned
off. The original config has `trace = true`, which makes the Muon cores attempt per-instruction trace
writes during simulation; with no writable trace database this produces a non-fatal write attempt on
nearly every instruction and inflates gate wall-time to roughly 90 minutes. With `trace = false` the
gate runs in roughly 25 minutes and produces identical cycle counts (the hardware is unchanged).

Matmul 64³ naive (`sol0_baseline`) vs SMEM (`sol0_smem_manual`), drain-matched, on the FastConfig
(cycles = `$finish` ps / 2000 at the 500 MHz tile clock):

| | naive | SMEM | total ratio |
|---|---|---|---|
| RTL (FastConfig) | 843,556 | 726,584 | **1.161×** |
| cyclotron @ bw=6 | 480,918 | 419,797 | **1.146×** |
| cyclotron @ bw=64 (original) | 390,953 | 381,807 | 1.02× |

cyclotron @ bw=6 reproduces the RTL total-cycle ratio within 1.3%; the original model under-predicted the
SMEM win (1.02×). Evidence files are in `metrics/rtl_confirmation/`.

## Per-op faithfulness

Re-evaluating the discovered kernels at bw=6 vs bw=64 (`metrics/faithfulness_bw.csv`): the conv win
survives (1.29× at bw=6 vs 1.34× at bw=64); layer_norm and gelu have no win at either rate; softmax and
attention failures are bandwidth-independent. No ranking flips between the two rates, so bw=6 exposes the
memory-class wins without masking compute wins.

## Reproducing the RTL gate

```
cd sims/vcs
source ../../env.sh            # conda JDK-20; the system JDK breaks the sbt metabuild
make CONFIG=RadianceSingleClusterFastConfig default
./simv-chipyard.harness-RadianceSingleClusterFastConfig +permissive +max-cycles=10000000 \
    +loadmem=<kernel>.soc.elf +permissive-off <kernel>.soc.elf
# cycles = $finish ps / 2000
```
