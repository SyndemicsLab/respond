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
        +Simulation()
        +Simulation(const string &)
        +Simulation(const string &, const string &)
        +Simulation(const Simulation &)
        +operator=(const Simulation &) Simulation&
        +Simulation(Simulation &&other)
        +operator=(Simulation &&) Simulation&
        +CreateNewModel(const string &) unique_ptr~Model~
        +ClearModels()
        +AddModel(const unique_ptr~Model~)
        +Run(int=-1)
        +operator[](size_t idx)
        +operator[](size_t idx) const
        +GetModels() const vector~unique_ptr~Model~~
        +GetModel(int model_idx) const unique_ptr~Model~
        +GetModelIndexNameMap() const map~size_t, string~
        +GetModelNames() vector~string~
        +GetModelHistory(size_t model_idx) const map~string, History~ &
        +GetModelHistoryNames(size_t idx) vector~string~
        +SetDuration(int)
        +operator<<(ostream &os, const Simulation &obj) ostream &
    }

    class Model {
        <<abstract>>
        +Create(const string &, const string &, const string &) unique_ptr~Model~
        +clone() unique_ptr~Model~ *
        +AddTimestep(const Timestep &) *
        +RunTimestep() *
        +RunTimestep(size_t) *
        +RunTimesteps() *
        +ClearTimesteps() *
        +ClearHistories() *
        +CreateDefaultHistories() *
        +GetTimestepAtIndex(size_t) Timestep *
        +GetState() Ref~const VectorXd~ *
        +GetName() string *
        +GetHistories() map~string, History~ *
        +GetTimestep() int *
        +GetHistoryCaptureInterval() int *
        +GetFinalTimestep() int *
        +GetInitialHistoryRecorded() bool *
        +SetState(const Ref~const VectorXd~ &) *
        +SetHistoryCaptureInterval(int) *
        +SetFinalTimestep(int) *
        +SetInitialHistoryRecorded(bool) *
        +Serialize(ostream &) *
        +operator<<(ostream &os, const Model &obj) ostream &
    }

    class Timestep {
        +Timestep()
        +Timestep(const string &)
        +Timestep(const string &, const string &)
        +Timestep(const Timestep &)
        +operator=(const Timestep &) Timestep &
        +Timestep(const Timestep &&)
        +operator=(const Timestep &&) Timestep &
        +CreateTransition(const string &) const unique_ptr~Transition~ &
        +AddTransition(const unique_ptr~Transition~ &)
        +RemoveTransition(size_t) unique_ptr~Transition~
        +AddMatrixToTransition(const size_t &, const Ref~const MatrixXd~ &)
        +AddMatrixToTransition(const string &, const Ref~const MatrixXd~ &)
        +GetTransition(const size_t &) const unique_ptr~Transition~ &
        +GetTransition(const string &) const unique_ptr~Transition~ &
        +GetTransitions() vector~unique_ptr~Transition~~
        +GetTransitionNames() vector~string~
        +operator[](size_t) TransitionSlotProxy
        +operator[](size_t) const Transition &
        +operator<<(ostream &os, const Timestep &obj) ostream &
        +operator==(const Timestep &, const Timestep &) bool
        +operator!=(const Timestep &, const Timestep &) bool
    }

    class Transition {
        <<abstract>>
        +Execute(const Ref Vector &, map~string, History~ &) VectorXd *
        +AddMatrix(Eigen::Ref~const MatrixXd~) *
        +GetMatrices() vector~MatrixXd~ *
        +GetName() string *
        +ClearMatrices() *
        +clone() unique_ptr~Transition~ *
        +Create(const string &, const string &, const string &, const string &) unique_ptr~Transition~
        +Serialize(ostream &) *
        +operator<<(ostream &, const Transition &) ostream &
    }

    class History {
        +History()
        +History(const string &)
        +History(const string &, const HistoryMode &)
        +History(const string &, const HistoryMode &, const string &)
        +History(const string &, const string &)
        +History(const string &, const string &, const string &)
        +History(const string &, const HistoryMode &, const string &, const string &)
        +History(const History &)
        +operator=(const History &) History &
        +History(History &&)
        +operator=(History &&) History &
        +AddState(const Ref~const VectorXd~ &, int)
        +AccumulateState(const Ref~const VectorXd~ &)
        +FlushPendingState(int, Index)
        +Clear()
        +HasPendingState() bool
        +GetStateMap() map~int, VectorXd~
        +GetRecordedTimesteps() const vector~int~ &
        +GetRecordedStates() const vector~VectorXd~ &
        +GetHistoryMode() HistoryMode
        +GetPendingState() VectorXd
        +GetLatestRecordedTimestep() int
        +GetName() string
        +GetStateAsVector() vector~VectorXd~
        +operator==(const History &) bool
        +operator!=(const History &) bool
        +operator<<(ostream &os, const History &obj) ostream &
        -GetNextTimestep() int
        -GetZeroVector(const int &) VectorXd
    }

    class HistoryMode {
        <<enumeration>>
        kSnapshot
        kAccumulated
    }

    class LoggingAPI {
        <<utility>>
        +CreateFileLogger(const string &, const string &)
        +CreateSharedFileSink(const string &)
        +CreateSharedLogger(const string &)
        +SetLogPattern(LogPattern)
        +GetLogPattern() LogPattern
        +SetFlushInterval(int)
        +FlushAllLoggers()
        +LogInfo(const string &, const string &)
        +LogWarning(const string &, const string &)
        +LogError(const string &, const string &)
        +LogDebug(const string &, const string &)
        +CheckLoggerExists(const string &)
        +GetLoggerInfo(const string &)
        +SetLoggerLevel(const string &, int)
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
        -vector~unique_ptr~Model~~ _models
        -int _duration
        -vector~int~ _parameter_change_times
        -bool _stratify_entering_cohort
        -bool _build_summary_stats
        -bool _save_state_history
        -vector~int~ _timesteps_to_report
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
        -vector~Timestep~ _timestep_vector
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
        +AddTimestep(const Timestep &) override
        +GetTimestepAtIndex(size_t) Timestep override
        +ClearTimesteps() override
        +RunTimestep() override
        +RunTimestep(size_t) override
        +RunTimesteps() override
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
        +GetMatrices() const vector~MatrixXd~
        +AddMatrix(const Eigen::Ref~const MatrixXd~ &) override
        +GetName() string override
        +ClearMatrices() override
        +Serialize(ostream &) override
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

    Markov *-- "0..*" Timestep : owns
    Timestep *-- "0..*" Transition : owns
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
