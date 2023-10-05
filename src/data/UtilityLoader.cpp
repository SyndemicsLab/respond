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

    UtilityLoader::UtilityLoader(std::string inputDir) : Loader(inputDir) {}

    Configuration UtilityLoader::loadConfigurationFile(std::string configPath) {
        return Loader::loadConfigurationFile(configPath);
    }

    Matrix3d UtilityLoader::loadBackgroundUtility(std::string csvName) {
        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        if (this->inputTables.find(csvName) == this->inputTables.end()) {
            return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
        }
        InputTable table = this->inputTables[csvName];

        this->backgroundUtility =
            Utilities::Matrix3dFactory::Create(numOUDStates, numInterventions,
                                               numDemographicCombos)
                .constant(0);

        ASSERTM(table.find("utility") != table.end(),
                "\'utility\' Column Successfully Found");

        for (int dem = 0; dem < numDemographicCombos; ++dem) {
            Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extent =
                this->backgroundUtility.dimensions();
            offset[Data::DEMOGRAPHIC_COMBO] = dem;
            extent[Data::DEMOGRAPHIC_COMBO] = 1;
            Matrix3d temp = this->backgroundUtility.slice(offset, extent);
            temp.setConstant(std::stod(table["utility"][dem]));
            this->backgroundUtility.slice(offset, extent) = temp;
        }
        return this->backgroundUtility;
    }

    Matrix3d UtilityLoader::loadOUDUtility(std::string csvName) {
        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        if (this->inputTables.find(csvName) == this->inputTables.end()) {
            return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
        }
        InputTable table = this->inputTables[csvName];

        this->oudUtility =
            Utilities::Matrix3dFactory::Create(numOUDStates, numInterventions,
                                               numDemographicCombos)
                .constant(0);

        ASSERTM(table.find("utility") != table.end(),
                "\'utility\' Column Successfully Found");

        int rowIdx = 0;

        for (int intervention = 0; intervention < numInterventions;
             intervention++) {
            for (int oud = 0; oud < numOUDStates; ++oud) {
                Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
                Eigen::array<Eigen::Index, 3> extent =
                    this->oudUtility.dimensions();
                offset[Data::OUD] = oud;
                offset[Data::INTERVENTION] = intervention;
                extent[Data::OUD] = 1;
                extent[Data::INTERVENTION] = 1;
                Matrix3d temp = this->oudUtility.slice(offset, extent);
                temp.setConstant(std::stod(table["utility"][rowIdx]));
                rowIdx++;
                this->oudUtility.slice(offset, extent) = temp;
            }
        }

        return this->oudUtility;
    }

    Matrix3d UtilityLoader::loadSettingUtility(std::string csvName) {
        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        if (this->inputTables.find(csvName) == this->inputTables.end()) {
            return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
        }
        InputTable table = this->inputTables[csvName];

        this->settingUtility =
            Utilities::Matrix3dFactory::Create(numOUDStates, numInterventions,
                                               numDemographicCombos)
                .constant(0);

        ASSERTM(table.find("utility") != table.end(),
                "\'utility\' Column Successfully Found");

        for (int intervention = 0; intervention < numInterventions;
             ++intervention) {
            Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extent =
                this->settingUtility.dimensions();
            offset[Data::INTERVENTION] = intervention;
            extent[Data::INTERVENTION] = 1;
            Matrix3d temp = this->settingUtility.slice(offset, extent);
            temp.setConstant(std::stod(table["utility"][intervention]));
            this->settingUtility.slice(offset, extent) = temp;
        }
        return this->settingUtility;
    }
} // namespace Data