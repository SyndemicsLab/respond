////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-09-22                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_SIMULATION_HPP_
#define RESPOND_SIMULATION_HPP_

#include <respond/constants.hpp>
#include <respond/history.hpp>
#include <respond/logging.hpp>
#include <respond/model.hpp>
#include <respond/runtime_config.hpp>

#include <atomic>
#include <exception>
#include <map>
#include <memory>
#include <mutex>
#include <stdexcept>
#include <string>
#include <thread>
#include <utility>
#include <vector>

#include <Eigen/Dense>

namespace respond {
/// @brief Manages and executes multiple models in a coordinated simulation.
/// A Simulation aggregates Model instances and coordinates their execution,
/// maintaining history records and providing access to simulation results.
class Simulation {
public:
    /// @brief Proxy for mutable model slot access with clone-based assignment.
    class ModelSlotProxy {
    public:
        ModelSlotProxy(Simulation &owner, size_t idx)
            : _owner(&owner), _idx(idx) {}

        /// @brief Access the underlying model pointer for member access.
        Model *operator->() { return &_owner->GetModelRefOrThrow(_idx); }

        /// @brief Dereference to the underlying model.
        Model &operator*() { return _owner->GetModelRefOrThrow(_idx); }

        /// @brief Implicit conversion to underlying mutable model reference.
        operator Model &() { return _owner->GetModelRefOrThrow(_idx); }

        /// @brief Replace this slot by cloning from another proxy's model.
        ModelSlotProxy &operator=(const ModelSlotProxy &other) {
            return *this = static_cast<const Model &>(
                       other._owner->GetModelRefOrThrow(other._idx));
        }

        /// @brief Replace this slot by cloning from a model reference.
        ModelSlotProxy &operator=(const Model &model) {
            _owner->GetModelRefOrThrow(_idx);
            _owner->_models[_idx] = model.clone();
            return *this;
        }

        /// @brief Replace this slot by cloning from a model unique_ptr.
        /// @throws std::invalid_argument if model is nullptr.
        ModelSlotProxy &operator=(const std::unique_ptr<Model> &model) {
            if (!model) {
                LogError(_owner->_runtime_config.logging.logger_name,
                         "Cannot assign null model pointer to simulation "
                         "slot.");
                throw std::invalid_argument(
                    "Error attempting to assign null model pointer.");
            }
            _owner->GetModelRefOrThrow(_idx);
            _owner->_models[_idx] = model->clone();
            return *this;
        }

    private:
        Simulation *_owner;
        size_t _idx;
    };

    ////////////////////////////////////////////////////////////////////////////
    //
    // Rule of Five: Copy and Move Semantics
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Default constructor for a Simulation instance.
    /// Initializes the simulation with the default logger.
    Simulation() : Simulation(RuntimeConfig{}) {}

    /// @brief Constructs a Simulation with a specified logger.
    /// @param log_name The name of the logger to use for simulation output.
    [[deprecated("Use Simulation(RuntimeConfig) instead")]]
    Simulation(const std::string &log_name)
        : Simulation(RuntimeConfig{
              ExecutionConfig{},
              LoggingConfig{log_name, RESPOND_DEFAULT_LOG_FILE, false}}) {}

    /// @brief Constructs a Simulation with a specified logger and log file.
    /// @param log_name The name of the logger to use for simulation output.
    /// @param log_filepath The file path for the logger output.
    [[deprecated("Use Simulation(RuntimeConfig) instead")]]
    Simulation(const std::string &log_name, const std::string &log_filepath)
        : Simulation(
              RuntimeConfig{ExecutionConfig{},
                            LoggingConfig{log_name, log_filepath, false}}) {}

    /// @brief Constructs a Simulation with logger and execution settings.
    /// @param log_name The name of the logger to use for simulation output.
    /// @param log_filepath The file path for the logger output.
    /// @param execution_config Resource settings for simulation execution.
    [[deprecated("Use Simulation(RuntimeConfig) instead")]]
    Simulation(const std::string &log_name, const std::string &log_filepath,
               const ExecutionConfig &execution_config)
        : Simulation(RuntimeConfig{
              execution_config, LoggingConfig{log_name, log_filepath, false}}) {
    }

