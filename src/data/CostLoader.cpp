//===-- CostLoader.cpp - CostLoader class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the CostLoader class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "CostLoader.hpp"

using namespace Data;

CostLoader::CostLoader(std::string inputDir) : Loader(inputDir) {}

Configuration CostLoader::loadConfigurationFile(std::string configPath) {
    return Loader::loadConfigurationFile(configPath);
}

Matrix3d CostLoader::loadHealthcareUtilizationCost(std::string csvName) {

    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable table = this->inputTables[csvName];

    this->healthcareUtilizationCost =
        Utilities::Matrix3dFactory::Create(numOUDStates, numInterventions,
                                           numDemographicCombos)
            .constant(0);

    ASSERTM(table.find("healthcare_utilization_cost_healthcare_system") != table.end(), "\'healthcare_utilization_cost_healthcare_system\' Column Successfully Found");

    for (int intervention = 0; intervention < numInterventions;
         ++intervention) {
        for (int dem = 0; dem < numDemographicCombos; ++dem) {
            for (int oud_state = 0; oud_state < numOUDStates; ++oud_state) {
                int rowIdx = ((intervention * numInterventions) +
                             (dem * numDemographicCombos) + oud_state);
                std::vector<std::string> v = table["healthcare_utilization_cost_healthcare_system"];
                double t = std::stod(v[rowIdx]);
                this->healthcareUtilizationCost(intervention, oud_state, dem) = t;
            } 
        }
    }
    return this->healthcareUtilizationCost;
}

std::unordered_map<std::string, double>
CostLoader::loadOverdoseCost(std::string csvName) {
    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        return this->overdoseCostsMap;
    }
    InputTable overdoseTable = this->inputTables[csvName];

    ASSERTM(overdoseTable.find("healthcare_system_cost_USD") !=
                overdoseTable.end(),
            "healthcare_system_cost_USD Column Successfully Found");

    ASSERTM(overdoseTable.find("X") != overdoseTable.end(),
            "\'X\' Column Successfully Found");

    for (size_t i = 0; i < overdoseTable["X"].size(); i++) {
        this->overdoseCostsMap[overdoseTable["X"][i]] =
            std::stod(overdoseTable["healthcare_system_cost_USD"][i]);
    }
    return this->overdoseCostsMap;
}

Matrix3d CostLoader::loadPharmaceuticalCost(std::string csvName) {
    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable pharmaTable = this->inputTables[csvName];

    ASSERTM(pharmaTable.find("block") != pharmaTable.end(),
            "\'block\' Column Successfully Found");

    ASSERTM(
        pharmaTable.find("pharmaceutical_cost_healthcare_system") !=
            pharmaTable.end(),
        "\'pharmaceutical_cost_healthcare_system\' Column Successfully Found");

    this->loadPharmaceuticalCostMap(pharmaTable);

    this->pharmaceuticalCost =
        Utilities::Matrix3dFactory::Create(numOUDStates, numInterventions,
                                           numDemographicCombos)
            .constant(0);

    std::vector<std::string> interventions = this->Config.getInterventions();

    for (int i = 0; i < numInterventions; ++i) {
        Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
        Eigen::array<Eigen::Index, 3> extent = this->pharmaceuticalCost.dimensions();
        offset[Data::INTERVENTION] = i;
        extent[Data::INTERVENTION] = 1;
        Matrix3d slice = this->pharmaceuticalCost.slice(offset, extent);
        if (this->pharmaceuticalCostsMap.find(interventions[i]) !=
            this->pharmaceuticalCostsMap.end()) {
            slice.setConstant(this->pharmaceuticalCostsMap[interventions[i]]);
        } else {
            slice.setConstant(0.0);
        }
        this->pharmaceuticalCost.slice(offset, extent) = slice;
    }
    return this->pharmaceuticalCost;
}

Matrix3d CostLoader::loadTreatmentUtilizationCost(std::string csvName) {
    size_t numOUDStates = this->Config.getOUDStates().size();
    size_t numDemographicCombos = this->Config.getNumDemographicCombos();
    size_t numInterventions = this->Config.getInterventions().size();

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable treatmentTable = this->inputTables[csvName];

    ASSERTM(treatmentTable.find("block") != treatmentTable.end(),
            "\'block\' Column Successfully Found");

    ASSERTM(
        treatmentTable.find("treatment_utilization_cost_healthcare_system") !=
            treatmentTable.end(),
        "\'treatment_utilization_cost_healthcare_system\' Column Successfully "
        "Found");

    this->loadTreatmentUtilizationCostMap(treatmentTable);

    this->treatmentUtilizationCost =
        Utilities::Matrix3dFactory::Create(numOUDStates, numInterventions,
                                           numDemographicCombos)
            .constant(0);

    std::vector<std::string> interventions = this->Config.getInterventions();

    for (int i = 0; i < numInterventions; ++i) {
        Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
        Eigen::array<Eigen::Index, 3> extent =
            this->treatmentUtilizationCost.dimensions();
        offset[Data::INTERVENTION] = i;
        extent[Data::INTERVENTION] = 1;
        Matrix3d slice = this->treatmentUtilizationCost.slice(offset, extent);
        if (this->treatmentUtilizationCostMap.find(interventions[i]) !=
            this->treatmentUtilizationCostMap.end()) {
            slice.setConstant(
                this->treatmentUtilizationCostMap[interventions[i]]);
        } else {
            slice.setConstant(0.0);
        }
        this->treatmentUtilizationCost.slice(offset, extent) = slice;
    }
    return this->treatmentUtilizationCost;
}

double CostLoader::getNonFatalOverdoseCost() {
    if (this->overdoseCostsMap.find("non_fatal_overdose") ==
        this->overdoseCostsMap.end()) {
        return 0.0;
    }
    return this->overdoseCostsMap["non_fatal_overdose"];
}

double CostLoader::getFatalOverdoseCost() {
    if (this->overdoseCostsMap.find("fatal_overdose") ==
        this->overdoseCostsMap.end()) {
        return 0.0;
    }
    return this->overdoseCostsMap["fatal_overdose"];
}

std::unordered_map<std::string, double>
CostLoader::loadPharmaceuticalCostMap(InputTable table) {
    for (size_t i = 0; i < table["block"].size(); i++) {
        this->pharmaceuticalCostsMap[table["block"][i]] =
            std::stod(table["pharmaceutical_cost_healthcare_system"][i]);
    }
    return this->pharmaceuticalCostsMap;
}

std::unordered_map<std::string, double>
CostLoader::loadTreatmentUtilizationCostMap(InputTable table) {
    for (size_t i = 0; i < table["block"].size(); i++) {
        this->treatmentUtilizationCostMap[table["block"][i]] =
            std::stod(table["treatment_utilization_cost_healthcare_system"][i]);
    }
    return this->treatmentUtilizationCostMap;
}
