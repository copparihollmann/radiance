# START HERE — autocomp-on-Muon optimization-journey bundle

This is a self-contained bundle of everything produced by running the autocomp kernel-optimization loop
against the Radiance Muon SIMT GPU: the results, the kernels, and — the point of this bundle — the **full
optimization journey** for each kernel (start → every transformation attempt → landing), plus the token/$
cost. It is meant to be handed to another agent to mine learnings for compiler heuristics and code
generation.

Everything needed is inside this folder. Move it with one command:

```
tar czf autocomp-bundle.tar.gz autocomp-bundle/      # or: cp -r autocomp-bundle /dest
```

## Headline results

- **matmul 64³, shared-memory tiling: 2.43× kernel-only / 1.146× total** (RTL-confirmed: RTL 1.161× vs the
  calibrated model 1.146×). The flagship win.
- **conv patch-embed: 1.48× kernel-only / 1.289× total** (shape specialization + bank-conflict-free SMEM).
- **Analytical-model change**: cyclotron global data bandwidth `bytes_per_cycle 64 → 6` makes the model
  see memory-reuse wins; it now matches RTL within 1.3% (details in `results/MODEL_CHANGES.md`).
- Most other problems land at ≈1.0× because their baselines were already SMEM-staged.

## What's in here (bundle map)

```
START_HERE.md            this file — the map
HEURISTICS_NOTES.md      starter "what worked / what didn't" analysis with outcome statistics
results/                 the results release, copied in
  README.md              executive summary
  MODEL_CHANGES.md       the cyclotron bw=6 change + RTL validation + the FastConfig
  CHECKOUT.md            the repos/branches that produced this
  metrics/
    results.md / results.csv   per-problem baseline/best cycles, total+net speedup, utilization, provenance
    UTILIZATION.md             conservative utilization (total + kernel-only), roofline
    PHASE_C_HARVEST.md         the harvest null-result analysis
    MUON_SEARCH_TUNING.md      the full tuning/methodology log
    util_calc.py               utilization calculator
    faithfulness_bw.csv        bw=6 vs bw=64 per-op check
    rtl_confirmation/          RTL gate evidence (naive vs SMEM cycle counts)
kernels/
  golden/        author-designed reference kernels (sgemm_muon, gemm_simt, sgemm_wg, sgemm_tcore,
                 gemm_mxgemmini, flash_attention[_virgo], softmax, rmsnorm, swiglu) — source only
  real_model/    the baseline kernels we constructed for each problem (the starting points)
  generated/     the best autocomp-discovered kernels + the hand-verified SMEM matmul
journeys/probN/[run]/   the optimization journey per problem (see schema below)
  baseline.cpp           starting kernel
  best.cpp               landing kernel (best_candidate_so_far)
  JOURNEY.md             human-readable timeline (iteration → candidates → outcomes, with cost)
  journey.jsonl          one row per attempted candidate (machine-readable; schema below)
  cost.json              per-iteration and run-total tokens + USD
  plans/                 the natural-language optimization strategies the planner proposed (+ prompts)
  code/                  the generated code variants (impl_*) for each plan
  eval/                  the eval verdict per candidate (compiled / correct / latency)
  candidates/            the beam-advanced candidate code per iteration
index/
  all_transforms.jsonl   every journey row across all problems (the master mineable table)
  summary.csv            per problem/run: baseline/best cycles, speedup, #iters, #candidates,
                         #compiled, #correct, #improved, total tokens, total $
  cost_by_problem.csv    per problem/iteration cost
  transform_ledger_muon.jsonl   the original transform ledger (muon rows)
  muon-spend_total.json         global spend ($153.29, by model/day)
tools/                   build_journeys.py (regenerates journeys/ + index/), util_calc.py,
                         faithfulness_bw.sh, run_problem.sh  (NOTE: these have hardcoded /scratch paths)
```

### `journey.jsonl` row schema

Ledger-sourced rows (probs 1,2,3,7 — `source:"transform_ledger"`):
`{prob_id, run, iteration, parent_idx, cand_idx, model, strategy_num, strategy, compiled, correct,
latency, parent_latency, speedup, outcome, plan_path, source}`.
`parent_idx`+`cand_idx`+`iteration` reconstruct the beam tree; `strategy` is the NL transform; `outcome`
∈ {improved, regressed, correct_no_gain, compile_error, incorrect}.

Reconstructed rows (probs 0,6,10 — `source:"reconstructed"`):
`{prob_id, run, iteration, cand_idx, compiled, correct, latency, source}` (strategies are in `plans/`).

