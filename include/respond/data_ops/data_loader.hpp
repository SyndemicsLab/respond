////////////////////////////////////////////////////////////////////////////////
// File: data_loader.hpp                                                      //
// Project: data_ops                                                          //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-05-27                                                  //
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

namespace respond {
namespace data_ops {

/// @brief
class DataLoader {
public:
    /// @brief
    virtual ~DataLoader() = default;

    /// @brief
    /// @return
    virtual Matrix3d GetInitialSample() const = 0;

    /// @brief
    /// @param time
    /// @return
    virtual Matrix3d GetEnteringSamples(const int &time) const = 0;

    /// @brief
    /// @return
    virtual Matrix3d GetOUDTransitionRates() const = 0;

    /// @brief
    /// @param time
    /// @return
    virtual Matrix3d GetInterventionTransitionRates(const int &time) const = 0;

    /// @brief
    /// @param time
    /// @return
    virtual Matrix3d GetOverdoseRates(const int &time) const = 0;

    /// @brief
    /// @param time
    /// @return
    virtual Matrix3d GetFatalOverdoseRates(const int &time) const = 0;

    /// @brief
    /// @return
    virtual Matrix3d GetMortalityRates() const = 0;

    /// @brief
    /// @return
    virtual Matrix3d GetInterventionInitRates() const = 0;

    /// @brief
    /// @param mat
    virtual void SetInitialSample(const Matrix3d &mat) = 0;

    /// @brief
    /// @param mat
    virtual void SetEnteringSamples(const TimedMatrix3d &mat) = 0;

    /// @brief
    /// @param mat
    virtual void SetOUDTransitionRates(const Matrix3d &mat) = 0;

    /// @brief
    /// @param mat
    virtual void SetInterventionTransitionRates(const TimedMatrix3d &mat) = 0;

    /// @brief
    /// @param mat
    virtual void SetOverdoseRates(const TimedMatrix3d &mat) = 0;

    /// @brief
    /// @param mat
    virtual void SetFatalOverdoseRates(const TimedMatrix3d &mat) = 0;

    /// @brief
    /// @param mat
    virtual void SetMortalityRates(const Matrix3d &mat) = 0;

    /// @brief
    /// @param mat
    virtual void SetInterventionInitRates(const Matrix3d &mat) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual Matrix3d LoadInitialSample(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @param intervention
    /// @param behavior
    /// @return
    virtual TimedMatrix3d LoadEnteringSamples(const std::string &file,
                                              const std::string &intervention,
                                              const std::string &behavior) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual TimedMatrix3d LoadEnteringSamples(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual Matrix3d LoadOUDTransitionRates(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual Matrix3d LoadInterventionInitRates(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual TimedMatrix3d
    LoadInterventionTransitionRates(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual TimedMatrix3d LoadOverdoseRates(const std::string &file) = 0;

    /// @brief
    /// @param file
    /// @return
    virtual TimedMatrix3d LoadFatalOverdoseRates(const std::string &file) = 0;

    /// @brief
    /// @param smr_file
    /// @param background_file
    /// @return
    virtual Matrix3d LoadMortalityRates(const std::string &smr_file,
                                        const std::string &background_file) = 0;

    /// @brief
    /// @return
    virtual Data::IConfigablePtr GetConfig() const = 0;

    /// @brief
    /// @param directory
    /// @param log_name
    /// @return
    static std::unique_ptr<DataLoader>
    Create(const std::string &directory = "",
           const std::string &log_name = "console");
};
} // namespace data_ops
} // namespace respond
#endif // RESPOND_DATAOPS_DATALOADER_HPP_
