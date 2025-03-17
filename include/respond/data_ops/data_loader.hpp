////////////////////////////////////////////////////////////////////////////////
// File: data_loader.hpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_DATALOADER_HPP_
#define RESPOND_DATAOPS_DATALOADER_HPP_

#include <memory>
#include <string>

#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/matrices.hpp>

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

        virtual Matrix3d LoadInitialSample(const std::string &file) = 0;

        virtual TimedMatrix3d
        LoadEnteringSamples(const std::string &file,
                            const std::string &intervention,
                            const std::string &behavior) = 0;

        virtual TimedMatrix3d LoadEnteringSamples(const std::string &file) = 0;

        virtual Matrix3d LoadOUDTransitionRates(const std::string &file) = 0;

        virtual Matrix3d LoadInterventionInitRates(const std::string &file) = 0;

        virtual TimedMatrix3d
        LoadInterventionTransitionRates(const std::string &file) = 0;

        virtual TimedMatrix3d LoadOverdoseRates(const std::string &file) = 0;

        virtual TimedMatrix3d
        LoadFatalOverdoseRates(const std::string &file) = 0;

        virtual Matrix3d
        LoadMortalityRates(const std::string &smr_file,
                           const std::string &background_file) = 0;

        virtual Data::IConfigablePtr GetConfig() const = 0;

        static std::unique_ptr<DataLoader>
        Create(const std::string &directory = "",
               const std::string &log_name = "console");
    };
} // namespace respond::data_ops
#endif // RESPOND_DATAOPS_DATALOADER_HPP_