**Latency note**: journey/eval `latency` is the search's *net* metric (total cycles minus a fixed
per-problem harness overhead) — it is what the search optimized. End-to-end *total* cycles are in
`results/metrics/results.csv`.

## Coverage (per problem)

Journey availability: **full** = per-iteration candidates/plans/eval present; **final-only** = best kernel
but journey not preserved; **baseline-only** = no search.

| prob | op (provenance) | baseline cyc | best cyc | speedup (total/net) | journey |
|---|---|---|---|---|---|
| 0 | matmul 64³ (synthetic anchor) | 480,918 | 419,797 | 1.146× / 2.43× | full |
| 1 | conv patch-embed (openvla) | 102,215 | 79,286 | 1.289× / 1.48× | full (2 runs) |
| 2 | attention seq64 (rdt) | 1,308,352 | 1,292,929 | 1.012× | full |
| 3 | attention seq96 (rdt) | 2,759,774 | 2,759,774 | 1.000× | full |
| 4 | swiglu (rdt) | 780,625 | 780,326 | 1.000× | final-only |
| 5 | softmax (rdt) | 509,523 | — (best fails) | — | final-only |
| 6 | matmul K128 (smolvla) | 487,016 | 480,444 | 1.014× | full |
| 7 | flash attn (pi05) | timeout @bw6 | — | — | full (search trace) |
| 8 | layer_norm (smolvla) | 5,735,899 | 5,507,821 | 1.041× | final-only |
| 9 | gelu (smolvla) | 3,059,220 | 3,059,220 | 1.000× | final-only |
| 10 | matmul K384 (smolvla) | 998,938 | 987,528 | 1.012× | full |
| 11 | attention seq96 d72 (pi05) | 3,465,399 | — | — | baseline-only |
| 12 | flash attn seq192 (pi05) | register-infeasible | — | — | baseline-only (dropped) |
| 13 | matmul 8×256×768 (tiny_llama/rdt) | 3,548,850 | — | — | baseline-only |

Cost of the journeyed runs ≈ $72 total (hybrid Flash-plan + Qwen-code); global muon spend $153.29 (incl.
earlier Sonnet runs) — see `index/`.

## Original locations (provenance / regeneration)

Published on GitHub forks under `copparihollmann/`, branch `feat/radiance-autocomp` (and this bundle on
`feat/radiance-autocomp-journeys` in the radiance fork):

| repo | github | branch | commit |
|---|---|---|---|
| radiance | copparihollmann/radiance | feat/radiance-autocomp-journeys | (this bundle) |
| radiance (release) | copparihollmann/radiance | feat/radiance-autocomp | tip |
| cyclotron | copparihollmann/cyclotron | feat/radiance-autocomp | e075019 (bw=6 model change) |
| autocomp | copparihollmann/autocomp | feat/radiance-autocomp | e93a8be (search infra + build_journeys.py) |
| radiance-kernels | copparihollmann/radiance-kernels | feat/radiance-autocomp | 77c4275 (generated kernels) |
| chipyard | copparihollmann/chipyard | feat/radiance-autocomp | tip (submodule pointer) |

On the source machine (paths will differ after a move — these are for regeneration only):
- Raw search output: `autocomp/output/built:muon_muon_<prob>_<run>_cyclotron/` (full per-iteration artifacts)
- Ledgers: `autocomp/muon_search_data/ledgers/`
- Problem harnesses / baselines: `autocomp/harnesses/muon/test<N>/`, `autocomp/sols/muon/sol<N>_baseline.cpp`
- Regenerate this dataset: `python3 tools/build_journeys.py <out>` (after fixing the paths at the top of
  the script to the autocomp checkout).
- Publishing used SSH key `/scratch2/agustin/DIMA_SLICE`.

## How to mine this for compiler heuristics

1. Start with `index/all_transforms.jsonl` — every attempt with its strategy and outcome/latency. Filter
   `source=="transform_ledger"` for rows that carry the NL `strategy` and `outcome` labels (probs 1,2,3,7).
2. For a given problem, read `journeys/probN/<run>/JOURNEY.md` for the narrative, then `plans/` for the
   exact strategies proposed and `code/` for what each produced, and `eval/` for whether it compiled/ran.
3. Join cost via `index/cost_by_problem.csv` / per-run `cost.json`.
4. `HEURISTICS_NOTES.md` has a starter analysis (failure-mode breakdown, the two real wins, what didn't
   help) to build on.
```
