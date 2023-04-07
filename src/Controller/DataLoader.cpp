#include "DataLoader.hpp"

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

/// @brief Reads input files from the provided input directory
/// @param inputDir the directory from which to read input files
std::unordered_map<std::string, InputTable> DataLoader::readInputDir(const std::string& inputDir) {
    std::string inputDirFixed = inputDir;
    std::unordered_map<std::string, InputTable> toReturn;
    if (inputDirFixed.back() != '/') {
        inputDirFixed.push_back('/');
    }

    for (std::string& inputFile: INPUT_FILES) {
        toReturn[inputFile] = readCSV(inputDir + inputFile);
    }
    return toReturn;
}
