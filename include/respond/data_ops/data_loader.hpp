////////////////////////////////////////////////////////////////////////////////
// File: data_loader.hpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_DATALOADER_HPP_
#define RESPOND_DATAOPS_DATALOADER_HPP_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include <respond/data_ops/data_types.hpp>

namespace respond::data_ops {

    /*!
     * @brief The object that processes model inputs
     *
     * DataLoader handles the primary configuration file, `sim.conf`, and the
     * tabular inputs necessary to run the model.
     *
     * Updated **[2023-04-28]**
     *
     * **Required Input Tables:**
     * - `sim.conf`
     * - `all_types_overdose.csv`
     * - `background_mortality.csv`
     * - `block_init_effect.csv`
     * - `block_trans.csv`
     * - `entering_cohort.csv`
     * - `fatal_overdose.csv`
     * - `init_cohort.csv`
     * - `oud_trans.csv`
     * - `SMR.csv`
     *
     * **Optional Input Tables:**
     * - `bg_utility.csv`
     * - `healthcare_utilization_cost.csv`
     * - `oud_utility.csv`
     * - `overdose_cost.csv`
     * - `pharmaceutical_cost.csv`
     * - `setting_utility.csv`
     * - `treatment_utilization_cost.csv`
     */
    class DataLoader {
    public:
        virtual ~DataLoader() = default;

        virtual Matrix3d GetInitialSample() const = 0;

        virtual TimedMatrix3d GetEnteringSamples() const = 0;

        virtual Matrix3d GetOUDTransitionRates() const = 0;

        virtual TimedMatrix3d GetInterventionTransitionRates() const = 0;

        virtual TimedMatrix3d GetOverdoseRates() const = 0;

        virtual TimedMatrix3d GetFatalOverdoseRates() const = 0;

        virtual Matrix3d GetMortalityRates() const = 0;

        virtual Matrix3d GetInterventionInitRates() const = 0;

        virtual void SetInitialSample(const Matrix3d &mat) = 0;

        virtual void SetEnteringSamples(const TimedMatrix3d &mat) = 0;

        virtual void SetOUDTransitionRates(const Matrix3d &mat) = 0;

        virtual void
        SetInterventionTransitionRates(const TimedMatrix3d &mat) = 0;

        virtual void SetOverdoseRates(const TimedMatrix3d &mat) = 0;

        virtual void SetFatalOverdoseRates(const TimedMatrix3d &mat) = 0;

        virtual void SetMortalityRates(const Matrix3d &mat) = 0;

        virtual void SetInterventionInitRates(const Matrix3d &mat) = 0;

        virtual Matrix3d LoadInitialSample(std::string const &csvName) = 0;

        virtual TimedMatrix3d
        LoadEnteringSamples(std::string const &csvName,
                            std::string const &enteringSampleIntervention,
                            std::string const &enteringSampleOUD) = 0;

        virtual TimedMatrix3d
        LoadEnteringSamples(std::string const &csvName) = 0;

        virtual Matrix3d LoadOUDTransitionRates(std::string const &csvName) = 0;

        virtual Matrix3d
        LoadInterventionInitRates(std::string const &csvName) = 0;

        virtual TimedMatrix3d
        LoadInterventionTransitionRates(std::string const &csvName) = 0;

        virtual TimedMatrix3d LoadOverdoseRates(std::string const &csvName) = 0;

        virtual TimedMatrix3d
        LoadFatalOverdoseRates(std::string const &csvName) = 0;

        virtual Matrix3d LoadMortalityRates(std::string const &smrCSVName,
                                            std::string const &bgmCSVName) = 0;
    };
} // namespace respond::data_ops
#endif // RESPOND_DATAOPS_DATALOADER_HPP_
