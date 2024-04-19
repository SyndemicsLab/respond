
#ifndef DATA_INTERFACELOADERS_HPP_
#define DATA_INTERFACELOADERS_HPP_

#include "DataTypes.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <DataManagement.hpp>

namespace Matrixify {
    class ILoggable {
    public:
        virtual void setLogger(std::shared_ptr<spdlog::logger> const) = 0;
        virtual std::shared_ptr<spdlog::logger> getLogger() const = 0;

    private:
        std::shared_ptr<spdlog::logger> logger;
    };

    class IOutputable {
    public:
        virtual bool getPerInterventionPredictions() const = 0;
        virtual bool getGeneralOutputsSwitch() const = 0;
        virtual std::vector<int> getGeneralStatsOutputTimesteps() const = 0;
    };

    class IConfigable {
    public:
        virtual bool loadConfigFile(std::string const &configPath) = 0;
        virtual bool loadConfigPtr(Data::IConfigablePtr configPtr) = 0;
        virtual Data::IConfigablePtr getConfig() const = 0;
    };

    class ISimulatable {
    public:
        virtual int getDuration() const = 0;
    };

    class ITableable {
    public:
        virtual Data::IDataTablePtr readCSV(std::string const &) = 0;
        virtual std::unordered_map<std::string, Data::IDataTablePtr>
        readInputDir(std::string const &) = 0;
        virtual Data::IDataTablePtr loadTable(std::string const &filename) = 0;
    };

    class IIntervention {
    public:
        virtual std::vector<std::string> getInterventions() const = 0;
        virtual int getNumInterventions() const = 0;
        virtual std::vector<int> getInterventionChangeTimes() const = 0;
    };

    class IBehavior {
    public:
        virtual std::vector<std::string> getOUDStates() const = 0;
        virtual int getNumOUDStates() const = 0;
    };

    class IDemographics {
    public:
        virtual std::vector<std::string> getDemographics() const = 0;
        virtual int getNumDemographics() const = 0;
        virtual std::vector<std::string> getDemographicCombos() const = 0;
        virtual int getNumDemographicCombos() const = 0;
        virtual int getAgeGroupShift() const = 0;
        virtual int getAgingInterval() const = 0;
        virtual std::vector<std::string> getAgeGroupBins() const = 0;
        virtual std::vector<int> getEnteringSampleChangeTimes() const = 0;
        virtual std::vector<int> getOverdoseChangeTimes() const = 0;
    };

    class ICosting {
    public:
        virtual bool getCostSwitch() const = 0;
        virtual std::vector<std::string> getCostPerspectives() const = 0;
        virtual double getDiscountRate() const = 0;
        virtual bool getCostCategoryOutputs() const = 0;
        virtual std::vector<int> getCostUtilityOutputTimesteps() const = 0;
    };

    class IBaseLoader : public virtual ILoggable,
                        public virtual IOutputable,
                        public virtual IConfigable,
                        public virtual ISimulatable,
                        public virtual ITableable,
                        public virtual IIntervention,
                        public virtual IBehavior,
                        public virtual IDemographics,
                        public virtual ICosting {};

    class IDataLoader : public virtual IBaseLoader {
    public:
        /// @brief Get the Initial Sample
        /// @return Matrix3d Initial Sample
        virtual Matrix3d getInitialSample() const = 0;

        /// @brief Get the Entering Samples
        /// @return Matrix4d Entering Samples
        virtual Matrix4d getEnteringSamples() const = 0;

        /// @brief Get the OUD Transition Rates
        /// @return Matrix3d OUD Transition Rates
        virtual Matrix3d getOUDTransitionRates() const = 0;

        /// @brief Get the Intervention Transition Rates
        /// @return Matrix4d Intervention Transition Rates
        virtual Matrix4d getInterventionTransitionRates() const = 0;

        /// @brief Get the Overdose Rates
        /// @return Matrix4d Overdose Rates
        virtual Matrix4d getOverdoseRates() const = 0;

