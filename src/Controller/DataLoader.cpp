#include "DataLoader.hpp"

using namespace Data;

DataLoader::DataLoader(const std::string& inputDir):
    dirName(inputDir), inputTables(readInputDir(inputDir)) {
    std::string configPath = inputDir;
    // account for no trailing slash in the provided input directory
    if (configPath.back() != '/') {
        configPath.push_back('/');
    }
    configPath += "sim.conf";
    config = readConfigFile(configPath);
}

std::string DataLoader::getDirName() {
    return dirName;
}

/// @brief Reads a configuration file to a Configuration object
/// @param inputFile path to the configuration file to be read
/// @return
Configuration DataLoader::readConfigFile(const std::string& inputFile) {
    return configure(inputFile);
}

/// @brief Read a CSV-formatted file into a map object where the headers are
/// keys and the rest of the columns are stored as vectors of strings
/// @param inputFile path to the CSV to be read
/// @return A map object containing columns stored as key-value pairs
InputTable DataLoader::readCSV(const std::string& inputFile) {
    using boost::tokenizer;

    std::string inputContents;
    std::ifstream inputStream(inputFile);
    InputTable toReturn;

    // use the headers of the csv table to create keys for the unordered map
    std::getline(inputStream, inputContents);
    std::vector<std::string> headerNames;
    tokenizer<boost::escaped_list_separator<char>> token(inputContents);
    for (tokenizer<boost::escaped_list_separator<char>>::iterator beg = token.begin();
         beg != token.end(); ++beg) {
        headerNames.push_back(*beg);
        toReturn[*beg] = {};
    }
    // populate the keys/headers with values
    int i = 0;
    while (std::getline(inputStream, inputContents)) {
        tokenizer<boost::escaped_list_separator<char>> token(inputContents);
        for (tokenizer<boost::escaped_list_separator<char>>::iterator beg = token.begin();
             beg != token.end(); ++beg) {
            toReturn[headerNames[i % headerNames.size()]].push_back(*beg);
            ++i;
        }
    }
    inputStream.close();
    return toReturn;
}

/// @brief Reads typical RESPOND input files from the provided input directory
/// @param inputDir the directory from which to read input files
/// @return an unordered map whose keys are table names and values are
/// CSV/InputTables
std::unordered_map<std::string, InputTable> DataLoader::readInputDir(const std::string& inputDir) {
    std::string inputDirFixed = inputDir;
    std::unordered_map<std::string, InputTable> toReturn;
    // account for no trailing slash in the provided input directory
    if (inputDirFixed.back() != '/') {
        inputDirFixed.push_back('/');
    }

    for (std::string& inputFile: INPUT_FILES) {
        toReturn[inputFile] = readCSV(inputDirFixed + inputFile);
    }
    return toReturn;
}

void DataLoader::loadSimulationInputs(Simulation::Sim& sim) {
    // for reference -- these are the methods to account for in this function
    // void LoadEnteringSamples(Matrix3dOverTime enteringSamples) override;
    // void LoadOUDTransitions(Matrix3dOverTime oudTransitions) override;
    // void LoadTreatmentTransitions(Matrix3dOverTime treatmentTransitions) override;
    // void LoadOverdoseTransitions(Matrix3dOverTime overdoseTransitions) override;
    // void LoadMortalityTransitions(Matrix3dOverTime mortalityTransitions) override;

    // Create the Initial Group
    // void LoadInitialGroup(Matrix3d initialGroup) override;
    Matrix3d initialGroup;
    InputTable& initialCohort = inputTables["init_cohort.csv"];
}
