# UML Diagrams

This page uses Mermaid so diagrams remain human-editable in plain text.

## Public API Diagram

This diagram is the external library view and reflects the target workflow:
one Simulation owns many Models, each Model owns many Timesteps, each
Timestep owns many Transitions.

```mermaid
classDiagram
    direction LR

    class Simulation {
        +CreateNewModel(model_name) string
        +AddModel(model)
        +Run()
        +GetModels() vector~unique_ptr~Model~~
        +GetModelNames() vector~string~
        +GetModelHistories()
        +GetModelSparseHistories()
    }

    class Model {
        <<abstract>>
        +SetState(state) *
        +GetState() VectorXd *
        +AddTimestep(timestep) *
        +GetTimesteps() 
        +RunTransitions() *
        +GetHistories() map~string, History~ *
        +SetHistories(h) *
        +CreateDefaultHistories() *
        +SetFinalTimestep(final_timestep) *
        +clone() unique_ptr~Model~ *
        +Create(name, log_name) unique_ptr~Model~
    }

    class Timestep {
        +AddTransition(t)
        +GetTransitions() vector~unique_ptr~Transition~~~
        +GetTransitionNames() vector~string~
    }

    class Transition {
        <<abstract>>
        +Execute(state, histories) VectorXd *
        +AddTransitionMatrix(matrix) *
        +GetTransitionName() string *
        +ClearTransitionMatrices() *
        +clone() unique_ptr~Transition~ *
    }

    class TransitionFactory {
        +CreateTransition(type, log_name) unique_ptr~Transition~
    }

    class History {
        +AddState(state, timestep)
        +RecordSnapshot(state, timestep)
        +AccumulateState(state)
        +FlushPendingState(timestep, state_size)
        +GetStateMap() map~int, VectorXd~
        +GetStateAsVector() vector~VectorXd~
    }

    class HistoryMode {
        <<enumeration>>
        Snapshot
        Accumulated
    }

    class LoggingAPI {
        <<utility>>
        +CreateFileLogger(name, filepath)
        +CreateSharedFileSink(filepath)
        +CreateSharedLogger(name)
        +SetLogPattern(pattern)
        +GetLogPattern()
        +SetFlushInterval(seconds)
        +FlushAllLoggers()
        +LogInfo(name, message)
        +LogWarning(name, message)
        +LogError(name, message)
        +LogDebug(name, message)
        +CheckLoggerExists(name)
        +GetLoggerInfo(name)
        +SetLoggerLevel(name, level)
    }

    class LogType {
        <<enumeration>>
        kInfo
        kWarn
        kError
        kDebug
    }

    class CreationStatus {
        <<enumeration>>
        kError
        kSuccess
        kExists
        kNotCreated
    }

    class LogPattern {
        <<enumeration>>
        kSimple
        kStandard
        kDetailed
        kThreadSafe
    }

    class CostEffectiveness {
        <<utility>>
        +Discount(data, discount_rate, week, is_discrete, total_weeks)
        +CwiseProduct(state, multiplier)
        +CwiseMin(state, multiplier)
        +CalculateLifeYears(history, discount, discount_rate, total_weeks)
    }

    class Version {
        <<constants>>
        RESPOND_VER_MAJOR
        RESPOND_VER_MINOR
        RESPOND_VER_PATCH
        RESPOND_VERSION
    }

    Simulation *-- "1..*" Model : owns
    Model *-- "1..*" Timestep : owns (target API)
    Timestep *-- "1..*" Transition : owns

    Model o-- "0..*" History : records
    Transition ..> History : updates
    History --> HistoryMode : mode

    TransitionFactory ..> Transition : creates

    LoggingAPI ..> LogType : uses
    LoggingAPI ..> CreationStatus : returns
    LoggingAPI ..> LogPattern : configures

    CostEffectiveness ..> History : consumes
```

## Internals Diagram

This diagram is implementation-focused and intentionally verbose.

