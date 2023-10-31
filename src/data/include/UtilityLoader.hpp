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

namespace Data {
    class IUtilityLoader : public virtual ILoader {
    public:
        /// @brief Load the Background Utilties from a File
        /// @param csvName Filename containing Background Utility
        /// @return Matrix3d of Background Utility
        virtual Matrix3d loadBackgroundUtility(std::string const &csvName) = 0;

        /// @brief Load the OUD Utility from a File
        /// @param csvName Filename containing OUD Utility
        /// @return Matrix3d of OUD Utility
        virtual Matrix3d loadOUDUtility(std::string const &csvName) = 0;

        /// @brief Load the Setting Utility from a File
        /// @param csvName Filename containing the Setting Utility
        /// @return Matrix3d of Setting Utility
        virtual Matrix3d loadSettingUtility(std::string const &csvName) = 0;

        // GETTERS
        /// @brief Get the Background Utility
        /// @return Matrix3d of Background Utility
        virtual Matrix3d getBackgroundUtility() const = 0;

        /// @brief Get the OUD Utility
        /// @return Matrix3d of OUD Utility
        virtual Matrix3d getOUDUtility() const = 0;

        /// @brief Get the Setting Utility
        /// @return Matrix3d of Setting Utility
        virtual Matrix3d getSettingUtility() const = 0;
    };

    class UtilityLoader : public Loader, public IUtilityLoader {
    public:
        UtilityLoader(){};
        UtilityLoader(std::string const &inputDir);

        Configuration loadConfigurationFile(std::string const &configPath);

        virtual Matrix3d loadBackgroundUtility(std::string const &csvName);

        virtual Matrix3d loadOUDUtility(std::string const &csvName);

        virtual Matrix3d loadSettingUtility(std::string const &csvName);

        virtual Matrix3d getBackgroundUtility() const {
            return backgroundUtility;
        }

        virtual Matrix3d getOUDUtility() const { return oudUtility; }

        virtual Matrix3d getSettingUtility() const { return settingUtility; }

    private:
        Matrix3d backgroundUtility;
        Matrix3d oudUtility;
        Matrix3d settingUtility;

        /// @brief
        /// @param csvName
        /// @param utilMatrix
        /// @return
        Matrix3d loadUtility(std::string const &csvName, Matrix3d &utilMatrix);
    };
} // namespace Data

#endif