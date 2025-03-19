////////////////////////////////////////////////////////////////////////////////
// File: simulation_internals.hpp                                             //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-19                                                  //
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

using namespace respond::data_ops;

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

        void Run(const DataLoader &data_loader) override;

        History GetHistory() const override { return history; }

    private:
        const std::string logger_name;
        data_ops::Matrix3d state;
        int time = 0;
        data_ops::History history;

        void ResetTime() { time = 0; }

        data_ops::Matrix3d Step(const DataLoader &data_loader);

        void LogDebugPoint(const std::string &message,
                           const Matrix3d &matrix) const;

        void ResetHistory();

        Matrix3d
        MultiplyUseAfterIntervention(const Matrix3d &mat, const int idx,
                                     const DataLoader &data_loader) const;

        Matrix3d AddEnteringCohort(const data_ops::Matrix3d &mat,
                                   const DataLoader &data_loader) const;

        Matrix3d
        MultiplyBehaviorTransition(const Matrix3d &mat,
                                   const DataLoader &data_loader) const;

        Matrix3d
        MultiplyInterventionTransition(const Matrix3d &mat,
                                       const DataLoader &data_loader) const;

        Matrix3d MultiplyFODGivenOD(const Matrix3d &mat,
                                    const DataLoader &data_loader) const;

        Matrix3d MultiplyOD(const Matrix3d &mat,
                            const DataLoader &data_loader) const;

        Matrix3d MultiplyMortality(const Matrix3d &mat,
                                   const DataLoader &data_loader) const;
    };
} // namespace respond::model

#endif // RESPOND_MODEL_SIMULATIONINTERNALS_HPP_