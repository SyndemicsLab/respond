//===-- UtilityLoader.cpp - UtilityLoader class definition ------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the UtilityLoader class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "UtilityLoader.hpp"

namespace Data {

    UtilityLoader::UtilityLoader(std::string const &inputDir)
        : Loader(inputDir) {
        this->populateCostParameters();
    }

    Configuration
    UtilityLoader::loadConfigurationFile(std::string const &configPath) {
        Loader::loadConfigurationFile(configPath);
        this->populateCostParameters();
        return this->Config;
    }

    std::unordered_map<std::string, Matrix3d>
    UtilityLoader::loadBackgroundUtility(std::string const &csvName) {
        this->backgroundUtility = this->loadUtility(csvName);
        return this->backgroundUtility;
    }

    std::unordered_map<std::string, Matrix3d>
    UtilityLoader::loadOUDUtility(std::string const &csvName) {
        this->oudUtility = this->loadUtility(csvName);
        return this->oudUtility;
    }

    std::unordered_map<std::string, Matrix3d>
    UtilityLoader::loadSettingUtility(std::string const &csvName) {
        this->settingUtility = this->loadUtility(csvName);
        return this->settingUtility;
    }

    std::unordered_map<std::string, Matrix3d>
    UtilityLoader::loadUtility(std::string const &csvName) {
        InputTable table = loadTable(csvName);

        for (std::string perspective : this->costPerspectives) {
            std::string message =
                "\'" + perspective + "\' Column Successfully Found";

            ASSERTM(table.find(perspective) != table.end(), message);
        }

        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        std::unordered_map<std::string, Data::Matrix3d> result;
        for (std::string perspective : this->costPerspectives) {
            Data::Matrix3d utilMatrix =
                Utilities::Matrix3dFactory::Create(
                    numOUDStates, numInterventions, numDemographicCombos)
                    .constant(0);

            std::string message =
                "\'" + perspective + "\' Column Successfully Found";
            ASSERTM(table.find(perspective) != table.end(), message);

            for (int intervention = 0; intervention < numInterventions;
                 ++intervention) {
                Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
                Eigen::array<Eigen::Index, 3> extent = utilMatrix.dimensions();
                offset[Data::INTERVENTION] = intervention;
                extent[Data::INTERVENTION] = 1;
                Matrix3d temp = utilMatrix.slice(offset, extent);
                if (table[perspective].size() > intervention) {
                    temp.setConstant(
                        std::stod(table[perspective][intervention]));
                } else {
                    temp.setConstant(0.0);
                }

                utilMatrix.slice(offset, extent) = temp;
            }
            result[perspective] = utilMatrix;
        }
        return result;
    }

    void UtilityLoader::populateCostParameters() {
        this->costSwitch = this->Config.getCostSwitch();
        if (this->costSwitch) {
            this->costPerspectives = this->Config.getCostPerspectives();
            this->discountRate = this->Config.getDiscountRate();
            this->costUtilityOutputTimesteps =
                this->Config.getCostUtilityOutputTimesteps();
            this->costCategoryOutputs = this->Config.getCostCategoryOutputs();
        }
    }
} // namespace Data