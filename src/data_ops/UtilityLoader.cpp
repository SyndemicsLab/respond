////////////////////////////////////////////////////////////////////////////////
// File: UtilityLoader.cpp                                                    //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respondsimulation/data_ops/UtilityLoader.hpp>

namespace Matrixify {
    UtilityLoader::UtilityLoader(Data::IConfigablePtr config,
                                 std::string const &inputDir,
                                 std::shared_ptr<spdlog::logger> logger)
        : BaseLoader(config, inputDir, logger) {}

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