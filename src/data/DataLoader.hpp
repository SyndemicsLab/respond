#ifndef DATA_DATALOADER_HPP_
#define DATA_DATALOADER_HPP_

#include <boost/tokenizer.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>

#include "Configuration.hpp"
#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include "Matrix3dPrinter.hpp"

namespace Data {
    /// @brief 
    using InputTable = std::unordered_map<std::string, std::vector<std::string>>;

    // tabular files from the current RESPOND directory structure, as of [2023-04-06]
    static std::vector<std::string> INPUT_FILES = {
        "all_types_overdose.csv",
        "background_mortality.csv",
        "block_init_effect.csv",
        "block_trans.csv",
        "entering_cohort.csv",
        "fatal_overdose.csv",
        "init_cohort.csv",
        "oud_trans.csv",
        "SMR.csv",
        "bg_utility.csv",
        "healthcare_utilization_cost.csv",
        "oud_utility.csv",
        "overdose_cost.csv",
        "pharmaceutical_cost.csv",
        "setting_utility.csv",
        "treatment_utilization_cost.csv"
    };

    /// @brief 
    class DataLoader {
    public:
        ~DataLoader() {};
        DataLoader();
        DataLoader(std::string inputDir);
        DataLoader(Configuration configFile, std::string inputDir);

        /// @brief 
        /// @param  
        /// @return 
        Configuration readConfigFile(std::string);

        /// @brief 
        /// @param  
        /// @return 
        InputTable readCSV(std::string);

        /// @brief 
        /// @param  
        /// @return 
        std::unordered_map<std::string, InputTable> readInputDir(std::string);

        /// @brief 
        /// @return 
        std::string getDirName() { return dirName; }

        /// @brief 
        /// @return 
        int getDuration() { return duration; }

        /// @brief 
        /// @return 
        int getNumOUDStates() { return numOUDStates; }

        /// @brief 
        /// @return 
        int getNumInterventions() { return numInterventions; }
        
        /// @brief 
        /// @param indices 
        /// @param table 
        /// @return 
        int getNumDemographics() { return numDemographics; }

        /// @brief 
        /// @return 
        Matrix3d getInitialSample() { return initialSample; }

        /// @brief 
        Matrix3dOverTime getEnteringSamples() { return enteringSamples; }

        /// @brief 
        Matrix3d getOUDTransitionRates() { return oudTransitionRates; }

        /// @brief 
        Matrix3dOverTime getInterventionTransitionRates() { return interventionTransitionRates; }

        /// @brief 
        /// @return 
        Matrix3dOverTime getOverdoseRates() { return overdoseRates; }

        /// @brief 
        Matrix3dOverTime getFatalOverdoseRates() { return fatalOverdoseRates; }

        /// @brief 
        Matrix3d getMortalityRates() { return mortalityRates; }

        /// @brief 
        /// @return 
        Matrix3d getInterventionInitRates() { return interventionInitRates; }

        /// @brief 
        /// @return 
        std::vector<std::string> getInterventions() { return interventions; }

        /// @brief 
        /// @return 
        std::vector<std::string> getOUDStates() { return oudStates; }

        /// @brief 
        /// @return 
        int getAgingInterval() { return agingInterval; }

        /// @brief 
        /// @return 
        int getAgeGroupShift() { return ageGroupShift; }

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3d loadInitialSample(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3dOverTime loadEnteringSamples(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3d loadOUDTransitionRates(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3d loadInterventionInitRates(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3dOverTime loadInterventionTransitionRates(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3dOverTime loadOverdoseRates(std::string csvName);

        /// @brief 
        /// @param csvName 
        /// @return 
        Matrix3dOverTime loadFatalOverdoseRates(std::string csvName);
        
        /// @brief 
        /// @param smrCSVName 
        /// @param bgmCSVName 
        /// @return 
        Matrix3d loadMortalityRates(std::string smrCSVName, std::string bgmCSVName);

    private:
        InputTable removeColumns(std::string colString, InputTable ogTable);
        Matrix3d createTransitionMatrix3d(std::vector<std::vector<int>> indicesVec, InputTable table, Data::Dimension dimension);
        Matrix3d buildInterventionMatrix(std::vector<int> indices, InputTable table);
        Matrix3d buildOverdoseTransitions(InputTable table, std::string key);
        std::vector<int> findIndices(std::vector<std::string> const &v, std::string target);
        std::vector<std::vector<int>> getIndicesByIntervention(std::vector<std::string> col);
        Matrix3dOverTime buildTransitionRatesOverTime(std::vector<int> ict, InputTable table, std::vector<std::vector<int>> indicesVec);
        std::string dirName;

        int duration;
        int agingInterval;
        int ageGroupShift;
        int numOUDStates;
        int numInterventions;
        int numDemographics;
        int numDemographicCombos;

        std::vector<std::string> interventions;
        std::vector<std::string> oudStates;
        std::vector<int> demographicCounts;
        std::map<std::string, std::vector<int>> simulationParameters;

        Configuration Config;

        Matrix3d initialSample;
        Matrix3dOverTime enteringSamples;
        Matrix3d oudTransitionRates;
        Matrix3d interventionInitRates;
        Matrix3dOverTime interventionTransitionRates;
        Matrix3dOverTime overdoseRates;
        Matrix3dOverTime fatalOverdoseRates;
        Matrix3d mortalityRates;

        std::unordered_map<std::string, InputTable> inputTables;
    };
}
#endif // DATA_DATALOADER_HPP_
