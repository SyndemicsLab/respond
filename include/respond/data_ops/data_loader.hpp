////////////////////////////////////////////////////////////////////////////////
// File: data_loader.hpp                                                      //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_DATALOADER_HPP_
#define RESPOND_DATAOPS_DATALOADER_HPP_

#include <memory>
#include <string>

#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/data_types.hpp>

namespace respond {
namespace data_ops {

/// @brief Class for Loading and Managing Data in RESPOND.
class DataLoader {
public:
    /// @brief Default Desctructor for the DataLoader class.
    virtual ~DataLoader() = default;

    /// @brief Get the Initial Cohort Matrix.
    /// @return The Initial Cohort.
    virtual Matrix3d GetInitialSample() const = 0;

    /// @brief Get the Entering Sample Matrix for a given time.
    /// @param time Time point for which to retrieve the entering samples.
    /// @return The Entering Sample Matrix at the specified time.
    virtual Matrix3d GetEnteringSamples(const int &time) const = 0;

    /// @brief Get the OUD Transition Rates Matrix.
    /// @return The OUD Transition Rates Matrix.
    virtual Matrix3d GetOUDTransitionRates() const = 0;

    /// @brief Get the Intervention Transition Rates Matrix for the specified
    /// time.
    /// @param time Time point for which to retrieve the intervention transition
    /// rates.
    /// @return The Intervention Transition Rates Matrix at the specified time.
    virtual Matrix3d GetInterventionTransitionRates(const int &time) const = 0;

    /// @brief Get the Overdose Rates Matrix for the specified time.
    /// @param time Time point for which to retrieve the overdose rates.
    /// @return The Overdose Rates Matrix at the specified time.
    virtual Matrix3d GetOverdoseRates(const int &time) const = 0;

    /// @brief Get the Fatal Overdose Rates Matrix for the specified time.
    /// @param time Time point for which to retrieve the fatal overdose rates.
    /// @return The Fatal Overdose Rates Matrix at the specified time.
    virtual Matrix3d GetFatalOverdoseRates(const int &time) const = 0;

    /// @brief Get the Mortality Rates Matrix.
    /// @return The Mortality Rates Matrix.
    virtual Matrix3d GetMortalityRates() const = 0;

    /// @brief Get the Intervention Initial Rates Matrix.
    /// @return The Intervention Initial Rates Matrix.
    virtual Matrix3d GetInterventionInitRates() const = 0;

    /// @brief Set the Initial Sample Matrix.
    /// @param mat The Initial Sample Matrix to set.
    virtual void SetInitialSample(const Matrix3d &mat) = 0;

    /// @brief Set the Entering Samples Matrix.
    /// @param mat The Entering Samples Matrix to set.
    virtual void SetEnteringSamples(const TimedMatrix3d &mat) = 0;

    /// @brief Set the OUD Transition Rates Matrix.
    /// @param mat The OUD Transition Rates Matrix to set.
    virtual void SetOUDTransitionRates(const Matrix3d &mat) = 0;

    /// @brief Set the Intervention Transition Rates Matrix.
    /// @param mat The Intervention Transition Rates Matrix to set.
    virtual void SetInterventionTransitionRates(const TimedMatrix3d &mat) = 0;

    /// @brief Set the Overdose Rates Matrix.
    /// @param mat The Overdose Rates Matrix to set.
    virtual void SetOverdoseRates(const TimedMatrix3d &mat) = 0;

    /// @brief Set the Fatal Overdose Rates Matrix.
    /// @param mat The Fatal Overdose Rates Matrix to set.
    virtual void SetFatalOverdoseRates(const TimedMatrix3d &mat) = 0;

    /// @brief Set the Mortality Rates Matrix.
    /// @param mat The Mortality Rates Matrix to set.
    virtual void SetMortalityRates(const Matrix3d &mat) = 0;

    /// @brief Set the Intervention Initial Rates Matrix.
    /// @param mat The Intervention Initial Rates Matrix to set.
    virtual void SetInterventionInitRates(const Matrix3d &mat) = 0;

    /// @brief Load the Initial Sample Matrix from a file.
    /// @param file The file path to load the initial sample from.
    /// @return The Initial Sample Matrix loaded from the file.
    virtual Matrix3d LoadInitialSample(const std::string &file) = 0;

    /// @brief Load the Entering Samples Matrix from a file.
    /// @param file The file path to load the entering samples from.
    /// @param intervention The intervention type to load samples to.
    /// @param behavior The behavior type to load samples for.
    /// @return The Entering Samples Matrix.
    virtual TimedMatrix3d LoadEnteringSamples(const std::string &file,
                                              const std::string &intervention,
                                              const std::string &behavior) = 0;

    /// @brief Load the Entering Samples Matrix from a file.
    /// @param file The file path to load the entering samples from.
    /// @return The Entering Samples Matrix.
    virtual TimedMatrix3d LoadEnteringSamples(const std::string &file) = 0;

    /// @brief Load the OUD Transition Rates Matrix from a file.
    /// @param file The file path to load the OUD transition rates from.
    /// @return The OUD Transition Rates Matrix loaded from the file.
    virtual Matrix3d LoadOUDTransitionRates(const std::string &file) = 0;

    /// @brief Load the Intervention Transition Rates Matrix from a file.
    /// @param file The file path to load the intervention transition rates
    /// from.
    /// @return The Intervention Transition Rates Matrix loaded from the file.
    virtual Matrix3d LoadInterventionInitRates(const std::string &file) = 0;

    /// @brief Load the Intervention Transition Rates Matrix from a file.
    /// @param file The file path to load the intervention transition rates
    /// from.
    /// @return The Intervention Transition Rates Matrix loaded from the file.
    virtual TimedMatrix3d
    LoadInterventionTransitionRates(const std::string &file) = 0;

    /// @brief Load the Overdose Rates Matrix from a file.
    /// @param file The file path to load the overdose rates from.
    /// @return The Overdose Rates Matrix loaded from the file.
    virtual TimedMatrix3d LoadOverdoseRates(const std::string &file) = 0;

    /// @brief Load the Fatal Overdose Rates Matrix from a file.
    /// @param file The file path to load the fatal overdose rates from.
    /// @return The Fatal Overdose Rates Matrix loaded from the file.
    virtual TimedMatrix3d LoadFatalOverdoseRates(const std::string &file) = 0;

    /// @brief Load the Mortality Rates Matrix from a file.
    /// @param smr_file The file containing the Standard Mortality Ratios (SMR).
    /// @param background_file The file containing the background mortality
    /// rates.
    /// @return The Mortality Rates Matrix from the files.
    virtual Matrix3d LoadMortalityRates(const std::string &smr_file,
                                        const std::string &background_file) = 0;

    /// @brief Getter for the Configuration object.
    /// @return Configuration Object.
    virtual Data::IConfigablePtr GetConfig() const = 0;

    /// @brief Factory method to create a DataLoader instance.
    /// @param directory Directory to load data from.
    /// @param log_name Log name for the DataLoader.
    /// @return A new DataLoader instance.
    static std::unique_ptr<DataLoader>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};
} // namespace data_ops
} // namespace respond
#endif // RESPOND_DATAOPS_DATALOADER_HPP_
