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
    class IDataBlock;
    class Tensor3d;
} // namespace data

/// @brief Namespace defining all simulation Operations
namespace simulation {
    class IRespond {
    public:
        virtual bool BuildModel() = 0;
        virtual bool LoadDataBlock(std::string &const) = 0;
        virtual std::shared_ptr<data::IDataBlock> GetDataBlock() const = 0;
        virtual void
            AddMigratingCohort(std::shared_ptr<data::Tensor3d>,
                               std::shared_ptr<data::IDataBlock>) const = 0;
        virtual void MultiplyBehaviorTransitions(
            std::shared_ptr<data::Tensor3d>,
            std::shared_ptr<data::IDataBlock>) const = 0;
        virtual void MultiplyInterventionTransitions(
            std::shared_ptr<data::Tensor3d>,
            std::shared_ptr<data::IDataBlock>) const = 0;
        virtual void MultiplyBehaviorTransitionsAfterInterventionChange(
            std::shared_ptr<data::Tensor3d>,
            std::shared_ptr<data::IDataBlock>) const = 0;
        virtual void MultiplyOverdoseProbabilities(
            std::shared_ptr<data::Tensor3d>,
            std::shared_ptr<data::IDataBlock>) const = 0;
        virtual void MultiplyProbabilitiesOfFatalityGivenOverdose(
            std::shared_ptr<data::Tensor3d>,
            std::shared_ptr<data::IDataBlock>) const = 0;
        virtual void MultiplyBackgroundMortalityProbabilities(
            std::shared_ptr<data::Tensor3d>,
            std::shared_ptr<data::IDataBlock>) const = 0;
    };
} // namespace simulation
#endif // MODEL_SIMULATION_HPP_
