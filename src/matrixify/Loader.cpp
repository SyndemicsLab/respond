//===-- Loader.cpp - Loader class definition --------------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Loader class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Loader.hpp"

namespace Matrixify {
    Loader::Loader(std::string const &inputDir) {
        // PROCESSING INPUT FILES
        std::filesystem::path configPath =
            (std::filesystem::path)inputDir / "sim.conf";
        this->Config = readConfigFile(configPath.string());
        this->inputTables = readInputDir(inputDir);
    }

    Configuration Loader::loadConfigurationFile(std::string const &configPath) {
        this->Config = readConfigFile(configPath);
        return this->Config;
    }

    InputTable Loader::readCSV(std::string const &inputFile) {
        using boost::tokenizer;

        std::string inputContents;
        std::ifstream inputStream(inputFile);
        InputTable toReturn;

        // use the headers of the csv table to create keys for the unordered map
        std::getline(inputStream, inputContents);
        std::vector<std::string> headerNames;
        tokenizer<boost::escaped_list_separator<char>> token(inputContents);
        for (tokenizer<boost::escaped_list_separator<char>>::iterator beg =
                 token.begin();
             beg != token.end(); ++beg) {
            headerNames.push_back(*beg);
            toReturn[*beg] = {};
        }
        // populate the keys/headers with values
        int i = 0;
        while (std::getline(inputStream, inputContents)) {
            tokenizer<boost::escaped_list_separator<char>> token(inputContents);
            for (tokenizer<boost::escaped_list_separator<char>>::iterator beg =
                     token.begin();
                 beg != token.end(); ++beg) {
                toReturn[headerNames[i % headerNames.size()]].push_back(*beg);
                ++i;
            }
        }
        inputStream.close();
        return toReturn;
    }

    std::unordered_map<std::string, InputTable>
    Loader::readInputDir(std::string const &inputDir) {
        std::filesystem::path inputDirFixed = inputDir;
        std::unordered_map<std::string, InputTable> toReturn;

        for (std::string inputFile : INPUT_FILES) {
            std::filesystem::path filePath = inputDirFixed / inputFile;
            toReturn[inputFile] = readCSV(filePath.string());
        }
        return toReturn;
    }

    Configuration Loader::readConfigFile(std::string const &inputFile) {
        Configuration config(inputFile);
        return config;
    }
} // namespace Matrixify
