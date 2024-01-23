//===-- UtilityLoader.hpp - UtilityLoader class definition ------*- C++ -*-===//
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

#ifndef DATA_UTILITYLOADER_HPP_
#define DATA_UTILITYLOADER_HPP_

#include "DataTypes.hpp"
#include "Loader.hpp"
#include "Matrix3dFactory.hpp"

namespace Matrixify {
    class IUtilityLoader : public virtual ILoader {
    public:
        /// @brief Load the Background Utilties from a File
        /// @param csvName Filename containing Background Utility
        /// @return Matrix3d of Background Utility
        virtual std::unordered_map<std::string, Matrix3d>
        loadBackgroundUtility(std::string const &csvName) = 0;

        /// @brief Load the OUD Utility from a File
        /// @param csvName Filename containing OUD Utility
        /// @return Matrix3d of OUD Utility
        virtual std::unordered_map<std::string, Matrix3d>
        loadOUDUtility(std::string const &csvName) = 0;

        /// @brief Load the Setting Utility from a File
        /// @param csvName Filename containing the Setting Utility
        /// @return Matrix3d of Setting Utility
        virtual std::unordered_map<std::string, Matrix3d>
        loadSettingUtility(std::string const &csvName) = 0;

        // GETTERS
        /// @brief Get the Background Utility
        /// @return Matrix3d of Background Utility
        virtual Matrix3d
        getBackgroundUtility(std::string const &perspective) const = 0;

        /// @brief Get the OUD Utility
        /// @return Matrix3d of OUD Utility
        virtual Matrix3d
        getOUDUtility(std::string const &perspective) const = 0;

        /// @brief Get the Setting Utility
        /// @return Matrix3d of Setting Utility
        virtual Matrix3d
        getSettingUtility(std::string const &perspective) const = 0;

        /// @brief
        virtual void populateCostParameters() = 0;

        /// @brief
        /// @return
        virtual std::vector<std::string> getCostPerspectives() const = 0;

        /// @brief
        /// @return
        virtual bool getCostSwitch() const = 0;

        /// @brief
        /// @return
        virtual double getDiscountRate() const = 0;
    };

    class UtilityLoader : public Loader, public IUtilityLoader {
    public:
        UtilityLoader(){};
        UtilityLoader(std::string const &inputDir);

        Configuration loadConfigurationFile(std::string const &configPath);

        virtual std::unordered_map<std::string, Matrix3d>
        loadBackgroundUtility(std::string const &csvName);

        virtual std::unordered_map<std::string, Matrix3d>
        loadOUDUtility(std::string const &csvName);

        virtual std::unordered_map<std::string, Matrix3d>
        loadSettingUtility(std::string const &csvName);

        virtual Matrix3d
        getBackgroundUtility(std::string const &perspective) const {
            if (this->backgroundUtility.find(perspective) !=
                this->backgroundUtility.end()) {
                return this->backgroundUtility.at(perspective);
            }

            // add warning
            Matrix3d result;
            return result;
        }

        virtual Matrix3d getOUDUtility(std::string const &perspective) const {
            if (this->oudUtility.find(perspective) != this->oudUtility.end()) {
                return this->oudUtility.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        virtual Matrix3d
        getSettingUtility(std::string const &perspective) const {
            if (this->settingUtility.find(perspective) !=
                this->settingUtility.end()) {
                return this->settingUtility.at(perspective);
            }
            // add warning
            Matrix3d result;
            return result;
        }

        virtual void populateCostParameters();

        virtual std::vector<std::string> getCostPerspectives() const {
            return this->costPerspectives;
        }

        virtual bool getCostSwitch() const { return this->costSwitch; }

        virtual double getDiscountRate() const { return this->discountRate; }

    private:
        std::unordered_map<std::string, Matrix3d> backgroundUtility;
        std::unordered_map<std::string, Matrix3d> oudUtility;
        std::unordered_map<std::string, Matrix3d> settingUtility;

        bool costSwitch = true;
        std::vector<std::string> costPerspectives;
        double discountRate;
        std::vector<int> costUtilityOutputTimesteps;
        bool costCategoryOutputs;

        /// @brief
        /// @param csvName
        /// @param utilMatrix
        /// @return
        std::unordered_map<std::string, Matrix3d>
        loadUtility(std::string const &csvName);
    };
} // namespace Matrixify

#endif