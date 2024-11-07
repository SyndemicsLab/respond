//===-- simulation.hpp - simulation class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the simulation class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef MODEL_SIMULATION_HPP_
#define MODEL_SIMULATION_HPP_

#include <cstdint>
#include <memory>
#include <vector>

namespace matrixify {
    class Matrix3d;
}

/// @brief Namespace defining all simulation Operations
namespace simulation {

    /// @brief Interface for all simulations
    class IRespond {
    public:
        virtual int Run() = 0;
        virtual std::uint64_t GetCurrentTime() const = 0;
        virtual std::uint64_t GetDuration() const = 0;
        virtual int LoadDatabase(std::string &const db_file) = 0;
        virtual int LoadConfig(std::string &const conf_file) = 0;
        virtual std::vector<std::shared_ptr<matrixify::Matrix3d>>
        GetStateHistory() const = 0;
        virtual std::shared_ptr<matrixify::Matrix3d>
        GetCurrentState() const = 0;
        virtual void ShiftAges() = 0;
    };
} // namespace simulation
#endif // MODEL_SIMULATION_HPP_
