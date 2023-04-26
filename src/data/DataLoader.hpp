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

#include "configuration.hpp"
#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"

namespace Data {
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

    class DataLoader {
    public:
        ~DataLoader() {};
        DataLoader() {};
        DataLoader(std::string);
        Configuration readConfigFile(std::string);
        InputTable readCSV(std::string);
        std::unordered_map<std::string, InputTable> readInputDir(std::string);

        // getter functions
        std::string getDirName() { return dirName; }
        int getDuration() { return duration; }
        int getNumOUDStates() { return numOUDStates; }
        int getNumInterventions() { return numInterventions; }
        int getNumDemographics() { return numDemographics; }
        Matrix3d getInitialSample() { return initialSample; }
        Matrix3dOverTime getEnteringSamples() { return enteringSamples; }
        Matrix3d getOUDTransitionRates() { return oudTransitionRates; }
        Matrix3dOverTime getInterventionTransitionRates() { return interventionTransitionRates; }
        Matrix3dOverTime getOverdoseRates() { return overdoseRates; }
        Matrix3dOverTime getFatalOverdoseRates() { return fatalOverdoseRates; }
        Matrix3d getMortalityRates() { return mortalityRates; }
        std::vector<std::string> getInterventions() { return interventions; }
        std::vector<std::string> getOUDStates() { return oudStates; }
        int getAgingInterval() { return agingInterval; }
        int getAgeGroupShift() { return ageGroupShift; }

        // Loading from Tables
        Matrix3d loadInitialSample(std::string csvName);
        Matrix3dOverTime loadEnteringSamples(std::string csvName);
        Matrix3d loadOUDTransitionRates(std::string csvName);
        Matrix3dOverTime loadInterventionTransitionRates(std::string csvName);
        Matrix3dOverTime loadOverdoseRates(std::string csvName);
        Matrix3dOverTime loadFatalOverdoseRates(std::string csvName);
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
        Matrix3dOverTime interventionTransitionRates;
        Matrix3dOverTime overdoseRates;
        Matrix3dOverTime fatalOverdoseRates;
        Matrix3d mortalityRates;

        std::unordered_map<std::string, InputTable> inputTables;
    };
}
#endif // DATA_DATALOADER_HPP_
