////////////////////////////////////////////////////////////////////////////////
// File: data_loader_internals.hpp                                            //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-07                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_DATALOADERINTERNALS_HPP_
#define RESPOND_DATAOPS_DATALOADERINTERNALS_HPP_

#include <respond/data_ops/data_loader.hpp>

namespace respond::data_ops {
    class DataLoaderImpl : public virtual DataLoader {
    public:
        /// @brief An alternative constructor for DataLoader for loading data
        /// when a Configuration object has already been created prior to
        /// specifying the directory containing tabular inputs
        /// @param config The object containing parameters read from an
        /// already-processed configuration file
        /// @param inputDir The name of the directory where input files are
        /// stored
        DataLoaderImpl(Data::IConfigablePtr config, std::string const &inputDir,
                       std::shared_ptr<spdlog::logger> logger);

        // delegating constructors
        DataLoaderImpl() : DataLoaderImpl(nullptr, "", nullptr) {}
        DataLoaderImpl(Data::IConfigablePtr config)
            : DataLoaderImpl(config, "", nullptr) {}
        DataLoaderImpl(Data::IConfigablePtr config, std::string const &inputDir)
            : DataLoaderImpl(config, inputDir, nullptr) {}
        DataLoaderImpl(std::string const &inputDir,
                       std::shared_ptr<spdlog::logger> logger)
            : DataLoaderImpl(nullptr, inputDir, logger) {}
        DataLoaderImpl(std::string const &inputDir)
            : DataLoaderImpl(nullptr, inputDir, nullptr) {}

        ~DataLoaderImpl() = default;

        Matrix3d GetInitialSample() const override { return initial_sample; }

        Matrix4d GetEnteringSamples() const override {
            return entering_samples;
        }

        Matrix3d GetOUDTransitionRates() const override {
            return oud_transition_rates;
        }

        Matrix4d GetInterventionTransitionRates() const override {
            return intervention_transition_rates;
        }

        Matrix4d GetOverdoseRates() const override { return overdose_rates; }

        Matrix4d GetFatalOverdoseRates() const override {
            return fatal_overdose_rates;
        }

        Matrix3d GetMortalityRates() const override { return mortality_rates; }

        Matrix3d GetInterventionInitRates() const override {
            return intervention_init_rates;
        }

        void SetInitialSample(Matrix3d mat) override {
            this->initial_sample = mat;
        }

        void SetEnteringSamples(Matrix4d mat) override {
            this->entering_samples = mat;
        }

        void SetOUDTransitionRates(Matrix3d mat) override {
            this->oud_transition_rates = mat;
        }

        void SetInterventionTransitionRates(Matrix4d mat) override {
            this->intervention_transition_rates = mat;
        }

        void SetOverdoseRates(Matrix4d mat) override {
            this->overdose_rates = mat;
        }

        void SetFatalOverdoseRates(Matrix4d mat) override {
            this->fatal_overdose_rates = mat;
        }

        void SetMortalityRates(Matrix3d mat) override {
            this->mortality_rates = mat;
        }

        void SetInterventionInitRates(Matrix3d mat) override {
            this->intervention_init_rates = mat;
        }

        Matrix3d LoadInitialSample(std::string const &csvName) override;

        Matrix4d
        LoadEnteringSamples(std::string const &csvName,
                            std::string const &enteringSampleIntervention,
                            std::string const &enteringSampleOUD) override;

        Matrix4d LoadEnteringSamples(std::string const &csvName) override;

        Matrix3d LoadOUDTransitionRates(std::string const &csvName) override;

        Matrix3d LoadInterventionInitRates(std::string const &csvName) override;

        Matrix4d
        LoadInterventionTransitionRates(std::string const &csvName) override;

        Matrix4d LoadOverdoseRates(std::string const &csvName) override;

        Matrix4d LoadFatalOverdoseRates(std::string const &csvName) override;

        Matrix3d LoadMortalityRates(std::string const &smrCSVName,
                                    std::string const &bgmCSVName) override;

    private:
        void FillTime(int &start, int const end, Matrix3d data,
                      Matrix4d &storage);

        Matrix3d StrVecToMatrix3d(std::vector<std::string> strVec, int matD1,
                                  int matD2, int matD3);

        Matrix3d DoubleToMatrix3d(double value, int matD1, int matD2,
                                  int matD3);

        /// @brief
        /// @param table
        /// @param dimension
        /// @return
        Matrix3d CreateTransitionMatrix3d(Data::IDataTablePtr const &table,
                                          data_ops::Dimension dimension,
                                          int timestep);

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
        Matrix4d BuildTransitionRatesOverTime(std::vector<int> const &ict,
                                              Data::IDataTablePtr const &table);

        std::vector<int> demographic_counts{};
        std::map<std::string, std::vector<int>> simulation_parameters = {};

        Matrix3d initial_sample;
        Matrix4d entering_samples;
        Matrix3d oud_transition_rates;
        Matrix3d intervention_init_rates;
        Matrix4d intervention_transition_rates;
        Matrix4d overdose_rates;
        Matrix4d fatal_overdose_rates;
        Matrix3d mortality_rates;
    };
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_DATALOADERINTERNALS_HPP_