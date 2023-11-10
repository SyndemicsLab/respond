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

namespace Data {

    /*********************************************************************
     *
     * Constructors
     *
     *********************************************************************/
    DataLoader::DataLoader() {
        this->duration = 0;
        this->agingInterval = 0;
        this->ageGroupShift = 0;
        this->numOUDStates = 0;
        this->numInterventions = 0;
        this->numDemographics = 0;
        this->numDemographicCombos = 0;
        this->costSwitch = false;

        this->interventions = {};
        this->oudStates = {};
        this->demographicCounts = {};
        this->simulationParameters = {};

        this->Config;

        this->initialSample;
        this->enteringSamples;
        this->oudTransitionRates;
        this->interventionTransitionRates;
        this->overdoseRates;
        this->fatalOverdoseRates;
        this->mortalityRates;

        Loader::inputTables = {};
    }

    DataLoader::DataLoader(std::string const &inputDir) : Loader(inputDir) {
        this->dirName = inputDir;
        // SETTING STRING VECTORS FOR DATA WRITER
        this->interventions = this->Config.getInterventions();
        this->oudStates = this->Config.getOUDStates();
        this->demographicCounts = this->Config.getDemographicCounts();

        // SETTING SIMULATION CONSTANTS
        this->numInterventions = this->interventions.size();
        this->numOUDStates = this->oudStates.size();

        this->duration = this->Config.getDuration();
        this->numDemographics = this->demographicCounts.size();
        this->numDemographicCombos = this->Config.getNumDemographicCombos();
        this->agingInterval = this->Config.getAgingInterval();
        // forcing the convention that age is the first demographic specified in
        // the simulation config; needed for aging the population
        this->ageGroupShift =
            this->numDemographicCombos / this->demographicCounts[0];

        std::vector<std::string> demographicCombos =
            this->Config.getDemographicCombos();

        // COST VARIABLES
        this->costSwitch = this->Config.getCostSwitch();
        this->populateCostParameters();

        // OUTPUT VARIABLES
        this->perInterventionPredictions =
            this->Config.getPerInterventionPredictions();
        this->generalOutputsSwitch = this->Config.getGeneralOutputsSwitch();
        this->generalStatsOutputTimesteps =
            this->Config.getGeneralStatsOutputTimesteps();
    }

    DataLoader::DataLoader(Configuration &config, std::string const &inputDir) {
        this->Config = config;
        this->inputTables = this->readInputDir(inputDir);

        // SETTING STRING VECTORS FOR DATA WRITER
        this->interventions = this->Config.getInterventions();
        this->oudStates = this->Config.getOUDStates();
        this->demographicCounts = this->Config.getDemographicCounts();

        // SETTING SIMULATION CONSTANTS
        this->numInterventions = this->interventions.size();
        this->numOUDStates = this->oudStates.size();

        this->duration = this->Config.getDuration();
        this->numDemographics = this->demographicCounts.size();
        this->numDemographicCombos = this->Config.getNumDemographicCombos();
        this->agingInterval = this->Config.getAgingInterval();

        // COST VARIABLES
        this->costSwitch = this->Config.getCostSwitch();
        this->populateCostParameters();

        // OUTPUT VARIABLES
        this->perInterventionPredictions =
            this->Config.getPerInterventionPredictions();
        this->generalOutputsSwitch = this->Config.getGeneralOutputsSwitch();
        this->generalStatsOutputTimesteps =
            this->Config.getGeneralStatsOutputTimesteps();
    }

    /*********************************************************************
     *
     * Public Methods
     *
     *********************************************************************/

    Configuration
    DataLoader::loadConfigurationFile(std::string const &configPath) {
        Loader::loadConfigurationFile(configPath);
        this->inputTables[configPath] = Loader::readCSV(configPath);
        this->interventions = this->Config.getInterventions();
        this->oudStates = this->Config.getOUDStates();
        this->demographicCounts = this->Config.getDemographicCounts();

        // SETTING SIMULATION CONSTANTS
        this->numInterventions = this->interventions.size();
        this->numOUDStates = this->oudStates.size();

        this->duration = this->Config.getDuration();
        this->numDemographics = this->demographicCounts.size();
        this->numDemographicCombos = this->Config.getNumDemographicCombos();
        this->agingInterval = this->Config.getAgingInterval();
        return this->Config;
    }

