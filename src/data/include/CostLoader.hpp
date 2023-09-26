//===-- CostLoader.hpp - CostLoader class definition ------------*- C++ -*-===//
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

#ifndef DATA_COSTLOADER_HPP_
#define DATA_COSTLOADER_HPP_

#include <string>
#include <unordered_map>

#include "DataTypes.hpp"
#include "Loader.hpp"
#include "Matrix3dFactory.hpp"
#include "Matrix3dPrinter.hpp"

namespace Data {
    class CostLoader : public Loader {
    public:
        CostLoader(){};
        CostLoader(std::string inputDir);

        Configuration loadConfigurationFile(std::string configPath);

        // SETTERS
        /// @brief Load Healthcare Utilization Cost from file
        /// @param csvName filename for Healthcare Utilization Cost
        /// @return Matrix3d containing the Healthcare Utilization Cost
        Matrix3d loadHealthcareUtilizationCost(std::string csvName);

        /// @brief Load Overdose Cost from file
        /// @param csvName filename for Overdose Cost
        /// @return unordered map for Overdose Costs, string to double
        std::unordered_map<std::string, double>
        loadOverdoseCost(std::string csvName);

        /// @brief
        /// @param csvName
        /// @return
        Matrix3d loadPharmaceuticalCost(std::string csvName);

        /// @brief
        /// @param csvName
        /// @return
        Matrix3d loadTreatmentUtilizationCost(std::string csvName);

        /// @brief
        /// @return
        Matrix3d getHealthcareUtilizationCost() {
            return this->healthcareUtilizationCost;
        }

        /// @brief
        /// @return
        Matrix3d getPharmaceuticalCost() { return this->pharmaceuticalCost; }

        /// @brief
        /// @return
        Matrix3d getTreatmentUtilizationCost() {
            return this->treatmentUtilizationCost;
        }

        /// @brief
        /// @return
        double getNonFatalOverdoseCost();

        /// @brief
        /// @return
        double getFatalOverdoseCost();

    private:
        Matrix3d healthcareUtilizationCost;
        Matrix3d pharmaceuticalCost;
        Matrix3d treatmentUtilizationCost;
        std::unordered_map<std::string, double> overdoseCostsMap;
        std::unordered_map<std::string, double> pharmaceuticalCostsMap;
        std::unordered_map<std::string, double> treatmentUtilizationCostMap;

        std::unordered_map<std::string, double>
        loadTreatmentUtilizationCostMap(InputTable table);
        std::unordered_map<std::string, double>
        loadPharmaceuticalCostMap(InputTable table);
    };
} // namespace Data

#endif