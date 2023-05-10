#ifndef UTILITIES_MATRIX3DPRINTER_HPP_
#define UTILITIES_MATRIX3DPRINTER_HPP_

#include <array>

#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#include "DataTypes.hpp"

namespace Utilities{

    class Matrix3dPrinter{
    public:
        Matrix3dPrinter(){};
        static void Print(Data::Matrix3d mat, std::ostream &stream);
        static void PrintOverTime(Data::Matrix3dOverTime matOverTime, std::ostream &stream);
    };

}

#endif
