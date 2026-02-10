////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-10                                                  //
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
class Simulation {
public:
    Simulation() : Simulation("console") {}
    Simulation(const std::string &log_name) : _log_name(log_name) {}
    ~Simulation() = default;

    /// @brief The core function to run the simulation. Runs all models
    /// associated with the simulation.
    /// @param duration The number of steps to take for each model.
    void Run(const int &duration) {
        for (int i = 0; i < duration; ++i) {
            for (const auto &model : _models) {
                model->RunTransitions();
            }
        }
    }

    void AddModel(const std::unique_ptr<Model> &model) {
        // because push_back is a move operation we're taking over ownership of
        // the unique pointer
        _models.push_back(model->clone());
    }

    const std::vector<std::unique_ptr<Model>> &GetModels() const {
        return _models;
    }

    std::vector<std::string> GetModelNames() const {
        std::vector<std::string> ret;
        for (auto &m : _models) {
            ret.push_back(m->GetModelName());
        }
        return ret;
    }

    void ClearModels() { _models.clear(); }

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

    /// @brief The default histories are:
    ///     1. State
    ///     2. Total Overdoses
    ///     3. Fatal Overdoses
    ///     4. Intervention Admissions
    ///     5. Background Mortality
    /// @return A vector of the default history objects.
    static std::map<std::string, History>
    CreateDefaultHistories(const std::string &log_name) {
        std::vector<std::string> names = {
            "state", "total_overdose", "fatal_overdose",
            "intervention_admission", "background_death"};

        std::map<std::string, History> ret;
        for (const auto &n : names) {
            History h(n, log_name);
            ret[n] = h;
        }
        return ret;
    }

    std::string GetLogName() const { return _log_name; }

    // Copying object
    Simulation(const Simulation &other) : _log_name(other.GetLogName()) {
        ClearModels();
        for (const auto &m : other.GetModels()) {
            _models.push_back(m->clone());
        }
    }
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