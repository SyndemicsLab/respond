#ifndef DATA_DATAFORMATTER_HPP_
#define DATA_DATAFORMATTER_HPP_

#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include <vector>

namespace Data {
    class DataFormatter {
    public:
        DataFormatter(){};
        ~DataFormatter(){};
        void extractTimesteps(std::vector<int> timesteps,
                              Data::History &history, Data::Costs &costs,
                              Data::UtilityList &utilities, bool costSwitch);

    private:
        Data::Matrix3dOverTime
        trimMatrix3dOverTime(std::vector<int> timesteps,
                             Data::Matrix3dOverTime matrix);

        Data::Matrix3dOverTime
        trimAndAddMatrix3dOverTime(std::vector<int> timesteps,
                                   Data::Matrix3dOverTime matrix);
    };
} // namespace Data

#endif