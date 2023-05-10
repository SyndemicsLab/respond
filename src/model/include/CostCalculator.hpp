#ifndef MODEL_COSTCALCULATOR_HPP_
#define MODEL_COSTCALCULATOR_HPP_

#include "DataTypes.hpp"
#include "CostLoader.hpp"
#include "UtilityLoader.hpp"
#include "Matrix3dFactory.hpp"

namespace Calculator{

    /// @brief 
    class CostCalculator{
    public:
        /// @brief 
        /// @param costLoader 
        /// @param utilityLoader
        /// @param history 
        CostCalculator(Data::CostLoader costLoader, Data::UtilityLoader utilityLoader, Data::History history);

        /// @brief 
        /// @return 
        Data::Cost calculateCost();
        Data::Utility calculateUtility();

    private:
        Data::History history;
        Data::CostLoader costLoader;
        Data::UtilityLoader utilityLoader;

        Data::Matrix3dOverTime calculateOverdoseCost(Data::Matrix3dOverTime overdose, double cost);
        Data::Matrix3dOverTime calculateCost(Data::Matrix3dOverTime state, Data::Matrix3d cost);
        
    };
}

#endif