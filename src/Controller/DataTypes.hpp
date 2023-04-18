#ifndef CONTROLLER_DATATYPES_HPP_
#define CONTROLLER_DATATYPES_HPP_

#include <vector>
#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

namespace Data{
    using Matrix3d = Eigen::Tensor<float, 3>;
    using Matrix3dOverTime = std::vector<Matrix3d>;

    /// @brief Structure to contain the entire history of the model including state, overdose, and mortality rates
    typedef struct History{
        Matrix3dOverTime stateHistory;
        Matrix3dOverTime overdoseHistory;
        Matrix3dOverTime mortalityHistory;
    }History;
}

#endif