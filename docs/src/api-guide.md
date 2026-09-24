# C++ API Guide

This guide provides an overview of the RESPOND C++ API for developers wishing to use the library in their own projects.

## Overview

The RESPOND library provides a flexible framework for building opioid use disorder models through composition of models, transitions, and history tracking. The core components are:

- **Model**: Abstract base class representing a state transition system
- **Simulation**: Aggregates and coordinates multiple models
- **Timestep**: Owns and sequences transitions for one simulation step
- **Transition**: Abstract base for specific transition types
- **History**: Tracks state vectors over time

### Runtime Configuration

`RuntimeConfig` groups the logging and execution settings used by `Simulation`
and `Model`. Use the configuration-based constructors and factory overloads for
new code.

```cpp
#include <respond/respond.hpp>

respond::RuntimeConfig runtime_config;
runtime_config.logging.logger_name = "respond";
runtime_config.logging.file_path = "respond.log";
runtime_config.logging.use_shared_sink = true;

runtime_config.execution.run_models_concurrently = true;
runtime_config.execution.total_threads = 4;
runtime_config.execution.eigen_threads = 1;
```

When multiple models run concurrently, keep `eigen_threads` at `1` because
Eigen's worker setting is process-global. For sequential execution, including a
single-model simulation, `eigen_threads` may be greater than `1`.

### Migrating from Legacy Constructors

The string-based `Model::Create`, `Simulation`, `Timestep`, and transition
factory overloads are deprecated. Replace separate logger-name and file-path
arguments with `LoggingConfig`, and pass it through `RuntimeConfig` when both
logging and execution settings are needed:

```cpp
// Legacy:
// respond::Simulation sim("app", "simulation.log");

respond::RuntimeConfig config;
config.logging = {"app", "simulation.log", false};
respond::Simulation sim(config);
```

## Core Concepts

### State Vectors

Models operate on state vectors (Eigen::VectorXd) representing the population distribution across model states. A state vector element at index i represents the count of individuals in state i.

### Transitions

Transitions apply transformations to state vectors using transition matrices or vectors. The RESPOND model supports several transition types:

- **Migration**: Population movement between states
- **Behavior**: Behavioral state changes
- **Intervention**: Intervention-driven state changes
- **Overdose**: Overdose-related transitions
- **BackgroundDeath**: Background mortality transitions

### History Tracking

History objects record state vectors at timesteps, enabling analysis of state trajectories over time. Histories support sparse timesteps and can return contiguous vectors with zero-filled gaps.

## Model Class

The Model class is the abstract base for all models in RESPOND.

```cpp
#include <respond/respond.hpp>

respond::RuntimeConfig runtime_config;
runtime_config.logging.logger_name = "model_logger";
runtime_config.logging.file_path = "model.log";

// Create a model
auto model = respond::Model::Create("markov", runtime_config);

// Set the initial state
Eigen::VectorXd initial_state(50);
initial_state.setZero();
model->SetState(initial_state);

// Build one timestep with transitions
respond::Timestep step(runtime_config.logging);
auto &behavior_transition = step.CreateTransition("behavior");
behavior_transition->AddMatrix(some_matrix);

auto migration_transition = respond::Transition::Create(
    "migration", "migration", runtime_config.logging);
step.AddTransition(migration_transition);

// Mutable index access to owned transition slots
step[0]->AddMatrix(some_other_matrix);
model->AddTimestep(step);

// Execute one simulation step
model->RunTimestep();

// Retrieve current state
Eigen::VectorXd current_state = model->GetState();

// Access history records
auto histories = model->GetHistories();
```

`Model::Create()` currently creates the Markov implementation. Its string
argument is an instance name, not a model-type selector, and any string is
accepted as that name. Additional model implementations may be exposed by the
factory in the future.

### Key Methods

