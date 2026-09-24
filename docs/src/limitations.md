# Limitations

RESPOND is under active development. The following limitations reflect the current C++ library behavior.

## Library

The core API is stable enough for integration, but there are important constraints:

- `Model::Create(...)` currently returns the Markov implementation; its string
	argument is an instance name, not a model-type selector. Additional model
	families are not yet exposed through the public factory.
- Execution is timestep-driven; users must construct timesteps and transitions explicitly.
- Transition creation is string-based (`Transition::Create(...)`), so invalid type names fail at runtime.
- `Simulation` mutation and result access are not synchronized. Callers must
	externally synchronize configuration, model mutation, and result access when
	sharing a simulation across threads, and must not mutate or inspect models
	while `Simulation::Run()` is executing.
- `Simulation::Run()` may execute independent models concurrently when enabled
	through `ExecutionConfig`. Separate `Simulation::Run()` calls are serialized
	because Eigen's worker setting is process-global; the configured Eigen worker
	limit must still be observed within each run.
- Logging registries and shared sinks are internally synchronized for
	concurrent logger creation and logging.
- GPU execution is not supported.
- Legacy standalone executable workflows are maintained separately from the modern library API.

## Data

RESPOND C++ focuses on simulation primitives (state vectors, transitions, histories) rather than built-in dataset ingestion.

- Users are responsible for preparing and validating transition inputs (matrices/vectors) before simulation.
- Schema conventions from legacy tooling (for example `sim.conf` and CSV pipelines) are not part of the required core C++ API.

Previous: [Under the Hood](math.md)

Next: [FAQs](faq.md)
