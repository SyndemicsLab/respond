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
                              Matrixify::Matrix3dOverTime &utilities,
                              bool costSwitch);

    private:
        Matrixify::Matrix3dOverTime
        trimMatrix3dOverTime(std::vector<int> timesteps,
                             Matrixify::Matrix3dOverTime matrix);

        Matrixify::Matrix3dOverTime
        trimAndAddMatrix3dOverTime(std::vector<int> timesteps,
                                   Matrixify::Matrix3dOverTime matrix);
    };
} // namespace Matrixify

#endif