#ifndef CONTROLLER_DATALOADER_HPP_
#define CONTROLLER_DATALOADER_HPP_

#include <boost/tokenizer.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>

#include "configuration.hpp"
#include "DataTypes.hpp"

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
        DataLoader() {};
        DataLoader(const std::string&);
        Configuration readConfigFile(const std::string&);
        InputTable readCSV(const std::string&);
        std::unordered_map<std::string, InputTable> readInputDir(const std::string&);

        // getter functions
        std::string getDirName() { return dirName; }
        Matrix3d getInitialGroup() { return initialGroup; }
        Matrix3dOverTime getEnteringSamples() { return enteringSamples; }
        Matrix3d getOUDTransitions() { return oudTransitions; }
        Matrix3dOverTime getInterventionTransitions() { return interventionTransitions; }
        Matrix3dOverTime getOverdoseTransitions() { return overdoseTransitions; }
        Matrix3dOverTime getFatalOverdoseTransitions() { return fatalOverdoseTransitions; }
        Matrix3d getMortalityTransitions() { return mortalityTransitions; }
    private:
        std::string dirName;
        Matrix3d initialGroup;
        Matrix3dOverTime enteringSamples;
        Matrix3d oudTransitions;
        Matrix3dOverTime interventionTransitions;
        Matrix3dOverTime overdoseTransitions;
        Matrix3dOverTime fatalOverdoseTransitions;
        Matrix3d mortalityTransitions;
    };
}
#endif // CONTROLLER_DATALOADER_HPP_