    /// @brief Constructs a Simulation with shared runtime settings.
    explicit Simulation(const RuntimeConfig &runtime_config)
        : _runtime_config(runtime_config) {
        ConfigureLogger(_runtime_config.logging);
    }

    /// @brief Virtual destructor for polymorphic cleanup.
    ~Simulation() = default;

    /// @brief Copy constructor creating an independent deep copy of the
    /// simulation. All models are cloned; modifications to the copy do not
    /// affect the original.
    /// @param other The Simulation instance to copy from.
    Simulation(const Simulation &other) {
        _runtime_config = other._runtime_config;
        for (const auto &m : other._models) {
            _models.push_back(m->clone());
        }
        _duration = other._duration;
        _parameter_change_times = other._parameter_change_times;
        _stratify_entering_cohort = other._stratify_entering_cohort;
        _build_summary_stats = other._build_summary_stats;
        _save_state_history = other._save_state_history;
        _timesteps_to_report = other._timesteps_to_report;
        _pivot_long = other._pivot_long;
    }

    /// @brief Copy assignment operator for deep copying simulation state.
    /// @param other The simulation to copy from.
    /// @return Reference to this simulation after assignment.
    Simulation &operator=(const Simulation &other) {
        if (this != &other) {
            std::vector<std::unique_ptr<Model>> models;
            for (const auto &m : other._models) {
                models.push_back(m->clone());
            }
            _runtime_config = other._runtime_config;
            _models = std::move(models);
            _duration = other._duration;
            _parameter_change_times = other._parameter_change_times;
            _stratify_entering_cohort = other._stratify_entering_cohort;
            _build_summary_stats = other._build_summary_stats;
            _save_state_history = other._save_state_history;
            _timesteps_to_report = other._timesteps_to_report;
            _pivot_long = other._pivot_long;
        }
        return *this;
    }

    /// @brief Move constructor for transferring simulation ownership.
    /// @param other The simulation to move from.
    Simulation(Simulation &&other) noexcept
        : _runtime_config(std::move(other._runtime_config)),
                    _models(std::move(other._models)),
          _duration(other._duration),
          _parameter_change_times(std::move(other._parameter_change_times)),
          _stratify_entering_cohort(other._stratify_entering_cohort),
          _build_summary_stats(other._build_summary_stats),
          _save_state_history(other._save_state_history),
                    _timesteps_to_report(std::move(other._timesteps_to_report)),
                    _pivot_long(other._pivot_long) {}

