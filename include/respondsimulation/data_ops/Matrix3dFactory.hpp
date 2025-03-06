////////////////////////////////////////////////////////////////////////////////
// File: Matrix3dFactory.hpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef UTILITIES_MATRIX3DFACTORY_HPP_
#define UTILITIES_MATRIX3DFACTORY_HPP_

#include <array>

#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

#include <respondsimulation/data_ops/DataTypes.hpp>

/// @brief Namespace containing all Helper Utility Classes
namespace data_ops {

    /// @brief Factory to create a Matrix3d Object
    class Matrix3dFactory {
    public:
        Matrix3dFactory() {};

        /// @brief Main Static Function used to generate Matrix3d Objects
        /// @param numOUDStates Integer specifying number of OUD States
        /// @param numInterventions Integer specifying number of Interventions
        /// @param numDemographics Integer specifying number of Demographic
        /// Combinations
        /// @return New Matrix3d Object of specified dimensions
        static data_ops::Matrix3d Create(const int numOUDStates,
                                         const int numInterventions,
                                         const int numDemographics);
    };

} // namespace data_ops

#endif
