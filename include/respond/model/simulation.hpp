////////////////////////////////////////////////////////////////////////////////
// File: Simulation.hpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_MODEL_SIMULATION_HPP_
#define RESPOND_MODEL_SIMULATION_HPP_

#include <respond/data_ops/cost_loader.hpp>
#include <respond/data_ops/data_loader.hpp>
#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/utility_loader.hpp>

namespace respond::model {
    class Respond {
    public:
        virtual ~Respond() = default;
        virtual void Run(const respond::data_ops::DataLoader &data_loader,
                         const respond::data_ops::CostLoader &cost_loader,
                         respond::data_ops::UtilityLoader &utility_loader) = 0;
        virtual respond::data_ops::History GetHistory() const = 0;
    };
} // namespace respond::model
#endif // RESPOND_MODEL_SIMULATION_HPP_
