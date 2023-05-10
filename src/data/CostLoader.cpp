#include "CostLoader.hpp"

using namespace Data;


CostLoader::CostLoader(std::string inputDir) : Loader(inputDir) { }

Matrix3d CostLoader::loadHealthcareUtilizationCost(std::string csvName){

    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable table = this->inputTables[csvName];

    this->healthcareUtilizationCost = 
        Utilities::Matrix3dFactory::Create(
            numOUDStates, 
            numInterventions, 
            numDemographicCombos
        ).constant(0);
    
    if(table.find("healthcare_utilization_cost_healthcare_system") == table.end()){
        std::string message = "\'healthcare_utilization_cost_healthcare_system\' column not found in Healthcare Utilization Cost file";
        throw std::invalid_argument(message);
    }

    for (int intervention = 0; intervention < numInterventions; ++intervention) {
        for (int dem = 0; dem < numDemographicCombos; ++dem) {
            for (int oud_state = 0; oud_state < numOUDStates; ++oud_state) {
                int rowIdx = (intervention * numInterventions) + (dem * numDemographicCombos) + oud_state;
                this->healthcareUtilizationCost(intervention, oud_state, dem) = std::stod(table["healthcare_utilization_cost_healthcare_system"][rowIdx]);
            }
        }
    }
    return this->healthcareUtilizationCost;
}

std::unordered_map<std::string, double> CostLoader::loadOverdoseCost(std::string csvName){
    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return this->overdoseCostsMap;
    }
    InputTable overdoseTable = this->inputTables[csvName];

    if(overdoseTable.find("healthcare_system_cost_USD") == overdoseTable.end()){
        std::string message = "\'healthcare_system_cost_USD\' column not found in Overdose Cost file";
        throw std::invalid_argument(message);
    }

    if(overdoseTable.find("X") == overdoseTable.end()){
        std::string message = "\'X\' column not found in Overdose Cost file";
        throw std::invalid_argument(message);
    }

    for(size_t i = 0; i < overdoseTable["X"].size(); i++){
        this->overdoseCostsMap[overdoseTable["X"][i]] = std::stod(overdoseTable["healthcare_system_cost_USD"][i]);
    }
    return this->overdoseCostsMap;
}

Matrix3d CostLoader::loadPharmaceuticalCost(std::string csvName){
    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable pharmaTable = this->inputTables[csvName];

    if(pharmaTable.find("block") == pharmaTable.end()){
        std::string message = "\'block\' column not found in Overdose Cost file";
        throw std::invalid_argument(message);
    }

    if(pharmaTable.find("pharmaceutical_cost_healthcare_system") == pharmaTable.end()){
        std::string message = "\'pharmaceutical_cost_healthcare_system\' column not found in Overdose Cost file";
        throw std::invalid_argument(message);
    }

    this->loadPharmaceuticalCostMap(pharmaTable);

    this->pharmaceuticalCost = 
        Utilities::Matrix3dFactory::Create(
            numOUDStates, 
            numInterventions, 
            numDemographicCombos
        ).constant(0);

    std::vector<std::string> interventions = this->Config.getInterventions();

    for (int i = 0; i < numInterventions; ++i) {
        std::array<long int, 3> offset = {0,0,0};
        std::array<long int, 3> extent = this->pharmaceuticalCost.dimensions();
        offset[Data::INTERVENTION] = i;
        extent[Data::INTERVENTION] = 1;
        Matrix3d slice = this->pharmaceuticalCost.slice(offset, extent);
        if(this->pharmaceuticalCostsMap.find(interventions[i]) != this->pharmaceuticalCostsMap.end()){
            slice.setConstant(this->pharmaceuticalCostsMap[interventions[i]]);
        }
        else{
            slice.setConstant(0.0);
        }
        this->pharmaceuticalCost.slice(offset, extent) = slice;
    }
    return this->pharmaceuticalCost;
}

Matrix3d CostLoader::loadTreatmentUtilizationCost(std::string csvName){
    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable treatmentTable = this->inputTables[csvName];

    if(treatmentTable.find("block") == treatmentTable.end()){
        std::string message = "\'block\' column not found in Healthcare Utilization Cost file";
        throw std::invalid_argument(message);
    }

    if(treatmentTable.find("treatment_utilization_cost_healthcare_system") == treatmentTable.end()){
        std::string message = "\'treatment_utilization_cost_healthcare_system\' column not found in Healthcare Utilization Cost file";
        throw std::invalid_argument(message);
    }

    this->loadTreatmentUtilizationCostMap(treatmentTable);

    this->treatmentUtilizationCost = 
        Utilities::Matrix3dFactory::Create(
            numOUDStates, 
            numInterventions, 
            numDemographicCombos
        ).constant(0);

    std::vector<std::string> interventions = this->Config.getInterventions();

    for (int i = 0; i < numInterventions; ++i) {
        std::array<long int, 3> offset = {0,0,0};
        std::array<long int, 3> extent = this->treatmentUtilizationCost.dimensions();
        offset[Data::INTERVENTION] = i;
        extent[Data::INTERVENTION] = 1;
        Matrix3d slice = this->treatmentUtilizationCost.slice(offset, extent);
        if(this->treatmentUtilizationCostMap.find(interventions[i]) != this->treatmentUtilizationCostMap.end()){
            slice.setConstant(this->treatmentUtilizationCostMap[interventions[i]]);
        }
        else{
            slice.setConstant(0.0);
        }
        this->treatmentUtilizationCost.slice(offset, extent) = slice;
    }
    return this->treatmentUtilizationCost;
}

double CostLoader::getNonFatalOverdoseCost(){
    if(this->overdoseCostsMap.find("non_fatal_overdose") == this->overdoseCostsMap.end()){
        return 0.0;
    }
    return this->overdoseCostsMap["non_fatal_overdose"];
}

double CostLoader::getFatalOverdoseCost(){
    if(this->overdoseCostsMap.find("fatal_overdose") == this->overdoseCostsMap.end()){
        return 0.0;
    }
    return this->overdoseCostsMap["fatal_overdose"];
}


std::unordered_map<std::string, double> CostLoader::loadPharmaceuticalCostMap(InputTable table){
    for(size_t i = 0; i < table["block"].size(); i++){
        this->pharmaceuticalCostsMap[table["block"][i]] = std::stod(table["pharmaceutical_cost_healthcare_system"][i]);
    }
    return this->pharmaceuticalCostsMap;
}

std::unordered_map<std::string, double> CostLoader::loadTreatmentUtilizationCostMap(InputTable table){
    for(size_t i = 0; i < table["block"].size(); i++){
        this->treatmentUtilizationCostMap[table["block"][i]] = std::stod(table["treatment_utilization_cost_healthcare_system"][i]);
    }
    return this->treatmentUtilizationCostMap;
}
