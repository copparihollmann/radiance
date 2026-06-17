# Problem 0 optimization journey (reconstructed)

- run: `built:muon_muon_0_beam_iters5_cyclotron`
- iterations: 5
- baseline cycles: 480918 | best cycles: 419797 | speedup_total: 1.146 | speedup_net: 2.431
- candidates: 73 | compiled: 7 | correct: 7 | improved: 0
- cost: $4.07, 1984347 tokens, 181 calls

## Attempts (iteration -> candidate)

(reconstructed from eval results; strategies are in `plans/`)

| iter | cand | compiled | correct | latency |
|---|---|---|---|---|
| 0 | 0 | True | True | 103838 |
| 1 | 0 | False | False | None |
| 1 | 1 | False | False | None |
| 1 | 2 | False | False | None |
| 1 | 3 | False | False | None |
| 1 | 4 | False | False | None |
| 1 | 5 | False | False | None |
| 1 | 6 | False | False | None |
| 1 | 7 | False | False | None |
| 1 | 8 | False | False | None |
| 2 | 0 | False | False | None |
| 2 | 1 | True | True | 42941 |
| 2 | 2 | False | False | None |
| 2 | 3 | False | False | None |
| 2 | 4 | False | False | None |
| 2 | 5 | False | False | None |
| 2 | 6 | False | False | None |
| 2 | 7 | False | False | None |
| 2 | 8 | False | False | None |
| 3 | 0 | False | False | None |
| 3 | 10 | False | False | None |
| 3 | 11 | False | False | None |
| 3 | 12 | False | False | None |
| 3 | 13 | False | False | None |
| 3 | 14 | False | False | None |
| 3 | 15 | False | False | None |
| 3 | 16 | False | False | None |
| 3 | 17 | False | False | None |
| 3 | 1 | False | False | None |
| 3 | 2 | False | False | None |
| 3 | 3 | False | False | None |
| 3 | 4 | False | False | None |
| 3 | 5 | False | False | None |
| 3 | 6 | True | True | 42941 |
| 3 | 7 | True | True | 42941 |
| 3 | 8 | True | True | 42941 |
| 3 | 9 | False | False | None |
| 4 | 0 | False | False | None |
| 4 | 10 | False | False | None |
| 4 | 11 | False | False | None |
| 4 | 12 | False | False | None |
| 4 | 13 | False | False | None |
| 4 | 14 | False | False | None |
| 4 | 15 | False | False | None |
| 4 | 16 | False | False | None |
| 4 | 17 | False | False | None |
| 4 | 1 | False | False | None |
| 4 | 2 | False | False | None |
| 4 | 3 | False | False | None |
| 4 | 4 | False | False | None |
| 4 | 5 | False | False | None |
| 4 | 6 | False | False | None |
| 4 | 7 | True | True | 103378 |
| 4 | 8 | False | False | None |
| 4 | 9 | False | False | None |
| 5 | 0 | False | False | None |
| 5 | 10 | False | False | None |
| 5 | 11 | False | False | None |
| 5 | 12 | False | False | None |
| 5 | 13 | False | False | None |
| 5 | 14 | False | False | None |
| 5 | 15 | False | False | None |
| 5 | 16 | False | False | None |
| 5 | 17 | False | False | None |
| 5 | 1 | False | False | None |
| 5 | 2 | True | True | 98729 |
| 5 | 3 | False | False | None |
| 5 | 4 | False | False | None |
| 5 | 5 | False | False | None |
| 5 | 6 | False | False | None |
| 5 | 7 | False | False | None |
| 5 | 8 | False | False | None |
| 5 | 9 | False | False | None |
