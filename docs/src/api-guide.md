# C++ API Guide

This guide provides an overview of the RESPOND C++ API for developers wishing to use the library in their own projects.

## Overview

The RESPOND library provides a flexible framework for building opioid use disorder models through composition of models, transitions, and history tracking. The core components are:

- **Model**: Abstract base class representing a state transition system
- **Simulation**: Aggregates and coordinates multiple models
- **Transition**: Abstract base for specific transition types
- **History**: Tracks state vectors over time
- **TransitionFactory**: Creates concrete transition instances

## Core Concepts

### State Vectors

Models operate on state vectors (Eigen::VectorXd) representing the population distribution across model states. A state vector element at index i represents the count of individuals in state i.

### Transitions

Transitions apply transformations to state vectors using transition matrices. The RESPOND model supports several transition types:

- **Migration**: Population movement between states
- **Behavior**: Behavioral state changes
- **Intervention**: Intervention-driven state changes
- **Overdose**: Overdose-related transitions
- **BackgroundDeath**: Background mortality transitions

### History Tracking

History objects record state vectors at each timestep, enabling analysis of state trajectories over time. Histories support sparse timesteps—gaps are automatically filled with zero vectors.

## Model Class

The Model class is the abstract base for all models in RESPOND.

```cpp
#include <respond/model.hpp>

// Create a model
auto model = respond::Model::Create("model_name", "logger_name");

// Set the initial state
Eigen::VectorXd initial_state(50);
initial_state.setZero();
model->SetState(initial_state);

// Add transitions
auto transition = respond::TransitionFactory::CreateTransition("behavior", "logger_name");
transition->AddTransitionMatrix(some_matrix);
model->AddTransition(transition);

// Execute one simulation step
model->RunTransitions();

// Retrieve current state
Eigen::VectorXd current_state = model->GetState();

// Access history records
auto histories = model->GetHistories();
```

### Key Methods

- `SetState(const Eigen::VectorXd &state)`: Sets the model's state vector (copied internally)
- `GetState() const`: Returns a copy of the current state
- `RunTransitions()`: Executes all registered transitions
- `AddTransition(const std::unique_ptr<Transition> &t)`: Adds a transition (assumes ownership)
- `GetTransitionNames() const`: Returns names of all transitions
- `ClearTransitions()`: Removes all transitions
- `GetHistories() const`: Returns map of history name to History objects
- `CreateDefaultHistories()`: Initializes default history tracking
- `SetHistories(const std::map<std::string, History> &h)`: Sets history records
- `GetModelName() const`: Returns model name
- `GetLogName() const`: Returns associated logger name
- `clone() const`: Creates a deep copy of the model

## Simulation Class

The Simulation class manages multiple models and coordinates their execution.

```cpp
#include <respond/simulation.hpp>

// Create a simulation
respond::Simulation sim("my_logger");

// Add models
auto model1 = respond::Model::Create("model1", "my_logger");
auto model2 = respond::Model::Create("model2", "my_logger");
sim.AddModel(model1);
sim.AddModel(model2);

// Run one step (executes all model transitions)
sim.Run();

// Retrieve results
auto all_histories = sim.GetModelHistories();
auto model_names = sim.GetModelNames();

// Get detailed history mapping
auto history_names = sim.GetModelHistoryNames();
// Returns vector of (model_name, history_name) pairs
```

### Key Methods

- `Run()`: Executes one simulation step for all models
- `AddModel(const std::unique_ptr<Model> &model)`: Adds a model (cloned internally)
- `GetModels() const`: Returns const reference to model vector
- `GetModelNames() const`: Returns all model names
- `ClearModels()`: Removes all models
- `GetModelHistories() const`: Returns state histories for all models
- `GetModelHistoryNames() const`: Returns (model_name, history_name) pairs
- `GetLogName() const`: Returns logger name

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
std::string name = hist.GetHistoryName();
std::string log_name = hist.GetLogName();