- `SetState(const Eigen::Ref<const Eigen::VectorXd> &state)`: Sets the model's state vector
- `GetState() const`: Returns a const Eigen ref to the current state
- `AddTimestep(const Timestep &timestep)`: Adds a timestep (deep-copied)
- `RunTimestep()`: Runs the current timestep and advances time
- `RunTimestep(size_t idx)`: Runs a specific timestep index
- `RunTimesteps()`: Runs all registered timesteps (bounded by final timestep when set)
- `ClearTimesteps()`: Removes all timesteps
- `GetHistories() const`: Returns map of history name to History objects
- `CreateDefaultHistories()`: Initializes default history tracking
- `ClearHistories()`: Clears history records and resets history tracking
- `GetName() const`: Returns model name
- `clone() const`: Creates a deep copy of the model

## Simulation Class

The Simulation class manages multiple models and coordinates their execution.

```cpp
#include <respond/respond.hpp>

respond::RuntimeConfig runtime_config;
runtime_config.logging.logger_name = "simulation_logger";
runtime_config.logging.file_path = "simulation.log";
runtime_config.logging.use_shared_sink = true;
runtime_config.execution.run_models_concurrently = true;
runtime_config.execution.total_threads = 2;
runtime_config.execution.eigen_threads = 1;

// Create a simulation with explicit logging and execution settings
respond::Simulation sim(runtime_config);

// Add models
auto model1 = respond::Model::Create("model1", runtime_config);
auto model2 = respond::Model::Create("model2", runtime_config);
sim.AddModel(model1);
sim.AddModel(model2);

// Mutate owned models directly via index
sim[0]->CreateDefaultHistories();
sim[1]->CreateDefaultHistories();

// Run 52 timesteps for all models
sim.Run(52);

// Retrieve results
auto model_0_histories = sim.GetModelHistory(0);
auto model_names = sim.GetModelNames();

// Get history names for one model
auto history_names = sim.GetModelHistoryNames(0);
```

### Key Methods

- `Run(int duration = -1)`: Runs all models for the configured duration
- `SetDuration(int duration)`: Sets default duration used by `Run()` when no argument is provided
- `CreateNewModel(const std::string &name)`: Creates and manages a model, then returns an editable deep copy; assign the edited copy through `sim[idx]`
- `AddModel(const std::unique_ptr<Model> &model)`: Adds a model (cloned internally)
- `operator[](size_t idx)`: Mutable index access to owned model slot (`sim[idx]->Method()`)
- `operator[](size_t idx) const`: Const index access to owned model
- `GetModels() const`: Returns a deep-copied vector of models
- `GetModel(int idx) const`: Returns one deep-copied model by index (`-1` returns last)
- `GetModelIndexNameMap() const`: Returns map of model index to model name
- `GetModelNames() const`: Returns all model names
- `ClearModels()`: Removes all models
- `GetModelHistory(size_t idx) const`: Returns one model's history map
- `GetModelHistoryNames(size_t idx) const`: Returns history names for one model

## Timestep Class

The Timestep class owns transitions for one model step and supports both
transition creation and clone-based insertion.

```cpp
#include <respond/respond.hpp>

respond::LoggingConfig logging_config{
    "timestep_logger", "timestep.log", false};
respond::Timestep step(logging_config);

// Build transition in-place
auto &behavior = step.CreateTransition("behavior");
behavior->AddMatrix(behavior_matrix);

// Add an existing transition by clone
auto migration = respond::Transition::Create(
    "migration", "migration", logging_config);
step.AddTransition(migration);

// Mutable slot access (in-place edits)
step[0]->AddMatrix(another_behavior_matrix);

// Replace a slot by cloning from another slot or transition pointer
step[1] = step[0];
step[1] = migration;

// Const slot access
const respond::Timestep &const_step = step;
const respond::Transition &t = const_step[0];
```

### Key Methods

- `CreateTransition(const std::string &transition_name)`: Creates and stores a transition by type
- `AddTransition(const std::unique_ptr<Transition> &transition)`: Clones and stores caller-provided transition
- `operator[](size_t idx)`: Mutable slot access for transition mutation/replacement
- `operator[](size_t idx) const`: Const transition reference by index
- `GetTransition(const size_t &idx) const`: Gets transition pointer by index
- `GetTransition(const std::string &transition_name) const`: Gets transition pointer by name
- `GetTransitionNames() const`: Returns transition names in execution order
- `RemoveTransition(size_t idx)`: Removes and returns transition at index

