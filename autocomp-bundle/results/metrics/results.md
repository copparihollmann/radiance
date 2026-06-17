# Results — autocomp on Muon (cyclotron @ bw=6)

All cycle counts are from cyclotron with the calibrated global-bandwidth model (`bytes_per_cycle = 6`
at the data cache level; see `../MODEL_CHANGES.md`). Cycles are TOTAL simulation cycles unless a NET
column is given. NET = TOTAL − harness overhead, where the harness overhead (empty-kernel run: input
staging + kernel launch + output verify loop) is measured per problem and is a benchmark artifact, not
part of the kernel. NET is the kernel-only figure.

Hardware peak used for utilization: 2 cores × 16 lanes × 2 flop/FMA = 64 flop/cycle (fp32), 500 MHz
tile clock = 32 GFLOP/s. Utilization = essential FLOPs / (peak × cycles). The MX-Gemmini tensor core is
not used by these kernels and is not in the denominator.

## Per-problem results

| prob | kernel | shape | baseline (total) | best (total) | speedup (total) | speedup (net) | best kernel |
|---|---|---|---|---|---|---|---|
| 0 | matmul | 64×64×64 | 480,918 | 419,797 | 1.146× | **2.43×** | sol0_smem_manual.cpp |
| 1 | conv patch-embed | C3 H64 →OC16 4×4 | 102,215 | 79,286 | **1.289×** | 1.48× | muon_1_beam_iters8.cpp |
| 2 | attention | seq64 d64 | 1,308,352 | 1,292,929 | 1.012× | 1.016× | muon_2_beam_iters8.cpp |
| 3 | attention | seq96 d64 | 2,759,774 | 2,759,774 | 1.000× | 1.000× | muon_3_beam_iters8.cpp |
| 4 | swiglu | 64×128 | 780,625 | 780,326 | 1.000× | — | muon_4_beam_iters8.cpp |
| 5 | softmax | 64×67 | 509,523 | — | — | — | best fails to run |
| 6 | matmul | 64×64×128 | 487,016 | 480,444 | 1.014× | 1.064× | muon_6_beam_iters8.cpp |
| 7 | flash attention | seq×d (pi05) | — | — | — | — | baseline times out @bw=6 |
| 8 | layer_norm | 64×768 | 5,735,899 | 5,507,821 | 1.041× | — | muon_8_beam_iters8.cpp |
| 9 | gelu | 64×512 | 3,059,220 | 3,059,220 | 1.000× | — | muon_9_beam_iters8.cpp |
| 10 | matmul | 64×64×384 | 998,938 | 987,528 | 1.012× | 1.019× | muon_10_harvest_best.cpp |
| 11 | attention (non-pow2) | seq96 d72 | 3,465,399 | — | — | — | no optimized best (search quota-blocked) |
| 12 | flash attention | seq192 d64 | — | — | — | — | register-infeasible (>256 phys regs); dropped |
| 13 | matmul (tall-skinny) | 8×256×768 | 3,548,850 | — | — | — | no optimized best (search quota-blocked) |

Full machine-readable form: `results.csv`. RTL confirmation for prob0: `rtl_confirmation/`.

### Provenance of each real-model shape

| prob | op | shape | source model |
|---|---|---|---|
| 1 | conv patch-embed | C3 H64 → OC16 4×4 | openvla patch embed |
| 2 / 3 | attention | seq64 / seq96, d64 | rdt SDPA (per-head tile) |
| 4 | swiglu | 64×128 | rdt FFN |
| 5 | softmax | 64×67 | rdt attention logits |
| 8 | layer_norm | 64×768 | smolvla / openvla hidden |
| 9 | gelu | 64×512 | smolvla / pi05 MLP |
| 10 | matmul | 64×64×384 | smolvla 768×768 (K-streaming) |
| 11 | attention (non-pow2) | seq96 d72 | pi05 |
| 12 | flash attention | seq192 d64 | pi05 (memory wall) |
| 13 | matmul (tall-skinny) | 8×256×768 | tiny_llama decode / rdt |
| 0 | matmul | 64×64×64 | synthetic anchor (not model-derived) |

