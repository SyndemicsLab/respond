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
            return "failure";
        }

        std::string result = "";

        std::stringstream stream;

        this->write4d(stream, history.stateHistory, writeColumnHeaders());
        this->writeFile("stateHistory.csv", stream);
        result = stream.str();
        stream.str("");

        this->write4d(stream, history.overdoseHistory, writeColumnHeaders());
        this->writeFile("overdoseHistory.csv", stream);
        result = result + " " + stream.str();
        stream.str("");

        this->write4d(stream, history.fatalOverdoseHistory,
                      writeColumnHeaders());
        this->writeFile("fatalOverdoseHistory.csv", stream);
        result = result + " " + stream.str();
        stream.str("");

        this->write4d(stream, history.mortalityHistory, writeColumnHeaders());
        this->writeFile("mortalityHistory.csv", stream);
        result = result + " " + stream.str();
        stream.str("");

        this->write4d(stream, history.interventionAdmissionHistory,
                      writeColumnHeaders());
        this->writeFile("admissionsHistory.csv", stream);
        result = result + " " + stream.str();
        stream.str("");

        return result;
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Matrixify::FILE
    /// @return string containing the result if output enum is Matrixify::STRING
    /// or description of status otherwise
    std::string CostWriter::writeCosts(const CostList costs) const {
        if (costs.empty()) {
            return "failure";
        }
        std::string result = "";
        for (Cost cost : costs) {

            if (cost.healthcareCost.getMatrices().empty() ||
                cost.pharmaCost.getMatrices().empty() ||
                cost.fatalOverdoseCost.getMatrices().empty() ||
                cost.nonFatalOverdoseCost.getMatrices().empty() ||
                cost.treatmentCost.getMatrices().empty()) {
                // log error
                return "failure";
            }

            std::stringstream stream;

            this->write4d(stream, cost.healthcareCost, writeColumnHeaders());
            this->writeFile("healthcareCost-" + cost.perspective + ".csv",
                            stream);
            result = stream.str();
            stream.str("");

            this->write4d(stream, cost.pharmaCost, writeColumnHeaders());
            this->writeFile("pharmaCost-" + cost.perspective + ".csv", stream);
            result = result + " " + stream.str();
            stream.str("");

            this->write4d(stream, cost.fatalOverdoseCost, writeColumnHeaders());
            this->writeFile("fatalOverdoseCost-" + cost.perspective + ".csv",
                            stream);
            result = result + " " + stream.str();
            stream.str("");

            this->write4d(stream, cost.nonFatalOverdoseCost,
                          writeColumnHeaders());
            this->writeFile("nonFatalOverdoseCost-" + cost.perspective + ".csv",
                            stream);
            result = result + " " + stream.str();
            stream.str("");

            this->write4d(stream, cost.treatmentCost, writeColumnHeaders());
            this->writeFile("treatmentCost-" + cost.perspective + ".csv",
                            stream);
            result = result + " " + stream.str();
            stream.str("");
        }
        return result;
    }

    /// @brief Main Operation of Class, write data to output
    /// @param outputType Output Enum, generally Matrixify::FILE
    /// @return string containing the result if output enum is
    /// Matrixify::STRING or description of status otherwise
    std::string
    UtilityWriter::writeUtilities(const Matrixify::Matrix4d utilities) const {
        std::stringstream stream;
        if (utilities.getMatrices().empty()) {
            // log error
            return "failure";
        }
        this->write4d(stream, utilities, writeColumnHeaders());
        this->writeFile("utilities.csv", stream);
        return stream.str();
    }

    std::string TotalsWriter::writeTotals(const Totals totals) const {
        if (totals.baseCosts.empty() || totals.discCosts.empty()) {
            // log error
            return "failure";
        }

        std::stringstream stream;

        stream << "Name, Base, Discount" << std::endl;
        for (int i = 0; i < totals.baseCosts.size(); ++i) {
            stream << "Perspective " << i << ", " << totals.baseCosts[i] << ", "
                   << totals.discCosts[i] << std::endl;
        }
        stream << "Life Years, " << totals.baseLifeYears << ", "
               << totals.discLifeYears << std::endl;
        stream << "Utility, " << totals.baseUtility << ", "
               << totals.discUtility << std::endl;
        this->writeFile("totals.csv", stream);
        return stream.str();
    }

    std::string InputWriter::writeParameters(
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
        std::stringstream stream;

        std::vector<std::string> columnNames{"intervention", "agegrp", "race",
                                             "sex", "initial_oud"};
        for (std::string oud : dataLoader->getOUDStates()) {
            columnNames.push_back(oud);
        }

        for (std::string col : columnNames) {
            stream << col << ",";
        }
        stream << std::endl;

        Matrix3d dm = dataLoader->getOUDTransitionRates();

        for (long int init = 0; init < dataLoader->getNumOUDStates(); init++) {
            for (long int i = 0; i < dataLoader->getNumInterventions(); i++) {
                for (long int k = 0;
                     k < dataLoader->getDemographicCombos().size(); k++) {
                    stream << dataLoader->getInterventions()[i] << ",";
                    std::string temp = dataLoader->getDemographicCombos()[k];
                    temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"),
                                              "$1");
                    std::replace(temp.begin(), temp.end(), ' ', ',');
                    stream << temp << ",";
                    stream << dataLoader->getOUDStates()[init] << ",";
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
                        stream << std::to_string(value) << ",";
                    }
                    stream << std::endl;
                }
            }
        }

        return writeFile("oud_trans.csv", stream);
    }

    std::string InputWriter::writeInterventionInitRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::stringstream stream;
        std::vector<std::string> columnNames{"initial_oud_state",
                                             "to_intervention"};

        for (std::string oud : dataLoader->getOUDStates()) {
            columnNames.push_back(oud);
        }

        for (std::string col : columnNames) {
            stream << col << ",";
        }
        stream << std::endl;

        Matrix3d dm = dataLoader->getInterventionInitRates();

        for (int init = 0; init < dataLoader->getNumOUDStates(); init++) {
            for (int inter = 0; inter < dataLoader->getNumInterventions();
                 inter++) {
                stream << dataLoader->getOUDStates()[init] << ",";
                stream << dataLoader->getInterventions()[inter] << ",";
                for (int res = 0; res < dataLoader->getNumOUDStates(); res++) {
                    std::array<long int, 3> index = {0, 0, 0};
                    index[Matrixify::INTERVENTION] = inter;
                    index[Matrixify::OUD] =
                        (init * dataLoader->getNumOUDStates()) + res;
                    index[Matrixify::DEMOGRAPHIC_COMBO] = 0;
                    ASSERTM(dm.NumDimensions == 3,
                            "3 Dimensions Found in Matrix3d");
                    double value = dm(index[0], index[1], index[2]);
                    stream << std::to_string(value) << ",";
                }
                stream << std::endl;
            }
        }

        return writeFile("block_init_effect.csv", stream);
    }

    std::string InputWriter::writeInterventionTransitionRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {

        std::stringstream stream;

        std::vector<std::string> columnNames{"agegrp", "race", "sex", "oud",
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
            stream << col << ",";
        }
        stream << std::endl;

        // add zero index and pop off final change time to get matrix
        // indices
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
                    stream << temp << ",";
                    stream << dataLoader->getOUDStates()[oud] << ",";
                    stream << dataLoader->getInterventions()[init] << ",";
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
                            stream << std::to_string(value) << ",";
                        }
                    }
                    stream << std::endl;
                }
            }
        }
        return writeFile("block_trans.csv", stream);
    }

    std::string InputWriter::writeOverdoseRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::stringstream stream;

        std::vector<std::string> columnNames{"intervention", "agegrp", "race",
                                             "sex", "oud"};

        std::vector<int> changeTimes = dataLoader->getOverdoseChangeTimes();
        int ct1 = 1;
        for (int ct2 : changeTimes) {
            columnNames.push_back("overdose_" + std::to_string(ct1) + "_" +
                                  std::to_string(ct2));
            ct1 = ct2;
        }
        for (std::string col : columnNames) {
            stream << col << ",";
        }
        stream << std::endl;

        // add zero index and pop off final change time to get matrix
        // indices
        changeTimes.insert(changeTimes.begin(), 0);
        changeTimes.pop_back();

        Matrix4d dm = dataLoader->getOverdoseRates();

        for (int inter = 0; inter < dataLoader->getNumInterventions();
             inter++) {
            for (long int dem = 0; dem < dataLoader->getNumDemographicCombos();
                 dem++) {
                for (int oud = 0; oud < dataLoader->getNumOUDStates(); oud++) {
                    {
                        stream << dataLoader->getInterventions()[inter] << ",";
                        std::string temp =
                            dataLoader->getDemographicCombos()[dem];
                        temp = std::regex_replace(
                            temp, std::regex("^ +| +$|( ) +"), "$1");
                        std::replace(temp.begin(), temp.end(), ' ', ',');
                        stream << temp << ",";
                        stream << dataLoader->getOUDStates()[oud] << ",";
                        for (int timestep : changeTimes) {
                            std::array<long int, 3> index = {0, 0, 0};
                            index[Matrixify::INTERVENTION] = inter;
                            index[Matrixify::OUD] = oud;
                            index[Matrixify::DEMOGRAPHIC_COMBO] = dem;
                            double value =
                                dm(timestep, index[0], index[1], index[2]);
                            stream << std::to_string(value) << ",";
                        }
                        stream << std::endl;
                    }
                }
            }
        }
        return writeFile("all_types_overdose.csv", stream);
    }

    std::string InputWriter::writeFatalOverdoseRates(
        const std::shared_ptr<IDataLoader> dataLoader) const {
        std::stringstream stream;

        std::vector<std::string> columnNames{"agegrp", "race", "sex"};

        std::vector<int> changeTimes = dataLoader->getOverdoseChangeTimes();
        int ct1 = 1;
        for (int ct2 : changeTimes) {
            columnNames.push_back("percent_overdoses_fatal_" +
                                  std::to_string(ct1) + "_" +
                                  std::to_string(ct2));
            ct1 = ct2;
        }

        for (std::string col : columnNames) {
            stream << col << ",";
        }
        stream << std::endl;

        // add zero index and pop off final change time to get matrix
        // indices
        changeTimes.insert(changeTimes.begin(), 0);
        changeTimes.pop_back();

        Matrix4d dm = dataLoader->getFatalOverdoseRates();
        for (long int dem = 0; dem < dataLoader->getNumDemographicCombos();
             dem++) {
            std::string temp = dataLoader->getDemographicCombos()[dem];
            temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"), "$1");
            std::replace(temp.begin(), temp.end(), ' ', ',');
            stream << temp << ",";
            for (int timestep : changeTimes) {
                std::array<long int, 3> index = {0, 0, 0};
                index[Matrixify::DEMOGRAPHIC_COMBO] = dem;
                double value = dm(timestep, index[0], index[1], index[2]);
                stream << std::to_string(value) << ",";
            }
            stream << std::endl;
        }

        return writeFile("fatal_overdose.csv", stream);
    }

    std::string Writer::writeFile(const std::string filename,
                                  std::stringstream &stream) const {
        if (this->writeType == Matrixify::WriteType::FILE) {
            std::filesystem::path iipFile(filename);
            std::filesystem::path dir(this->getDirname());
            std::filesystem::path fullPath = dir / iipFile;
            try {
                std::ofstream file(fullPath.string());
                file << stream.rdbuf();
                file.close();
                return "success";
            } catch (...) {
                return "failure";
            }
        } else {
            return stream.str();
        }
    }

    /// @brief Helper function to write data to the specified stream
    /// @param stream Stream type to write data to
    /// @param historyToWrite Specific portion of history to save
    void OutputWriter::write4d(std::stringstream &stream,
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
                        stream << std::to_string(value);
                        if (this->pivot) {
                            stream << std::endl;
                        } else {
                            stream << ",";
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
        std::string ret = "Interventions,OUD States,";
        for (std::string demographic : this->demographics) {
            ret += demographic + ",";
        }
        if (this->pivot) {
            ret += "time,value";
        } else {
            for (int timestep : this->getTimesteps()) {
                ret += "t+" + std::to_string(timestep) + ",";
            }
        }

        return ret;
    }
} // namespace Matrixify