### Model Access Semantics

- `sim[idx]` accesses the model owned by `Simulation` and can be used for in-place mutation.
- `sim[idx] = *other_model` replaces the model at `idx` by cloning `other_model`.
- `sim[idx] = other_model_ptr` replaces the model at `idx` by cloning the pointee (caller retains ownership).
- `GetModels()` and `GetModel(...)` return clones for safe detached access.
- Name-based retrieval is not provided; use `GetModelIndexNameMap()` to resolve names to indices.

## History Class

The History class records and manages state vectors across timesteps.

```cpp
#include <respond/history.hpp>

// Create a history
respond::History hist("population_states", "my_logger");

// Add states at specific timesteps
hist.AddState(state_vector_0, 0);
hist.AddState(state_vector_1, 1);
hist.AddState(state_vector_2, 2);

// Or let it auto-assign timesteps
hist.AddState(another_state);  // Assigned to next available timestep

// Retrieve states
auto state_at_t0 = hist.GetStateMap()[0];
auto all_states = hist.GetStateAsVector();  // Contiguous vector, fills gaps

// Query history properties
std::string name = hist.GetName();
respond::HistoryMode mode = hist.GetHistoryMode();

// Clear history
hist.Clear();
```

### Key Methods

- `AddState(const Eigen::VectorXd &state, int timestep = -1)`: Records a state
  - If timestep < 0, automatically assigns next available timestep
  - If timestep already exists, currently overwrites
- `GetStateMap() const`: Returns map of timestep → state vector
- `GetRecordedTimesteps() const`: Returns stored timesteps without densifying
- `GetRecordedStates() const`: Returns stored states without densifying
- `GetStateAsVector() const`: Returns contiguous vector of states (fills gaps with zeros)
- `GetName() const`: Returns history identifier
- `GetLatestRecordedTimestep() const`: Returns latest recorded timestep
- `GetPendingState() const`: Returns pending aggregate for accumulated histories
- `HasPendingState() const`: Indicates pending aggregate state
- `Clear()`: Removes all recorded states
- `operator==`, `operator!=`: Comparison operators

## Transition Class

The Transition class is abstract; use `Transition::Create(...)` to create concrete instances.

```cpp
#include <respond/respond.hpp>

respond::LoggingConfig logging_config{
    "transition_logger", "transitions.log", false};

// Create a transition
auto transition = respond::Transition::Create(
    "behavior",      // Type
    "behavior_name", // Instance name
    logging_config    // Logger configuration
);

// Add transformation matrices
Eigen::MatrixXd trans_matrix = ...;
transition->AddMatrix(trans_matrix);

// Execute the transition (typically done by model timesteps)
auto histories_map = ...; // From model
Eigen::VectorXd result = transition->Execute(current_state, histories_map);

// Get transition properties
std::string name = transition->GetName();

// Clear matrices
transition->ClearMatrices();
```

### Supported Transition Types

| Type | Description |
|------|-------------|
| "migration" | Population migration transitions |
| "behavior" | Behavioral state changes |
| "intervention" | Intervention-driven transitions |
| "overdose" | Overdose-related transitions |
| "background_death" | Background mortality transitions |

## Logging Integration

RESPOND uses the spdlog library for logging. Models, timesteps, and transitions
accept `LoggingConfig`, which specifies the logger name, file path, and whether
the logger uses a shared sink:

```cpp
respond::LoggingConfig logging_config{
    "my_logger", "path/to/logfile.log", false};
auto model = respond::Model::Create(
    "my_model", respond::RuntimeConfig{{}, logging_config});

// Reusing the same logger name and destination is idempotent. Reusing the
// name with a different destination returns CreationStatus::kError.
respond::ConfigureLogger(logging_config);
```

## Complete Example

