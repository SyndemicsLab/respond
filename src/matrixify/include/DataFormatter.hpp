#ifndef DATA_DATAFORMATTER_HPP_
#define DATA_DATAFORMATTER_HPP_

#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include <vector>

namespace Matrixify {
    class DataFormatter {
    public:
        DataFormatter(){};
        ~DataFormatter(){};
        void extractTimesteps(std::vector<int> timesteps,
                              Matrixify::History &history,
                              Matrixify::CostList &costs,
                              Matrixify::Matrix4d &utilities,
                              bool costSwitch);

    private:
        Matrixify::Matrix4d
        trimMatrix4d(std::vector<int> timesteps,
                             Matrixify::Matrix4d matrix);

        Matrixify::Matrix4d
        trimAndAddMatrix4d(std::vector<int> timesteps,
                                   Matrixify::Matrix4d matrix);
    };
} // namespace Matrixify

#endif