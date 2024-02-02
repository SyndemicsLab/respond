//===-- DataLoader.cpp - DataLoader class definition -----------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the DataLoader class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "DataLoader.hpp"
#include "Matrix3dFactory.hpp"
#include "Matrix3dPrinter.hpp"

#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace Matrixify {

    /*********************************************************************
     *
     * Constructors
     *
     *********************************************************************/
    DataLoader::DataLoader() : Loader() {
        this->duration = 0;
        this->agingInterval = 0;
        this->ageGroupShift = 0;
        this->costSwitch = false;

        this->interventions = {};
        this->oudStates = {};
        this->demographicCounts = {};
        this->simulationParameters = {};

        this->initialSample;
        this->enteringSamples;
        this->oudTransitionRates;
        this->interventionTransitionRates;
        this->overdoseRates;
        this->fatalOverdoseRates;
        this->mortalityRates;

        Loader::inputTables = {};
    }

    DataLoader::DataLoader(std::string const &inputDir,
                           std::shared_ptr<spdlog::logger> logger)
        : Loader(inputDir) {
        this->inputTables = this->readInputDir(inputDir);
    }

    DataLoader::DataLoader(Data::IConfigurationPtr &config,
                           std::string const &inputDir,
                           std::shared_ptr<spdlog::logger> logger)
        : Loader(inputDir, logger) {
        loadConfigurationPointer(config);
        this->inputTables = this->readInputDir(inputDir);
    }

    /*********************************************************************
     *
     * Public Methods
     *
     *********************************************************************/
    /// @brief
    /// @param csvName
    Matrix3d DataLoader::loadInitialSample(std::string const &csvName) {
        // INITIAL GROUP

        Data::IDataTablePtr initialCohort = loadTable(csvName);

        int nonPostInterventions = ((getNumInterventions() - 1) / 2 + 1);

        std::vector<std::string> counts = initialCohort->getColumn("counts");

        if (counts.empty()) {
            logger->error("Counts Column Not Found in init_cohorts.csv");
            throw std::out_of_range("counts not in init_cohorts.csv");
        }

        if (counts.size() > (nonPostInterventions * getNumDemographicCombos() *
                             getNumOUDStates())) {
            logger->error("Incorrect Number of Counts Found");
            logger->error("Number of Counts Found: {}", counts.size());
            logger->error("Number of Non-Post Interventions: {}",
                          nonPostInterventions);
            logger->error("Number of Demographic Combos: {}",
                          getNumDemographicCombos());
            logger->error("Number of OUD States {}", getNumOUDStates());
            throw std::invalid_argument(
                "Number of Counts Found in init_cohorts.csv Greater Than "
                "Interventions * Demographics * OUDStates");
        }

        this->initialSample = Matrixify::Matrix3dFactory::Create(
                                  getNumOUDStates(), getNumInterventions(),
                                  getNumDemographicCombos())
                                  .constant(0);

        int row = 0;
        for (int intervention = 0; intervention < nonPostInterventions;
             ++intervention) {
            for (int dem = 0; dem < getNumDemographicCombos(); ++dem) {
                for (int oud_state = 0; oud_state < getNumOUDStates();
                     ++oud_state) {
                    // have to use the row counter here because
                    // nonPostInterventions != numInterventions
                    this->initialSample(intervention, oud_state, dem) =
                        (row < counts.size()) ? std::stod(counts[row]) : 0.0;
                    ++row;
                }
            }
        }
        return this->initialSample;
    }

    /// @brief
    /// @param csvName
    Matrix4d DataLoader::loadEnteringSamples(
        std::string const &csvName,
        std::string const &enteringSampleIntervention,
        std::string const &enteringSampleOUD) {

        // ENTERING SAMPLES
        Data::IDataTablePtr enteringSamplesTable = loadTable(csvName);

        auto itr = find(this->interventions.begin(), this->interventions.end(),
                        enteringSampleIntervention);
        int esiIdx = (itr != this->interventions.end())
                         ? itr - this->interventions.begin()
                         : 0;

        itr = find(this->oudStates.begin(), this->oudStates.end(),
                   enteringSampleOUD);
        int esoIdx =
            (itr != this->oudStates.end()) ? itr - this->oudStates.begin() : 0;

        std::string columnPrefix = "number_of_new_comers_cycle";

        int startTime = 0;
        for (int changepoint : this->enteringSampleChangeTimes) {
            std::string column = columnPrefix + std::to_string(changepoint);
            std::vector<std::string> col =
                enteringSamplesTable->getColumn(column);
            if (col.empty()) {
                logger->error("{} Column Not Found in entering_cohort.csv",
                              column);
                throw std::out_of_range(column + " not in entering_cohort.csv");
            }

            Matrix3d enteringSample = Matrixify::Matrix3dFactory::Create(
                getNumOUDStates(), getNumInterventions(),
                getNumDemographicCombos());

            for (int dem = 0; dem < getNumDemographicCombos(); ++dem) {
                enteringSample(esiIdx, esoIdx, dem) =
                    (dem < col.size())
                        ? std::stod(col[dem])
                        : enteringSample(esiIdx, esoIdx, dem) = 0.0;
            }
            while (startTime <= changepoint) {
                this->enteringSamples.insert(enteringSample, startTime);
                startTime++;
            }
            startTime = changepoint;
        }
        return this->enteringSamples;
    }

    /// @brief
    /// @param csvName
    Matrix3d DataLoader::loadOUDTransitionRates(std::string const &csvName) {

        // OUD TRANSITIONS
        Data::IDataTablePtr oudTransitionTable = loadTable(csvName);
        // end dimensions of oudTransitionRates are getNumInterventions() x
        // getNumOUDStates()^2 x demographics start with a vector of
        // StateTensor-sized Matrix3d objects and stack at the end
        std::vector<Matrix3d> tempOUDTransitions;
        for (int i = 0; i < getNumOUDStates(); ++i) {
            tempOUDTransitions.push_back(Matrixify::Matrix3dFactory::Create(
                getNumOUDStates(), getNumInterventions(),
                getNumDemographicCombos()));
        }
        // Matrix3d objects in the vector are matched with the order that oud
        // states are specified in the Config file. the order represents the
        // "initial oud state"
        for (int initial_state = 0; initial_state < getNumOUDStates();
             ++initial_state) {
            int row = 0;
            for (int intervention = 0; intervention < getNumInterventions();
                 ++intervention) {
                for (int dem = 0; dem < getNumDemographicCombos(); ++dem) {
                    for (int result_state = 0; result_state < getNumOUDStates();
                         ++result_state) {

                        std::string column =
                            "to_" + this->oudStates[result_state];

                        int idx = getNumOUDStates() * row + initial_state;
                        std::vector<std::string> col =
                            oudTransitionTable->getColumn(column);
                        if (col.empty()) {
                            logger->error(
                                "{} Column Not Found in oud_trans.csv", column);
                            throw std::out_of_range(column +
                                                    " not in oud_trans.csv");
                        }

                        if (idx < col.size()) {
                            double val = std::stod(col[idx]);

                            tempOUDTransitions[initial_state](
                                intervention, result_state, dem) = val;
                        } else {
                            tempOUDTransitions[initial_state](
                                intervention, result_state, dem) = 0.0;
                        }
                    }
                    ++row;
                }
            }
        }

        // stack the Matrix3d objects along the OUD axis
        this->oudTransitionRates = tempOUDTransitions[0];
        for (int i = 1; i < tempOUDTransitions.size(); ++i) {
            Matrix3d temp =
                this->oudTransitionRates
                    .concatenate(tempOUDTransitions[i], Matrixify::OUD)
                    .eval()
                    .reshape(Matrix3d::Dimensions(getNumInterventions(),
                                                  (i + 1) * getNumOUDStates(),
                                                  getNumDemographicCombos()));
            this->oudTransitionRates = std::move(temp);
        }
        return this->oudTransitionRates;
    }

    /// @brief
    /// @param csvName
    Matrix3d DataLoader::loadInterventionInitRates(std::string const &csvName) {

        // OUD TRANSITIONS
        Data::IDataTablePtr interventionInitTable = loadTable(csvName);
        // end dimensions of oudTransitionRates are getNumInterventions() x
        // getNumOUDStates()^2 x demographics start with a vector of
        // StateTensor-sized Matrix3d objects and stack at the end
        std::vector<Matrix3d> tempinterventionInit;
        int activeNonActiveOffset = getNumOUDStates() / 2;
        for (int i = 0; i < getNumOUDStates(); ++i) {
            tempinterventionInit.push_back(Matrixify::Matrix3dFactory::Create(
                getNumOUDStates(), getNumInterventions(),
                getNumDemographicCombos()));
        }
        // Matrix3d objects in the vector are matched with the order that oud
        // states are specified in the Config file. the order represents the
        // "initial oud state"
        for (int initial_state = 0; initial_state < getNumOUDStates();
             ++initial_state) {
            std::string currentOUDState = this->oudStates[initial_state];
            std::vector<std::string> column =
                interventionInitTable->getColumn("initial_oud_state");

            auto iterator =
                std::find(column.begin(), column.end(), currentOUDState);

            int idx =
                (iterator != column.end()) ? iterator - column.begin() : 0;

            bool nonactiveFlag =
                (currentOUDState.find("Nonactive") != std::string::npos)
                    ? true
                    : false;

            for (int intervention = 0; intervention < getNumInterventions();
                 ++intervention) {
                std::string currentIntervention =
                    this->interventions[intervention];
                std::string dash("-");
                std::string period(".");
                size_t pos = currentIntervention.find(dash);
                if (pos != std::string::npos) {
                    currentIntervention.replace(pos, dash.length(), period);
                }
                currentIntervention = "to_" + currentIntervention;

                std::vector<std::string> col =
                    interventionInitTable->getColumn(currentIntervention);
                if (col.empty()) {
                    logger->error("{} Column Not Found in block_trans.csv",
                                  currentIntervention);
                    throw std::out_of_range(currentIntervention +
                                            " not in block_trans.csv");
                }

                double tableVal = std::stod(col[idx]);
                double oppVal = 1 - tableVal;
                int oppValIdx = (nonactiveFlag) ? idx - activeNonActiveOffset
                                                : idx + activeNonActiveOffset;
                for (int dem = 0; dem < getNumDemographicCombos(); ++dem) {
                    tempinterventionInit[initial_state](intervention, idx,
                                                        dem) = tableVal;
                    tempinterventionInit[initial_state](intervention, oppValIdx,
                                                        dem) = oppVal;
                }
            }
        }

        // stack the Matrix3d objects along the OUD axis
        this->interventionInitRates = tempinterventionInit[0];
        for (int i = 1; i < tempinterventionInit.size(); ++i) {
            Matrix3d temp =
                this->interventionInitRates
                    .concatenate(tempinterventionInit[i], Matrixify::OUD)
                    .eval()
                    .reshape(Matrix3d::Dimensions(getNumInterventions(),
                                                  (i + 1) * getNumOUDStates(),
                                                  getNumDemographicCombos()));
            this->interventionInitRates = std::move(temp);
        }

        return this->interventionInitRates;
    }

    /// @brief
    /// @param csvName
    Matrix4d
    DataLoader::loadInterventionTransitionRates(std::string const &csvName) {

        // INTERVENTION TRANSITIONS
        Data::IDataTablePtr interventionTransitionTable = loadTable(csvName);

        try {
            this->interventionTransitionRates =
                this->buildTransitionRatesOverTime(
                    this->interventionChangeTimes, interventionTransitionTable);
        } catch (const std::exception &e) {
            this->logger->error(e.what());
        }

        return this->interventionTransitionRates;
    }

    /// @brief
    /// @param csvName
    Matrix4d DataLoader::loadOverdoseRates(std::string const &csvName) {

        // loadTable(csvName);

        // OVERDOSE
        Data::IDataTablePtr overdoseTransitionTable = loadTable(csvName);
        int startTime = 0;
        for (auto timestep : this->overdoseChangeTimes) {
            std::string str_timestep = "cycle" + std::to_string(timestep);
            overdoseTransitionTable->dropColumn(str_timestep);
            std::vector<std::string> headers =
                overdoseTransitionTable->getHeaders();

            for (std::string header : headers) {
                if (header.find("overdose_cycle") != std::string::npos) {
                    Matrix3d temp = this->buildOverdoseTransitions(
                        overdoseTransitionTable, header);
                    while (startTime <= timestep) {
                        this->overdoseRates.insert(temp, startTime);
                        startTime++;
                    }
                }
            }
        }
        return this->overdoseRates;
    }

    /// @brief
    /// @param csvName
    /// @return
    Matrix4d DataLoader::loadFatalOverdoseRates(std::string const &csvName) {

        std::vector<Matrix3d> tempFatalOverdoseTransitions;

        Data::IDataTablePtr fatalOverdoseTable = loadTable(csvName);
        int startTime = 0;
        for (int timestep : this->overdoseChangeTimes) {
            Matrix3d overdoseTransition =
                Matrixify::Matrix3dFactory::Create(getNumOUDStates(),
                                                   getNumInterventions(),
                                                   getNumDemographicCombos())
                    .constant(0);

            std::string fodColumn = "fatal_to_all_types_overdose_ratio_cycle" +
                                    std::to_string(timestep);

            std::vector<std::string> col =
                fatalOverdoseTable->getColumn(fodColumn);

            if (col.empty()) {
                this->logger->error("{} column not found in fatal_overdose.csv",
                                    fodColumn);
                throw std::out_of_range(fodColumn +
                                        " not in fatal_overdose.csv");
            }

            Matrix3d temp = Matrixify::Matrix3dFactory::Create(
                                getNumOUDStates(), getNumInterventions(),
                                getNumDemographicCombos())
                                .setZero();

            if (!fatalOverdoseTable->getColumn("block").empty()) {
                for (int intervention = 0;
                     intervention < this->getNumInterventions();
                     ++intervention) {
                    double t = (col[intervention].size() > 0)
                                   ? std::stod(col[intervention])
                                   : 0.0;

                    Eigen::array<Eigen::Index, 3> offsets = {intervention, 0,
                                                             0};
                    Eigen::array<Eigen::Index, 3> extents = {
                        1, getNumOUDStates(), getNumDemographicCombos()};
                    temp.slice(offsets, extents) =
                        temp.slice(offsets, extents).setConstant(t);
                }
            } else {
                double t = (col.size() > 0) ? std::stod(col[0]) : 0.0;

                temp = temp.constant(t);
            }

            while (startTime <= timestep) {
                this->fatalOverdoseRates.insert(temp, startTime);
                startTime++;
            }
        }
        return this->fatalOverdoseRates;
    }

    /// @brief
    /// @param csvName
    Matrix3d DataLoader::loadMortalityRates(std::string const &smrCSVName,
                                            std::string const &bgmCSVName) {

        // MORTALITY TRANSITIONS
        // mortality here refers to death from reasons other than oud and is
        // calculated by combining the SMR and background mortality calculation
        // to combine these into the mortality is 1-exp(log(1-bg_mort)*SMR)
        Data::IDataTablePtr temp = loadTable(smrCSVName);
        std::vector<std::string> smrColumn = temp->getColumn("SMR");
        if (smrColumn.empty()) {
            this->logger->error("SMR column not found in SMR.csv");
            throw std::out_of_range("SMR not in SMR.csv");
        }

        // only stratified by the demographics, needs to be expanded for oud and
        // intervention

        temp = loadTable(bgmCSVName);
        std::vector<std::string> backgroundMortalityColumn =
            temp->getColumn("death_prob");

        if (backgroundMortalityColumn.empty()) {
            this->logger->error(
                "death_prob column not found in background_mortality.csv");
            throw std::out_of_range(
                "death_prob not in background_mortality.csv");
        }

        Matrix3d mortalityTransition = Matrixify::Matrix3dFactory::Create(
            getNumOUDStates(), getNumInterventions(),
            getNumDemographicCombos());
        // mortality is one element per stratum, no time variability
        int smrIndex = 0;
        for (int intervention = 0; intervention < getNumInterventions();
             ++intervention) {
            for (int dem = 0; dem < getNumDemographicCombos(); dem++) {
                for (int oud = 0; oud < getNumOUDStates(); ++oud) {
                    if (backgroundMortalityColumn.size() > dem &&
                        smrColumn.size() > smrIndex) {
                        mortalityTransition(intervention, oud, dem) =
                            1 -
                            exp(log(1 -
                                    std::stod(backgroundMortalityColumn[dem])) *
                                std::stod(smrColumn[smrIndex]));
                        smrIndex++;
                    } else {
                        mortalityTransition(intervention, oud, dem) = 0.0;
                    }
                }
            }
        }
        this->mortalityRates = mortalityTransition;
        return this->mortalityRates;
    }

    /// @brief
    /// @param indices
    /// @param table
    /// @return
    Matrixify::Matrix3d
    DataLoader::buildInterventionMatrix(Data::IDataTablePtr const &table,
                                        std::string interventionName,
                                        int timestep) {
        Matrixify::Matrix3d transMat =
            Matrixify::Matrix3dFactory::Create(getNumOUDStates(),
                                               getNumInterventions(),
                                               getNumDemographicCombos())
                .constant(0);

        std::unordered_map<std::string, std::string> selectConditions;

        for (std::string oudState : getOUDStates()) {
            for (std::string demographicCombo : getDemographicCombos()) {
                selectConditions.clear();
                selectConditions["oud"] = oudState;
                std::stringstream ss(demographicCombo);
                std::string word;
                std::vector<std::string> v;
                while (ss >> word) {
                    v.push_back(word);
                }
                selectConditions["agegrp"] = v[0];
                selectConditions["sex"] = v[1];
                selectConditions["initial_block"] = interventionName;
                Data::IDataTablePtr temp = table->selectWhere(selectConditions);
                if (temp->getShape().getNRows() == 0) {
                    continue;
                }

                for (std::string intervention : getInterventions()) {
                    std::string header = "";
                    if (intervention.find("Post") != std::string::npos) {
                        if (intervention.find(interventionName) !=
                            std::string::npos) {
                            header = "to_corresponding_post_trt_cycle" +
                                     std::to_string(timestep);
                            std::vector<std::string> value =
                                temp->getColumn(header);
                            transMat(
                                oudIndices[oudState],
                                interventionsIndices[intervention],
                                demographicComboIndices[demographicCombo]) =
                                stod(value[0]);

                        } else {
                            transMat(
                                oudIndices[oudState],
                                interventionsIndices[intervention],
                                demographicComboIndices[demographicCombo]) =
                                0.0;
                        }
                    } else {
                        header = "to_" + intervention + "_cycle" +
                                 std::to_string(timestep);
                        std::vector<std::string> value =
                            temp->getColumn(header);
                        transMat(oudIndices[oudState],
                                 interventionsIndices[intervention],
                                 demographicComboIndices[demographicCombo]) =
                            stod(value[0]);
                    }
                }
            }
        }
        return transMat;
    }

    /// @brief
    /// @param indicesVec
    /// @param table
    /// @param dimension
    /// @return
    Matrixify::Matrix3d
    DataLoader::createTransitionMatrix3d(Data::IDataTablePtr const &table,
                                         Matrixify::Dimension dimension,
                                         int timestep) {

        std::shared_ptr<Data::DataTable> dynaCast =
            std::dynamic_pointer_cast<Data::DataTable>(table);

        Data::DataTable temp(*dynaCast);
        Data::IDataTablePtr tempPtr =
            std::make_shared<Data::DataTable>(std::move(temp));

        if (dimension == Matrixify::INTERVENTION) {
            Matrix3d stackingMatrices =
                Matrixify::Matrix3dFactory::Create(getNumOUDStates(),
                                                   getNumInterventions() *
                                                       getNumInterventions(),
                                                   getNumDemographicCombos())
                    .constant(0);
            for (int i = 0; i < this->interventions.size(); i++) {
                // assign to index + offset of numInterventions
                Eigen::array<Eigen::Index, 3> offsets = {0, 0, 0};
                offsets[Matrixify::INTERVENTION] = i * getNumInterventions();
                offsets[Matrixify::OUD] = i * 0;
                offsets[Matrixify::DEMOGRAPHIC_COMBO] = 0;
                Eigen::array<Eigen::Index, 3> extents = {0, 0, 0};
                extents[Matrixify::INTERVENTION] = getNumInterventions();
                extents[Matrixify::OUD] = getNumOUDStates();
                extents[Matrixify::DEMOGRAPHIC_COMBO] =
                    getNumDemographicCombos();
                Matrixify::Matrix3d temp = this->buildInterventionMatrix(
                    table, this->interventions[i], timestep);
                stackingMatrices.slice(offsets, extents) = temp;
            }
            return stackingMatrices;

        } else if (dimension == Matrixify::OUD) {
            Matrix3d stackingMatrices =
                Matrixify::Matrix3dFactory::Create(
                    getNumOUDStates() * getNumOUDStates(),
                    getNumInterventions(), getNumDemographicCombos())
                    .constant(0);
            return stackingMatrices;
        }
        Matrix3d stackingMatrices =
            Matrixify::Matrix3dFactory::Create(getNumOUDStates(),
                                               getNumInterventions(),
                                               getNumDemographicCombos())
                .constant(0);
        return stackingMatrices;
    }

    /// @brief
    /// @param ict
    /// @param table
    /// @return
    Matrix4d
    DataLoader::buildTransitionRatesOverTime(std::vector<int> const &ict,
                                             Data::IDataTablePtr const &table) {
        Matrix4d m3dot;
        int startTime = 0;
        for (int timestep : ict) {
            Matrix3d transMat = this->createTransitionMatrix3d(
                table, Matrixify::INTERVENTION, timestep);
            while (startTime <= timestep) {
                m3dot.insert(transMat, startTime);
                startTime++;
            }
        }
        return m3dot;
    }

    /// @brief
    /// @param table
    /// @param key
    /// @return
    Matrix3d
    DataLoader::buildOverdoseTransitions(Data::IDataTablePtr const &table,
                                         std::string const &key) {
        Matrix3d overdoseTransitionsCycle =
            Matrixify::Matrix3dFactory::Create(getNumOUDStates(),
                                               getNumInterventions(),
                                               getNumDemographicCombos())
                .constant(0);

        int row = 0;
        for (int intervention = 0; intervention < getNumInterventions();
             ++intervention) {
            for (int dem = 0; dem < getNumDemographicCombos(); ++dem) {
                for (int oud_state = 0; oud_state < getNumOUDStates();
                     ++oud_state) {
                    if (this->oudStates[oud_state].find("Nonactive") !=
                            std::string::npos ||
                        row >= table->getColumn(key).size()) {
                        overdoseTransitionsCycle(intervention, oud_state, dem) =
                            0.0f;
                    } else {
                        overdoseTransitionsCycle(intervention, oud_state, dem) =
                            std::stod((table->getColumn(key))[row]);
                        ++row;
                    }
                }
            }
        }
        return overdoseTransitionsCycle;
    }
} // namespace Matrixify