```cpp
#include <respond/respond.hpp>

int main() {
    respond::RuntimeConfig config;
    config.logging = {"app", "simulation.log", false};

    // Create simulation and model with the same runtime settings
    respond::Simulation sim(config);
    auto model = respond::Model::Create("markov", config);

    // Set initial state (e.g., 1000 individuals across 50 states)
    Eigen::VectorXd initial_state = Eigen::VectorXd::Zero(50);
    initial_state(0) = 1000;  // All in first state
    model->SetState(initial_state);

    // Create a reusable timestep with transitions
    respond::Timestep step(config.logging);

    auto &behavior_transition = step.CreateTransition("behavior");
    // behavior_transition->AddMatrix(...);

    auto &migration_transition = step.CreateTransition("migration");
    // migration_transition->AddMatrix(...);

    // Register timesteps on the model
    for (int t = 0; t < 52; ++t) {
        model->AddTimestep(step);
    }

    // Add model to simulation
    sim.AddModel(model);

    // Configure the owned model through Simulation indexing
    sim[0]->CreateDefaultHistories();

    // Run simulation for 52 timesteps
    sim.Run(52);

    // Extract results
    auto histories = sim.GetModelHistory(0);
    auto history_names = sim.GetModelHistoryNames(0);
    
    // Process results...
    
    return 0;
}
```

## Memory Management

RESPOND uses `std::unique_ptr` for ownership management:

- Models and Transitions are typically managed by Simulation or parent objects
- History objects are copyable and can be freely copied
- All models are cloned when added to a Simulation
- Clearing containers (for example `ClearModels`, `ClearTimesteps`) deletes contained objects

## Best Practices

1. **Use `Transition::Create`** to create transitions by type.
2. **Build timesteps explicitly** and add them to models in execution order.
3. **Set simulation duration intentionally** (`SetDuration` or `Run(duration)`) to match timestep plans.
4. **Initialize loggers early** before creating models and transitions.
5. **Validate matrix dimensions and ranges** before adding matrices.

## Common Patterns

### Running Multiple Independent Simulations

```cpp
for (int run = 0; run < num_runs; ++run) {
    respond::RuntimeConfig config;
    config.logging.logger_name = "logger_" + std::to_string(run);
    config.logging.file_path = "run_" + std::to_string(run) + ".log";
    respond::Simulation sim(config);

    auto model = respond::Model::Create("markov", config);
    // Configure model...
    
    sim.AddModel(model);
    sim.Run(duration);
    
    // Store results...
}
```

### Resetting Model State

```cpp
// To reset a model to initial state
Eigen::VectorXd initial_state = ...;
model->SetState(initial_state);

// To also clear history and timesteps
model->ClearTimesteps();
model->CreateDefaultHistories();
```

### Copying Simulations

```cpp
respond::RuntimeConfig config;
config.logging.logger_name = "logger";
respond::Simulation sim1(config);
// ... configure sim1 ...

// Create independent copy
respond::Simulation sim2 = sim1;  // All models are cloned

// Modifications to sim2 don't affect sim1
```

## Parallel Execution with Shared Logging

When running multiple models in parallel, all loggers can safely write to the same file using RESPOND's shared sink functionality. This ensures thread-safe logging without file corruption.

### Basic Parallel Logging Setup

```cpp
#include <respond/respond.hpp>
#include <thread>
#include <vector>

int main() {
    // Configure shared logging (all loggers write to the same file)
    respond::ConfigureLogger({"model_1", "unified.log", true});
    respond::SetLogPattern(respond::LogPattern::kThreadSafe);
    respond::SetFlushInterval(0);  // Flush each log message immediately
    
    respond::ConfigureLogger({"model_2", "unified.log", true});
    respond::ConfigureLogger({"model_3", "unified.log", true});
    
    // Now multiple threads can safely write to shared log
    return 0;
}
```

### Running Models in Parallel with Unified Logging

