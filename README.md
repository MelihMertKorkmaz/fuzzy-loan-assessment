# Fuzzy loan assessment (Mamdani, hierarchical, crisp handoff)

Three stages: house (market + location) -> applicant (asset + income) ->
credit (house + applicant + income + interest). Mamdani inference with
centroid defuzzification. Written in pure C with no fuzzy toolbox.

## Files

- `fuzzy.h` / `fuzzy.c` — generic engine: Tri/Trap shapes, MF union, centroid.
- `params.h` — all MF numbers, universe limits, N_SAMPLES (numbers only).
- `model.h` / `model.c` — MF tables, 14/12/15 rule tables, infer_house/applicant/credit.
- `main.c` — 6 applicant scenarios (5 typical + the all-maximum corner) and printing.
- `Makefile` — builds `./fuzzy_loan` (`all`, `run`, `clean` targets).

## Build and run

- `make` — builds `./fuzzy_loan`.
- `make run` — builds and runs it.

## Units

Market, asset, income, credit in thousands of USD; interest in %;
house and applicant scores on 0-10.
