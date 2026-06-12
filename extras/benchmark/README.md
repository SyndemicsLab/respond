# RESPOND Benchmarking Procedure

This benchmark target is designed for reproducible performance measurement of core RESPOND state-transition execution using only the C++ standard library timing utilities.

## Principles

- Use `std::chrono::steady_clock` to avoid wall-clock discontinuities.
- Separate warm-up from timed measurement.
- Collect multiple samples and summarize robust statistics (min, max, mean, median, p95, stddev).
- Keep benchmark setup deterministic so samples are comparable across runs.
- Prevent dead-code elimination using a checksum sink.

## What Is Measured

The benchmark constructs one RESPOND model and measures repeated execution of:

- `Model::RunTransitions()` for a fixed number of timesteps
- Transition mix: behavior, intervention, overdose, background death
- Deterministic transition matrices/vectors and deterministic initial state

Warm-up runs are excluded from reported timings.

## Build

Enable benchmark builds in CMake:

```bash
cmake -S . -B build/bench -DRESPOND_BUILD_BENCH=ON
cmake --build build/bench --target respond_benchmark
```

## Run

```bash
./build/bench/bin/respond_benchmark \
  --state-size 64 \
  --steps 52 \
  --history-capture-interval 1 \
  --warmup 5 \
  --samples 500 \
  --repetitions 3
```

## Interpreting Output

Before timing rows, the benchmark prints environment metadata to make results reproducible across machines and builds:

- compiler and compiler version
- C++ standard level
- build type (Debug/Release)
- pointer size
- CPU model (Linux reads `/proc/cpuinfo`)
- hardware thread count
- Eigen version and active Eigen thread count

Each repetition reports:

- `mean_ms`: average sample time
- `p50_ms`: median sample time
- `p95_ms`: 95th percentile sample time
- `min_ms` / `max_ms`: observed range
- `std_ms`: standard deviation
- `ns/step`: normalized cost per simulation timestep
- `state_pts`: number of recorded state-history points captured per sample
- `checksum`: final state sum sink to guard against optimization artifacts

Use the `overall` row for broad comparisons and repetition rows to detect instability.

## Recommended Comparison Workflow

1. Run baseline benchmark on the reference branch.
2. Run benchmark after your change with the same arguments and machine load.
3. Compare `overall` mean, p95, and stddev first.
4. If regression appears, inspect repetition spread to determine variance vs consistent slowdown.
5. Re-run with higher `--samples` for tighter confidence when results are close.