```cpp
#include <respond/respond.hpp>
#include <thread>
#include <vector>

void RunSimulation(int id, const std::string& log_file) {
    std::string logger_name = "model_" + std::to_string(id);
    
    respond::RuntimeConfig config;
    config.logging = {logger_name, log_file, true};

    // Create and run simulation
    respond::Simulation sim(config);
    auto model = respond::Model::Create("markov", config);
    
    // Configure model...
    Eigen::VectorXd initial_state = Eigen::VectorXd::Zero(50);
    initial_state(0) = 1000;
    model->SetState(initial_state);
    
    // Add timesteps...
    sim.AddModel(model);
    
    // Run simulation
    sim.Run(52);
    
    // Flush logs for this model
    respond::FlushAllLoggers();
}

int main() {
    // Setup shared logging once
    respond::SetLogPattern(respond::LogPattern::kThreadSafe);
    respond::SetFlushInterval(0);  // Flush immediately
    
    const int num_threads = 4;
    std::vector<std::thread> threads;
    
    // Launch parallel simulations
    for (int i = 0; i < num_threads; ++i) {
        threads.emplace_back(RunSimulation, i, "unified.log");
    }
    
    // Wait for all to complete
    for (auto& t : threads) {
        t.join();
    }
    
    // All output safely written to unified.log
    return 0;
}
```

### Shared Logger Pattern Options

The `LogPattern` enum controls log format for all shared loggers:

- **`kSimple`**: Minimal format `[logger_name] message`
- **`kStandard`**: Includes time and thread ID (default)
- **`kDetailed`**: Full timestamp with milliseconds; best for debugging
- **`kThreadSafe`**: Optimized for concurrent writes with sequence numbers

```cpp
// Change pattern anytime
respond::SetLogPattern(respond::LogPattern::kDetailed);

// Query current pattern
auto current = respond::GetLogPattern();

// Get pattern as string for programmatic use
// The active pattern is available through GetLogPattern().
```

### Monitoring Shared Loggers

```cpp
// Check if logger exists
bool exists = (respond::CheckLoggerExists("model_1") == respond::CreationStatus::kExists);

// Get detailed logger information
std::string info = respond::GetLoggerInfo("model_1");
// Returns: "Logger: model_1\n  Level: debug\n  Sinks: 1"

// Set individual logger level
respond::SetLoggerLevel("model_1", 2);  // 2 = info
// SetLoggerLevel returns void and does nothing if the logger is not found.

// Flush all loggers immediately
respond::FlushAllLoggers();
```

### Thread-Safe File Sink Management

Shared sinks are automatically cached and reused by `ConfigureLogger` when
`LoggingConfig::use_shared_sink` is `true`:

```cpp
respond::ConfigureLogger({"logger_1", "logs/simulation.log", true});
respond::ConfigureLogger({"logger_2", "logs/simulation.log", true});

// Both loggers use the cached sink for the same path.
// Both logger_1 and logger_2 write to same file safely
```

The legacy `CreateSharedFileSink` and `CreateSharedLogger` functions remain
available for existing code. New code should use `ConfigureLogger` so the
destination is explicit. Reusing a logger name with the same destination
returns `CreationStatus::kExists`; using a different destination returns
`CreationStatus::kError`.

### Best Practices for Parallel Logging

1. **Call `SetLogPattern()` once** at program startup, before creating any loggers
2. **Set `LoggingConfig::use_shared_sink` to `true`** when parallel loggers should write to one file
3. **Use `kThreadSafe` pattern** when logs will have high concurrent write volume
4. **Set `FlushInterval(0)`** when each message must be flushed immediately; positive values are currently reserved and do not enable periodic flushing
5. **Call `FlushAllLoggers()`** at end of main before exit to ensure all writes complete
6. **Monitor logger levels** with `GetLoggerInfo()` when debugging multi-model runs

### Troubleshooting

- **Assertion failures**: Ensure matrix dimensions match state vector size before adding to transitions
- **Empty histories**: Call `CreateDefaultHistories()` after model setup or manually add histories
- **Logger errors**: Ensure each logger name has one consistent destination; use
    `ConfigureLogger` with the intended `LoggingConfig`
- **Memory issues**: Verify no circular unique_ptr references; models own transitions

For more information, see the [Doxygen-generated API documentation](../doxygen/html/index.html) or the [Architecture and Design guide](architecture.md).

Previous: [Architecture and Design](architecture.md)

Next: [Data Guide](data.md)