### Golden (author-designed) reference kernels — structural, not re-benchmarked

These are the upstream Radiance kernels (source in `../kernels/golden/`). They are included for reference
and were not re-run here (per scope: only kernels we optimized are benchmarked).

| kernel | shape | data type | notes |
|---|---|---|---|
| sgemm_muon | 64×64×64 | fp32 | SIMT GEMM (self-contained data) |
| gemm_simt | 64×64×64 | fp32 | tiled SIMT GEMM (TM/TN/warps params) |
| sgemm_wg | runtime dims | fp32 | workgroup SMEM-tiled GEMM (BM/BN/BK) |
| sgemm_tcore | BM128 BN64 BK128 | fp16 | Gemmini tensor-core GEMM |
| gemm_mxgemmini | 128×128×512 | fp6 | MX-Gemmini GEMM |
| flash_attention | seq1024 d64 | bf16 | flash attention |
| flash_attention_virgo | 64×64 d64 | — | flash attention on Virgo/Gemmini |
| softmax | 8×4096 | fp32 | row softmax |
| rmsnorm | 128×192 | fp32 | RMSNorm (data checked in) |
| swiglu | 4×4096 | fp32 | SwiGLU |

## What is and isn't a win

Two kernels carry real speedup:

- **matmul (prob0): 2.43× net / 1.146× total.** RTL-confirmed: on `RadianceSingleClusterFastConfig` the
  naive vs SMEM total ratio is 1.161×, matching cyclotron@bw=6 (1.146×) within 1.3%. The total figure is
  modest because a 64³ matmul is dominated by fixed launch/verify overhead; the kernel-region speedup is
  2.43×. Note the autocomp search's saved best for prob0 (`muon_0_beam_iters8`) equals the baseline — it
  was found under the old bw=64 oracle, which could not see the SMEM win; the win comes from the
  hand-verified SMEM kernel `sol0_smem_manual.cpp`.
- **conv patch-embed (prob1): 1.48× net / 1.289× total.** From specializing the patch-embed shapes to
  compile-time constants, padding the SMEM patch row to avoid bank conflicts, and address-space
  qualifying the pointers.

The remaining problems sit at ≈1.0× because their baselines are already SMEM-staged: the memory-class
win the bw=6 model exposes is already captured in those baselines, leaving little SIMT-level headroom.
softmax (prob5) and flash attention (prob7) are not measured here — prob5's saved best fails to run and
prob7's baseline exceeds the cyclotron timeout at bw=6 (memory wall).

## How each speedup was achieved (baseline → best)

**matmul — naive → SMEM tiling.** Baseline computes one output per thread, reading `A[row,k]` and
`B[k,col]` from global memory inside the k-loop, so every multiply-add touches global memory. Under the
finite-bandwidth model this is bandwidth-bound. The best kernel stages the full A (row-major) and B
(column-major, padded by 16 to avoid shared-memory bank conflicts) into shared memory once, issues a
cross-core barrier (`mu_barrier(0, num_warps)`), then computes all outputs from shared memory. Global
traffic drops from O(MNK) to O(MK + KN); the kernel becomes compute-bound.

**conv patch-embed — generic im2col → shape-specialized im2col.** Baseline is already a register-frugal
im2col + SMEM matmul. The best kernel folds the fixed patch-embed dimensions (C=3, K=16, OC=16, OH=OW=4)
into compile-time constants, removing dynamic index arithmetic and freeing registers; pads the SMEM
patch row (CKK 768 → 784) so consecutive rows map to distinct banks; and uses address-space-qualified
pointers. The result is fewer issued instructions and conflict-free SMEM access.

**already-tiled baselines (prob2/3/6/10, etc.).** The baselines stage operands into shared memory, so
the search's transformations (loop reordering, minor unrolling) move total cycles by <2%. These confirm
the bw=6 model is not introducing phantom wins where none exist.
