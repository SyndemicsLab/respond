#ifndef DATA_TENSOR3D_HPP_
#define DATA_TENSOR3D_HPP_

#include <unsupported/Eigen/CXX11/Tensor>

namespace data {
    class Tensor3d : public Eigen::Tensor<double, 3> {
        using Eigen::Tensor<double, 3>::Tensor;
    };
} // namespace data

#endif