    /// @brief Move assignment operator for transferring simulation ownership.
    /// @param other The simulation to move from.
    /// @return Reference to this simulation after assignment.
    Simulation &operator=(Simulation &&other) noexcept {
        if (this != &other) {
            _runtime_config = std::move(other._runtime_config);
            _models = std::move(other._models);
            _duration = other._duration;
            _parameter_change_times = std::move(other._parameter_change_times);
            _stratify_entering_cohort = other._stratify_entering_cohort;
            _build_summary_stats = other._build_summary_stats;
            _save_state_history = other._save_state_history;
            _timesteps_to_report = std::move(other._timesteps_to_report);
            _pivot_long = other._pivot_long;
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Simulation Behavior Methods: Model Management
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Creates a new model instance and adds it to the simulation.
    /// @param model_name The name identifier for the model to create. This name
    /// is used to identify the model type and initialize it accordingly.
    /// @return A deep-copied model instance representing the newly created
    /// model.
    std::unique_ptr<Model> CreateNewModel(const std::string &model_name) {
        _models.push_back(Model::Create(model_name, _runtime_config));
        return _models.back()->clone();
    }

    /// @brief Removes all models from the simulation.
    void ClearModels() { _models.clear(); }

    /// @brief Adds a model to the simulation.
    /// The model is cloned and managed by the simulation.
    /// @param model A unique_ptr to a Model instance to add.
    void AddModel(const std::unique_ptr<Model> &model) {
        if (!model) {
            LogError(_runtime_config.logging.logger_name,
                     "Cannot add a null model to the simulation.");
            throw std::invalid_argument(
                "Error attempting to add a null model to simulation.");
        }
        _models.push_back(model->clone());
    }

    /// @brief Executes the simulation for all registered models.
    /// Models run sequentially by default. When model concurrency is enabled,
    /// independent models may run in parallel using the configured worker
    /// limit.
    /// @throws std::invalid_argument if multiple models would execute in
    /// parallel while more than one Eigen worker thread is configured.
    /// @throws Any exception raised by a model after all workers have joined.
    void Run(int duration = -1) {
        if (duration > 0) {
            _duration = duration;
        }
        LogInfo(_runtime_config.logging.logger_name,
                "Running simulation for duration of " +
                    std::to_string(_duration) + " timesteps.");
        const auto run_model = [this](const std::unique_ptr<Model> &model) {
            model->SetFinalTimestep(_duration);
            model->RunTimesteps();
        };

        const auto &execution = _runtime_config.execution;
        if (!execution.run_models_concurrently || _models.size() < 2) {
            for (const auto &model : _models) {
                run_model(model);
            }
            return;
        }

        if (execution.eigen_threads > 1) {
            throw std::invalid_argument(
                "Concurrent model execution requires eigen_threads <= 1.");
        }

        unsigned int worker_limit = execution.total_threads;
        if (worker_limit == 0) {
            worker_limit = std::thread::hardware_concurrency();
            if (worker_limit == 0) {
                worker_limit = 1;
            }
        }
        const auto worker_count = std::min<size_t>(worker_limit, _models.size());
        if (worker_count <= 1) {
            for (const auto &model : _models) {
                run_model(model);
            }
            return;
        }

        std::atomic<size_t> next_model{0};
        std::exception_ptr first_exception;
        std::mutex exception_mutex;
        std::vector<std::thread> workers;
        workers.reserve(worker_count);

        for (size_t worker = 0; worker < worker_count; ++worker) {
            workers.emplace_back([&]() {
                while (true) {
                    const size_t index = next_model.fetch_add(1);
                    if (index >= _models.size()) {
                        return;
                    }
                    try {
                        run_model(_models[index]);
                    } catch (...) {
                        std::lock_guard<std::mutex> lock(exception_mutex);
                        if (!first_exception) {
                            first_exception = std::current_exception();
                        }
                        return;
                    }
                }
            });
        }

        for (auto &worker : workers) {
            worker.join();
        }
        if (first_exception) {
            std::rethrow_exception(first_exception);
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Getters and Setters for Transitions and Metadata
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Retrieves all models in the simulation.
    /// @return A deep-copied vector of Model unique_ptrs.
    std::vector<std::unique_ptr<Model>> GetModels() const {
        std::vector<std::unique_ptr<Model>> _models_copy;
        for (const auto &model : _models) {
            _models_copy.push_back(model->clone());
        }
        return _models_copy;
    }

    /// @brief Mutable index-based model access.
    /// @details Returns a proxy that supports both model member access and
    /// clone-based replacement assignment.
    /// @param idx The index of the model to access.
    /// @return A mutable proxy for the model slot.
    /// @throws std::out_of_range if idx is out of range.
    ModelSlotProxy operator[](size_t idx) {
        GetModelRefOrThrow(idx);
        return ModelSlotProxy(*this, idx);
    }

    /// @brief Const index-based model access.
    /// @param idx The index of the model to access.
    /// @return Const reference to the model at the index.
    /// @throws std::out_of_range if idx is out of range.
    const Model &operator[](size_t idx) const {
        return GetModelRefOrThrow(idx);
    }

    /// @brief Provide a mapping of model indices to their names for all models
    /// in the simulation.
    /// @details This method returns a map where the keys are the indices of the
    /// models in the simulation, and the values are the corresponding model
    /// names. This allows for easy identification of models by their index in
    /// the simulation.
    /// @return The map of model indices to model names.
    std::map<size_t, std::string> GetModelIndexNameMap() const {
        std::map<size_t, std::string> ret;
        for (size_t i = 0; i < _models.size(); ++i) {
            ret[i] = _models[i]->GetName();
        }
        return ret;
    }

    /// @brief Retrieves a specific model by index in the simulation.
    /// @param idx The index of the model to retrieve.
    /// @return A deep-copied Model at the specified index.
    /// @details If idx is -1, the last model is returned.
    /// @throws std::out_of_range if no models exist or idx is out of range.
    std::unique_ptr<Model> GetModel(int idx) const {
        if (_models.empty()) {
            LogError(_runtime_config.logging.logger_name,
                     "No models available in GetModel.");
            throw std::out_of_range("Error attempting to GetModel: no models.");
        }
        if (idx < -1 || idx >= static_cast<int>(_models.size())) {
            LogError(_runtime_config.logging.logger_name,
                     "Index out of range in GetModel: " + std::to_string(idx));
            throw std::out_of_range("Error attempting to GetModel by index.");
        }
        // Return the last model if idx is -1
        if (idx == -1) {
            return _models.back()->clone();
        }
        return _models[idx]->clone();
    }

    /// @brief Retrieves the names of all models in the simulation.
    /// @return Vector of model names in the order they were added.
    std::vector<std::string> GetModelNames() const {
        std::vector<std::string> ret;
        for (auto &m : _models) {
            ret.push_back(m->GetName());
        }
        return ret;
    }

    /// @brief Retrieves the complete state histories for the model at the
    /// index.
    /// @param idx The index of the model to retrieve histories for.
    /// @return Map of history names to history records for the selected model.
    const std::map<std::string, History> &GetModelHistory(size_t idx) const {
        if (idx >= _models.size()) {
            LogError(_runtime_config.logging.logger_name,
                     "Index out of range in GetModelHistory: " +
                         std::to_string(idx));
            throw std::out_of_range(
                "Error attempting to GetModelHistory by index.");
        }
        return _models[idx]->GetHistories();
    }

    /// @brief Retrieves history names for the model at the specified index.
    /// @param idx The index of the model to retrieve history names for.
    /// @return Vector of history names for the selected model.
    const std::vector<std::string> GetModelHistoryNames(size_t idx) const {
        if (idx >= _models.size()) {
            LogError(_runtime_config.logging.logger_name,
                     "Index out of range in GetModelHistoryNames: " +
                         std::to_string(idx));
            throw std::out_of_range(
                "Error attempting to GetModelHistoryNames by index.");
        }
        std::vector<std::string> ret;
        for (const auto &kv : _models[idx]->GetHistories()) {
            ret.push_back(kv.first);
        }
        return ret;
    }

    void SetDuration(int duration) { _duration = duration; }

    /// @brief Retrieves the simulation execution settings.
    /// @return The current execution configuration.
    const ExecutionConfig &GetExecutionConfig() const {
        return _runtime_config.execution;
    }

    /// @brief Sets the simulation execution settings.
    /// @param execution_config Resource settings for simulation execution.
    void SetExecutionConfig(const ExecutionConfig &execution_config) {
        _runtime_config.execution = execution_config;
    }

    const RuntimeConfig &GetRuntimeConfig() const { return _runtime_config; }

    void SetRuntimeConfig(const RuntimeConfig &runtime_config) {
        _runtime_config = runtime_config;
        ConfigureLogger(_runtime_config.logging);
    }

private:
    Model &GetModelRefOrThrow(size_t idx) {
        if (idx >= _models.size()) {
            LogError(_runtime_config.logging.logger_name,
                     "Index out of range in model access: " +
                         std::to_string(idx));
            throw std::out_of_range("Error attempting to access model by "
                                    "index.");
        }
        return *_models[idx];
    }

    const Model &GetModelRefOrThrow(size_t idx) const {
        if (idx >= _models.size()) {
            LogError(_runtime_config.logging.logger_name,
                     "Index out of range in model access: " +
                         std::to_string(idx));
            throw std::out_of_range("Error attempting to access model by "
                                    "index.");
        }
        return *_models[idx];
    }

    RuntimeConfig _runtime_config;
    std::vector<std::unique_ptr<Model>> _models;

    int _duration = 1; // Default simulation duration in timesteps
    std::vector<int> _parameter_change_times;
    bool _stratify_entering_cohort = false;

    bool _build_summary_stats = false;
    bool _save_state_history = false;
    std::vector<int> _timesteps_to_report;
    bool _pivot_long = false;
};
} // namespace respond

#endif // RESPOND_SIMULATION_HPP_