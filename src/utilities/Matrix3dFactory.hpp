#ifndef UTILITIES_MATRIX3DFACTORY_HPP_
#define UTILITIES_MATRIX3DFACTORY_HPP_

#include <array>

#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#include "DataTypes.hpp"

namespace Utilities{

    class Matrix3dFactory{
    public:
        Matrix3dFactory(){};
        static Data::Matrix3d Create(int numOUDStates, int numInterventions, int numDemographics);
    };

}

#endif
