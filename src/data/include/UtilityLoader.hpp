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

        // SETTERS
        /// @brief
        /// @param csvName
        /// @return
        Matrix3d loadBackgroundUtility(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3d loadOUDUtility(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3d loadSettingUtility(std::string csvName);

        // GETTERS
        /// @brief 
        /// @return 
        Matrix3d getBackgroundUtility() { return backgroundUtility; }

        /// @brief 
        /// @return 
        Matrix3d getOUDUtility() { return oudUtility; }

        /// @brief 
        /// @return 
        Matrix3d getSettingUtility() { return settingUtility; }

    private:
        Matrix3d backgroundUtility;
        Matrix3d oudUtility;
        Matrix3d settingUtility;
    };
}

#endif