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
        Matrix3d getInitialGroup() { return initialGroup; }
        Matrix3dOverTime getEnteringSamples() { return enteringSamples; }
        Matrix3d getOUDTransitions() { return oudTransitions; }
        Matrix3dOverTime getInterventionTransitions() { return interventionTransitions; }
        Matrix3dOverTime getOverdoseTransitions() { return overdoseTransitions; }
        Matrix3dOverTime getFatalOverdoseTransitions() { return fatalOverdoseTransitions; }
        Matrix3d getMortalityTransitions() { return mortalityTransitions; }
        std::vector<std::string> getInterventions() { return interventions; }
        std::vector<std::string> getOUDStates() { return oudStates; }

        // Loading from Tables
        void LoadInitialGroup(std::string csvName);
        void LoadEnteringSamples(std::string csvName);
        void LoadOUDTransitions(std::string csvName);
        void LoadInterventionTransitions(std::string csvName);
        void LoadOverdoseTransitions(std::string csvName);
        void LoadFatalOverdoseTransitions(std::string csvName);
        void LoadMortalityTransitions(std::string csvName);

    private:
        InputTable RemoveColumns(std::string colString, InputTable ogTable);
        Matrix3d BuildMatrixFromTransitionData(std::vector<std::vector<int>> indicesVec, InputTable table, Data::Dimension dimension);
        Matrix3d BuildInterventionMatrix(std::vector<int> indices, InputTable table);
        Matrix3d BuildOverdoseTransitions(InputTable table, std::string key);
        std::vector<int> FindIndices(std::vector<std::string> const &v, std::string target);
        std::vector<int> FindIndicesExactMatch(std::vector<std::string> const &v, std::string target);
        std::vector<std::vector<int>> GetIndicesVector(std::vector<std::string> col);
        Matrix3dOverTime CalcInterventionTransitions(std::vector<int> ict, InputTable table, std::vector<std::vector<int>> indicesVec);
        std::string dirName;

        int duration;
        int numOUDStates;
        int numInterventions;
        int numDemographics;
        int numDemographicCombos;

        std::vector<std::string> interventions;
        std::vector<std::string> oudStates;
        // std::vector<std::vector<std::string>> demographicCombos;
        std::vector<int> demographicCounts;
        std::map<std::string, std::vector<int>> simulationParameters;

        Configuration Config;

        Matrix3d initialGroup;
        Matrix3dOverTime enteringSamples;
        Matrix3d oudTransitions;
        Matrix3dOverTime interventionTransitions;
        Matrix3dOverTime overdoseTransitions;
        Matrix3dOverTime fatalOverdoseTransitions;
        Matrix3d mortalityTransitions;

        std::unordered_map<std::string, InputTable> inputTables;
    };
}
#endif // DATA_DATALOADER_HPP_
