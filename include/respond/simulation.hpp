////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_SIMULATION_HPP_
#define RESPOND_SIMULATION_HPP_

#include <map>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include <Eigen/Dense>

#include <respond/history.hpp>
#include <respond/model.hpp>

namespace respond {
/// @brief Manages and executes multiple models in a coordinated simulation.
/// A Simulation aggregates Model instances and coordinates their execution,
/// maintaining history records and providing access to simulation results.
class Simulation {
public:
    /// @brief Default constructor initializing with "console" logger.
    Simulation() : Simulation("console") {}

    /// @brief Constructs a Simulation with a specified logger.
    /// @param log_name Name of the logger for this simulation (default:
    /// "console").
    Simulation(const std::string &log_name) : _log_name(log_name) {}

    /// @brief Virtual destructor for polymorphic cleanup.
    ~Simulation() = default;

    /// @brief Executes one step of the simulation for all models.
    /// Calls RunTransitions() on each registered model in sequence.
    void Run() {
        for (const auto &model : _models) {
            model->RunTransitions();
        }
    }

    /// @brief Adds a model to the simulation.
    /// The model is cloned and managed by the simulation.
    /// @param model A unique_ptr to a Model instance to add.
    void AddModel(const std::unique_ptr<Model> &model) {
        // because push_back is a move operation we're taking over ownership of
        // the unique pointer
        _models.push_back(model->clone());
    }

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
            ret.push_back(m->GetModelName());
        }
        return ret;
    }

    /// @brief Removes all models from the simulation.
    void ClearModels() { _models.clear(); }

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

    /// @brief Retrieves pairs of (model name, history name) for all histories.
    /// @return Vector of pairs associating each history with its parent model.
    const std::vector<std::pair<std::string, std::string>>
    GetModelHistoryNames() const {
        std::vector<std::pair<std::string, std::string>> ret;
        for (const auto &model : _models) {
            for (const auto &kv : model->GetHistories()) {
                std::pair<std::string, std::string> p = {model->GetModelName(),
                                                         kv.first};
                ret.push_back(p);
            }
        }
        return ret;
    }

    /// @brief Retrieves the logger name used by this simulation.
    /// @return The name of the associated logger.
    std::string GetLogName() const { return _log_name; }

    /// @brief Copy constructor creating an independent deep copy of the
    /// simulation. All models are cloned; modifications to the copy do not
    /// affect the original.
    Simulation(const Simulation &other) : _log_name(other.GetLogName()) {
        ClearModels();
        for (const auto &m : other.GetModels()) {
            _models.push_back(m->clone());
        }
    }

    /// @brief Copy assignment operator for deep copying simulation state.
    /// @param other The simulation to copy from.
    /// @return Reference to this simulation after assignment.
    Simulation &operator=(const Simulation &other) {
        if (this != &other) {
            ClearModels();
            _log_name = other.GetLogName();
            for (const auto &m : other.GetModels()) {
                _models.push_back(m->clone());
            }
        }
        return *this;
    }

private:
    std::string _log_name;
    std::vector<std::unique_ptr<Model>> _models;
};
} // namespace respond

#endif // RESPOND_SIMULATION_HPP_