```mermaid
classDiagram
    direction LR

    class Model {
        <<abstract>>
        +SetState(state) *
        +GetState() VectorXd *
        +RunTransitions() *
        +AddTimestep(transitions) *
        +GetTransitionNames() vector~string~ *
        +GetHistories() map~string, History~ *
        +CreateDefaultHistories() *
        +SetHistories(h) *
        +SetFinalTimestep(final_timestep) *
        +clone() unique_ptr~Model~ *
        +Create(name, log_name) unique_ptr~Model~
    }

    class Markov {
        -vector~unique_ptr~Transition~~ _transition_vector
        -VectorXd _state
        -string _name
        -string _log_name
        -map~string, History~ _histories
        -int _current_timestep
        -int _history_capture_interval
        -int _final_timestep
        -bool _initial_history_recorded
        +RunTransitions()
        +AddTimestep(transitions)
        +GetTransitionNames() vector~string~
        +GetHistories() map~string, History~
        +clone() unique_ptr~Model~
    }

    class Transition {
        <<abstract>>
        +Execute(state, histories) VectorXd *
        +AddTransitionMatrix(matrix) *
        +GetTransitionName() string *
        +ClearTransitionMatrices() *
        +GetLogName() string *
        +clone() unique_ptr~Transition~ *
    }

    class TransitionBase {
        <<abstract>>
        -string _name
        -string _log_name
        -vector~MatrixXd~ _transition_matrices
        +AddTransitionMatrix(matrix)
        +GetTransitionName() string
        +ClearTransitionMatrices()
        +GetLogName() string
    }

    class Migration {
        +Execute(state, histories) VectorXd
        +Create(name, log_name) unique_ptr~Transition~
    }

    class Behavior {
        +Execute(state, histories) VectorXd
        +Create(name, log_name) unique_ptr~Transition~
    }

    class Intervention {
        +Execute(state, histories) VectorXd
        +Create(name, log_name) unique_ptr~Transition~
    }

    class Overdose {
        +Execute(state, histories) VectorXd
        +Create(name, log_name) unique_ptr~Transition~
    }

    class BackgroundDeath {
        +Execute(state, histories) VectorXd
        +Create(name, log_name) unique_ptr~Transition~
    }

    class TransitionFactory {
        +CreateTransition(type, log_name) unique_ptr~Transition~
    }

    class History {
        -string _name
        -string _log_name
        -HistoryMode _mode
        -vector~int~ _timesteps
        -vector~VectorXd~ _states
        -VectorXd _pending_state
        +AddState(state, timestep)
        +RecordSnapshot(state, timestep)
        +AccumulateState(state)
        +FlushPendingState(timestep, state_size)
        +GetLatestRecordedTimestep() int
        +GetStateMap() map~int, VectorXd~
        +GetStateAsVector() vector~VectorXd~
    }

    class HistoryMode {
        <<enumeration>>
        Snapshot
        Accumulated
    }

    class LoggingConfig {
        <<singleton>>
        +GetInstance() LoggingConfig
        +GetSharedSink(filepath)
        +GetPattern()
        +SetPattern(pattern)
        +GetFlushInterval()
        +SetFlushInterval(seconds)
    }

    class LogPattern {
        <<enumeration>>
        kSimple
        kStandard
        kDetailed
        kThreadSafe
    }

    Model <|-- Markov

    Transition <|-- TransitionBase
    TransitionBase <|-- Migration
    TransitionBase <|-- Behavior
    TransitionBase <|-- Intervention
    TransitionBase <|-- Overdose
    TransitionBase <|-- BackgroundDeath

    Markov *-- "0..*" Transition : executes
    Markov *-- "0..*" History : stores
    Transition ..> History : mutates
    History --> HistoryMode : mode

    TransitionFactory ..> Migration : creates
    TransitionFactory ..> Behavior : creates
    TransitionFactory ..> Intervention : creates
    TransitionFactory ..> Overdose : creates
    TransitionFactory ..> BackgroundDeath : creates

    LoggingConfig ..> LogPattern : stores
```

## Execution Flow Diagram

This sequence follows the requested external flow and execution semantics.

```mermaid
sequenceDiagram
    autonumber
    actor User
    participant Sim as Simulation
    participant Mod as Model[i]
    participant Step as Timestep[j]
    participant Tr as Transition[k]

    User->>Sim: Create Simulation
    loop For each model to add
        User->>Sim: Create/Add Model
        loop For each timestep in model
            User->>Mod: Create/Add Timestep
            loop For each transition in timestep
                User->>Step: Create/Add Transition
            end
        end
    end

    User->>Sim: Run()

    par For each Model in Simulation (parallel)
        Sim->>Mod: Run
        loop For each Timestep in Model (series)
            Mod->>Step: Execute
            loop For each Transition in Timestep (series)
                Step->>Tr: Execute(state, histories)
                Tr-->>Step: updated state/history contribution
            end
            Step-->>Mod: updated state/history
        end
        Mod-->>Sim: model complete
    end

    Sim-->>User: simulation complete
```

## Notes

- Public API diagram includes every component from headers in include/respond: simulation, model, timestep, transition, transition_factory, history, logging, cost_effectiveness, and version.
- Current implementation detail: Model::Create currently returns a Markov instance.
- Current implementation detail: TransitionFactory::CreateTransition resolves migration, behavior, intervention, overdose, and background_death.
