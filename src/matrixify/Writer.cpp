//===-- Writer.cpp - Writer class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Writer class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Writer.hpp"
#include <regex>

namespace Matrixify {
    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Matrixify::FILE
    /// @return string containing the result if output enum is Matrixify::STRING
    /// or description of status otherwise
    std::string HistoryWriter::writeHistory(const History history) const {
        if (history.stateHistory.getMatrices().empty() ||
            history.overdoseHistory.getMatrices().empty() ||
            history.fatalOverdoseHistory.getMatrices().empty() ||
            history.mortalityHistory.getMatrices().empty() ||
            this->getDirname().empty()) {
            // log error
            std::ostringstream s;
            return s.str();
        }

        if (this->getWriteType() == FILE) {
            if (!std::filesystem::exists(this->getDirname())) {
                std::stringstream dircheck(this->getDirname());
                std::string buildingPath = "";
                std::string temp;
                while (getline(dircheck, temp, '/')) {
                    buildingPath += temp + "/";
                    if (!std::filesystem::exists(buildingPath)) {
                        std::filesystem::create_directory(buildingPath);
                    }
                }
                std::filesystem::create_directory(this->getDirname());
            }

            std::filesystem::path dir(this->getDirname());
            std::filesystem::path stateFile("stateHistory.csv");
            std::filesystem::path overdoseFile("overdoseHistory.csv");
            std::filesystem::path fatalOverdoseFile("fatalOverdoseHistory.csv");
            std::filesystem::path mortalityFile("mortalityHistory.csv");
            std::filesystem::path admissionsFile("admissionsHistory.csv");
            std::filesystem::path stateFullPath = dir / stateFile;
            std::filesystem::path overdoseFullPath = dir / overdoseFile;
            std::filesystem::path fatalOverdoseFullPath =
                dir / fatalOverdoseFile;
            std::filesystem::path mortalityFullPath = dir / mortalityFile;
            std::filesystem::path admissionsFullPath = dir / admissionsFile;

            std::ofstream file;

            file.open(stateFullPath.string());
            this->writer(file, history.stateHistory);
            file.close();

            file.open(overdoseFullPath.string());
            this->writer(file, history.overdoseHistory);
            file.close();

            file.open(fatalOverdoseFullPath.string());
            this->writer(file, history.fatalOverdoseHistory);
            file.close();

            file.open(mortalityFullPath.string());
            this->writer(file, history.mortalityHistory);
            file.close();

            file.open(admissionsFullPath.string());
            this->writer(file, history.interventionAdmissionHistory);
            file.close();
            return "success";
        }
        std::ostringstream stringstream;
        this->writer(stringstream, history.stateHistory);
        this->writer(stringstream, history.overdoseHistory);
        this->writer(stringstream, history.fatalOverdoseHistory);
        this->writer(stringstream, history.mortalityHistory);
        return stringstream.str();
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Matrixify::FILE
    /// @return string containing the result if output enum is Matrixify::STRING
    /// or description of status otherwise
    std::string CostWriter::writeCosts(const CostList costs) const {
        std::ostringstream stringstream;

        for (Cost cost : costs) {

            if (cost.healthcareCost.getMatrices().empty() ||
                cost.pharmaCost.getMatrices().empty() ||
                cost.fatalOverdoseCost.getMatrices().empty() ||
                cost.nonFatalOverdoseCost.getMatrices().empty() ||
                cost.treatmentCost.getMatrices().empty()) {
                // log error
                std::ostringstream s;
                return s.str();
            }

            if (this->getWriteType() == FILE) {
                if (!std::filesystem::exists(this->getDirname())) {
                    std::filesystem::create_directory(this->getDirname());
                }

                std::filesystem::path dir(this->getDirname());

                std::filesystem::path healthUtilFile("healthcareCost-" +
                                                     cost.perspective + ".csv");
                std::filesystem::path pharmaFile("pharmaCost-" +
                                                 cost.perspective + ".csv");
                std::filesystem::path fatalOverdoseFile(
                    "fatalOverdoseCost-" + cost.perspective + ".csv");
                std::filesystem::path nonFatalOverdoseFile(
                    "nonFatalOverdoseCost-" + cost.perspective + ".csv");
                std::filesystem::path treatmentFile("treatmentCost-" +
                                                    cost.perspective + ".csv");
                std::filesystem::path healthUtilFullPath = dir / healthUtilFile;
                std::filesystem::path pharmaFullPath = dir / pharmaFile;
                std::filesystem::path fatalOverdoseFullPath =
                    dir / fatalOverdoseFile;
                std::filesystem::path nonFatalOverdoseFullPath =
                    dir / nonFatalOverdoseFile;
                std::filesystem::path treatmentFullPath = dir / treatmentFile;

                std::ofstream file;

                file.open(healthUtilFullPath.string());

                this->writer(file, cost.healthcareCost);
                file.close();

                file.open(pharmaFullPath.string());
                this->writer(file, cost.pharmaCost);
                file.close();

                file.open(fatalOverdoseFullPath.string());
                this->writer(file, cost.fatalOverdoseCost);
                file.close();

                file.open(nonFatalOverdoseFullPath.string());
                this->writer(file, cost.nonFatalOverdoseCost);
                file.close();

                file.open(treatmentFullPath.string());
                this->writer(file, cost.treatmentCost);
                file.close();
            }

            this->writer(stringstream, cost.healthcareCost);
            this->writer(stringstream, cost.pharmaCost);
            this->writer(stringstream, cost.fatalOverdoseCost);
            this->writer(stringstream, cost.nonFatalOverdoseCost);
            this->writer(stringstream, cost.treatmentCost);
        }
        if (this->getWriteType() == FILE) {
            return "success";
        }
        return stringstream.str();
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Matrixify::FILE
    /// @return string containing the result if output enum is Matrixify::STRING
    /// or description of status otherwise
    std::string
    UtilityWriter::writeUtilities(const Matrixify::Matrix4d utilities) const {
        std::ostringstream stringstream;

        if (utilities.getMatrices().empty()) {
            // log error
            std::ostringstream s;
            return s.str();
        }

        if (this->getWriteType() == FILE) {
            if (!std::filesystem::exists(this->getDirname())) {
                std::filesystem::create_directory(this->getDirname());
            }

            std::filesystem::path dir(this->getDirname());
            std::filesystem::path utilFile("utility.csv");
            std::filesystem::path utilFullPath = dir / utilFile;

            std::ofstream file;

            file.open(utilFullPath.string());
            this->writer(file, utilities);
            file.close();
        }
        this->writer(stringstream, utilities);

        if (this->getWriteType() == FILE) {
            return "success";
        }
        return stringstream.str();
    }

    std::string TotalsWriter::writeTotals(const Totals totals) const {
        std::ostringstream stringstream;

        if (totals.baseCosts.empty() || totals.discCosts.empty()) {
            // log error
            std::ostringstream s;
            return s.str();
        }

        if (this->getWriteType() == FILE) {
            if (!std::filesystem::exists(this->getDirname())) {
                std::filesystem::create_directory(this->getDirname());
            }

            std::filesystem::path dir(this->getDirname());
            std::filesystem::path utilFile("totals.csv");
            std::filesystem::path utilFullPath = dir / utilFile;

            std::ofstream file;

            file.open(utilFullPath.string());
            file << "Name, Base, Discount" << std::endl;
            for (int i = 0; i < totals.baseCosts.size(); ++i) {
                file << "Perspective " << i << ", " << totals.baseCosts[i]
                     << ", " << totals.discCosts[i] << std::endl;
            }
            file << "Life Years, " << totals.baseLifeYears << ", "
                 << totals.discLifeYears << std::endl;
            file << "Utility, " << totals.baseUtility << ", "
                 << totals.discUtility << std::endl;

            file.close();
        }

        stringstream << "Name, Base, Discount" << std::endl;
        for (int i = 0; i < totals.baseCosts.size(); ++i) {
            stringstream << "Perspective " << i << ", " << totals.baseCosts[i]
                         << ", " << totals.discCosts[i] << std::endl;
        }
        stringstream << "Life Years, " << totals.baseLifeYears << ", "
                     << totals.discLifeYears << std::endl;
        stringstream << "Utility, " << totals.baseUtility << ", "
                     << totals.discUtility << std::endl;

        if (this->getWriteType() == FILE) {
            return "success";
        }
        return stringstream.str();
    }

    std::string InputWriter::writeInputs(const DataLoader dataLoader) const {
        if (this->getWriteType() != WriteType::FILE) {
            std::ostringstream stringstream;
            this->writer(stringstream, history.stateHistory);
            this->writer(stringstream, history.overdoseHistory);
            this->writer(stringstream, history.fatalOverdoseHistory);
            this->writer(stringstream, history.mortalityHistory);
            return stringstream.str();
        }
        if (this->getDirname().empty()) {
            // log error
            std::ostringstream s;
            return s.str();
        }

        if (!std::filesystem::exists(this->getDirname())) {
            std::stringstream dircheck(this->getDirname());
            std::string buildingPath = "";
            std::string temp;
            while (getline(dircheck, temp, '/')) {
                buildingPath += temp + "/";
                if (!std::filesystem::exists(buildingPath)) {
                    std::filesystem::create_directory(buildingPath);
                }
            }
            std::filesystem::create_directory(this->getDirname());
        }

        std::filesystem::path dir(this->getDirname());

        std::filesystem::path overdoseFile("overdoseHistory.csv");
        std::filesystem::path fatalOverdoseFile("fatalOverdoseHistory.csv");
        std::filesystem::path mortalityFile("mortalityHistory.csv");
        std::filesystem::path admissionsFile("admissionsHistory.csv");
        std::filesystem::path stateFullPath = dir / stateFile;
        std::filesystem::path overdoseFullPath = dir / overdoseFile;
        std::filesystem::path fatalOverdoseFullPath = dir / fatalOverdoseFile;
        std::filesystem::path mortalityFullPath = dir / mortalityFile;
        std::filesystem::path admissionsFullPath = dir / admissionsFile;

        std::ofstream file;

        file.open(stateFullPath.string());
        this->writer(file, history.stateHistory);
        file.close();

        file.open(overdoseFullPath.string());
        this->writer(file, history.overdoseHistory);
        file.close();

        file.open(fatalOverdoseFullPath.string());
        this->writer(file, history.fatalOverdoseHistory);
        file.close();

        file.open(mortalityFullPath.string());
        this->writer(file, history.mortalityHistory);
        file.close();

        file.open(admissionsFullPath.string());
        this->writer(file, history.interventionAdmissionHistory);
        file.close();
        return "success";
    }

    std::string InputWriter::writeOUDTransitionRates(Matrix3d oudRates) const {
        std::filesystem::path orFile("oud_trans.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / orFile;

        std::ofstream file;
        file.open(fullPath.string());
        this->writer(file, history.stateHistory);
        file.close();
    }

    std::string InputWriter::writeInterventionInitRates(
        Matrix3d interventionInitProb) const {
        std::filesystem::path iipFile("block_init_effect.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / iipFile;

        std::ofstream file;
        file.open(fullPath.string());
        this->writer(file, history.stateHistory);
        file.close();
    }

    std::string InputWriter::writeInterventionTransitionRates(
        Matrix4d interventionRates) const {
        std::filesystem::path irFile("block_trans.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / irFile;

        std::ofstream file;
        file.open(fullPath.string());
        this->writer(file, history.stateHistory);
        file.close();
    }

    std::string InputWriter::writeOverdoseRates(Matrix4d odRate) const {
        std::filesystem::path odFile("all_types_overdose.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / odFile;

        std::ofstream file;
        file.open(fullPath.string());
        this->writer(file, history.stateHistory);
        file.close();
    }

    std::string InputWriter::writeFatalOverdoseRates(Matrix4d fodRate) const {
        std::filesystem::path fodFile("fatal_overdose.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / fodFile;

        std::ofstream file;
        file.open(fullPath.string());
        this->writer(file, history.stateHistory);
        file.close();
    }

    /// @brief Helper function to write data to the specified stream
    /// @param stream Stream type to write data to
    /// @param historyToWrite Specific portion of history to save
    void Writer::writer(std::ostream &stream,
                        const Matrix4d historyToWrite) const {
        std::vector<Matrix3d> Matrix3dVec = historyToWrite.getMatrices();
        stream << writeColumnHeaders() << std::endl;
        for (long int i = 0; i < this->interventions.size(); i++) {
            for (long int j = 0; j < this->oudStates.size(); j++) {
                for (long int k = 0; k < this->demographicCombos.size(); k++) {
                    stream << this->interventions[i] << ",";
                    stream << this->oudStates[j] << ",";
                    std::string temp = demographicCombos[k];
                    temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"),
                                              "$1");
                    std::replace(temp.begin(), temp.end(), ' ', ',');
                    stream << temp << ",";

                    for (Matrix3d dm : Matrix3dVec) {
                        std::array<long int, 3> index = {0, 0, 0};
                        index[Matrixify::INTERVENTION] = i;
                        index[Matrixify::OUD] = j;
                        index[Matrixify::DEMOGRAPHIC_COMBO] = k;
                        ASSERTM(dm.NumDimensions == 3,
                                "3 Dimensions Found in Matrix3d");
                        double value = dm(index[0], index[1], index[2]);
                        stream << std::to_string(value) << ",";
                    }
                    stream << std::endl;
                }
            }
        }
    }

    /// @brief Helper function to write Headers to CSVs
    /// @param timesteps Total duration incurred during the simulation
    /// @return String containing the CSV Column Headers
    std::string Writer::writeColumnHeaders() const {
        std::string ret = "Interventions, OUD States,";
        for (std::string demographic : this->demographics) {
            ret += demographic + ",";
        }
        for (int timestep : this->timesteps) {
            ret += "t+" + std::to_string(timestep) + ",";
        }
        return ret;
    }
} // namespace Matrixify
