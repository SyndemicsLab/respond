////////////////////////////////////////////////////////////////////////////////
// File: data_loader_internals.hpp                                            //
// Project: internals                                                         //
// Created Date: 2025-03-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-05                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_DATALOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_DATALOADERINTERNALS_HPP_

#include <respond/data_ops/data_loader.hpp>

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

#include "base_loader_internals.hpp"

namespace respond {
namespace data_ops {
class DataLoaderImpl : public virtual DataLoader, public BaseLoader {
public:
    /// @brief An alternative constructor for DataLoader for loading data
    /// when a Configuration object has already been created prior to
    /// specifying the directory containing tabular inputs
    /// @param config The object containing parameters read from an
    /// already-processed configuration file
    /// @param inputDir The name of the directory where input files are
    /// stored
    DataLoaderImpl(const std::string &directory = "",
                   const std::string &log_name = "console")
        : BaseLoader(directory, log_name) {}

    ~DataLoaderImpl() = default;

    Matrix3d GetInitialSample() const override { return initial_sample; }

    Matrix3d GetEnteringSamples(const int &time) const override {
        if (entering_samples.find(time) != entering_samples.end()) {
            return entering_samples.at(time);
        }
        return entering_samples.lower_bound(time)->second;
    }

    Matrix3d GetOUDTransitionRates() const override {
        return oud_transition_rates;
    }

    Matrix3d GetInterventionTransitionRates(const int &time) const override {
        if (intervention_transition_rates.find(time) !=
            intervention_transition_rates.end()) {
            return intervention_transition_rates.at(time);
        }
        return intervention_transition_rates.lower_bound(time)->second;
    }

    Matrix3d GetOverdoseRates(const int &time) const override {
        if (overdose_rates.find(time) != overdose_rates.end()) {
            return overdose_rates.at(time);
        }
        return overdose_rates.lower_bound(time)->second;
    }

    Matrix3d GetFatalOverdoseRates(const int &time) const override {
        if (fatal_overdose_rates.find(time) != fatal_overdose_rates.end()) {
            return fatal_overdose_rates.at(time);
        }
        return fatal_overdose_rates.lower_bound(time)->second;
    }

    Matrix3d GetMortalityRates() const override { return mortality_rates; }

    Matrix3d GetInterventionInitRates() const override {
        return intervention_init_rates;
    }

    void SetInitialSample(const Matrix3d &mat) override {
        this->initial_sample = mat;
    }

    void SetEnteringSamples(const TimedMatrix3d &mat) override {
        this->entering_samples = mat;
    }

    void SetOUDTransitionRates(const Matrix3d &mat) override {
        this->oud_transition_rates = mat;
    }

    void SetInterventionTransitionRates(const TimedMatrix3d &mat) override {
        this->intervention_transition_rates = mat;
    }

    void SetOverdoseRates(const TimedMatrix3d &mat) override {
        this->overdose_rates = mat;
    }

    void SetFatalOverdoseRates(const TimedMatrix3d &mat) override {
        this->fatal_overdose_rates = mat;
    }

    void SetMortalityRates(const Matrix3d &mat) override {
        this->mortality_rates = mat;
    }

    void SetInterventionInitRates(const Matrix3d &mat) override {
        this->intervention_init_rates = mat;
    }

    Matrix3d LoadInitialSample(std::string const &csvName) override;

    TimedMatrix3d
    LoadEnteringSamples(std::string const &csvName,
                        std::string const &enteringSampleIntervention,
                        std::string const &enteringSampleOUD) override;

    TimedMatrix3d LoadEnteringSamples(std::string const &csvName) override;

    Matrix3d LoadOUDTransitionRates(std::string const &csvName) override;

    Matrix3d LoadInterventionInitRates(std::string const &csvName) override;

    TimedMatrix3d
    LoadInterventionTransitionRates(std::string const &csvName) override;

    TimedMatrix3d LoadOverdoseRates(std::string const &csvName) override;

    TimedMatrix3d LoadFatalOverdoseRates(std::string const &csvName) override;

    Matrix3d LoadMortalityRates(std::string const &smrCSVName,
                                std::string const &bgmCSVName) override;

    Data::IConfigablePtr GetConfig() const { return BaseLoader::GetConfig(); }

private:
    void FillTime(int &start, int const end, Matrix3d data,
                  TimedMatrix3d &storage);

    Matrix3d StrVecToMatrix3d(std::vector<std::string> strVec, int matD1,
                              int matD2, int matD3);

    Matrix3d DoubleToMatrix3d(double value, int matD1, int matD2, int matD3);

    /// @brief
    /// @param table
    /// @param dimension
    /// @return
    Matrix3d CreateTransitionMatrix3d(Data::IDataTablePtr const &table,
                                      Dimension dimension, int timestep);

    /// @brief
    /// @param indices
    /// @param table
    /// @return
    Matrix3d BuildInterventionMatrix(Data::IDataTablePtr const &table,
                                     std::string interventionName,
                                     int timestep);

    /// @brief
    /// @param table
    /// @param key
    /// @return
    Matrix3d BuildOverdoseTransitions(Data::IDataTablePtr const &table,
                                      std::string const &key);

    /// @brief
    /// @param table
    /// @param key
    /// @return
    Matrix3d BuildFatalOverdoseTransitions(Data::IDataTablePtr const &table,
                                           std::string const &key);

    /// @brief
    /// @param ict
    /// @param table
    /// @return
    TimedMatrix3d
    BuildTransitionRatesOverTime(std::vector<int> const &ict,
                                 Data::IDataTablePtr const &table);

    std::vector<int> demographic_counts{};
    std::map<std::string, std::vector<int>> simulation_parameters = {};

    Matrix3d initial_sample;
    TimedMatrix3d entering_samples;
    Matrix3d oud_transition_rates;
    Matrix3d intervention_init_rates;
    TimedMatrix3d intervention_transition_rates;
    TimedMatrix3d overdose_rates;
    TimedMatrix3d fatal_overdose_rates;
    Matrix3d mortality_rates;
};

} // namespace data_ops
} // namespace respond
#endif // RESPOND_DATAOPS_DATALOADERINTERNALS_HPP_