    /// @brief
    /// @param csvName
    Matrix3d DataLoader::loadInitialSample(std::string const &csvName) {
        // INITIAL GROUP

        InputTable initialCohort = loadTable(csvName);

        int nonPostInterventions = ((this->numInterventions - 1) / 2 + 1);

        auto itr = initialCohort.find("counts");

        ASSERTM(itr != initialCohort.end(),
                "\'counts\' Column Successfully Found");

        ASSERTM(itr->second.size() <=
                    (nonPostInterventions * this->numDemographicCombos *
                     this->numOUDStates),
                "Correct number of Counts Found");

        this->initialSample = Utilities::Matrix3dFactory::Create(
                                  this->numOUDStates, this->numInterventions,
                                  this->numDemographicCombos)
                                  .constant(0);

        int row = 0;
        for (int intervention = 0; intervention < nonPostInterventions;
             ++intervention) {
            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                for (int oud_state = 0; oud_state < this->numOUDStates;
                     ++oud_state) {
                    // have to use the row counter here because
                    // nonPostInterventions != numInterventions
                    this->initialSample(intervention, oud_state, dem) =
                        (row < itr->second.size()) ? std::stod(itr->second[row])
                                                   : 0.0;
                    ++row;
                }
            }
        }
        return this->initialSample;
    }

    /// @brief
    /// @param csvName
    Matrix3dOverTime DataLoader::loadEnteringSamples(
        std::string const &csvName,
        std::string const &enteringSampleIntervention,
        std::string const &enteringSampleOUD) {

        // ENTERING SAMPLES
        InputTable enteringSamplesTable = loadTable(csvName);

        std::vector<std::string> vec = this->Config.getInterventions();
        auto itr = find(vec.begin(), vec.end(), enteringSampleIntervention);
        int esiIdx = (itr != vec.end()) ? itr - vec.begin() : 0;

        vec = this->Config.getOUDStates();
        itr = find(vec.begin(), vec.end(), enteringSampleOUD);
        int esoIdx = (itr != vec.end()) ? itr - vec.begin() : 0;

        std::string columnPrefix = "number_of_new_comers_cycle";

        // generate each unique `Matrix3d enteringSamples`
        std::vector<int> changeTimes =
            this->Config.getEnteringSampleChangeTimes();
        int startTime = 0;
        for (int changepoint : changeTimes) {
            std::string column = columnPrefix + std::to_string(changepoint);
            ASSERTM(enteringSamplesTable.find(column) !=
                        enteringSamplesTable.end(),
                    (column + " Successfully Found"));

            Matrix3d enteringSample = Utilities::Matrix3dFactory::Create(
                this->numOUDStates, this->numInterventions,
                this->numDemographicCombos);

            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                enteringSample(esiIdx, esoIdx, dem) =
                    (dem < enteringSamplesTable[column].size())
                        ? std::stod(enteringSamplesTable[column][dem])
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
        InputTable oudTransitionTable = loadTable(csvName);
        // end dimensions of oudTransitionRates are this->numInterventions x
        // this->numOUDStates^2 x demographics start with a vector of
        // StateTensor-sized Matrix3d objects and stack at the end
        std::vector<Matrix3d> tempOUDTransitions;
        for (int i = 0; i < this->numOUDStates; ++i) {
            tempOUDTransitions.push_back(Utilities::Matrix3dFactory::Create(
                this->numOUDStates, this->numInterventions,
                this->numDemographicCombos));
        }
        // Matrix3d objects in the vector are matched with the order that oud
        // states are specified in the Config file. the order represents the
        // "initial oud state"
        for (int initial_state = 0; initial_state < this->numOUDStates;
             ++initial_state) {
            int row = 0;
            for (int intervention = 0; intervention < this->numInterventions;
                 ++intervention) {
                for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                    for (int result_state = 0;
                         result_state < this->numOUDStates; ++result_state) {
                        std::string column =
                            "to_" + this->oudStates[result_state];
                        int idx = this->numOUDStates * row + initial_state;
                        if (idx < oudTransitionTable[column].size()) {
                            tempOUDTransitions[initial_state](
                                intervention, result_state, dem) =
                                std::stod(oudTransitionTable[column][idx]);
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
                    .concatenate(tempOUDTransitions[i], Data::OUD)
                    .eval()
                    .reshape(Matrix3d::Dimensions(this->numInterventions,
                                                  (i + 1) * this->numOUDStates,
                                                  this->numDemographicCombos));
            this->oudTransitionRates = std::move(temp);
        }
        return this->oudTransitionRates;
    }

    /// @brief
    /// @param csvName
    Matrix3d DataLoader::loadInterventionInitRates(std::string const &csvName) {

        // OUD TRANSITIONS
        InputTable interventionInitTable = loadTable(csvName);
        // end dimensions of oudTransitionRates are this->numInterventions x
        // this->numOUDStates^2 x demographics start with a vector of
        // StateTensor-sized Matrix3d objects and stack at the end
        std::vector<Matrix3d> tempinterventionInit;
        int activeNonActiveOffset = this->numOUDStates / 2;
        for (int i = 0; i < this->numOUDStates; ++i) {
            tempinterventionInit.push_back(Utilities::Matrix3dFactory::Create(
                this->numOUDStates, this->numInterventions,
                this->numDemographicCombos));
        }
        // Matrix3d objects in the vector are matched with the order that oud
        // states are specified in the Config file. the order represents the
        // "initial oud state"
        for (int initial_state = 0; initial_state < this->numOUDStates;
             ++initial_state) {
            std::string currentOUDState = this->oudStates[initial_state];
            auto beginIterator =
                interventionInitTable["initial_oud_state"].begin();
            auto endIterator = interventionInitTable["initial_oud_state"].end();
            auto iterator =
                std::find(beginIterator, endIterator, currentOUDState);

            int idx = (iterator != endIterator) ? iterator - beginIterator : 0;

            bool nonactiveFlag =
                (currentOUDState.find("Nonactive") != std::string::npos)
                    ? true
                    : false;

            for (int intervention = 0; intervention < this->numInterventions;
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

                double tableVal =
                    std::stod(interventionInitTable[currentIntervention][idx]);
                double oppVal = 1 - tableVal;
                int oppValIdx = (nonactiveFlag) ? idx - activeNonActiveOffset
                                                : idx + activeNonActiveOffset;
                for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
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
                    .concatenate(tempinterventionInit[i], Data::OUD)
                    .eval()
                    .reshape(Matrix3d::Dimensions(this->numInterventions,
                                                  (i + 1) * this->numOUDStates,
                                                  this->numDemographicCombos));
            this->interventionInitRates = std::move(temp);
        }

        return this->interventionInitRates;
    }

    /// @brief
    /// @param csvName
    Matrix3dOverTime
    DataLoader::loadInterventionTransitionRates(std::string const &csvName) {

        // INTERVENTION TRANSITIONS
        InputTable interventionTransitionTable = loadTable(csvName);
        std::vector<int> ict = this->Config.getInterventionChangeTimes();

        std::vector<std::vector<int>> indicesVec =
            this->getIndicesByIntervention(
                interventionTransitionTable["initial_block"]);
        this->interventionTransitionRates = this->buildTransitionRatesOverTime(
            ict, interventionTransitionTable, indicesVec);
        return this->interventionTransitionRates;
    }

    /// @brief
    /// @param csvName
    Matrix3dOverTime DataLoader::loadOverdoseRates(std::string const &csvName) {

        loadTable(csvName);

        // OVERDOSE
        InputTable overdoseTransitionTable = loadTable(csvName);
        std::vector<int> oct = this->Config.getOverdoseChangeTimes();
        int startTime = 0;
        for (auto timestep : oct) {
            std::string str_timestep = "cycle" + std::to_string(timestep);
            InputTable currentTimeTable =
                this->removeColumns(str_timestep, overdoseTransitionTable);
            for (auto kv : currentTimeTable) {
                std::string str = kv.first;
                if (str.find("overdose_cycle") != std::string::npos) {
                    Matrix3d temp =
                        this->buildOverdoseTransitions(currentTimeTable, str);
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
    Matrix3dOverTime
    DataLoader::loadFatalOverdoseRates(std::string const &csvName) {

        std::vector<Matrix3d> tempFatalOverdoseTransitions;

        InputTable fatalOverdoseTable = loadTable(csvName);
        std::vector<int> oct = this->Config.getOverdoseChangeTimes();
        int startTime = 0;
        for (int timestep : oct) {
            Matrix3d overdoseTransition =
                Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                                   this->numInterventions,
                                                   this->numDemographicCombos)
                    .constant(0);

            std::string fodColumn = "fatal_to_all_types_overdose_ratio_cycle" +
                                    std::to_string(timestep);

            Matrix3d temp = Utilities::Matrix3dFactory::Create(
                                this->numOUDStates, this->numInterventions,
                                this->numDemographicCombos)
                                .setZero();

            if (fatalOverdoseTable.find("block") != fatalOverdoseTable.end()) {
                for (int intervention = 0;
                     intervention < this->getNumInterventions();
                     ++intervention) {
                    double t =
                        (fatalOverdoseTable[fodColumn][intervention].size() > 0)
                            ? std::stod(
                                  fatalOverdoseTable[fodColumn][intervention])
                            : 0.0;

                    Eigen::array<Eigen::Index, 3> offsets = {intervention, 0,
                                                             0};
                    Eigen::array<Eigen::Index, 3> extents = {
                        1, this->numOUDStates, this->numDemographicCombos};
                    temp.slice(offsets, extents) =
                        temp.slice(offsets, extents).setConstant(t);
                }
            } else {
                double t = (fatalOverdoseTable[fodColumn].size() > 0)
                               ? std::stod(fatalOverdoseTable[fodColumn][0])
                               : 0.0;

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
        std::vector<std::string> smrColumn = loadTable(smrCSVName)["SMR"];
        // only stratified by the demographics, needs to be expanded for oud and
        // intervention
        std::vector<std::string> backgroundMortalityColumn =
            loadTable(bgmCSVName)["death_prob"];

        Matrix3d mortalityTransition = Utilities::Matrix3dFactory::Create(
            this->numOUDStates, this->numInterventions,
            this->numDemographicCombos);
        // mortality is one element per stratum, no time variability
        int smrIndex = 0;
        for (int intervention = 0; intervention < this->numInterventions;
             ++intervention) {
            for (int dem = 0; dem < this->numDemographicCombos; dem++) {
                for (int oud = 0; oud < this->numOUDStates; ++oud) {
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

    /*********************************************************************
     *
     * Private Methods
     *
     *********************************************************************/

    /// @brief
    /// @param colString
    /// @param ogTable
    /// @return
    InputTable DataLoader::removeColumns(std::string const &colString,
                                         InputTable const &ogTable) {
        InputTable res = ogTable;
        std::vector<std::string> keys;
        for (auto i : res) {
            if ((i.first.find(colString) == std::string::npos) &&
                (i.first.find("cycle") != std::string::npos)) {
                std::string temp = i.first;
                keys.push_back(temp);
            }
        }

        for (auto i : keys) {
            res.erase(i);
        }

        return res;
    }

    /// @brief
    /// @param indices
    /// @param table
    /// @return
    Data::Matrix3d
    DataLoader::buildInterventionMatrix(std::vector<int> const &indices,
                                        InputTable const &table) {
        Data::Matrix3d transMat =
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions,
                                               this->numDemographicCombos)
                .constant(0);

        if (table.find("initial_block") == table.end()) {
            return transMat;
        }

        std::vector<std::string> t1 = table.at("initial_block");
        if (indices.size() == 0 || indices[0] >= t1.size()) {
            return transMat;
        }

        std::string currentIntervention = t1[indices[0]];
        std::vector<std::string> keys;

        // reconstructing order of interventions based on column keys
        bool postFlag = false;
        int idx = 0;
        int offset = 0;
        std::vector<std::string> interventions =
            this->Config.getInterventions();
        for (std::string inter : interventions) {
            InputTable tempTable = table;
            for (auto kv : tempTable) {
                if (kv.first.find(inter) != std::string::npos) {
                    std::string key = kv.first;
                    keys.push_back(key);
                } else if ((inter.find("Post") != std::string::npos) &&
                           (kv.first.find("post") != std::string::npos) &&
                           !postFlag) {
                    std::string key = kv.first;
                    keys.push_back(key);
                    postFlag = true; // only assumes one
                                     // "to_corresponding_post_trt column"
                }
            }
            if (currentIntervention.compare(inter) == 0) {
                offset = idx;
            }
            idx++;
        }

        for (int i = 0; i < keys.size(); i++) {
            int interventionOffset = i;

            if (currentIntervention.compare("No_Treatment") == 0) {
                interventionOffset = i;
            } else if (keys[i].find("post") != std::string::npos) {
                interventionOffset = i + offset - 1;
                if (currentIntervention.find("Post") != std::string::npos) {
                    interventionOffset -= ((this->numInterventions - 1) / 2);
                }
            }

            std::string key = keys[i];
            int oudIdx = 0;
            int demIdx = 0;
            for (int idx : indices) {
                std::vector<std::string> t2 = table.at(key);
                std::string val = t2[idx];
                double v = std::stod(val);
                transMat(interventionOffset, oudIdx, demIdx) = v;
                oudIdx++;
                if (oudIdx % this->numOUDStates == 0) {
                    oudIdx = 0;
                    demIdx++;
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
    Data::Matrix3d DataLoader::createTransitionMatrix3d(
        std::vector<std::vector<int>> const &indicesVec,
        InputTable const &table, Data::Dimension dimension) {
        if (dimension == Data::INTERVENTION) {
            Matrix3d stackingMatrices =
                Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                                   this->numInterventions *
                                                       this->numInterventions,
                                                   this->numDemographicCombos)
                    .constant(0);
            for (int i = 0; i < indicesVec.size(); i++) {
                // assign to index + offset of numInterventions
                Eigen::array<Eigen::Index, 3> offsets = {
                    i * this->numInterventions, 0, 0};
                Eigen::array<Eigen::Index, 3> extents = {
                    this->numInterventions, this->numOUDStates,
                    this->numDemographicCombos};
                Data::Matrix3d temp =
                    this->buildInterventionMatrix(indicesVec[i], table);
                stackingMatrices.slice(offsets, extents) = temp;
            }
            return stackingMatrices;

        } else if (dimension == Data::OUD) {
            Matrix3d stackingMatrices =
                Utilities::Matrix3dFactory::Create(
                    this->numOUDStates * this->numOUDStates,
                    this->numInterventions, this->numDemographicCombos)
                    .constant(0);
            return stackingMatrices;
        }
        Matrix3d stackingMatrices =
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions,
                                               this->numDemographicCombos)
                .constant(0);
        return stackingMatrices;
    }

    /// @brief
    /// @param v
    /// @param target
    /// @return
    std::vector<int> DataLoader::findIndices(std::vector<std::string> const &v,
                                             std::string const &target) {
        std::vector<int> indices;
        auto it = v.begin();
        while ((it = std::find_if(it, v.end(), [&](std::string const &e) {
                    return e.compare(target) == 0;
                })) != v.end()) {
            indices.push_back(std::distance(v.begin(), it));
            it++;
        }
        return indices;
    }

    /// @brief
    /// @param col
    /// @return
    std::vector<std::vector<int>>
    DataLoader::getIndicesByIntervention(std::vector<std::string> const &col) {
        std::vector<std::vector<int>> indicesVec;
        for (std::string in : this->Config.getInterventions()) {
            indicesVec.push_back(this->findIndices(col, in));
        }
        return indicesVec;
    }

    /// @brief
    /// @param ict
    /// @param table
    /// @param indicesVec
    /// @return
    Matrix3dOverTime DataLoader::buildTransitionRatesOverTime(
        std::vector<int> const &ict, InputTable const &table,
        std::vector<std::vector<int>> const &indicesVec) {
        Matrix3dOverTime m3dot;

        int startTime = 0;
        for (int timestep : ict) {
            // get rid of the pointless columns for this iteration
            std::string str_timestep = "cycle" + std::to_string(timestep);
            InputTable currentTimeTable =
                this->removeColumns(str_timestep, table);

            Matrix3d transMat = this->createTransitionMatrix3d(
                indicesVec, currentTimeTable, Data::INTERVENTION);
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
    Matrix3d DataLoader::buildOverdoseTransitions(InputTable const &table,
                                                  std::string const &key) {
        std::vector<std::string> oudStates = this->Config.getOUDStates();

        Matrix3d overdoseTransitionsCycle =
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions,
                                               this->numDemographicCombos)
                .constant(0);

        int row = 0;
        for (int intervention = 0; intervention < this->numInterventions;
             ++intervention) {
            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                for (int oud_state = 0; oud_state < this->numOUDStates;
                     ++oud_state) {
                    if (oudStates[oud_state].find("Nonactive") !=
                            std::string::npos ||
                        row >= table.at(key).size()) {
                        overdoseTransitionsCycle(intervention, oud_state, dem) =
                            0.0f;
                    } else {
                        overdoseTransitionsCycle(intervention, oud_state, dem) =
                            std::stod((table.at(key))[row]);
                        ++row;
                    }
                }
            }
        }
        return overdoseTransitionsCycle;
    }

    void DataLoader::populateCostParameters() {
        if (this->costSwitch) {
            this->costPerspectives = this->Config.getCostPerspectives();
            this->discountRate = this->Config.getDiscountRate();
            this->reportingInterval =
                this->Config.getCostUtilityOutputTimesteps();
            this->costCategoryOutputs = this->Config.getCostCategoryOutputs();
        }
    }

    std::vector<std::string> DataLoader::getCostPerspectives() const {
        if (!this->costSwitch) {
            return {};
        }
        return this->costPerspectives;
    }

    double DataLoader::getDiscountRate() const {
        if (!this->costSwitch) {
            return 0.0;
        }
        return this->discountRate;
    }

    int DataLoader::getCostUtilityOutputTimesteps() const {
        if (!this->costSwitch) {
            return 0;
        }
        return this->reportingInterval;
    }

    bool DataLoader::getCostCategoryOutputs() const {
        if (!this->costSwitch) {
            return false;
        }
        return this->costCategoryOutputs;
    }
} // namespace Data