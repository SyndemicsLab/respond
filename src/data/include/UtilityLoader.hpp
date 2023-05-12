#ifndef DATA_UTILITYLOADER_HPP_
#define DATA_UTILITYLOADER_HPP_

#include "DataTypes.hpp"
#include "Loader.hpp"
#include "Matrix3dFactory.hpp"

namespace Data{
    class UtilityLoader : public Loader{
    public:
        UtilityLoader() {};
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
        Matrix3d getBackgroundUtility() { return backgroundUtility; }

        /// @brief Get the OUD Utility
        /// @return Matrix3d of OUD Utility
        Matrix3d getOUDUtility() { return oudUtility; }

        /// @brief Get the Setting Utility 
        /// @return Matrix3d of Setting Utility
        Matrix3d getSettingUtility() { return settingUtility; }

    private:
        Matrix3d backgroundUtility;
        Matrix3d oudUtility;
        Matrix3d settingUtility;
    };
}

#endif