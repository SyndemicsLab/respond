#ifndef UTILITIES_MATRIX3DPRINTER_HPP_
#define UTILITIES_MATRIX3DPRINTER_HPP_

#include <array>

#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#include "DataTypes.hpp"

namespace Utilities{

    /// @brief Helper Class to Cleanly Print Matrix3d Objects
    class Matrix3dPrinter{
    public:
        Matrix3dPrinter(){};

        /// @brief Static Function to Print Standard Matrix3d
        /// @param mat Matrix3d to Print
        /// @param stream Stream to Print to
        static void Print(Data::Matrix3d mat, std::ostream &stream);

        /// @brief Static Function to Print Matrix3dOverTime object
        /// @param matOverTime Matrix3dOverTime to Print
        /// @param stream Stream to Print to
        static void PrintOverTime(Data::Matrix3dOverTime matOverTime, std::ostream &stream);
    };

}

#endif
