#ifndef UTILITIES_MATRIX3DFACTORY_HPP_
#define UTILITIES_MATRIX3DFACTORY_HPP_

#include <array>

#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#include "DataTypes.hpp"

/// @brief Namespace containing all Helper Utility Classes
namespace Utilities{

    /// @brief Factory to create a Matrix3d Object
    class Matrix3dFactory{
    public:
        Matrix3dFactory(){};

        /// @brief Main Static Function used to generate Matrix3d Objects
        /// @param numOUDStates Integer specifying number of OUD States
        /// @param numInterventions Integer specifying number of Interventions
        /// @param numDemographics Integer specifying number of Demographic Combinations
        /// @return New Matrix3d Object of specified dimensions
        static Data::Matrix3d Create(int numOUDStates, int numInterventions, int numDemographics);
    };

}

#endif
