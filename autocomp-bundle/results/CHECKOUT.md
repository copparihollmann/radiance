# Modules to check out

This work spans several repositories. All carry a branch named `feat/radiance-autocomp`. To reproduce
the full setup, check out that branch in each module below. Commit hashes are filled in once the branches
are pushed.

| Repository | Path (under chipyard) | Branch | Commit | Notes |
|---|---|---|---|---|
| radiance | `generators/radiance` | `feat/radiance-autocomp` | branch tip | this dir (`autocomp/`), `RadianceSingleClusterFastConfig`, sfilter L0d backpressure fix |
| cyclotron | `generators/radiance/cyclotron` | `feat/radiance-autocomp` | `e075019` | global-bandwidth model change (`bytes_per_cycle 64→6`) |
| autocomp | (standalone) | `feat/radiance-autocomp` | `e93a8be` | the search backend, muon eval/harnesses, `muon_search_data/` |
| radiance-kernels | (standalone) | `feat/radiance-autocomp` | `77c4275` | the generated kernel directories |
| chipyard | (superproject) | `feat/radiance-autocomp` | branch tip | submodule pointer bump to the radiance branch |

All branches are published on forks under `copparihollmann/` (the upstreams are not writable):

- radiance: `github.com/copparihollmann/radiance` (upstream `ucb-bar/radiance`)
- cyclotron: `github.com/copparihollmann/cyclotron` (upstream `hansungk/cyclotron`)
- autocomp: `github.com/copparihollmann/autocomp` (upstream `ucb-bar/autocomp`)
- radiance-kernels: `github.com/copparihollmann/radiance-kernels` (upstream `ucb-bar/radiance-kernels`)
- chipyard: `github.com/copparihollmann/chipyard` (upstream `ucb-bar/chipyard`)

Each carries branch `feat/radiance-autocomp`.

## Build / run notes

- The cyclotron model change is config-only (`config/timing/gmem.toml`); rebuild of cyclotron is the
  normal `cargo build --release`.
- RTL gating uses `RadianceSingleClusterFastConfig`. Build it from `sims/vcs` after `source env.sh`
  (the conda environment pins JDK-20; the system JDK breaks the sbt metabuild). See `MODEL_CHANGES.md`.
- The autocomp Muon loop and the metric scripts live in the autocomp repo under `scripts/muon/` and
  `backend/muon/`.
