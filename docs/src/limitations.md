# Limitations

RESPOND is under active development. The following limitations reflect the current C++ library behavior.

## Library

The core API is stable enough for integration, but there are important constraints:

- `Model::Create(...)` currently returns the Markov implementation; additional model families are not yet exposed through the public factory.
- Execution is timestep-driven; users must construct timesteps and transitions explicitly.
- Transition creation is string-based (`Transition::Create(...)`), so invalid type names fail at runtime.
- The library is not internally synchronized for shared mutable use across threads.
- GPU execution is not supported.
- Legacy standalone executable workflows are maintained separately from the modern library API.

## Data

RESPOND C++ focuses on simulation primitives (state vectors, transitions, histories) rather than built-in dataset ingestion.

- Users are responsible for preparing and validating transition inputs (matrices/vectors) before simulation.
- Schema conventions from legacy tooling (for example `sim.conf` and CSV pipelines) are not part of the required core C++ API.

Previous: [Under the Hood](math.md)

Next: [FAQs](faq.md)
