////////////////////////////////////////////////////////////////////////////////
// File: data_formatter.hpp                                                   //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-12                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_DATAFORMATTER_HPP_
#define RESPOND_DATAOPS_DATAFORMATTER_HPP_

#include <memory>
#include <vector>

#include <respond/data_ops/data_types.hpp>

namespace respond::data_ops {
    class DataFormatter {
    public:
        virtual ~DataFormatter() = default;
        virtual void ExtractTimesteps(std::vector<int> &timesteps,
                                      History &history, CostList &costs,
                                      TimedMatrix3d &utilities,
                                      bool costSwitch) = 0;

        static std::unique_ptr<DataFormatter> Create();
    };
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_DATAFORMATTER_HPP_