// Clear history
hist.Clear();
```

### Key Methods

- `AddState(const Eigen::VectorXd &state, int timestep = -1)`: Records a state
  - If timestep < 0, automatically assigns next available timestep
  - If timestep already exists, currently overwrites
- `GetStateMap() const`: Returns map of timestep → state vector
- `GetStateAsVector() const`: Returns contiguous vector of states (fills gaps with zeros)
- `GetHistoryName() const`: Returns history identifier
- `GetLogName() const`: Returns logger name
- `Clear()`: Removes all recorded states
- `operator==`, `operator!=`: Comparison operators

## Transition Class

The Transition class is abstract; use TransitionFactory to create concrete instances.

```cpp
#include <respond/transition.hpp>
#include <respond/transition_factory.hpp>

// Create a transition using the factory
auto transition = respond::TransitionFactory::CreateTransition(
    "behavior",  // Type: migration, behavior, intervention, overdose, background_death
    "my_logger"  // Logger name
);

// Add transformation matrices
Eigen::MatrixXd trans_matrix = ...;
transition->AddTransitionMatrix(trans_matrix);

// Execute the transition (typically done via Model::RunTransitions)
auto histories_map = ...; // From model
Eigen::VectorXd result = transition->Execute(current_state, histories_map);

// Get transition properties
std::string name = transition->GetTransitionName();
std::string log = transition->GetLogName();

// Clear matrices
transition->ClearTransitionMatrices();
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

RESPOND uses the spdlog library for logging. Models and transitions accept a logger name:

```cpp
// All logging is handled by passing logger names
auto model = respond::Model::Create("my_model", "my_logger");

// The model will use this logger for any errors or warnings
// Create loggers separately using respond::CreateFileLogger
respond::CreateFileLogger("my_logger", "path/to/logfile.log");
```

## Complete Example

```cpp
#include <respond/simulation.hpp>
#include <respond/model.hpp>
#include <respond/transition_factory.hpp>
#include <respond/logging.hpp>

int main() {
    // Create logger
    respond::CreateFileLogger("app", "simulation.log");

    // Create simulation
    respond::Simulation sim("app");

    // Create and configure a model
    auto model = respond::Model::Create("population_model", "app");
    
    // Set initial state (e.g., 1000 individuals across 50 states)
    Eigen::VectorXd initial_state = Eigen::VectorXd::Zero(50);
    initial_state(0) = 1000;  // All in first state
    model->SetState(initial_state);

    // Add transitions
    auto behavior_transition = respond::TransitionFactory::CreateTransition(
        "behavior", "app");
    // Add matrices...
    model->AddTransition(behavior_transition);

    auto migration_transition = respond::TransitionFactory::CreateTransition(
        "migration", "app");
    // Add matrices...
    model->AddTransition(migration_transition);

    // Add model to simulation
    sim.AddModel(model);

    // Run simulation for 52 timesteps
    for (int t = 0; t < 52; ++t) {
        sim.Run();
    }

    // Extract results
    auto histories = sim.GetModelHistories();
    auto history_names = sim.GetModelHistoryNames();
    
    // Process results...
    
    return 0;
}
```

## Memory Management

RESPOND uses `std::unique_ptr` for ownership management:

- Models and Transitions are typically managed by Simulation or parent objects
- History objects are copyable and can be freely copied
- All models are cloned when added to a Simulation (ownership transfer)
- Clearing containers (ClearModels, ClearTransitions) deletes contained objects

## Best Practices

1. **Use TransitionFactory** to create transitions—it handles type dispatch
2. **Let Simulation manage models** for automatic cloning and lifecycle management
3. **Reuse History objects** for multiple runs to accumulate results
4. **Use const references** where available (GetState returns a copy for safety)
5. **Initialize loggers early** before creating models to enable error tracking
6. **Validate matrix dimensions** before adding to transitions (not checked by API)

## Common Patterns

### Running Multiple Independent Simulations

```cpp
for (int run = 0; run < num_runs; ++run) {
    respond::Simulation sim("logger_" + std::to_string(run));
    
    auto model = respond::Model::Create("model", "logger_" + std::to_string(run));
    // Configure model...
    
    sim.AddModel(model);
    for (int t = 0; t < duration; ++t) {
        sim.Run();
    }
    
    // Store results...
}
```

### Resetting Model State

```cpp
// To reset a model to initial state
Eigen::VectorXd initial_state = ...;
model->SetState(initial_state);

// To also clear history
model->ClearTransitions();
model->CreateDefaultHistories();
```

