#ifndef MODEL_COSTCALCULATOR_HPP_
#define MODEL_COSTCALCULATOR_HPP_

#include "DataTypes.hpp"
#include "CostLoader.hpp"
#include "UtilityLoader.hpp"
#include "Matrix3dFactory.hpp"

/// @brief Namespace for all Post Simulation Calculations
namespace Calculator{

    /// @brief Class used to Calculate Costs after the Simulation has been Run
    class CostCalculator{
    public:
        /// @brief Main Constructor to create the cost calculator
        /// @param costLoader Cost Loader Class containing Information to calculate the Cost
        /// @param utilityLoader Utility Loader Class containing Information to calculate the Utility
        /// @param history History Class containing Simulation State History
        CostCalculator(Data::CostLoader costLoader, Data::UtilityLoader utilityLoader, Data::History history);

        /// @brief Main function to calculate the Cost
        /// @return Cost Struct filled with calculated Cost
        Data::Cost calculateCost();

        /// @brief Main function to calculate the Utility
        /// @return Cost Struct filled with calculated Utility
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