////////////////////////////////////////////////////////////////////////////////
// File: Writer.hpp                                                           //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_WRITER_HPP_
#define RESPOND_DATAOPS_WRITER_HPP_

#include <memory>
#include <string>

#include <respond/data_ops/data_loader.hpp>
#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>

namespace respond {
namespace data_ops {

enum class WriterType : int {
    kInput = 0,
    kOutput = 1,
    kHistory = 2,
    kCost = 3,
    kUtilities = 4,
    kTotals = 5,
    kCount = 6
};

enum class OutputType : int { kString = 0, kFile = 1, kCount = 2 };

class Writer {
public:
    virtual ~Writer() = default;
    virtual std::string WriteInputData(const DataLoader &data_loader,
                                       const std::string &directory,
                                       const OutputType output_type) const = 0;
    virtual std::string
    WriteHistoryData(const History &history, const std::string &directory,
                     const OutputType output_type) const = 0;
    virtual std::string WriteCostData(const CostList &cost_list,
                                      const std::string &directory,
                                      const OutputType output_type) const = 0;
    virtual std::string
    WriteUtilityData(const TimedMatrix3d &utilities,
                     const std::string &directory,
                     const OutputType output_type) const = 0;
    virtual std::string WriteTotalsData(const Totals &totals,
                                        const std::string &directory,
                                        const OutputType output_type) const = 0;

    static std::unique_ptr<Writer>
    Create(const Data::IConfigablePtr &cfg,
           const std::string &log_name = "console");
};

} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_WRITER_HPP_