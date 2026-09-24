////////////////////////////////////////////////////////////////////////////////
// File: eigen_config.hpp                                                     //
// Project: respond                                                           //
// Created Date: 2026-09-24                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-09-24                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_EIGEN_CONFIG_HPP_
#define RESPOND_EIGEN_CONFIG_HPP_

#include <mutex>

#include <Eigen/Core>

namespace respond {
namespace detail {

/// @brief Returns the mutex coordinating Eigen configuration and execution.
inline std::mutex &GetEigenExecutionMutex() {
    static std::mutex mutex;
    return mutex;
}

/// @brief Sets Eigen's process-global worker count under shared coordination.
inline void SetEigenThreads(unsigned int thread_count) {
    std::lock_guard<std::mutex> lock(GetEigenExecutionMutex());
    Eigen::setNbThreads(thread_count);
}

} // namespace detail
} // namespace respond

#endif // RESPOND_EIGEN_CONFIG_HPP_