# Architecture and Design

This document describes the architectural decisions and design patterns used in RESPOND.

## Design Philosophy

RESPOND follows the **inversion of control** principle, abstracting the model to its core components and allowing users to customize it to their needs rather than maintaining a rigid, monolithic structure. This enables:

1. **Extensibility** - New transition types can be added without modifying core code
2. **Testability** - Components can be tested independently
3. **Maintainability** - Clear separation of concerns
4. **Portability** - Easy to integrate into different applications

## Component Architecture

```
┌─────────────────────────────────────────────────┐
│         Simulation                               │
│  (aggregates and coordinates Models)             │
└──────────────────┬──────────────────────────────┘
                   │
        ┌──────────┴──────────┬──────────────┐
        │                     │              │
    ┌───▼────┐           ┌───▼────┐     ┌──▼────┐
    │ Model  │           │ Model  │     │ Model │
    │ (PopA) │           │ (PopB) │     │(PopC) │
    └───┬────┘           └───┬────┘     └──┬────┘
        │                     │             │
        ├─ Transitions ────┐  │             │
        │   - Migration    │  │             │
        │   - Behavior     │  │             │
        │   - Intervention │  │             │
        │   - Overdose     │  │             │
        │   - Background   │  │             │
        └──────────────────┘  │             │
        │                     │             │
        └─ Histories ────┐    │             │
            - State      │    │             │
            - Outcomes   │    │             │
            - Costs      │    │             │
            └────────────┘    └─────────────┘
```

## Core Classes

### Model (Abstract Base Class)

- **Role**: Represents a state transition system
- **Responsibilities**:
  - Manages state vector
  - Owns and executes transitions
  - Tracks history
  - Provides cloning capability
- **Key Design Decisions**:
  - Non-copyable by assignment (enforces `clone()` usage for clarity)
  - Owns transitions (unique_ptr for memory safety)
  - Read-only GetState() (returns copy to prevent external state modification)

### Simulation

- **Role**: Aggregates multiple independent models
- **Responsibilities**:
  - Coordinates model execution
  - Collects results from all models
  - Manages simulation-level state
- **Key Design Decisions**:
  - Clones models on addition (ownership clarity)
  - Copyable (deep copy semantics)
  - Provides convenient result collection methods

### Transition (Abstract Base Class)

- **Role**: Represents a specific type of model transition
- **Responsibilities**:
  - Applies state transformations
  - Updates history records
  - Manages transformation matrices
- **Implementation Types**:
  - **Migration**: Population movement between states
  - **Behavior**: Behavioral state transitions
  - **Intervention**: Intervention effects
  - **Overdose**: Overdose dynamics
  - **BackgroundDeath**: Mortality transitions
