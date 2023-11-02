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

namespace Data {

    CostLoader::CostLoader(std::string const &inputDir) : Loader(inputDir) {
        this->populateCostParameters();
    }

    Configuration
    CostLoader::loadConfigurationFile(std::string const &configPath) {
        Loader::loadConfigurationFile(configPath);
        this->populateCostParameters();
        return this->Config;
    }

    std::unordered_map<std::string, Matrix3d>
    CostLoader::loadHealthcareUtilizationCost(std::string const &csvName) {
        InputTable table = loadTable(csvName);
        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        for (std::string perspective : this->costPerspectives) {
            std::string message =
                "\'" + perspective + "\' Column Successfully Found";
            ASSERTM(table.find(perspective) != table.end(), message);

            this->healthcareUtilizationCost[perspective] =
                Utilities::Matrix3dFactory::Create(
                    numOUDStates, numInterventions, numDemographicCombos);

            std::vector<std::string> healthColumn = table[perspective];

            int rowIdx = 0;
            for (int intervention = 0; intervention < numInterventions;
                 ++intervention) {
                for (int dem = 0; dem < numDemographicCombos; ++dem) {
                    for (int oud_state = 0; oud_state < numOUDStates;
                         ++oud_state) {
                        double cost = (healthColumn.size() > rowIdx)
                                          ? std::stod(healthColumn[rowIdx])
                                          : 0.0;
                        this->healthcareUtilizationCost[perspective](
                            intervention, oud_state, dem) = cost;
                        rowIdx++;
                    }
                }
            }
        }
        return this->healthcareUtilizationCost;
    }

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
    CostLoader::loadOverdoseCost(std::string const &csvName) {
        InputTable table = loadTable(csvName);

        for (std::string perspective : this->costPerspectives) {
            std::string message =
                "\'" + perspective + "\' Column Successfully Found";

            ASSERTM(table.find(perspective) != table.end(), message);
        }

        ASSERTM(table.find("X") != table.end(),
                "\'X\' Column Successfully Found");

        for (std::string perspective : this->costPerspectives) {
            for (size_t i = 0; i < table["X"].size(); i++) {
                this->overdoseCostsMap[perspective][table["X"][i]] =
                    std::stod(table[perspective][i]);
            }
        }
        return this->overdoseCostsMap;
    }

    std::unordered_map<std::string, Matrix3d>
    CostLoader::loadPharmaceuticalCost(std::string const &csvName) {
        InputTable table = loadTable(csvName);

        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        ASSERTM(table.find("block") != table.end(),
                "\'block\' Column Successfully Found");

        for (std::string perspective : this->costPerspectives) {
            std::string message =
                "\'" + perspective + "\' Column Successfully Found";
            ASSERTM(table.find(perspective) != table.end(), message);
        }

        this->loadPharmaceuticalCostMap(table);

        this->loadCostViaPerspective(this->pharmaceuticalCost,
                                     this->pharmaceuticalCostsMap);

        return this->pharmaceuticalCost;
    }

    std::unordered_map<std::string, Matrix3d>
    CostLoader::loadTreatmentUtilizationCost(std::string const &csvName) {
        InputTable table = loadTable(csvName);
        ASSERTM(table.find("block") != table.end(),
                "\'block\' Column Successfully Found");

        for (std::string perspective : this->costPerspectives) {
            std::string message =
                "\'" + perspective + "\' Column Successfully Found";
            ASSERTM(table.find(perspective) != table.end(), message);
        }

        this->loadTreatmentUtilizationCostMap(table);

        this->loadCostViaPerspective(this->treatmentUtilizationCost,
                                     this->treatmentUtilizationCostMap);
        return this->treatmentUtilizationCost;
    }

    double
    CostLoader::getNonFatalOverdoseCost(std::string const &perspective) const {
        if (this->overdoseCostsMap.at(perspective).find("non_fatal_overdose") ==
            this->overdoseCostsMap.at(perspective).end()) {
            return 0.0;
        }
        return this->overdoseCostsMap.at(perspective).at("non_fatal_overdose");
    }

    double
    CostLoader::getFatalOverdoseCost(std::string const &perspective) const {
        if (this->overdoseCostsMap.at(perspective).find("fatal_overdose") ==
            this->overdoseCostsMap.at(perspective).end()) {
            return 0.0;
        }
        return this->overdoseCostsMap.at(perspective).at("fatal_overdose");
    }

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
    CostLoader::loadPharmaceuticalCostMap(InputTable table) {
        for (std::string perspective : this->costPerspectives) {
            for (size_t i = 0; i < table["block"].size(); i++) {
                this->pharmaceuticalCostsMap[perspective][table["block"][i]] =
                    std::stod(table[perspective][i]);
            }
        }
        return this->pharmaceuticalCostsMap;
    }

    void CostLoader::loadCostViaPerspective(
        std::unordered_map<std::string, Matrix3d> &costParameter,
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            &costParameterMap) {

        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        for (std::string perspective : this->costPerspectives) {
            costParameter[perspective] =
                Utilities::Matrix3dFactory::Create(
                    numOUDStates, numInterventions, numDemographicCombos)
                    .constant(0);

            std::vector<std::string> interventions =
                this->Config.getInterventions();

            for (int i = 0; i < numInterventions; ++i) {
                Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
                Eigen::array<Eigen::Index, 3> extent =
                    costParameter[perspective].dimensions();
                offset[Data::INTERVENTION] = i;
                extent[Data::INTERVENTION] = 1;
                Matrix3d slice =
                    costParameter[perspective].slice(offset, extent);
                if (costParameterMap[perspective].find(interventions[i]) !=
                    costParameterMap[perspective].end()) {
                    slice.setConstant(
                        costParameterMap[perspective][interventions[i]]);
                } else {
                    slice.setConstant(0.0);
                }
                costParameter[perspective].slice(offset, extent) = slice;
            }
        }
    }

    std::unordered_map<std::string, std::unordered_map<std::string, double>>
    CostLoader::loadTreatmentUtilizationCostMap(InputTable table) {
        for (std::string perspective : this->costPerspectives) {
            for (size_t i = 0; i < table["block"].size(); i++) {
                this->treatmentUtilizationCostMap[perspective]
                                                 [table["block"][i]] =
                    std::stod(table[perspective][i]);
            }
        }
        return this->treatmentUtilizationCostMap;
    }

    void CostLoader::populateCostParameters() {
        this->costSwitch = this->Config.getCostSwitch();
        if (this->costSwitch) {
            this->costPerspectives = this->Config.getCostPerspectives();
            this->discountRate = this->Config.getDiscountRate();
            this->reportingInterval = this->Config.getReportingInterval();
            this->costCategoryOutputs = this->Config.getCostCategoryOutputs();
        }
    }

} // namespace Data
