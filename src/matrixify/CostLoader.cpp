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
#include "Matrix3dFactory.hpp"
#include "Matrix3dPrinter.hpp"

namespace Matrixify {

    CostLoader::CostLoader() : Loader() {}

    CostLoader::CostLoader(std::string const &inputDir,
                           std::shared_ptr<spdlog::logger> logger)
        : Loader(inputDir, logger) {
        this->inputTables = this->readInputDir(inputDir);
    }

    CostLoader::CostLoader(Data::IConfigurationPtr &config,
                           std::string const &inputDir,
                           std::shared_ptr<spdlog::logger> logger)
        : Loader(inputDir, logger) {
        loadConfigurationPointer(config);
        this->inputTables = this->readInputDir(inputDir);
    }

    std::unordered_map<std::string, Matrix3d>
    CostLoader::loadHealthcareUtilizationCost(std::string const &csvName) {
        Data::IDataTablePtr table = loadTable(csvName);
        size_t numOUDStates = this->oudStates.size();

        size_t numDemographicCombos = this->demographicCombos.size();
        size_t numInterventions = interventions.size();

        for (std::string perspective : this->costPerspectives) {

            std::vector<std::string> healthColumn =
                table->getColumn(perspective);
            std::string message =
                "\'" + perspective + "\' Column Successfully Found";
            ASSERTM(!healthColumn.empty(), message);

            this->healthcareUtilizationCost[perspective] =
                Matrixify::Matrix3dFactory::Create(
                    numOUDStates, numInterventions, numDemographicCombos);

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
        Data::IDataTablePtr table = loadTable(csvName);

        std::vector<std::string> xCol = table->getColumn("X");

        for (std::string perspective : this->costPerspectives) {
            for (size_t i = 0; i < xCol.size(); i++) {
                std::vector<std::string> persCol =
                    table->getColumn(perspective);
                if (persCol.empty()) {
                    this->logger->error(
                        "Cost perspective {} not found in table", perspective);
                }
                this->overdoseCostsMap[perspective][xCol[i]] =
                    std::stod(persCol[i]);
            }
        }
        return this->overdoseCostsMap;
    }

    std::unordered_map<std::string, Matrix3d>
    CostLoader::loadPharmaceuticalCost(std::string const &csvName) {
        Data::IDataTablePtr table = loadTable(csvName);

        size_t numOUDStates = this->oudStates.size();
        size_t numDemographicCombos = this->demographicCombos.size();
        size_t numInterventions = interventions.size();

        this->loadPharmaceuticalCostMap(table);

        this->loadCostViaPerspective(this->pharmaceuticalCost,
                                     this->pharmaceuticalCostsMap);

        return this->pharmaceuticalCost;
    }

    std::unordered_map<std::string, Matrix3d>
    CostLoader::loadTreatmentUtilizationCost(std::string const &csvName) {
        Data::IDataTablePtr table = loadTable(csvName);
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
    CostLoader::loadPharmaceuticalCostMap(Data::IDataTablePtr table) {
        std::vector<std::string> blockCol = table->getColumn("block");
        for (std::string perspective : this->costPerspectives) {
            std::vector<std::string> perspectiveCol =
                table->getColumn(perspective);
            for (size_t i = 0; i < blockCol.size(); i++) {
                this->pharmaceuticalCostsMap[perspective][blockCol[i]] =
                    std::stod(perspectiveCol[i]);
            }
        }
        return this->pharmaceuticalCostsMap;
    }

    void CostLoader::loadCostViaPerspective(
        std::unordered_map<std::string, Matrix3d> &costParameter,
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            &costParameterMap) {

        size_t numOUDStates = oudStates.size();
        size_t numDemographicCombos = this->demographicCombos.size();

        size_t numInterventions = this->interventions.size();

        for (std::string perspective : this->costPerspectives) {
            costParameter[perspective] =
                Matrixify::Matrix3dFactory::Create(
                    numOUDStates, numInterventions, numDemographicCombos)
                    .constant(0);

            for (int i = 0; i < numInterventions; ++i) {
                Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
                Eigen::array<Eigen::Index, 3> extent =
                    costParameter[perspective].dimensions();
                offset[Matrixify::INTERVENTION] = i;
                extent[Matrixify::INTERVENTION] = 1;
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
    CostLoader::loadTreatmentUtilizationCostMap(Data::IDataTablePtr table) {
        std::vector<std::string> blockCol = table->getColumn("block");
        for (std::string perspective : this->costPerspectives) {
            std::vector<std::string> perspectiveCol =
                table->getColumn(perspective);
            for (size_t i = 0; i < blockCol.size(); i++) {
                this->treatmentUtilizationCostMap[perspective][blockCol[i]] =
                    std::stod(perspectiveCol[i]);
            }
        }
        return this->treatmentUtilizationCostMap;
    }

} // namespace Matrixify
