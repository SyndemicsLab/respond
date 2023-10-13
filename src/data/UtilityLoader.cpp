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
        : Loader(inputDir) {}

    Configuration
    UtilityLoader::loadConfigurationFile(std::string const &configPath) {
        return Loader::loadConfigurationFile(configPath);
    }

    Matrix3d UtilityLoader::loadBackgroundUtility(std::string const &csvName) {
        return this->loadUtility(csvName, this->backgroundUtility);
    }

    Matrix3d UtilityLoader::loadOUDUtility(std::string const &csvName) {
        return this->loadUtility(csvName, this->oudUtility);
    }

    Matrix3d UtilityLoader::loadSettingUtility(std::string const &csvName) {
        return this->loadUtility(csvName, this->settingUtility);
    }

    Matrix3d UtilityLoader::loadUtility(std::string const &csvName,
                                        Matrix3d &utilMatrix) {
        InputTable table = loadTable(csvName);

        size_t numOUDStates = this->Config.getOUDStates().size();
        size_t numDemographicCombos = this->Config.getNumDemographicCombos();
        size_t numInterventions = this->Config.getInterventions().size();

        utilMatrix = Utilities::Matrix3dFactory::Create(
                         numOUDStates, numInterventions, numDemographicCombos)
                         .constant(0);

        ASSERTM(table.find("utility") != table.end(),
                "\'utility\' Column Successfully Found");

        for (int intervention = 0; intervention < numInterventions;
             ++intervention) {
            Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extent = utilMatrix.dimensions();
            offset[Data::INTERVENTION] = intervention;
            extent[Data::INTERVENTION] = 1;
            Matrix3d temp = utilMatrix.slice(offset, extent);
            if (table["utility"].size() > intervention) {
                temp.setConstant(std::stod(table["utility"][intervention]));
            } else {
                temp.setConstant(0.0);
            }

            utilMatrix.slice(offset, extent) = temp;
        }
        return utilMatrix;
    }
} // namespace Data