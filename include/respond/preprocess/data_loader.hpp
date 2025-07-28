////////////////////////////////////////////////////////////////////////////////
// File: data_loader.hpp                                                      //
// Project: preprocess // Created Date: 2025-07-07 // Author: Matthew Carroll //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_PREPROCESS_DATALOADER_HPP_
#define RESPOND_PREPROCESS_DATALOADER_HPP_

#include <memory>
#include <string>

#include <datamanagement/DataManagement.hpp>

#include <respond/utils/types.hpp>

#include <respond/preprocess/base_loader.hpp>

namespace respond {
namespace preprocess {

/// @brief Class for Loading and Managing Data in RESPOND.
class DataLoader : public virtual BaseLoader {
public:
    /// @brief Default Desctructor for the DataLoader class.
    virtual ~DataLoader() = default;

    /// @brief Get the Entering Sample Matrix for a given time.
    /// @return The Entering Sample Matrix at the specified time.
    virtual Eigen::VectorXd GetMigratingState() const = 0;

    /// @brief Get the Initial Cohort Matrix.
    /// @return The Initial Cohort.
    virtual Eigen::VectorXd GetSimulationState() const = 0;

    /// @brief
    /// @return
    virtual Eigen::VectorXd GetDeathState() const = 0;

    /// @brief
    /// @return
    virtual Eigen::MatrixXd GetMigrationTransition() const = 0;

    /// @brief Get the OUD Transition Rates Matrix.
    /// @return The OUD Transition Rates Matrix.
    virtual Eigen::MatrixXd GetBehaviorTransition() const = 0;

    /// @brief Get the Intervention Transition Rates Matrix for the specified
    /// time.
    /// @param time Time point for which to retrieve the intervention transition
    /// rates.
    /// @return The Intervention Transition Rates Matrix at the specified time.
    virtual Eigen::MatrixXd
    GetInterventionTransition(const int &time) const = 0;

    /// @brief Get the Overdose Rates Matrix for the specified time.
    /// @param time Time point for which to retrieve the overdose rates.
    /// @return The Overdose Rates Matrix at the specified time.
    virtual Eigen::MatrixXd GetOverdoseTransition(const int &time) const = 0;

    /// @brief Get the Fatal Overdose Rates Matrix for the specified time.
    /// @param time Time point for which to retrieve the fatal overdose rates.
    /// @return The Fatal Overdose Rates Matrix at the specified time.
    virtual Eigen::MatrixXd
    GetFatalOverdoseTransition(const int &time) const = 0;

    /// @brief Get the Mortality Rates Matrix.
    /// @return The Mortality Rates Matrix.
    virtual Eigen::MatrixXd GetMortalityTransition() const = 0;

    /// @brief Get the Intervention Initial Rates Matrix.
    /// @return The Intervention Initial Rates Matrix.
    virtual Eigen::MatrixXd GetInterventionInitTransition() const = 0;

    /// @brief Set the Initial Sample Matrix.
    /// @param mat The Initial Sample Matrix to set.
    virtual void SetInitialState(const Eigen::VectorXd &vec) = 0;

    /// @brief Set the Entering Samples Matrix.
    /// @param mat The Entering Samples Matrix to set.
    virtual void SetMigratingState(const Eigen::VectorXd &vec) = 0;

    /// @brief
    /// @return
    virtual void SetDeathState(const Eigen::VectorXd &vec) = 0;

    /// @brief
    /// @return
    virtual void SetMigrationTransition(const Eigen::MatrixXd &mat) = 0;

    /// @brief Set the Mortality Rates Matrix.
    /// @param mat The Mortality Rates Matrix to set.
    virtual void SetDeathTransition(const Eigen::MatrixXd &mat) = 0;

    /// @brief Set the OUD Transition Rates Matrix.
    /// @param mat The OUD Transition Rates Matrix to set.
    virtual void SetOUDTransitionRate(const Eigen::MatrixXd &mat) = 0;

    /// @brief Set the Intervention Transition Rates Matrix.
    /// @param mat The Intervention Transition Rates Matrix to set.
    virtual void SetInterventionTransition(const Eigen::MatrixXd &mat) = 0;

    /// @brief Set the Overdose Rates Matrix.
    /// @param mat The Overdose Rates Matrix to set.
    virtual void SetOverdoseTransition(const Eigen::MatrixXd &mat) = 0;

    /// @brief Set the Fatal Overdose Rates Matrix.
    /// @param mat The Fatal Overdose Rates Matrix to set.
    virtual void SetFatalOverdoseTransition(const Eigen::MatrixXd &mat) = 0;

    /// @brief Set the Intervention Initial Rates Matrix.
    /// @param mat The Intervention Initial Rates Matrix to set.
    virtual void SetInterventionInitTransition(const Eigen::MatrixXd &mat) = 0;

    /// @brief Load the Initial Sample Matrix from a file.
    /// @param file The file path to load the initial sample from.
    virtual void LoadInitialSample(const std::string &file) = 0;

    /// @brief Load the Entering Samples Matrix from a file.
    /// @param file The file path to load the entering samples from.
    /// @param intervention The intervention type to load samples to.
    /// @param behavior The behavior type to load samples for.
    virtual void LoadEnteringSamples(const std::string &file,
                                     const std::string &intervention,
                                     const std::string &behavior) = 0;

    /// @brief Load the Entering Samples Matrix from a file.
    /// @param file The file path to load the entering samples from.
    virtual void LoadEnteringSamples(const std::string &file) = 0;

    /// @brief Load the OUD Transition Rates Matrix from a file.
    /// @param file The file path to load the OUD transition rates from.
    virtual void LoadOUDTransitionRates(const std::string &file) = 0;

    /// @brief Load the Intervention Transition Rates Matrix from a file.
    /// @param file The file path to load the intervention transition rates
    /// from.
    virtual void LoadInterventionInitRates(const std::string &file) = 0;

    /// @brief Load the Intervention Transition Rates Matrix from a file.
    /// @param file The file path to load the intervention transition rates
    /// from.
    virtual void LoadInterventionTransitionRates(const std::string &file) = 0;

    /// @brief Load the Overdose Rates Matrix from a file.
    /// @param file The file path to load the overdose rates from.
    virtual void LoadOverdoseRates(const std::string &file) = 0;

    /// @brief Load the Fatal Overdose Rates Matrix from a file.
    /// @param file The file path to load the fatal overdose rates from.
    virtual void LoadFatalOverdoseRates(const std::string &file) = 0;

    /// @brief Load the Mortality Rates Matrix from a file.
    /// @param smr_file The file containing the Standard Mortality Ratios (SMR).
    /// @param background_file The file containing the background mortality
    /// rates.
    virtual void LoadMortalityRates(const std::string &smr_file,
                                    const std::string &background_file) = 0;

    /// @brief Factory method to create a DataLoader instance.
    /// @param directory Directory to load data from.
    /// @param log_name Log name for the DataLoader.
    /// @return A new DataLoader instance.
    static std::unique_ptr<DataLoader>
    Create(const std::string &log_name = "console");
};
} // namespace preprocess
} // namespace respond
#endif // RESPOND_PREPROCESS_DATALOADER_HPP_
