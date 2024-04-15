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

#include "BaseLoader.hpp"
#include "DataTypes.hpp"
#include "InterfaceLoaders.hpp"
#include "Matrix3dFactory.hpp"

namespace Matrixify {
    class UtilityLoader : public BaseLoader, public virtual IUtilityLoader {
    public:
        UtilityLoader(Data::IConfigablePtr config, std::string const &inputDir,
                      std::shared_ptr<spdlog::logger> logger);

        // delegating constructors
        UtilityLoader() : UtilityLoader(nullptr, "", nullptr) {}
        UtilityLoader(Data::IConfigablePtr config)
            : UtilityLoader(config, "", nullptr) {}
        UtilityLoader(Data::IConfigablePtr config, std::string const &inputDir)
            : UtilityLoader(config, inputDir, nullptr) {}
        UtilityLoader(std::string const &inputDir,
                      std::shared_ptr<spdlog::logger> logger)
            : UtilityLoader(nullptr, inputDir, logger) {}
        UtilityLoader(std::string const &inputDir)
            : UtilityLoader(nullptr, inputDir, nullptr) {}

        ~UtilityLoader(){};

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

    private:
        std::unordered_map<std::string, Matrix3d> backgroundUtility;
        std::unordered_map<std::string, Matrix3d> oudUtility;
        std::unordered_map<std::string, Matrix3d> settingUtility;

        /// @brief
        /// @param csvName
        /// @param utilMatrix
        /// @return
        std::unordered_map<std::string, Matrix3d>
        loadUtility(std::string const &csvName);
    };
} // namespace Matrixify

#endif