- **Key Design Decisions**:
  - Non-copyable (prevents accidental duplication of stateful transformations)
  - Uses TransitionFactory for creation (encapsulates type selection)
  - Const-correct Execute() (doesn't modify transition state)

### History

- **Role**: Records state vectors over time
- **Responsibilities**:
  - Sparse timestep tracking
  - State retrieval (by index or as vector)
  - Comparison operations
- **Key Design Decisions**:
  - Copyable (lightweight data container)
  - Sparse internal storage (efficient memory for gaps)
  - Auto-fills gaps with zeros (simplifies downstream analysis)
  - Map-based storage (allows non-sequential timesteps)

### TransitionFactory

- **Role**: Creates concrete Transition instances
- **Responsibilities**:
  - Encapsulates type dispatch logic
  - Provides single point of extensibility for new transitions
- **Key Design Decisions**:
  - Static factory method (no factory state needed)
  - String-based type identification (simple, extensible)
  - Case-insensitive type matching (user-friendly)

## Design Patterns

### Factory Pattern (TransitionFactory)

Encapsulates object creation for transitions:

```cpp
auto transition = TransitionFactory::CreateTransition("behavior", "logger");
```

**Benefits**:
- Decouples transition creation from usage
- Centralizes type dispatch logic
- Easy to add new transition types

### Template Method Pattern (Model → Transitions)

Model delegates to transitions in RunTransitions():

```cpp
void Model::RunTransitions() {
    for (const auto& transition : _transitions) {
        _state = transition->Execute(_state, _histories);
    }
}
```

**Benefits**:
- Flexible transition composition
- Order-dependent execution
- Custom transition behavior per model

### Strategy Pattern (Transitions)

Different transition types implement Execute() differently:

```cpp
// Migration::Execute() - handles population movement
// Behavior::Execute() - handles behavior changes
// etc.
```

**Benefits**:
- Runtime selection of transition algorithms
- No conditional logic in Model
- Easy to add new strategies

### Object Pool / Clone Pattern

Models and Transitions support cloning for independent copies:

```cpp
auto model_copy = model->clone();  // Deep copy
auto transition_copy = transition->clone();  // Deep copy
```

**Benefits**:
- Explicit control over deep vs. shallow copies
- Clear ownership semantics
- Safe concurrent execution

## Memory Management

RESPOND uses modern C++ memory management practices:

### Unique Ownership (unique_ptr)

Used for objects with clear ownership:
- Model owns its Transitions
- Simulation owns its Models (via cloning)

### Shared Ownership (None by default)

RESPOND minimizes shared state. History objects are the exception—they're:
- Copyable (value semantics)
- Used as values in Model maps
- Accessed through const references where possible

### Safety Mechanisms

- **Deleted copy operators** on base classes prevent slicing:
  ```cpp
  Model(const Model&) = delete;
  Model& operator=(const Model&) = delete;
  virtual unique_ptr<Model> clone() const = 0;  // Force explicit cloning
  ```
- **const-correctness** throughout API
- **Pass-by-value** for small objects (Eigen uses move semantics internally)

## Extensibility Points

### Adding a New Transition Type

1. Create a new header in `include/respond/internals/`
2. Implement concrete Transition subclass
3. Add factory entry in `TransitionFactory::CreateTransition()`

Example:
```cpp
// respond/internals/custom_transition.hpp
class CustomTransition : public Transition {
public:
    static std::unique_ptr<Transition> Create(...);
    // ... implement virtual methods
};

// In transition_factory.cpp
if (type == "custom") {
    return CustomTransition::Create(type, log_name);
}
```

### Adding New History Types

1. Extend History class or create a new class
2. Update Model::CreateDefaultHistories() to instantiate new types
3. Update GetHistories() documentation

## Dependencies

### External Libraries

- **Eigen** - Linear algebra (state vectors, transition matrices)
- **spdlog** - Logging
- **GoogleTest** - Unit testing (optional)

### Internal Organization

- **`include/respond/`** - Public API headers
- **`src/internals/`** - Internal implementation headers
- **`src/`** - Implementation files
- **`tests/`** - Unit and integration tests

## Threading and Concurrency

Current implementation is **not thread-safe**:

- No internal locking mechanisms
- State modification is not atomic
- Multiple simulations can run independently (each with own state)

For concurrent execution:
- Create separate Simulation instances
- Each thread manages its own simulation
- Synchronize result collection externally

## Performance Considerations

### State Vector Operations

- Heavy use of Eigen for linear algebra
- Matrices stored by value (memory efficient)
- Copy elision via move semantics

### Sparse History

- Map-based storage avoids allocating full history
- Gap-filling only occurs during GetStateAsVector()
- Minimal memory overhead for sparse timesteps

### Transition Execution Order

- Transitions execute sequentially in order added
- Each transition reads from current state, writes results
- History updated after each transition

## Validation and Error Handling

### Current Approach

- Minimal runtime validation
- Relies on preconditions (documented in comments)
- Errors logged through spdlog

### Improvements for Future Versions

- Add matrix dimension validation
- Validate state vector sizes
- Stricter type checking in factory

## Testing Strategy

### Unit Tests

Located in `tests/unit/`, testing individual components:
- State management
- Transition execution
- History recording
- Factory creation

### Integration Tests

Located in `tests/integration/`, testing end-to-end scenarios:
- Full simulation execution
- Multi-model coordination
- Result aggregation

### Mock Objects

`tests/mocks/` provides:
- Model mock for testing Simulation
- Transition mock for testing Model

## Future Architectural Considerations

1. **Async Execution** - Enable parallel model execution
2. **Plugin System** - Dynamic loading of transitions
3. **Serialization** - Save/restore simulation state
4. **Validation Framework** - Compile-time and runtime checks
5. **Performance Profiling** - Built-in timing/statistics

---

For more implementation details, refer to the [Doxygen documentation](../doxygen/html/index.html) and source code comments.

Previous: [FAQs](faq.md)

Next: [API Guide](api-guide.md)
