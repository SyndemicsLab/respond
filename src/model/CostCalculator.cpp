#include "CostCalculator.hpp"

using namespace Calculator;

CostCalculator::CostCalculator(Data::CostLoader costLoader, Data::UtilityLoader utilityLoader, Data::History history){
    this->history = history;
    this->costLoader = costLoader;
    this->utilityLoader = utilityLoader;
}

Data::Cost CostCalculator::calculateCost(){
    Data::Cost cost;
    cost.healthcareCost = this->calculateCost(this->history.stateHistory, this->costLoader.getHealthcareUtilizationCost());
    cost.pharmaCost = this->calculateCost(this->history.stateHistory, this->costLoader.getPharmaceuticalCost());
    cost.treatmentCost = this->calculateCost(this->history.stateHistory, this->costLoader.getTreatmentUtilizationCost());
    cost.nonFatalOverdoseCost = this->calculateOverdoseCost(this->history.overdoseHistory, this->costLoader.getNonFatalOverdoseCost());
    cost.fatalOverdoseCost = this->calculateOverdoseCost(this->history.fatalOverdoseHistory, this->costLoader.getFatalOverdoseCost());
    return cost;
}

Data::Utility CostCalculator::calculateUtility(){
    Data::Utility util;
    util.backgroundUtility = this->calculateCost(this->history.stateHistory, this->utilityLoader.getBackgroundUtility());
    util.oudUtility = this->calculateCost(this->history.stateHistory, this->utilityLoader.getOUDUtility());
    util.settingUtility = this->calculateCost(this->history.stateHistory, this->utilityLoader.getSettingUtility());
    return util;
}

Data::Matrix3dOverTime CostCalculator::calculateOverdoseCost(Data::Matrix3dOverTime overdose, double cost){
    std::vector<Data::Matrix3d> overdoseVec = overdose.getMatrices();
    std::vector<Data::Matrix3d> result;

    for(Data::Matrix3d timeMat : overdoseVec){
        Data::Matrix3d costMat = Utilities::Matrix3dFactory::Create(
            timeMat.dimension(Data::OUD), 
            timeMat.dimension(Data::INTERVENTION), 
            timeMat.dimension(Data::DEMOGRAPHIC_COMBO)).constant(cost);

        result.push_back(timeMat * costMat);
    }

    Data::Matrix3dOverTime ret(result);
    return ret;
}

Data::Matrix3dOverTime CostCalculator::calculateCost(Data::Matrix3dOverTime state, Data::Matrix3d cost){
    std::vector<Data::Matrix3d> result;
    std::vector<Data::Matrix3d> stateVec = state.getMatrices();
    for(Data::Matrix3d timeMat : stateVec){
        result.push_back(timeMat * cost);
    }

    Data::Matrix3dOverTime ret(result);
    return ret;
}