        /// @brief Get the Fatal Overdose Rates
        /// @return Matrix4d Fatal Overdose Rates
        virtual Matrix4d getFatalOverdoseRates() const = 0;

        /// @brief Get the Mortality Rates
        /// @return Matrix3d Mortality Rates
        virtual Matrix3d getMortalityRates() const = 0;

        /// @brief Get the Intervention Initialization Rates
        /// @return Matrix3d Intervention Initialization Rates
        virtual Matrix3d getInterventionInitRates() const = 0;

        /// @brief set the Initial Sample
        virtual void setInitialSample(Matrix3d mat) = 0;

        /// @brief set the Entering Samples
        virtual void setEnteringSamples(Matrix4d mat) = 0;

        /// @brief set the OUD Transition Rates
        virtual void setOUDTransitionRates(Matrix3d mat) = 0;

        /// @brief set the Intervention Transition Rates
        virtual void setInterventionTransitionRates(Matrix4d mat) = 0;

        /// @brief set the Overdose Rates
        virtual void setOverdoseRates(Matrix4d mat) = 0;

        /// @brief set the Fatal Overdose Rates
        virtual void setFatalOverdoseRates(Matrix4d mat) = 0;

        /// @brief set the Mortality Rates
        virtual void setMortalityRates(Matrix3d mat) = 0;

        /// @brief set the Intervention Initialization Rates
        virtual void setInterventionInitRates(Matrix3d mat) = 0;

        /// @brief Load the Initial Sample from a File
        /// @param csvName Filename to the Initial Sample
        /// @return Matrix3d Initial Sample
        virtual Matrix3d loadInitialSample(std::string const &csvName) = 0;

        /// @brief Load the Entering Samples from a File
        /// @param csvName Filename to the Entering Samples
        /// @param enteringSampleIntervention The Intervention to Load Entering
        /// Samples Into
        /// @param enteringSampleOUD The OUD State to Load Entering Samples Into
        /// @return Matrix4d Entering Samples
        virtual Matrix4d
        loadEnteringSamples(std::string const &csvName,
                            std::string const &enteringSampleIntervention,
                            std::string const &enteringSampleOUD) = 0;

        /// @brief Load the OUD Transition Rates
        /// @param csvName Filename to the OUD Transition Rates
        /// @return Matrix3d OUD Transition Rates
        virtual Matrix3d loadOUDTransitionRates(std::string const &csvName) = 0;

        /// @brief Load the Intervention Initialization Rates
        /// @param csvName Filename to the Intervention Initialization Rates
        /// @return Matrix3d Intervention Initialization Rates
        virtual Matrix3d
        loadInterventionInitRates(std::string const &csvName) = 0;

        /// @brief Load the Intervention Transition Rates
        /// @param csvName Filename to the Intervention Transition Rates
        /// @return Matrix4d Intervention Transition Rates
        virtual Matrix4d
        loadInterventionTransitionRates(std::string const &csvName) = 0;

        /// @brief Load the Overdose Rates
        /// @param csvName Filename to the Overdose Rates
        /// @return Matrix4d Overdose Rates
        virtual Matrix4d loadOverdoseRates(std::string const &csvName) = 0;

        /// @brief Load the Fatal Overdose Rates
        /// @param csvName Filename to the Fatal Overdose Rates
        /// @return Matrix4d Fatal Overdose Rates
        virtual Matrix4d loadFatalOverdoseRates(std::string const &csvName) = 0;

        /// @brief Load the Mortality Rates
        /// @param smrCSVName Filename to the SMR File
        /// @param bgmCSVName Filename to the Background Mortality File
        /// @return Matrix3d Mortality Rates
        virtual Matrix3d loadMortalityRates(std::string const &smrCSVName,
                                            std::string const &bgmCSVName) = 0;
    };

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

        virtual bool getCostSwitch() const = 0;

        virtual double getDiscountRate() const = 0;
    };

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
} // namespace Matrixify

#endif