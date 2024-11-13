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

#ifndef SIMULATION_RESPOND_HPP_
#define SIMULATION_RESPOND_HPP_

#include <memory>
#include <string>

namespace data {
    class IRespondDataBlock;
    class Tensor3d;
} // namespace data

/// @brief Namespace defining all simulation Operations
namespace simulation {
    class IRespond {
    public:
        virtual bool BuildModel(const std::string &db,
                                const std::string &confile, const int input_set,
                                const int parameter_set,
                                const int start_year = 2015) = 0;
        virtual bool Run() = 0;
        virtual bool Step() = 0;
        virtual std::shared_ptr<data::Tensor3d> GetState() const = 0;
        virtual bool LoadDataBlock(const std::string &) = 0;
        virtual std::shared_ptr<data::IRespondDataBlock>
        GetDataBlock() const = 0;
    };
} // namespace simulation
#endif // MODEL_SIMULATION_HPP_
