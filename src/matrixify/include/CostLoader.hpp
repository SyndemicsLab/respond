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

// #include <DataManagement.hpp>
#include "DataManagement.hpp"
#include <string>
#include <unordered_map>

#include "BaseLoader.hpp"
#include "DataTypes.hpp"
#include "Interfaces.hpp"

namespace matrixify {
    class ICostLoader : public virtual IBaseLoader {
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
    };

    using ICostLoaderPtr = std::shared_ptr<ICostLoader>;

    class CostLoader : public BaseLoader, public virtual ICostLoader {
    public:
        CostLoader(Data::IConfigablePtr config, std::string const &inputDir,
                   std::shared_ptr<spdlog::logger> logger);

        // delegating constructors
        CostLoader() : CostLoader(nullptr, "", nullptr) {}
        CostLoader(Data::IConfigablePtr config)
            : CostLoader(config, "", nullptr) {}
        CostLoader(Data::IConfigablePtr config, std::string const &inputDir)
            : CostLoader(config, inputDir, nullptr) {}
        CostLoader(std::string const &inputDir,
                   std::shared_ptr<spdlog::logger> logger)
            : CostLoader(nullptr, inputDir, logger) {}
        CostLoader(std::string const &inputDir)
            : CostLoader(nullptr, inputDir, nullptr) {}

        ~CostLoader() {};

        virtual std::unordered_map<std::string, Matrix3d>
        loadHealthcareUtilizationCost(std::string const &csvName);

        virtual std::unordered_map<std::string,
                                   std::unordered_map<std::string, double>>
        loadOverdoseCost(std::string const &csvName);

        virtual std::unordered_map<std::string, Matrix3d>
        loadPharmaceuticalCost(std::string const &csvName);

        virtual std::unordered_map<std::string, Matrix3d>
        loadTreatmentUtilizationCost(std::string const &csvName);

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

        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        loadTreatmentUtilizationCostMap(Data::IDataTablePtr table);
        std::unordered_map<std::string, std::unordered_map<std::string, double>>
        loadPharmaceuticalCostMap(Data::IDataTablePtr table);

        void loadCostViaPerspective(
            std::unordered_map<std::string, Matrix3d> &costParameter,
            std::unordered_map<std::string,
                               std::unordered_map<std::string, double>>
                &costParameterMap);
    };
} // namespace matrixify

#endif