////////////////////////////////////////////////////////////////////////////////
// File: simulation.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-05                                                  //
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
    Simulation(const std::string &log_name = "console") : _log_name(log_name) {}

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

    void AddModel(std::unique_ptr<Model> model) {
        // because push_back is a move operation we're taking over ownership of
        // the unique pointer
        _models.push_back(model);
    }

    std::vector<std::string> GetModelNames() const {
        std::vector<std::string> ret;
        for (auto &m : _models) {
            ret.push_back(m->GetModelName());
        }
        return ret;
    }

    void ClearModels() { _models.clear(); }

    const std::map<std::string,
                   std::map<std::string, std::vector<Eigen::VectorXd>>>
    GetModelHistories() const {
        std::map<std::string,
                 std::map<std::string, std::vector<Eigen::VectorXd>>>
            ret;
        for (const auto &model : _models) {
            std::map<std::string, std::vector<Eigen::VectorXd>> inner_ret;
            for (const auto &history : model->GetHistories()) {
                inner_ret[history.GetHistoryName()] =
                    history.GetStateAsVector();
            }
            ret[model->GetModelName()] = inner_ret;
        }
        return ret;
    }

    const std::vector<std::pair<std::string, std::string>>
    GetModelHistoryNames() const {
        std::vector<std::pair<std::string, std::string>> ret;
        for (const auto &model : _models) {
            for (const auto &history : model->GetHistories()) {
                std::pair<std::string, std::string> p = {
                    model->GetModelName(), history.GetHistoryName()};
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
    std::map<std::string, History> CreateDefaultHistories() {
        std::vector<std::string> names = {
            "state", "total_overdose", "fatal_overdose",
            "intervention_admission", "background_death"};

        std::map<std::string, History> ret;
        for (const auto &n : names) {
            History h(n, _log_name);
            ret[n] = h;
        }
        return ret;
    }

    const std::string &GetLogName() const { return _log_name; }

private:
    const std::string _log_name;
    std::vector<std::unique_ptr<Model>> _models;
};
} // namespace respond

#endif // RESPOND_SIMULATION_HPP_