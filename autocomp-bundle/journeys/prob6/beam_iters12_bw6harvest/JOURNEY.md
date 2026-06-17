# Problem 6 optimization journey (reconstructed)

- run: `built:muon_muon_6_beam_iters12_bw6harvest_cyclotron`
- iterations: 12
- baseline cycles: 487016 | best cycles: 480444 | speedup_total: 1.014 | speedup_net: 1.064
- candidates: 109 | compiled: 1 | correct: 1 | improved: 0
- cost: $7.43, 3422947 tokens, 275 calls

## Attempts (iteration -> candidate)

(reconstructed from eval results; strategies are in `plans/`)

| iter | cand | compiled | correct | latency |
|---|---|---|---|---|
| 0 | 0 | True | True | 109835 |
| 1 | 0 | False | False | None |
| 1 | 1 | False | False | None |
| 1 | 2 | False | False | None |
| 1 | 3 | False | False | None |
| 1 | 4 | False | False | None |
| 1 | 5 | False | False | None |
| 1 | 6 | False | False | None |
| 1 | 7 | False | False | None |
| 1 | 8 | False | False | None |
| 10 | 0 | False | False | None |
| 10 | 1 | False | False | None |
| 10 | 2 | False | False | None |
| 10 | 3 | False | False | None |
| 10 | 4 | False | False | None |
| 10 | 5 | False | False | None |
| 10 | 6 | False | False | None |
| 10 | 7 | False | False | None |
| 10 | 8 | False | False | None |
| 11 | 0 | False | False | None |
| 11 | 1 | False | False | None |
| 11 | 2 | False | False | None |
| 11 | 3 | False | False | None |
| 11 | 4 | False | False | None |
| 11 | 5 | False | False | None |
| 11 | 6 | False | False | None |
| 11 | 7 | False | False | None |
| 11 | 8 | False | False | None |
| 12 | 0 | False | False | None |
| 12 | 1 | False | False | None |
| 12 | 2 | False | False | None |
| 12 | 3 | False | False | None |
| 12 | 4 | False | False | None |
| 12 | 5 | False | False | None |
| 12 | 6 | False | False | None |
| 12 | 7 | False | False | None |
| 12 | 8 | False | False | None |
| 2 | 0 | False | False | None |
| 2 | 1 | False | False | None |
| 2 | 2 | False | False | None |
| 2 | 3 | False | False | None |
| 2 | 4 | False | False | None |
| 2 | 5 | False | False | None |
| 2 | 6 | False | False | None |
| 2 | 7 | False | False | None |
| 2 | 8 | False | False | None |
| 3 | 0 | False | False | None |
| 3 | 1 | False | False | None |
| 3 | 2 | False | False | None |
| 3 | 3 | False | False | None |
| 3 | 4 | False | False | None |
| 3 | 5 | False | False | None |
| 3 | 6 | False | False | None |
| 3 | 7 | False | False | None |
| 3 | 8 | False | False | None |
| 4 | 0 | False | False | None |
| 4 | 1 | False | False | None |
| 4 | 2 | False | False | None |
| 4 | 3 | False | False | None |
| 4 | 4 | False | False | None |
| 4 | 5 | False | False | None |
| 4 | 6 | False | False | None |
| 4 | 7 | False | False | None |
| 4 | 8 | False | False | None |
| 5 | 0 | False | False | None |
| 5 | 1 | False | False | None |
| 5 | 2 | False | False | None |
| 5 | 3 | False | False | None |
| 5 | 4 | False | False | None |
| 5 | 5 | False | False | None |
| 5 | 6 | False | False | None |
| 5 | 7 | False | False | None |
| 5 | 8 | False | False | None |
| 6 | 0 | False | False | None |
| 6 | 1 | False | False | None |
| 6 | 2 | False | False | None |
| 6 | 3 | False | False | None |
| 6 | 4 | False | False | None |
| 6 | 5 | False | False | None |
| 6 | 6 | False | False | None |
| 6 | 7 | False | False | None |
| 6 | 8 | False | False | None |
| 7 | 0 | False | False | None |
| 7 | 1 | False | False | None |
| 7 | 2 | False | False | None |
| 7 | 3 | False | False | None |
| 7 | 4 | False | False | None |
| 7 | 5 | False | False | None |
| 7 | 6 | False | False | None |
| 7 | 7 | False | False | None |
| 7 | 8 | False | False | None |
| 8 | 0 | False | False | None |
| 8 | 1 | False | False | None |
| 8 | 2 | False | False | None |
| 8 | 3 | False | False | None |
| 8 | 4 | False | False | None |
| 8 | 5 | False | False | None |
| 8 | 6 | False | False | None |
| 8 | 7 | False | False | None |
| 8 | 8 | False | False | None |
| 9 | 0 | False | False | None |
| 9 | 1 | False | False | None |
| 9 | 2 | False | False | None |
| 9 | 3 | False | False | None |
| 9 | 4 | False | False | None |
| 9 | 5 | False | False | None |
| 9 | 6 | False | False | None |
| 9 | 7 | False | False | None |
| 9 | 8 | False | False | None |
