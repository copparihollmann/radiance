# Starter analysis: what worked and what didn't

A seed for mining `index/all_transforms.jsonl` and the per-problem `journeys/`. All numbers are from the
committed data; treat the qualitative patterns as hypotheses to confirm against the journeys.

## Outcome statistics (835 attempted candidates across 7 problems / 8 runs)

- Compiled AND correct: **94 / 835 (~11%)**. The large majority of generated candidates never become a
  valid kernel.
- Of the 84 candidates with a rich ledger outcome label: **compile_error 68 (81%)**, improved 10,
  correct_no_gain 4, regressed 2.

The dominant failure mode is generated code that does not compile or is RTL-illegal. From the search logs
the compile/illegal failures break down into:
- **Register oversubscription** (>32 live registers/warp → RTL `globalOverSubscription`). The single most
  common *semantic* rejection; the Muon physical-register budget is the binding constraint.
- **Syntax / API errors**: undeclared identifiers (e.g. using a constant out of its scope), wrong address-
  space qualifiers (`__local`/`__global`), missing helper functions, calling libc (`fmaf`, `sqrtf`).

## What produced the real wins (confirm in the journeys)

- **matmul (prob0): naive → shared-memory staging = 2.43x kernel-only.** Stage A and B into SMEM once
  (B transposed + padded by 16 to avoid bank conflicts), one cross-core barrier, then compute from SMEM.
  Converts a bandwidth-bound inner loop into a compute-bound one. The single highest-value transform.
- **conv patch-embed (prob1): shape specialization = 1.29x total / 1.48x net.** Fold fixed dims into
  `constexpr` (lets the compiler turn divide/modulo by powers of two into shifts/masks and frees
  registers), pad the SMEM patch row to dodge bank conflicts, use address-space-qualified pointers.

## What did NOT help

- **Re-optimizing already-tiled baselines** (prob2/3/6/10): when the baseline already stages to SMEM, the
  search's loop reorders / unrolls move total cycles by <2%. No SIMT-level headroom remains.
- **Aggressive register-heavy tiling / wide unrolling**: repeatedly tripped the register wall and was
  rejected. On a 256-physical-register SIMT machine, register pressure is a first-order constraint, not an
  afterthought — heuristics should bound live state before proposing unrolling.

## Heuristic takeaways (hypotheses for a compiler)

1. Prioritize memory-reuse (SMEM staging, operand caching) over compute micro-optimization for GEMM-class
   kernels — it is where the order-of-magnitude headroom is, and it is invisible to an issue-bound cost
   model (see `results/MODEL_CHANGES.md`).
2. Treat the register budget as a hard pre-filter: estimate live registers before emitting a candidate;
   most rejected candidates die here.
3. Specialize known-constant shapes to compile-time constants — large, cheap win on conv-class kernels.
4. Bank-conflict-aware SMEM layout (pad odd strides) is a recurring enabler.
5. Expect a low hit rate (~10% compile+correct); a search/compiler loop must be cheap per attempt or filter
   hard before evaluation.

## Where to look

- Per-attempt records with strategy/outcome/latency: `index/all_transforms.jsonl`.
- Per-problem timeline and the actual code/plans: `journeys/prob<N>/<run>/` (`JOURNEY.md`, `plans/`,
  `code/`, `eval/`, `journey.jsonl`).
- Cost of each run: `journeys/prob<N>/<run>/cost.json` and `index/cost_by_problem.csv`.
