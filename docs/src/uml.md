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
        +CreateNewModel(type) shared_ptr~Model~
        +AddNewModel(shared_ptr~Model~) bool
        +Run()
        +GetModel(size_t model_idx) const Model &
        +GetModelNames() vector~string~
        +GetModelHistory(size_t model_idx) map~string, History~
        +operator<<(ostream &os, const Simulation &obj) ostream &
    }

    class Model {
        <<abstract>>
        +SetState(state) *
        +GetState() VectorXd *
        +AddTimestep(shared_ptr~timestep~) *
        +GetTimesteps() *
        +RunTransitions() *
        +ClearTimesteps() *
        +GetHistories() map~string, History~ *
        +ClearHistories() *
        +CreateDefaultHistories() *
        +SetFinalTimestep(final_timestep) *
        +clone() unique_ptr~Model~ *
        +Create(name, log_name) unique_ptr~Model~
        +operator<<(ostream &os, const Model &obj) ostream &
    }

    class Timestep {
        +Timestep()
        +Timestep(const string &log_name)
        +Timestep(const string &log_name, const string &log_filepath)
        +Timestep(const Timestep &other)
        +operator=(const Timestep &other) Timestep &
        +Timestep(const Timestep &&other)
        +operator=(const Timestep &&other) Timestep &
        +CreateTransition(type) const Transition &
        +AddMatrixToTransition(size_t index, MatrixXd mat)
        +GetTransition(size_t idx) const Transition &
        +GetTransition(string name) const Transition &
        +GetTransitions() const vector~const Transition &~
        +GetTransitionNames() vector~const string~
        +operator<<(ostream &os, const Timestep &obj) ostream &
    }

    class Transition {
        <<abstract>>
        +Execute(state, histories) VectorXd *
        +AddMatrix(const Eigen::Ref~const MatrixXd~ &matrix, size_t idx) *
        +GetMatrix(size_t idx) Eigen::Ref~MatrixXd~ *
        +GetName() string *
        +ClearMatrices() *
        +clone() unique_ptr~Transition~ *
        +Create(type, log_name) unique_ptr~Transition~
        +operator<<(ostream &os, const Transition &obj) ostream &
    }

    class History {
        +AddState(state, timestep)
        +RecordSnapshot(state, timestep)
        +AccumulateState(state)
        +FlushPendingState(timestep, state_size)
        +GetStateMap() map~int, VectorXd~
        +GetStateAsVector() vector~VectorXd~
        +operator<<(ostream &os, const History &obj) ostream &
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

    Simulation *-- "0..*" Model : owns
    Model *-- "0..*" Timestep : owns
    Timestep *-- "0..*" Transition : owns

    Model *-- "0..*" History : owns

    Simulation ..> LoggingAPI: uses
    LoggingAPI ..> LogType : uses
    LoggingAPI ..> CreationStatus : returns
    LoggingAPI ..> LogPattern : configures
    History ..> HistoryMode : uses
    CostEffectiveness ..> History : consumes
```

## Internals Diagram

This diagram is implementation-focused and shows the relationship between
abstract classes and how it impacts the stored components for ownership
purposes.

```mermaid
classDiagram
    direction LR

    class Simulation {
        -string _log_name
        -vector~shared_ptr~Model~~ _models
        -size_t _duration
        -vector~size_t~ _parameter_change_times
        -bool _stratify_entering_cohort
        -bool _build_summary_stats
        -bool _save_state_history
        -vector~size_t~ _timesteps_to_report
        -bool _pivot_long
        +Simulation()
        +Simulation(const string log_name)
        +Simulation(const string log_name, const string log_filepath)
        +Simulation(const Simulation &other)
        +operator=(const Simulation &other) Simulation &
        +Simulation(const Simulation &&other)
        +operator=(const Simulation &&other) Simulation &
    }

    class Model {
        <<abstract>>
    }

    class Markov {
        -vector~shared_ptr~Transition~~ _transition_vector
        -VectorXd _state
        -string _name
        -string _log_name
        -map~string, History~ _histories
        -int _current_timestep
        -int _history_capture_interval
        -int _final_timestep
        -bool _initial_history_recorded
        -ResetHistoryTracking()
        -GetLatestRecordedTimestep()
        -ShouldRecordHistoryAtTimestep(int timestep)
        -RecordHistoryAtCurrentTimestep()
        -SetupHistory()
        +Markov()
        +Markov(const string &name, const string log_name)
        +Markov(Markov &other)
        +operator=(Markov &other) Markov &
        +Markov(Markov &&other)
        +operator=(Markov &&other) Markov &
        +SetState(state) override
        +GetState() VectorXd override
        +AddTimestep(shared_ptr~timestep~) override
        +GetTimesteps() override
        +ClearTimesteps() override
        +RunTransitions() override
        +GetHistories() map~string, History~ override
        +ClearHistories() override
        +CreateDefaultHistories() override
        +SetFinalTimestep(final_timestep) override
        +clone() unique_ptr~Model~ override
    }

    class Transition {
        <<abstract>>
    }

    class TransitionBase {
        <<abstract>>
        -string _name
        -string _log_name
        -vector~MatrixXd~ _transition_matrices
        -GetMatrices() const vector<MatrixXd> &
        +AddMatrix(const Eigen::Ref~const MatrixXd~ &matrix, size_t idx) override
        +GetName() string override
        +ClearMatrices() override
        +GetLogName() string override
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

    Simulation *-- "0..*" Model : owns
    Model <|-- Markov : implements

    Transition <|-- TransitionBase : extends
    TransitionBase <|-- Migration : implements
    TransitionBase <|-- Behavior : implements
    TransitionBase <|-- Intervention : implements
    TransitionBase <|-- Overdose : implements
    TransitionBase <|-- BackgroundDeath : implements

    Markov *-- "0..*" Transition : owns
    Markov *-- "0..*" History : owns
    Migration <.. History : uses
    Behavior <.. History : uses
    Intervention <..> History : uses and updates
    Overdose <..> History : uses and updates
    BackgroundDeath <..> History : uses and updates



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
