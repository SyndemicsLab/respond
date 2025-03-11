////////////////////////////////////////////////////////////////////////////////
// File: DataLoader.hpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-11                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_DATALOADER_HPP_
#define DATA_DATALOADER_HPP_

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

        virtual Matrix4d GetEnteringSamples() const = 0;

        virtual Matrix3d GetOUDTransitionRates() const = 0;

        virtual Matrix4d GetInterventionTransitionRates() const = 0;

        virtual Matrix4d GetOverdoseRates() const = 0;

        virtual Matrix4d GetFatalOverdoseRates() const = 0;

        virtual Matrix3d GetMortalityRates() const = 0;

        virtual Matrix3d GetInterventionInitRates() const = 0;

        virtual void SetInitialSample(Matrix3d mat) = 0;

        virtual void SetEnteringSamples(Matrix4d mat) = 0;

        virtual void SetOUDTransitionRates(Matrix3d mat) = 0;

        virtual void SetInterventionTransitionRates(Matrix4d mat) = 0;

        virtual void SetOverdoseRates(Matrix4d mat) = 0;

        virtual void SetFatalOverdoseRates(Matrix4d mat) = 0;

        virtual void SetMortalityRates(Matrix3d mat) = 0;

        virtual void SetInterventionInitRates(Matrix3d mat) = 0;

        virtual Matrix3d LoadInitialSample(std::string const &csvName) = 0;

        virtual Matrix4d
        LoadEnteringSamples(std::string const &csvName,
                            std::string const &enteringSampleIntervention,
                            std::string const &enteringSampleOUD) = 0;

        virtual Matrix4d LoadEnteringSamples(std::string const &csvName) = 0;

        virtual Matrix3d LoadOUDTransitionRates(std::string const &csvName) = 0;

        virtual Matrix3d
        LoadInterventionInitRates(std::string const &csvName) = 0;

        virtual Matrix4d
        LoadInterventionTransitionRates(std::string const &csvName) = 0;

        virtual Matrix4d LoadOverdoseRates(std::string const &csvName) = 0;

        virtual Matrix4d LoadFatalOverdoseRates(std::string const &csvName) = 0;

        virtual Matrix3d LoadMortalityRates(std::string const &smrCSVName,
                                            std::string const &bgmCSVName) = 0;
    };
} // namespace respond::data_ops
#endif // DATA_DATALOADER_HPP_
