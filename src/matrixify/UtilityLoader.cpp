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

namespace Matrixify {

    UtilityLoader::UtilityLoader() : Loader() {}

    UtilityLoader::UtilityLoader(std::string const &inputDir,
                                 std::shared_ptr<spdlog::logger> logger)
        : Loader(inputDir, logger) {
        this->inputTables = this->readInputDir(inputDir);
    }

    UtilityLoader::UtilityLoader(Data::IConfigurationPtr &config,
                                 std::string const &inputDir,
                                 std::shared_ptr<spdlog::logger> logger)
        : Loader(inputDir, logger) {
        loadConfigurationPointer(config);
        this->inputTables = this->readInputDir(inputDir);
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
        Data::IDataTablePtr table = loadTable(csvName);
        std::unordered_map<std::string, Matrixify::Matrix3d> result;

        Matrixify::Matrix3d utilMatrix =
            Matrixify::Matrix3dFactory::Create(getNumOUDStates(),
                                               getNumInterventions(),
                                               getNumDemographicCombos())
                .constant(0);

        std::vector<std::string> utilCol = table->getColumn("utility");

        for (int intervention = 0; intervention < getNumInterventions();
             ++intervention) {
            Eigen::array<Eigen::Index, 3> offset = {0, 0, 0};
            Eigen::array<Eigen::Index, 3> extent = utilMatrix.dimensions();
            offset[Matrixify::INTERVENTION] = intervention;
            extent[Matrixify::INTERVENTION] = 1;
            Matrix3d temp = utilMatrix.slice(offset, extent);
            if (utilCol.size() > intervention) {
                temp.setConstant(std::stod(utilCol[intervention]));
            } else {
                temp.setConstant(0.0);
            }

            utilMatrix.slice(offset, extent) = temp;
        }
        result["utility"] = utilMatrix;

        return result;
    }
} // namespace Matrixify