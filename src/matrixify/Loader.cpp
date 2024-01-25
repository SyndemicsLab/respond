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
    Loader::Loader(std::string const &inputDir) : Loader(inputDir, nullptr) {}

    Loader::Loader(std::shared_ptr<spdlog::logger> logger)
        : Loader("", logger) {}

    Loader::Loader(std::string const &inputDir,
                   std::shared_ptr<spdlog::logger> logger) {
        if (!inputDir.empty()) {
            this->Config = std::make_shared<Data::Configuration>(inputDir);
        }

        if (!logger) {
            logger = spdlog::stdout_color_mt("console");
        }
        this->logger = logger;
    }

    Data::IConfigurationPtr
    Loader::loadConfigurationFile(std::string const &configPath) {
        if (!configPath.empty()) {
            this->Config = std::make_shared<Data::Configuration>(configPath);
        }
        return this->Config;
    }

    Data::IDataTablePtr Loader::readCSV(std::string const &inputFile) {
        Data::IDataTablePtr table =
            std::make_shared<Data::DataTable>(inputFile);
        return table;
    }

    std::unordered_map<std::string, Data::IDataTablePtr>
    Loader::readInputDir(std::string const &inputDir) {
        std::filesystem::path inputDirFixed = inputDir;
        std::unordered_map<std::string, Data::IDataTablePtr> toReturn;

        for (std::string inputFile : INPUT_FILES) {
            std::filesystem::path filePath = inputDirFixed / inputFile;
            toReturn[inputFile] = readCSV(filePath.string());
        }
        return toReturn;
    }
} // namespace Matrixify
