////////////////////////////////////////////////////////////////////////////////
// File: data_formatter.hpp                                                   //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-12                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_DATAFORMATTER_HPP_
#define RESPOND_DATAOPS_DATAFORMATTER_HPP_

#include <vector>

#include <respond/data_ops/data_types.hpp>

namespace respond::data_ops {
    class DataFormatter {
    public:
        virtual ~DataFormatter() = default;
        virtual void ExtractTimesteps(const std::vector<int> &timesteps,
                                      History &history, CostList &costs,
                                      TimedMatrix3d &utilities,
                                      bool costSwitch) = 0;
    };
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_DATAFORMATTER_HPP_