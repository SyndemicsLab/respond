////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-22                                                  //
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

#include <map>
#include <memory>
#include <string>
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
                LogError(_owner->_log_name,
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
    Simulation() : Simulation(RESPOND_DEFAULT_LOG) {}

    /// @brief Constructs a Simulation with a specified logger.
    /// @param log_name The name of the logger to use for simulation output.
    Simulation(const std::string &log_name)
        : Simulation(log_name, RESPOND_DEFAULT_LOG_FILE) {}

    /// @brief Constructs a Simulation with a specified logger and log file.
    /// @param log_name The name of the logger to use for simulation output.
    /// @param log_filepath The file path for the logger output.
    Simulation(const std::string &log_name, const std::string &log_filepath)
        : _log_name(log_name) {
        CreateFileLogger(log_name, log_filepath);
    }

    /// @brief Virtual destructor for polymorphic cleanup.
    ~Simulation() = default;

    /// @brief Copy constructor creating an independent deep copy of the
    /// simulation. All models are cloned; modifications to the copy do not
    /// affect the original.
    /// @param other The Simulation instance to copy from.
    Simulation(const Simulation &other) {
        _log_name = other._log_name;
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
            _log_name = other._log_name;
            _models.clear();
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
        return *this;
    }

    /// @brief Move constructor for transferring simulation ownership.
    /// @param other The simulation to move from.
    Simulation(Simulation &&other) noexcept
        : _log_name(std::move(other._log_name)), _duration(other._duration),
          _parameter_change_times(std::move(other._parameter_change_times)),
          _stratify_entering_cohort(other._stratify_entering_cohort),
          _build_summary_stats(other._build_summary_stats),
          _save_state_history(other._save_state_history),
          _timesteps_to_report(std::move(other._timesteps_to_report)),
          _pivot_long(other._pivot_long) {
        for (const auto &m : other._models) {
            _models.push_back(m->clone());
        }
        other._models.clear();
    }

    /// @brief Move assignment operator for transferring simulation ownership.
    /// @param other The simulation to move from.
    /// @return Reference to this simulation after assignment.
    Simulation &operator=(Simulation &&other) noexcept {
        if (this != &other) {
            _log_name = std::move(other._log_name);
            _duration = other._duration;
            _parameter_change_times = std::move(other._parameter_change_times);
            _stratify_entering_cohort = other._stratify_entering_cohort;
            _build_summary_stats = other._build_summary_stats;
            _save_state_history = other._save_state_history;
            _timesteps_to_report = std::move(other._timesteps_to_report);
            _pivot_long = other._pivot_long;

            for (const auto &m : other._models) {
                _models.push_back(m->clone());
            }
            other._models.clear();
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
    /// @return The unique identifier for the newly created model, combining its
    /// index and name.
    std::unique_ptr<Model> CreateNewModel(const std::string &model_name) {
        _models.push_back(Model::Create(model_name, _log_name));
        return _models.back()->clone();
    }

    /// @brief Removes all models from the simulation.
    void ClearModels() { _models.clear(); }

    /// @brief Adds a model to the simulation.
    /// The model is cloned and managed by the simulation.
    /// @param model A unique_ptr to a Model instance to add.
    void AddModel(const std::unique_ptr<Model> &model) {
        // because push_back is a move operation we're taking over ownership of
        // the unique pointer
        _models.push_back(model->clone());
    }

    /// @brief Executes one step of the simulation for all models.
    /// Calls RunTransitions() on each registered model in sequence.
    void Run(int duration = -1) {
        if (duration > 0) {
            _duration = duration;
        }
        LogInfo(_log_name, "Running simulation for duration of " +
                               std::to_string(_duration) + " timesteps.");
        for (const auto &model : _models) {
            model->SetFinalTimestep(_duration);
            model->RunTimesteps();
        }
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Getters and Setters for Transitions and Metadata
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Retrieves all models in the simulation.
    /// @return Const reference to the vector of Model unique_ptrs.
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
    /// @return A const reference to the Model unique_ptr at the specified
    /// index. Throws an exception if the index is out of range.
    std::unique_ptr<Model> GetModel(int idx) const {
        if (_models.empty()) {
            LogError(_log_name, "No models available in GetModel.");
            throw std::out_of_range("Error attempting to GetModel: no models.");
        }
        if (idx < -1 || idx >= static_cast<int>(_models.size())) {
            LogError(_log_name,
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
    /// @return Vector of maps (one per model) mapping history names to state
    /// vector trajectories.
    const std::map<std::string, History> &GetModelHistory(size_t idx) const {
        if (idx >= _models.size()) {
            LogError(_log_name, "Index out of range in GetModelHistory: " +
                                    std::to_string(idx));
            throw std::out_of_range(
                "Error attempting to GetModelHistory by index.");
        }
        return _models[idx]->GetHistories();
    }

    /// @brief Retrieves pairs of (model name, history name) for all histories.
    /// @return Vector of pairs associating each history with its parent model.
    const std::vector<std::string> GetModelHistoryNames(size_t idx) const {
        if (idx >= _models.size()) {
            LogError(_log_name, "Index out of range in GetModelHistoryNames: " +
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

private:
    Model &GetModelRefOrThrow(size_t idx) {
        if (idx >= _models.size()) {
            LogError(_log_name, "Index out of range in model access: " +
                                    std::to_string(idx));
            throw std::out_of_range("Error attempting to access model by "
                                    "index.");
        }
        return *_models[idx];
    }

    const Model &GetModelRefOrThrow(size_t idx) const {
        if (idx >= _models.size()) {
            LogError(_log_name, "Index out of range in model access: " +
                                    std::to_string(idx));
            throw std::out_of_range("Error attempting to access model by "
                                    "index.");
        }
        return *_models[idx];
    }

    std::string _log_name;
    std::vector<std::unique_ptr<Model>> _models;

    int _duration = 1; // Default simulation duration in timesteps
    std::vector<int> _parameter_change_times;
    bool _stratify_entering_cohort;

    bool _build_summary_stats;
    bool _save_state_history;
    std::vector<int> _timesteps_to_report;
    bool _pivot_long;
};
} // namespace respond

#endif // RESPOND_SIMULATION_HPP_