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
    class ICostLoader : public virtual ILoader {
    public:
        /// @brief Load Healthcare Utilization Cost from file
        /// @param csvName filename for Healthcare Utilization Cost
        /// @return Matrix3d containing the Healthcare Utilization Cost
        virtual std::unordered_map<std::string, Matrix3d>
        loadHealthcareUtilizationCost(std::string const &csvName) = 0;

        /// @brief Load Overdose Cost from file
        /// @param csvName filename for Overdose Cost
        /// @return unordered map for Overdose CostList, string to double
        virtual std::unordered_map<std::string,
                                   std::unordered_map<std::string, double>>
        loadOverdoseCost(std::string const &csvName) = 0;

        /// @brief
        /// @param csvName
        /// @return
        virtual std::unordered_map<std::string, Matrix3d>
        loadPharmaceuticalCost(std::string const &csvName) = 0;

        /// @brief
        /// @param csvName
        /// @return
        virtual std::unordered_map<std::string, Matrix3d>
        loadTreatmentUtilizationCost(std::string const &csvName) = 0;

        /// @brief
        virtual void populateCostParameters() = 0;

        /// @brief
        /// @return
        virtual Matrix3d
        getHealthcareUtilizationCost(std::string const &perspective) const = 0;

        /// @brief
        /// @return
        virtual Matrix3d
        getPharmaceuticalCost(std::string const &perspective) const = 0;

        /// @brief
        /// @return
        virtual Matrix3d
        getTreatmentUtilizationCost(std::string const &perspective) const = 0;

        /// @brief
        /// @return
        virtual double
        getNonFatalOverdoseCost(std::string const &perspective) const = 0;

        /// @brief
        /// @return
        virtual double
        getFatalOverdoseCost(std::string const &perspective) const = 0;

        virtual std::vector<std::string> getCostPerspectives() const = 0;

        virtual bool getCostSwitch() const = 0;

        virtual double getDiscountRate() const = 0;
    };

    class CostLoader : public Loader, public ICostLoader {
    public:
        CostLoader(){};
        CostLoader(std::string const &inputDir);

        Configuration loadConfigurationFile(std::string const &configPath);

        virtual std::unordered_map<std::string, Matrix3d>
        loadHealthcareUtilizationCost(std::string const &csvName);

        virtual std::unordered_map<std::string,
                                   std::unordered_map<std::string, double>>
        loadOverdoseCost(std::string const &csvName);

        virtual std::unordered_map<std::string, Matrix3d>
        loadPharmaceuticalCost(std::string const &csvName);

        virtual std::unordered_map<std::string, Matrix3d>
        loadTreatmentUtilizationCost(std::string const &csvName);

        virtual void populateCostParameters();

        virtual Matrix3d
        getHealthcareUtilizationCost(std::string const &perspective) const {
            if (this->healthcareUtilizationCost.find(perspective) !=
                this->healthcareUtilizationCost.end()) {
                return this->healthcareUtilizationCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        virtual Matrix3d
        getPharmaceuticalCost(std::string const &perspective) const {
            if (this->pharmaceuticalCost.find(perspective) !=
                this->pharmaceuticalCost.end()) {
                return this->pharmaceuticalCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        virtual Matrix3d
        getTreatmentUtilizationCost(std::string const &perspective) const {
            if (this->treatmentUtilizationCost.find(perspective) !=
                this->treatmentUtilizationCost.end()) {
                return this->treatmentUtilizationCost.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        virtual double
        getNonFatalOverdoseCost(std::string const &perspective) const;

        virtual double
        getFatalOverdoseCost(std::string const &perspective) const;

        virtual std::vector<std::string> getCostPerspectives() const {
            return this->costPerspectives;
        }

        virtual bool getCostSwitch() const { return this->costSwitch; }

        virtual double getDiscountRate() const { return this->discountRate; }

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

        void loadCostViaPerspective(
            std::unordered_map<std::string, Matrix3d> &costParameter,
            std::unordered_map<std::string,
                               std::unordered_map<std::string, double>>
                &costParameterMap);
    };
} // namespace Data

#endif