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
    class UtilityLoader : public Loader {
    public:
        UtilityLoader(){};
        UtilityLoader(std::string inputDir);

        Configuration loadConfigurationFile(std::string configPath);

        // SETTERS
        /// @brief Load the Background Utilties from a File
        /// @param csvName Filename containing Background Utility
        /// @return Matrix3d of Background Utility
        Matrix3d loadBackgroundUtility(std::string csvName);

        /// @brief Load the OUD Utility from a File
        /// @param csvName Filename containing OUD Utility
        /// @return Matrix3d of OUD Utility
        Matrix3d loadOUDUtility(std::string csvName);

        /// @brief Load the Setting Utility from a File
        /// @param csvName Filename containing the Setting Utility
        /// @return Matrix3d of Setting Utility
        Matrix3d loadSettingUtility(std::string csvName);

        // GETTERS
        /// @brief Get the Background Utility
        /// @return Matrix3d of Background Utility
        Matrix3d getBackgroundUtility() const { return backgroundUtility; }

        /// @brief Get the OUD Utility
        /// @return Matrix3d of OUD Utility
        Matrix3d getOUDUtility() const { return oudUtility; }

        /// @brief Get the Setting Utility
        /// @return Matrix3d of Setting Utility
        Matrix3d getSettingUtility() const { return settingUtility; }

    private:
        Matrix3d backgroundUtility;
        Matrix3d oudUtility;
        Matrix3d settingUtility;
    };
} // namespace Data

#endif