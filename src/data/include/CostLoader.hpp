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
        CostLoader()
            : Loader(){
                  // healthcareUtilizationCost =
                  //     Utilities::Matrix3dFactory::Create(0, 0, 0);

                  // pharmaceuticalCost = Utilities::Matrix3dFactory::Create(0,
                  // 0, 0);

                  // treatmentUtilizationCost =
                  //     Utilities::Matrix3dFactory::Create(0, 0, 0);
              };
        CostLoader(std::string const &inputDir);

        Configuration loadConfigurationFile(std::string const &configPath);

        // SETTERS
        /// @brief Load Healthcare Utilization Cost from file
        /// @param csvName filename for Healthcare Utilization Cost
        /// @return Matrix3d containing the Healthcare Utilization Cost
        std::unordered_map<std::string, Matrix3d>
        loadHealthcareUtilizationCost(std::string const &csvName);

        /// @brief Load Overdose Cost from file
        /// @param csvName filename for Overdose Cost
        /// @return unordered map for Overdose Costs, string to double
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        loadOverdoseCost(std::string const &csvName);

        /// @brief
        /// @param csvName
        /// @return
        std::unordered_map<std::string, Matrix3d>
        loadPharmaceuticalCost(std::string const &csvName);

        /// @brief
        /// @param csvName
        /// @return
        std::unordered_map<std::string, Matrix3d>
        loadTreatmentUtilizationCost(std::string const &csvName);

        /// @brief
        void populateCostParameters();

        /// @brief
        /// @return
        Matrix3d
        getHealthcareUtilizationCost(std::string const &perspective) const {
            if (this->healthcareUtilizationCost.find(perspective) !=
                this->healthcareUtilizationCost.end()) {
                return this->healthcareUtilizationCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        /// @brief
        /// @return
        Matrix3d getPharmaceuticalCost(std::string const &perspective) const {
            if (this->pharmaceuticalCost.find(perspective) !=
                this->pharmaceuticalCost.end()) {
                return this->pharmaceuticalCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        /// @brief
        /// @return
        Matrix3d
        getTreatmentUtilizationCost(std::string const &perspective) const {
            if (this->treatmentUtilizationCost.find(perspective) !=
                this->treatmentUtilizationCost.end()) {
                return this->treatmentUtilizationCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        /// @brief
        /// @return
        double getNonFatalOverdoseCost(std::string const &perspective) const;

        /// @brief
        /// @return
        double getFatalOverdoseCost(std::string const &perspective) const;

        std::vector<std::string> getCostPerspectives() const {
            return this->costPerspectives;
        }

        bool getCostSwitch() const { return this->costSwitch; }

    private:
        std::unordered_map<std::string, Matrix3d> healthcareUtilizationCost;
        std::unordered_map<std::string, Matrix3d> pharmaceuticalCost;
        std::unordered_map<std::string, Matrix3d> treatmentUtilizationCost;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            overdoseCostsMap;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            pharmaceuticalCostsMap;
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
            treatmentUtilizationCostMap;

        bool costSwitch = true;
        std::vector<std::string> costPerspectives;
        double discountRate;
        int reportingInterval;
        bool costCategoryOutputs;

        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        loadTreatmentUtilizationCostMap(InputTable table);
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        loadPharmaceuticalCostMap(InputTable table);
    };
} // namespace Data

#endif