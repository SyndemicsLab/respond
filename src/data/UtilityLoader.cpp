#include "UtilityLoader.hpp"

using namespace Data;

UtilityLoader::UtilityLoader(std::string inputDir) : Loader(inputDir) { }

Configuration UtilityLoader::loadConfigurationFile(std::string configPath){
    return Loader::loadConfigurationFile(configPath);
}

Matrix3d UtilityLoader::loadBackgroundUtility(std::string csvName){
    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable table = this->inputTables[csvName];

    this->backgroundUtility = 
        Utilities::Matrix3dFactory::Create(
            numOUDStates, 
            numInterventions, 
            numDemographicCombos
        ).constant(0);
    
    if(table.find("utility") == table.end()){
        std::string message = "\'utility\' column not found in Background Utility file";
        throw std::invalid_argument(message);
    }

    for (int dem = 0; dem < numDemographicCombos; ++dem) {
        std::array<long int, 3> offset = {0,0,0};
        std::array<long int, 3> extent = this->backgroundUtility.dimensions();
        offset[Data::DEMOGRAPHIC_COMBO] = dem;
        extent[Data::DEMOGRAPHIC_COMBO] = 1;
        Matrix3d temp = this->backgroundUtility.slice(offset, extent);
        temp.setConstant(std::stod(table["utility"][dem]));
        this->backgroundUtility.slice(offset, extent) = temp;
    }
    return this->backgroundUtility;
}

Matrix3d UtilityLoader::loadOUDUtility(std::string csvName){
    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable table = this->inputTables[csvName];

    this->oudUtility = 
        Utilities::Matrix3dFactory::Create(
            numOUDStates, 
            numInterventions, 
            numDemographicCombos
        ).constant(0);
    
    if(table.find("utility") == table.end()){
        std::string message = "\'utility\' column not found in OUD Utility file";
        throw std::invalid_argument(message);
    }
    for (int intervention = 0; intervention < numInterventions; intervention++){
        for (int oud = 0; oud < numOUDStates; ++oud) {
            std::array<long int, 3> offset = {0,0,0};
            std::array<long int, 3> extent = this->oudUtility.dimensions();
            offset[Data::OUD] = oud;
            offset[Data::INTERVENTION] = intervention;
            extent[Data::OUD] = 1;
            extent[Data::INTERVENTION] = 1;
            Matrix3d temp = this->oudUtility.slice(offset, extent);
            temp.setConstant(std::stod(table["utility"][(intervention * numInterventions) + oud]));
            this->oudUtility.slice(offset, extent) = temp;
        }
    }
    
    return this->oudUtility;
}

Matrix3d UtilityLoader::loadSettingUtility(std::string csvName){
    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable table = this->inputTables[csvName];

    this->settingUtility = 
        Utilities::Matrix3dFactory::Create(
            numOUDStates, 
            numInterventions, 
            numDemographicCombos
        ).constant(0);
    
    if(table.find("utility") == table.end()){
        std::string message = "\'utility\' column not found in Setting Utility file";
        throw std::invalid_argument(message);
    }

    for (int intervention = 0; intervention < numInterventions; ++intervention) {
        std::array<long int, 3> offset = {0,0,0};
        std::array<long int, 3> extent = this->settingUtility.dimensions();
        offset[Data::INTERVENTION] = intervention;
        extent[Data::INTERVENTION] = 1;
        Matrix3d temp = this->settingUtility.slice(offset, extent);
        temp.setConstant(std::stod(table["utility"][intervention]));
        this->settingUtility.slice(offset, extent) = temp;
    }
    return this->settingUtility;
}
