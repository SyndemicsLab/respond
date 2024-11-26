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

#ifndef MODELS_RESPOND_HPP_
#define MODELS_RESPOND_HPP_

#include <memory>
#include <string>
#include <vector>

namespace Eigen {
    template <typename _Scalar, int _Rows, int _Cols, int _Options,
              int _MaxRows, int _MaxCols>
    class Matrix;
    using MatrixXd = Matrix<double, -1, -1, 0, -1, -1>;
    using VectorXd = Matrix<double, -1, 1, 0, -1, 1>;
} // namespace Eigen

namespace data {
    class IRespondDataStore;
} // namespace data

namespace kernels {
    class IStateTransitionModel;
} // namespace kernels

/// @brief Namespace defining all simulation Operations
namespace models {
    class IRespond {
    public:
        virtual bool Run() = 0;
        virtual bool Step() = 0;
        virtual std::shared_ptr<kernels::IStateTransitionModel>
        GetModel() const = 0;
        virtual std::shared_ptr<data::IRespondDataStore>
        GetDataStore() const = 0;

        virtual std::shared_ptr<std::vector<std::string>>
        GetInterventions() const = 0;
        virtual std::shared_ptr<std::vector<std::string>>
        GetBehaviors() const = 0;

        virtual void
        SetInterventions(const std::shared_ptr<std::vector<std::string>> &) = 0;
        virtual void
        SetBehaviors(const std::shared_ptr<std::vector<std::string>> &) = 0;
    };
} // namespace models
#endif // MODEL_SIMULATION_HPP_