### Copying Simulations

```cpp
respond::Simulation sim1("logger");
// ... configure sim1 ...

// Create independent copy
respond::Simulation sim2 = sim1;  // All models are cloned

// Modifications to sim2 don't affect sim1
```

## Parallel Execution with Shared Logging

When running multiple models in parallel, all loggers can safely write to the same file using RESPOND's shared sink functionality. This ensures thread-safe logging without file corruption.

### Basic Parallel Logging Setup

```cpp
#include <respond/logging.hpp>
#include <respond/model.hpp>
#include <thread>
#include <vector>

int main() {
    // Configure shared logging (all loggers write to same file)
    respond::SetLogPattern(respond::LogPattern::kThreadSafe);
    respond::SetFlushInterval(3);  // Auto-flush every 3 seconds
    
    // Create multiple loggers that share the same file sink
    respond::CreateSharedLogger("model_1");
    respond::CreateSharedLogger("model_2");
    respond::CreateSharedLogger("model_3");
    
    // Now multiple threads can safely write to shared log
    return 0;
}
```

### Running Models in Parallel with Unified Logging

```cpp
#include <respond/logging.hpp>
#include <respond/simulation.hpp>
#include <thread>
#include <vector>

void RunSimulation(int id, const std::string& log_file) {
    std::string logger_name = "model_" + std::to_string(id);
    
    // Create logger that uses shared sink
    respond::CreateSharedLogger(logger_name);
    
    // Create and run simulation
    respond::Simulation sim(logger_name);
    auto model = respond::Model::Create("model", logger_name);
    
    // Configure model...
    Eigen::VectorXd initial_state = Eigen::VectorXd::Zero(50);
    initial_state(0) = 1000;
    model->SetState(initial_state);
    
    // Add transitions...
    sim.AddModel(model);
    
    // Run simulation
    for (int t = 0; t < 52; ++t) {
        sim.Run();
    }
    
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
std::string pattern_str = respond::LoggingConfig::GetPatternString(current);
```

### Monitoring Shared Loggers

```cpp
// Check if logger exists
bool exists = (respond::CheckLoggerExists("model_1") == respond::CreationStatus::kExists);

// Get detailed logger information
std::string info = respond::GetLoggerInfo("model_1");
// Returns: "Logger: model_1\n  Level: debug\n  Sinks: 1"

// Set individual logger level
respond::SetLoggerLevel("model_1", spdlog::level::info);

// Flush all loggers immediately
respond::FlushAllLoggers();
```

### Thread-Safe File Sink Management

The `CreateSharedFileSink` function creates file sinks that are automatically cached and reused:

```cpp
// Create or get cached sink for filepath
auto sink = respond::CreateSharedFileSink("logs/simulation.log");
// If called again with same path, returns existing sink (no duplicate file handles)

// Multiple loggers using same sink (no file conflicts)
respond::CreateSharedLogger("logger_1");  // Uses default sink
respond::CreateSharedLogger("logger_2");  // Uses same sink
// Both logger_1 and logger_2 write to same file safely
```

### Best Practices for Parallel Logging

1. **Call `SetLogPattern()` once** at program startup, before creating any loggers
2. **Call `CreateSharedLogger()` instead of `CreateFileLogger()`** when using parallel execution
3. **Use `kThreadSafe` pattern** when logs will have high concurrent write volume
4. **Set `FlushInterval(0)`** for critical logging; use `FlushInterval(3-5)` for performance
5. **Call `FlushAllLoggers()`** at end of main before exit to ensure all writes complete
6. **Monitor logger levels** with `GetLoggerInfo()` when debugging multi-model runs

### Troubleshooting

- **Assertion failures**: Ensure matrix dimensions match state vector size before adding to transitions
- **Empty histories**: Call `CreateDefaultHistories()` after model setup or manually add histories
- **Logger errors**: Ensure logger names exist (create with `CreateFileLogger` if needed)
- **Memory issues**: Verify no circular unique_ptr references; models own transitions

For more information, see the [Doxygen-generated API documentation](../doxygen/html/index.html) or the [Architecture and Design guide](architecture.md).

Previous: [Architecture and Design](architecture.md)

Next: [Data Guide](data.md)
