////////////////////////////////////////////////////////////////////////////////
// File: UtilityLoader.hpp                                                    //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef DATA_UTILITYLOADER_HPP_
#define DATA_UTILITYLOADER_HPP_

#include <respondsimulation/data_ops/BaseLoader.hpp>
#include <respondsimulation/data_ops/Matrix3dFactory.hpp>

namespace Matrixify {
    class IUtilityLoader : public virtual IBaseLoader {
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
    };

    using IUtilityLoaderPtr = std::shared_ptr<IUtilityLoader>;

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

        ~UtilityLoader() {};

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