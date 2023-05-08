#ifndef MODEL_COSTCALCULATOR_HPP_
#define MODEL_COSTCALCULATOR_HPP_

#include "DataTypes.hpp"
#include "CostLoader.hpp"
#include "Matrix3dFactory.hpp"

namespace Calculator{

    /// @brief 
    class CostCalculator{
    public:
        /// @brief 
        /// @param costLoader 
        /// @param history 
        CostCalculator(Data::CostLoader costLoader, Data::History history);

        /// @brief 
        /// @return 
        Data::Cost calculate();

    private:
        Data::History history;
        Data::CostLoader costLoader;

        Data::Matrix3dOverTime calculateOverdoseCost(Data::Matrix3dOverTime overdose, double cost);
        Data::Matrix3dOverTime calculateCost(Data::Matrix3dOverTime state, Data::Matrix3d cost);
        
    };
}

#endif