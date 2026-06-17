# autocomp on Muon — results and analysis

This directory packages the results of running the autocomp kernel-optimization loop against the Radiance
Muon SIMT GPU, evaluated on the cyclotron analytical model and spot-checked on RTL. It is self-contained:
the analytical-model change, the kernels, the search traces, and the measured metrics are all here.

Contents:

- `MODEL_CHANGES.md` — the change to the cyclotron timing model (finite global bandwidth) and the
  `RadianceSingleClusterFastConfig` used for fast RTL gating, with the calibration and RTL validation.
- `metrics/` — measured cycle counts, speedups, utilization, the RTL confirmation, the bandwidth
  faithfulness sweep, and the full session writeups (`results.md`, `results.csv`, `UTILIZATION.md`,
  `PHASE_C_HARVEST.md`, `MUON_SEARCH_TUNING.md`).
- `kernels/` — the three kernel families (see below): `golden/`, `real_model/`, `generated/`.
- `traces/` — the autocomp search traces for the best attempts (per-candidate code evolution, eval
  verdicts, cost ledgers, best kernel), for later compiler-information mining.
- `CHECKOUT.md` — the repositories and branches to check out together to reproduce this.

## Kernel families

All three are reported. Their roles differ:

1. **Golden** (`kernels/golden/`) — the reference kernels designed by the Radiance authors, from the
   radiance-kernels repository (sgemm_muon, gemm_simt, sgemm_wg, sgemm_tcore, gemm_mxgemmini,
   flash_attention, flash_attention_virgo, softmax, rmsnorm, swiglu). Included as source for reference;
   they are not re-benchmarked here.
2. **Real-model** (`kernels/real_model/`) — the baseline (golden/benchmark) kernels we constructed for the
   autocomp problem suite, with shapes taken from real models (openvla, rdt, smolvla, pi05, tiny_llama).
   These are the starting points that were optimized.
3. **Generated** (`kernels/generated/`) — the kernels produced by the autocomp search, plus the
   hand-verified SMEM matmul.

## Headline results

Two kernels carry real speedup (cyclotron @ bw=6; full table in `metrics/results.md`):

- **matmul 64³: 2.43× kernel-only / 1.146× total**, RTL-confirmed (RTL total ratio 1.161×, matching the
  model within 1.3%). naive global-memory matmul → shared-memory staged matmul.
- **conv patch-embed: 1.48× kernel-only / 1.289× total**, from shape specialization + bank-conflict-free
  SMEM padding.

The remaining problems sit near 1.0× because their baselines are already shared-memory-staged: the
memory-class win the model exposes is already present in those baselines, leaving little SIMT-level
headroom. This is the central finding — see `metrics/PHASE_C_HARVEST.md`.

## Utilization

Reported conservatively against the Muon SIMT FP peak (64 flop/cycle = 32 GFLOP/s at 500 MHz). Total-cycle
utilization is 1–5% because the benchmark verify loop dominates tiny-matmul totals; kernel-only it is
higher (SMEM matmul ~19%, conv-class ~15%). See `metrics/UTILIZATION.md`. The MX-Gemmini tensor core is
not used by these kernels and is not in the denominator.

## The methodological result

For an accelerator like Muon the evaluator's fidelity, not the code-generating model, was the ceiling: the
issue-bound model could not see shared-memory reuse, so every search found ≈1.0× on memory-bound kernels.
Calibrating the model to RTL with a finite global-bandwidth term let a cheap search see and confirm the
memory-class wins. See `MODEL_CHANGES.md`.
