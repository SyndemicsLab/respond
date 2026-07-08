////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-08                                                  //
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
    const std::string CreateNewModel(const std::string &model_name) {
        _models.push_back(Model::Create(model_name, _log_name));
        return std::to_string(_models.size()) + "_" + _models.back()->GetName();
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
    const std::vector<std::unique_ptr<Model>> &GetModels() const {
        return _models;
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

    /// @brief Retrieves the complete state histories for all models.
    /// @return Vector of maps (one per model) mapping history names to state
    /// vector trajectories.
    const std::vector<std::map<std::string, std::vector<Eigen::VectorXd>>>
    GetModelHistories() const {
        std::vector<std::map<std::string, std::vector<Eigen::VectorXd>>> ret;
        int model_idx = 0;
        for (const auto &model : _models) {
            std::map<std::string, std::vector<Eigen::VectorXd>> inner_ret;
            for (const auto &kv : model->GetHistories()) {
                inner_ret[kv.first] = kv.second.GetStateAsVector();
            }
            ret.push_back(inner_ret);
            model_idx++;
        }
        return ret;
    }

    /// @brief Retrieves sparse history objects for all models.
    /// @return Vector of maps (one per model) mapping history names to sparse
    /// History objects.
    const std::vector<std::map<std::string, History>>
    GetModelSparseHistories() const {
        std::vector<std::map<std::string, History>> ret;
        for (const auto &model : _models) {
            ret.push_back(model->GetHistories());
        }
        return ret;
    }

    /// @brief Retrieves pairs of (model name, history name) for all histories.
    /// @return Vector of pairs associating each history with its parent model.
    const std::vector<std::pair<std::string, std::string>>
    GetModelHistoryNames() const {
        std::vector<std::pair<std::string, std::string>> ret;
        for (const auto &model : _models) {
            for (const auto &kv : model->GetHistories()) {
                std::pair<std::string, std::string> p = {model->GetName(),
                                                         kv.first};
                ret.push_back(p);
            }
        }
        return ret;
    }

    void SetDuration(int duration) { _duration = duration; }

private:
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