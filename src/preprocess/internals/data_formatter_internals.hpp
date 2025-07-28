////////////////////////////////////////////////////////////////////////////////
// File: data_formatter_internals.hpp                                         //
// Project: internals                                                         //
// Created Date: 2025-06-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-28                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_PREPROCESS_DATAFORMATTERINTERNALS_HPP_
#define RESPOND_PREPROCESS_DATAFORMATTERINTERNALS_HPP_

#include <respond/preprocess/data_formatter.hpp>

#include <memory>
#include <vector>

#include <respond/preprocess/matrices.hpp>
#include <respond/preprocess/types.hpp>

namespace respond {
namespace preprocess {
class DataFormatterImpl : public virtual DataFormatter {
public:
    DataFormatterImpl() {};
    ~DataFormatterImpl() {};
    void ExtractTimesteps(std::vector<int> &timesteps, History &history,
                          CostList &costs, TimedMatrix3d &utilities,
                          bool costSwitch) override;

private:
    TimedMatrix3d TrimTimedMatrix3d(const std::vector<int> &timesteps,
                                    const TimedMatrix3d &matrix);

    TimedMatrix3d TrimAndAddTimedMatrix3d(const std::vector<int> &timesteps,
                                          const TimedMatrix3d &matrices);
};
} // namespace preprocess
} // namespace respond

#endif // RESPOND_PREPROCESS_DATAFORMATTERINTERNALS_HPP_