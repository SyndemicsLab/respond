////////////////////////////////////////////////////////////////////////////////
// File: simulation_internals.hpp                                             //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_MODEL_SIMULATIONINTERNALS_HPP_
#define RESPOND_MODEL_SIMULATIONINTERNALS_HPP_

#include <respond/model/simulation.hpp>

#include <string>
#include <thread>

#include <Eigen/Eigen>

namespace respond::model {
    class RespondImpl : public virtual Respond {
    public:
        /// @brief
        RespondImpl(const std::string &log_name = "console")
            : logger_name(log_name) {
            const auto processor_count = std::thread::hardware_concurrency();
            Eigen::setNbThreads(processor_count);
            time = 0;
        }

        ~RespondImpl() = default;

        void Run(const respond::data_ops::DataLoader &data_loader) override;

        respond::data_ops::History GetHistory() const override {
            return history;
        }

    private:
        const std::string logger_name;
        data_ops::Matrix3d state;
        int time = 0;
        data_ops::History history;

        void ResetTime() { time = 0; }

        data_ops::Matrix3d
        Step(const respond::data_ops::DataLoader &data_loader);

        void LogDebugPoint(const std::string &message,
                           const respond::data_ops::Matrix3d &matrix) const;

        void ResetHistory();

        respond::data_ops::Matrix3d MultiplyUseAfterIntervention(
            const respond::data_ops::Matrix3d &mat, const int idx,
            const respond::data_ops::DataLoader &data_loader) const;

        respond::data_ops::Matrix3d AddEnteringCohort(
            const data_ops::Matrix3d &mat,
            const respond::data_ops::DataLoader &data_loader) const;

        respond::data_ops::Matrix3d MultiplyBehaviorTransition(
            const respond::data_ops::Matrix3d &mat,
            const respond::data_ops::DataLoader &data_loader) const;

        respond::data_ops::Matrix3d MultiplyInterventionTransition(
            const respond::data_ops::Matrix3d &mat,
            const respond::data_ops::DataLoader &data_loader) const;

        respond::data_ops::Matrix3d MultiplyFODGivenOD(
            const respond::data_ops::Matrix3d &mat,
            const respond::data_ops::DataLoader &data_loader) const;

        respond::data_ops::Matrix3d
        MultiplyOD(const respond::data_ops::Matrix3d &mat,
                   const respond::data_ops::DataLoader &data_loader) const;

        respond::data_ops::Matrix3d MultiplyMortality(
            const respond::data_ops::Matrix3d &mat,
            const respond::data_ops::DataLoader &data_loader) const;
    };

    std::unique_ptr<Respond> Respond::Create(const std::string &log_name) {
        return std::make_unique<RespondImpl>(log_name);
    }
} // namespace respond::model

#endif // RESPOND_MODEL_SIMULATIONINTERNALS_HPP_