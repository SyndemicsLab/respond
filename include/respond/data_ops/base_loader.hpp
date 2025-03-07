////////////////////////////////////////////////////////////////////////////////
// File: base_loader.hpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-07                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_DATAOPS_BASELOADER_HPP_
#define RESPOND_DATAOPS_BASELOADER_HPP_

#include <memory>
#include <string>
#include <unordered_map>
#include <vector>

#include <datamanagement/DataManagement.hpp>
#include <spdlog/spdlog.h>

namespace respond::data_ops {
    class BaseLoader {
    public:
        BaseLoader(Data::IConfigablePtr config = nullptr,
                   const std::string &input_dir = "",
                   std::shared_ptr<spdlog::logger> logger = nullptr);

        virtual bool loadConfigFile(const std::string &config_path) = 0;

        virtual bool loadConfigPtr(Data::IConfigablePtr config_ptr) = 0;

        virtual Data::IDataTablePtr readCSV(const std::string &path,
                                            bool headers = true) = 0;

        virtual std::unordered_map<std::string, Data::IDataTablePtr>
        readInputDir(std::string const &) = 0;

        virtual Data::IConfigablePtr getConfig() const = 0;

        virtual Data::IDataTablePtr loadTable(std::string const &filename) = 0;
        //  {
        //     if (this->inputTables.find(filename) == this->inputTables.end())
        //     {
        //         this->inputTables[filename] =
        //             BaseLoader::readCSV(filename, true);
        //     }
        //     return this->inputTables[filename];
        // }

        virtual int getDuration() const = 0;

        virtual bool getEnteringCohortToggle() const = 0;

        virtual int getAgingInterval() const = 0;

        virtual std::vector<int> getInterventionChangeTimes() const = 0;

        virtual std::vector<int> getEnteringSampleChangeTimes() const = 0;

        virtual std::vector<int> getOverdoseChangeTimes() const = 0;

        virtual std::vector<std::string> getInterventions() const = 0;

        virtual std::vector<std::string> getOUDStates() const = 0;

        virtual int getNumOUDStates() const = 0;

        virtual int getNumInterventions() const = 0;

        virtual void
        setInterventions(std::vector<std::string> interventions) = 0;

        virtual void setOUDStates(std::vector<std::string> ouds) = 0;

        virtual void setInterventionChangeTimes(const std::vector<int> ict) = 0;

        virtual std::vector<std::string> getAgeGroupBins() const = 0;

        virtual std::vector<std::string> getDemographics() const = 0;

        virtual int getNumDemographics() const = 0;

        virtual std::vector<std::string> getDemographicCombos() const = 0;

        virtual int getNumDemographicCombos() const = 0;

        virtual void setDemographicCombos(std::vector<std::string> combos) = 0;

        virtual void setDemographics(std::vector<std::string> dem) = 0;

        virtual int getAgeGroupShift() const = 0;

        virtual bool getCostSwitch() const = 0;

        virtual std::vector<std::string> getCostPerspectives() const = 0;

        virtual double getDiscountRate() const = 0;

        virtual bool getCostCategoryOutputs() const = 0;

        virtual std::vector<int> getCostUtilityOutputTimesteps() const = 0;

        virtual bool getPerInterventionPredictions() const = 0;

        virtual bool getGeneralOutputsSwitch() const = 0;

        virtual std::vector<int> getGeneralStatsOutputTimesteps() const = 0;

        virtual std::shared_ptr<spdlog::logger> getLogger() const = 0;

        // void setLogger(std::shared_ptr<spdlog::logger> const log) {
        //     if (!log) {
        //         if (!spdlog::get("console")) {
        //             this->logger = spdlog::stdout_color_mt("console");
        //         } else {
        //             this->logger = spdlog::get("console");
        //         }
        //     } else {
        //         this->logger = log;
        //     }
        // }

    protected:
        static const std::vector<std::string> INPUT_FILES;
        std::unordered_map<std::string, Data::IDataTablePtr> inputTables = {};
        Data::IConfigablePtr Config;
        std::shared_ptr<spdlog::logger> logger;

        // simulation
        int duration = 0;
        bool enterCohortToggle = false;
        int agingInterval = 0;
        std::vector<int> interventionChangeTimes = {};
        std::vector<int> enteringSampleChangeTimes = {};
        std::vector<int> overdoseChangeTimes = {};

        // state
        std::vector<std::string> interventions = {};
        std::map<std::string, int> interventionsIndices;
        std::vector<std::string> oudStates = {};
        std::map<std::string, int> oudIndices;

        // demographic
        int ageGroupShift;
        std::vector<std::string> demographics;
        std::vector<std::string> demographicCombos = {};
        std::map<std::string, int> demographicComboIndices;

        // cost
        bool costSwitch = false;
        std::vector<std::string> costPerspectives = {};
        double discountRate = 0.0;
        int reportingInterval = 1;
        bool costCategoryOutputs = false;
        std::vector<int> costUtilityOutputTimesteps = {};

        // output
        bool perInterventionPredictions;
        bool generalOutputsSwitch;
        std::vector<int> generalStatsOutputTimesteps;

        void loadObjectData();

    private:
        void loadFromConfig();
        std::map<std::string, int>
        buildIndiceMaps(std::vector<std::string> keys) const;
        void recursiveHelper(
            std::vector<std::vector<std::string>> &finalResultVector,
            std::vector<std::string> &currentResultVector,
            std::vector<std::vector<std::string>>::const_iterator currentInput,
            std::vector<std::vector<std::string>>::const_iterator finalInput);
    };
} // namespace respond::data_ops
#endif // RESPOND_DATAOPS_BASELOADER_HPP_
