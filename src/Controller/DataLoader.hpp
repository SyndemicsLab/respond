#ifndef CONTROLLER_DATALOADER_HPP_
#define CONTROLLER_DATALOADER_HPP_

#include <boost/tokenizer.hpp>
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
        std::string getDirName();
        Configuration readConfigFile(const std::string&);
        InputTable readCSV(const std::string&);
        std::unordered_map<std::string, InputTable> readInputDir(const std::string&);
    private:
        std::string dirName;
        Configuration config;
        std::unordered_map<std::string, InputTable> inputTables;
    };
}
#endif // CONTROLLER_DATALOADER_HPP_
