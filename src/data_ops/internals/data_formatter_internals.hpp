////////////////////////////////////////////////////////////////////////////////
// File: data_formatter_internals.hpp                                         //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_DATAFORMATTERINTERNALS_HPP_
#define RESPOND_DATAOPS_DATAFORMATTERINTERNALS_HPP_

#include <respond/data_ops/data_formatter.hpp>

#include <memory>
#include <vector>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

namespace respond {
namespace data_ops {
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
} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_DATAFORMATTERINTERNALS_HPP_