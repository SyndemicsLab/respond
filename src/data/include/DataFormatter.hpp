#ifndef DATA_DATAFORMATTER_HPP_
#define DATA_DATAFORMATTER_HPP_

#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include <vector>

namespace matrixify {
    class DataFormatter {
    public:
        DataFormatter() {};
        ~DataFormatter() {};
        void extractTimesteps(std::vector<int> timesteps,
                              matrixify::History &history,
                              matrixify::CostList &costs,
                              matrixify::Matrix4d &utilities, bool costSwitch);

    private:
        matrixify::Matrix4d trimMatrix4d(std::vector<int> timesteps,
                                         matrixify::Matrix4d matrix);

        matrixify::Matrix4d trimAndAddMatrix4d(std::vector<int> timesteps,
                                               matrixify::Matrix4d matrix);
    };
} // namespace matrixify

#endif