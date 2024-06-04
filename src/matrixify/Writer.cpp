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
    bool Writer::checkDirectory() const {
        if (this->getDirname().empty()) {
            // log error
            return false;
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
        return true;
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Matrixify::FILE
    /// @return string containing the result if output enum is Matrixify::STRING
    /// or description of status otherwise
    std::string HistoryWriter::writeHistory(const History history) const {
        if (history.stateHistory.getMatrices().empty() ||
            history.overdoseHistory.getMatrices().empty() ||
            history.fatalOverdoseHistory.getMatrices().empty() ||
            history.mortalityHistory.getMatrices().empty()) {
            // log error
            std::ostringstream s;
            return s.str();
        }

        if (this->getWriteType() == FILE) {
            if (!checkDirectory()) {
                return "";
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
            this->write4d(file, history.stateHistory, writeColumnHeaders());
            file.close();

            file.open(overdoseFullPath.string());
            this->write4d(file, history.overdoseHistory, writeColumnHeaders());
            file.close();

            file.open(fatalOverdoseFullPath.string());
            this->write4d(file, history.fatalOverdoseHistory,
                          writeColumnHeaders());
            file.close();

            file.open(mortalityFullPath.string());
            this->write4d(file, history.mortalityHistory, writeColumnHeaders());
            file.close();

            file.open(admissionsFullPath.string());
            this->write4d(file, history.interventionAdmissionHistory,
                          writeColumnHeaders());
            file.close();
            return "success";
        }
        std::ostringstream stringstream;
        this->write4d(stringstream, history.stateHistory, writeColumnHeaders());
        this->write4d(stringstream, history.overdoseHistory,
                      writeColumnHeaders());
        this->write4d(stringstream, history.fatalOverdoseHistory,
                      writeColumnHeaders());
        this->write4d(stringstream, history.mortalityHistory,
                      writeColumnHeaders());
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

                this->write4d(file, cost.healthcareCost, writeColumnHeaders());
                file.close();

                file.open(pharmaFullPath.string());
                this->write4d(file, cost.pharmaCost, writeColumnHeaders());
                file.close();

                file.open(fatalOverdoseFullPath.string());
                this->write4d(file, cost.fatalOverdoseCost,
                              writeColumnHeaders());
                file.close();

                file.open(nonFatalOverdoseFullPath.string());
                this->write4d(file, cost.nonFatalOverdoseCost,
                              writeColumnHeaders());
                file.close();

                file.open(treatmentFullPath.string());
                this->write4d(file, cost.treatmentCost, writeColumnHeaders());
                file.close();
            }

            this->write4d(stringstream, cost.healthcareCost,
                          writeColumnHeaders());
            this->write4d(stringstream, cost.pharmaCost, writeColumnHeaders());
            this->write4d(stringstream, cost.fatalOverdoseCost,
                          writeColumnHeaders());
            this->write4d(stringstream, cost.nonFatalOverdoseCost,
                          writeColumnHeaders());
            this->write4d(stringstream, cost.treatmentCost,
                          writeColumnHeaders());
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
            this->write4d(file, utilities, writeColumnHeaders());
            file.close();
        }
        this->write4d(stringstream, utilities, writeColumnHeaders());

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

    std::string InputWriter::writeInputs(
        const std::shared_ptr<IDataLoader> dataLoader) const {

        std::string res = writeOUDTransitionRates(dataLoader);
        res = res + " " + writeInterventionTransitionRates(dataLoader);
        res = res + " " + writeInterventionInitRates(dataLoader);
        res = res + " " + writeOverdoseRates(dataLoader);
        res = res + " " + writeFatalOverdoseRates(dataLoader);
        return res;
    }

    std::string InputWriter::writeOUDTransitionRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::filesystem::path orFile("oud_trans.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / orFile;

        std::vector<std::string> columnNames{"intervention", "agegrp", "sex",
                                             "initial_oud"};
        for (std::string oud : dataLoader->getOUDStates()) {
            columnNames.push_back(oud);
        }

        std::ofstream file;
        file.open(fullPath.string());

        for (std::string col : columnNames) {
            file << col << ",";
        }
        file << std::endl;

        Matrix3d dm = dataLoader->getOUDTransitionRates();

        for (long int init = 0; init < dataLoader->getNumOUDStates(); init++) {
            for (long int i = 0; i < dataLoader->getNumInterventions(); i++) {
                for (long int k = 0;
                     k < dataLoader->getDemographicCombos().size(); k++) {
                    file << dataLoader->getInterventions()[i] << ",";
                    std::string temp = dataLoader->getDemographicCombos()[k];
                    temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"),
                                              "$1");
                    std::replace(temp.begin(), temp.end(), ' ', ',');
                    file << temp << ",";
                    file << dataLoader->getOUDStates()[init] << ",";
                    for (long int j = 0; j < dataLoader->getNumOUDStates();
                         j++) {
                        std::array<long int, 3> index = {0, 0, 0};
                        index[Matrixify::INTERVENTION] = i;
                        index[Matrixify::OUD] =
                            (init * dataLoader->getNumOUDStates()) + j;
                        index[Matrixify::DEMOGRAPHIC_COMBO] = k;
                        ASSERTM(dm.NumDimensions == 3,
                                "3 Dimensions Found in Matrix3d");
                        double value = dm(index[0], index[1], index[2]);
                        file << std::to_string(value) << ",";
                    }
                    file << std::endl;
                }
            }
        }
        file.close();
        return "success";
    }

    std::string InputWriter::writeInterventionInitRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::filesystem::path iipFile("block_init_effect.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / iipFile;

        std::vector<std::string> columnNames{"initial_oud_state",
                                             "to_intervention"};

        for (std::string oud : dataLoader->getOUDStates()) {
            columnNames.push_back(oud);
        }

        std::ofstream file;
        file.open(fullPath.string());

        for (std::string col : columnNames) {
            file << col << ",";
        }
        file << std::endl;

        Matrix3d dm = dataLoader->getInterventionInitRates();

        for (int init = 0; init < dataLoader->getNumOUDStates(); init++) {
            for (int inter = 0; inter < dataLoader->getNumInterventions();
                 inter++) {
                file << dataLoader->getOUDStates()[init] << ",";
                file << dataLoader->getInterventions()[inter] << ",";
                for (int res = 0; res < dataLoader->getNumOUDStates(); res++) {
                    std::array<long int, 3> index = {0, 0, 0};
                    index[Matrixify::INTERVENTION] = inter;
                    index[Matrixify::OUD] =
                        (init * dataLoader->getNumOUDStates()) + res;
                    index[Matrixify::DEMOGRAPHIC_COMBO] = 0;
                    ASSERTM(dm.NumDimensions == 3,
                            "3 Dimensions Found in Matrix3d");
                    double value = dm(index[0], index[1], index[2]);
                    file << std::to_string(value) << ",";
                }
                file << std::endl;
            }
        }
        file.close();
        return "success";
    }

    std::string InputWriter::writeInterventionTransitionRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::filesystem::path irFile("block_trans.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / irFile;

        std::ofstream file;
        file.open(fullPath.string());

        std::vector<std::string> columnNames{"agegrp", "sex", "oud",
                                             "initial_intervention"};

        std::vector<int> changeTimes = dataLoader->getInterventionChangeTimes();
        int ct1 = 1;
        for (int ct2 : changeTimes) {
            for (std::string col : dataLoader->getInterventions()) {
                columnNames.push_back(col + "_" + std::to_string(ct1) + "_" +
                                      std::to_string(ct2));
            }
            ct1 = ct2;
        }

        for (std::string col : columnNames) {
            file << col << ",";
        }
        file << std::endl;

        // add zero index and pop off final change time to get matrix indices
        changeTimes.insert(changeTimes.begin(), 0);
        changeTimes.pop_back();

        Matrix4d dm = dataLoader->getInterventionTransitionRates();

        for (long int k = 0; k < dataLoader->getNumDemographicCombos(); k++) {
            for (int oud = 0; oud < dataLoader->getNumOUDStates(); oud++) {
                for (int init = 0; init < dataLoader->getNumInterventions();
                     init++) {
                    std::string temp = dataLoader->getDemographicCombos()[k];
                    temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"),
                                              "$1");
                    std::replace(temp.begin(), temp.end(), ' ', ',');
                    file << temp << ",";
                    file << dataLoader->getOUDStates()[oud] << ",";
                    file << dataLoader->getInterventions()[init] << ",";
                    for (int timestep : changeTimes) {
                        for (int res = 0;
                             res < dataLoader->getNumInterventions(); res++) {
                            std::array<long int, 3> index = {0, 0, 0};
                            index[Matrixify::INTERVENTION] =
                                (init * dataLoader->getNumInterventions()) +
                                res;
                            index[Matrixify::OUD] = oud;
                            index[Matrixify::DEMOGRAPHIC_COMBO] = k;
                            double value =
                                dm(timestep, index[0], index[1], index[2]);
                            file << std::to_string(value) << ",";
                        }
                    }
                    file << std::endl;
                }
            }
        }
        file.close();
        return "success";
    }

    std::string InputWriter::writeOverdoseRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::filesystem::path odFile("all_types_overdose.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / odFile;

        std::ofstream file;
        file.open(fullPath.string());

        std::vector<std::string> columnNames{"intervention", "agegrp", "sex",
                                             "oud"};

        std::vector<int> changeTimes = dataLoader->getOverdoseChangeTimes();
        int ct1 = 1;
        for (int ct2 : changeTimes) {
            columnNames.push_back("overdose_" + std::to_string(ct1) + "_" +
                                  std::to_string(ct2));
            ct1 = ct2;
        }
        for (std::string col : columnNames) {
            file << col << ",";
        }
        file << std::endl;

        // add zero index and pop off final change time to get matrix indices
        changeTimes.insert(changeTimes.begin(), 0);
        changeTimes.pop_back();

        Matrix4d dm = dataLoader->getOverdoseRates();

        for (int inter = 0; inter < dataLoader->getNumInterventions(); inter++)
            for (long int dem = 0; dem < dataLoader->getNumDemographicCombos();
                 dem++) {
                for (int oud = 0; oud < dataLoader->getNumOUDStates(); oud++) {
                    {
                        file << dataLoader->getInterventions()[inter] << ",";
                        std::string temp =
                            dataLoader->getDemographicCombos()[dem];
                        temp = std::regex_replace(
                            temp, std::regex("^ +| +$|( ) +"), "$1");
                        std::replace(temp.begin(), temp.end(), ' ', ',');
                        file << temp << ",";
                        file << dataLoader->getOUDStates()[oud] << ",";
                        for (int timestep : changeTimes) {
                            std::array<long int, 3> index = {0, 0, 0};
                            index[Matrixify::INTERVENTION] = inter;
                            index[Matrixify::OUD] = oud;
                            index[Matrixify::DEMOGRAPHIC_COMBO] = dem;
                            double value =
                                dm(timestep, index[0], index[1], index[2]);
                            file << std::to_string(value) << ",";
                        }
                        file << std::endl;
                    }
                }
            }
        file.close();
        return "success";
    }

    std::string InputWriter::writeFatalOverdoseRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::filesystem::path fodFile("fatal_overdose.csv");
        std::filesystem::path dir(this->getDirname());
        std::filesystem::path fullPath = dir / fodFile;

        std::ofstream file;
        file.open(fullPath.string());

        std::vector<std::string> columnNames{};

        std::vector<int> changeTimes = dataLoader->getOverdoseChangeTimes();
        int ct1 = 1;
        for (int ct2 : changeTimes) {
            columnNames.push_back("percent_overdoses_fatal_" +
                                  std::to_string(ct1) + "_" +
                                  std::to_string(ct2));
            ct1 = ct2;
        }

        for (std::string col : columnNames) {
            file << col << ",";
        }
        file << std::endl;

        // add zero index and pop off final change time to get matrix indices
        changeTimes.insert(changeTimes.begin(), 0);
        changeTimes.pop_back();

        Matrix4d dm = dataLoader->getFatalOverdoseRates();
        for (int timestep : changeTimes) {
            file << std::to_string(dm(timestep, 0, 0, 0)) << ",";
        }
        file << std::endl;

        file.close();
        return "success";
    }

    /// @brief Helper function to write data to the specified stream
    /// @param stream Stream type to write data to
    /// @param historyToWrite Specific portion of history to save
    void OutputWriter::write4d(std::ostream &stream,
                               const Matrix4d historyToWrite,
                               const std::string columnHeaders) const {
        std::vector<Matrix3d> Matrix3dVec = historyToWrite.getMatrices();
        stream << columnHeaders << std::endl;
        for (long int i = 0; i < this->interventions.size(); i++) {
            for (long int j = 0; j < this->oudStates.size(); j++) {
                for (long int k = 0; k < this->demographicCombos.size(); k++) {
                    if (!this->pivot) {
                        stream << this->interventions[i] << ",";
                        stream << this->oudStates[j] << ",";
                        std::string temp = demographicCombos[k];
                        temp = std::regex_replace(
                            temp, std::regex("^ +| +$|( ) +"), "$1");
                        std::replace(temp.begin(), temp.end(), ' ', ',');
                        stream << temp << ",";
                    }

                    for (int timeCtr = 0; timeCtr < this->getTimesteps().size();
                         timeCtr++) {
                        if (this->pivot) {
                            stream << this->interventions[i] << ",";
                            stream << this->oudStates[j] << ",";
                            std::string temp = demographicCombos[k];
                            temp = std::regex_replace(
                                temp, std::regex("^ +| +$|( ) +"), "$1");
                            std::replace(temp.begin(), temp.end(), ' ', ',');
                            stream
                                << temp << ","
                                << std::to_string(this->getTimesteps()[timeCtr])
                                << ",";
                        }
                        std::array<long int, 3> index = {0, 0, 0};
                        index[Matrixify::INTERVENTION] = i;
                        index[Matrixify::OUD] = j;
                        index[Matrixify::DEMOGRAPHIC_COMBO] = k;
                        ASSERTM(Matrix3dVec[timeCtr].NumDimensions == 3,
                                "3 Dimensions Found in Matrix3d");
                        double value =
                            Matrix3dVec[timeCtr](index[0], index[1], index[2]);
                        stream << std::to_string(value) << ",";
                        if (this->pivot) {
                            stream << std::endl;
                        }
                    }
                    if (!this->pivot) {
                        stream << std::endl;
                    }
                }
            }
        }
    }

    /// @brief Helper function to write Headers to CSVs
    /// @param timesteps Total duration incurred during the simulation
    /// @return String containing the CSV Column Headers
    std::string OutputWriter::writeColumnHeaders() const {
        std::string ret = "Interventions, OUD States,";
        for (std::string demographic : this->demographics) {
            ret += demographic + ",";
        }
        if (this->pivot) {
            ret += "time, value";
        } else {
            for (int timestep : this->getTimesteps()) {
                ret += "t+" + std::to_string(timestep) + ",";
            }
        }

        return ret;
    }
} // namespace Matrixify
