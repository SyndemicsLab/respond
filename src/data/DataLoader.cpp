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

using namespace Data;

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

DataLoader::DataLoader(std::string inputDir) : Loader(inputDir) {
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
    // forcing the convention that age is the first demographic specified in the
    // simulation config; needed for aging the population
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

DataLoader::DataLoader(Configuration config, std::string inputDir) {
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

Configuration DataLoader::loadConfigurationFile(std::string configPath) {
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
Matrix3d DataLoader::loadInitialSample(std::string csvName) {
    // INITIAL GROUP
    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        this->inputTables[csvName] = Loader::readCSV(csvName);
    }

    int nonPostInterventions = ((this->numInterventions - 1) / 2 + 1);

    InputTable initialCohort = this->inputTables[csvName];

    auto itr = initialCohort.find("counts");

    ASSERTM(itr != initialCohort.end(), "\'counts\' Column Successfully Found");

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
                    std::stod(itr->second[row]);
                ++row;
            }
        }
    }
    return this->initialSample;
}

/// @brief
/// @param csvName
Matrix3dOverTime
DataLoader::loadEnteringSamples(std::string csvName,
                                std::string enteringSampleIntervention,
                                std::string enteringSampleOUD) {

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        this->inputTables[csvName] = Loader::readCSV(csvName);
    }

    int esiIdx = 0;

    std::vector<std::string> vec = this->Config.getInterventions();
    auto itr = find(vec.begin(), vec.end(), enteringSampleIntervention);
    if (itr != vec.end()) {
        esiIdx = itr - vec.begin();
    }

    int esoIdx = 0;

    vec = this->Config.getOUDStates();
    itr = find(vec.begin(), vec.end(), enteringSampleOUD);
    if (itr != vec.end()) {
        esoIdx = itr - vec.begin();
    }

    // ENTERING SAMPLES
    InputTable enteringSamplesTable = inputTables[csvName];

    std::string columnPrefix = "number_of_new_comers_cycle";

    // generate each unique `Matrix3d enteringSamples`
    std::vector<int> changeTimes = this->Config.getEnteringSampleChangeTimes();
    for (int timestep : changeTimes) {
        std::string column = columnPrefix + std::to_string(timestep + 1);
        ASSERTM(enteringSamplesTable.find(column) != enteringSamplesTable.end(),
                (column + " Successfully Found"));

        Matrix3d enteringSample =
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions,
                                               this->numDemographicCombos)
                .constant(0);

        for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
            enteringSample(esiIdx, esoIdx, dem) =
                std::stod(enteringSamplesTable[column][dem]);
        }

        this->enteringSamples.insert(enteringSample, timestep);
    }
    return this->enteringSamples;
}

/// @brief
/// @param csvName
Matrix3d DataLoader::loadOUDTransitionRates(std::string csvName) {

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        this->inputTables[csvName] = Loader::readCSV(csvName);
    }

    // OUD TRANSITIONS
    InputTable oudTransitionTable = inputTables[csvName];
    // end dimensions of oudTransitionRates are this->numInterventions x
    // this->numOUDStates^2 x demographics start with a vector of
    // StateTensor-sized Matrix3d objects and stack at the end
    std::vector<Matrix3d> tempOUDTransitions;
    for (int i = 0; i < this->numOUDStates; ++i) {
        tempOUDTransitions.push_back(Utilities::Matrix3dFactory::Create(
            this->numOUDStates, this->numInterventions,
            this->numDemographicCombos));
    }
    // Matrix3d objects in the vector are matched with the order that oud states
    // are specified in the Config file. the order represents the "initial oud
    // state"
    for (int initial_state = 0; initial_state < this->numOUDStates;
         ++initial_state) {
        int row = 0;
        for (int intervention = 0; intervention < this->numInterventions;
             ++intervention) {
            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                for (int result_state = 0; result_state < this->numOUDStates;
                     ++result_state) {
                    std::string column = "to_" + this->oudStates[result_state];
                    tempOUDTransitions[initial_state](intervention,
                                                      result_state, dem) =
                        std::stod(oudTransitionTable[column]
                                                    [this->numOUDStates * row +
                                                     initial_state]);
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
Matrix3d DataLoader::loadInterventionInitRates(std::string csvName) {

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        this->inputTables[csvName] = Loader::readCSV(csvName);
    }

    // OUD TRANSITIONS
    InputTable interventionInitTable = inputTables[csvName];
    // end dimensions of oudTransitionRates are this->numInterventions x
    // this->numOUDStates^2 x demographics start with a vector of
    // StateTensor-sized Matrix3d objects and stack at the end
    std::vector<Matrix3d> tempinterventionInit;
    int activeNonActiveOffset = this->numOUDStates / 2;
    for (int i = 0; i < this->numOUDStates; ++i) {
        tempinterventionInit.push_back(
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions,
                                               this->numDemographicCombos)
                .constant(0));
    }
    // Matrix3d objects in the vector are matched with the order that oud states
    // are specified in the Config file. the order represents the "initial oud
    // state"
    for (int initial_state = 0; initial_state < this->numOUDStates;
         ++initial_state) {
        std::string currentOUDState = this->oudStates[initial_state];
        auto iterator = std::find(
            interventionInitTable["initial_oud_state"].begin(),
            interventionInitTable["initial_oud_state"].end(), currentOUDState);

        int idx = 0;
        if (iterator != interventionInitTable["initial_oud_state"].end()) {
            idx = iterator - interventionInitTable["initial_oud_state"].begin();
        }

        int nonactiveFlag = false;
        if (currentOUDState.find("Nonactive") != std::string::npos) {
            nonactiveFlag = true;
        } else {
            nonactiveFlag = false;
        }

        for (int intervention = 0; intervention < this->numInterventions;
             ++intervention) {
            std::string currentIntervention = this->interventions[intervention];
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
                tempinterventionInit[initial_state](intervention, idx, dem) =
                    tableVal;
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
DataLoader::loadInterventionTransitionRates(std::string csvName) {

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        this->inputTables[csvName] = Loader::readCSV(csvName);
    }

    // INTERVENTION TRANSITIONS
    InputTable interventionTransitionTable = inputTables[csvName];
    std::vector<int> ict = this->Config.getInterventionChangeTimes();

    std::vector<std::vector<int>> indicesVec = this->getIndicesByIntervention(
        interventionTransitionTable["initial_block"]);
    this->interventionTransitionRates = this->buildTransitionRatesOverTime(
        ict, interventionTransitionTable, indicesVec);
    return this->interventionTransitionRates;
}

/// @brief
/// @param csvName
Matrix3dOverTime DataLoader::loadOverdoseRates(std::string csvName) {

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        this->inputTables[csvName] = Loader::readCSV(csvName);
    }

    // OVERDOSE
    InputTable overdoseTransitionTable = this->inputTables[csvName];
    std::vector<int> oct = this->Config.getOverdoseChangeTimes();

    for (auto timestep : oct) {
        std::string str_timestep = "cycle" + std::to_string(timestep + 1);
        InputTable currentTimeTable =
            this->removeColumns(str_timestep, overdoseTransitionTable);
        for (auto kv : currentTimeTable) {
            std::string str = kv.first;
            if (str.find("overdose_cycle") != std::string::npos) {
                this->overdoseRates.insert(
                    this->buildOverdoseTransitions(currentTimeTable, str),
                    timestep);
            }
        }
    }
    return this->overdoseRates;
}

/// @brief
/// @param csvName
/// @return
Matrix3dOverTime DataLoader::loadFatalOverdoseRates(std::string csvName) {

    if (this->inputTables.find(csvName) == this->inputTables.end()) {
        this->inputTables[csvName] = Loader::readCSV(csvName);
    }

    InputTable fatalOverdoseTable = inputTables[csvName];
    std::vector<Matrix3d> tempFatalOverdoseTransitions;
    for (int timestep : this->Config.getOverdoseChangeTimes()) {
        Matrix3d overdoseTransition =
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions,
                                               this->numDemographicCombos)
                .constant(0);
        // fatal overdose is a constant across all strata
        std::string fodColumn = "fatal_to_all_types_overdose_ratio_cycle" +
                                std::to_string(timestep + 1);
        double t = std::stod(fatalOverdoseTable[fodColumn][0]);
        this->fatalOverdoseRates.insert(
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions,
                                               this->numDemographicCombos)
                .constant(t),
            timestep);
    }
    return this->fatalOverdoseRates;
}

/// @brief
/// @param csvName
Matrix3d DataLoader::loadMortalityRates(std::string smrCSVName,
                                        std::string bgmCSVName) {

    if (this->inputTables.find(smrCSVName) == this->inputTables.end()) {
        this->inputTables[smrCSVName] = Loader::readCSV(smrCSVName);
    }
    if (this->inputTables.find(bgmCSVName) == this->inputTables.end()) {
        this->inputTables[bgmCSVName] = Loader::readCSV(bgmCSVName);
    }

    // MORTALITY TRANSITIONS
    // mortality here refers to death from reasons other than oud and is
    // calculated by combining the SMR and background mortality calculation to
    // combine these into the mortality is 1-exp(log(1-bg_mort)*SMR)
    std::vector<std::string> smrColumn = inputTables[smrCSVName]["SMR"];
    // only stratified by the demographics, needs to be expanded for oud and
    // intervention
    std::vector<std::string> backgroundMortalityColumn =
        inputTables[bgmCSVName]["death_prob"];
    Matrix3d mortalityTransition = Utilities::Matrix3dFactory::Create(
        this->numOUDStates, this->numInterventions, this->numDemographicCombos);
    // mortality is one element per stratum, no time variability
    int smrIndex = 0;
    for (int intervention = 0; intervention < this->numInterventions;
         ++intervention) {
        for (int dem = 0; dem < this->numDemographicCombos; dem++) {
            for (int oud = 0; oud < this->numOUDStates; ++oud) {
                mortalityTransition(intervention, oud, dem) =
                    1 - exp(log(1 - std::stod(backgroundMortalityColumn[dem])) *
                            std::stod(smrColumn[smrIndex]));
                smrIndex++;
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
InputTable DataLoader::removeColumns(std::string colString,
                                     InputTable ogTable) {
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
Data::Matrix3d DataLoader::buildInterventionMatrix(std::vector<int> indices,
                                                   InputTable table) {
    Data::Matrix3d transMat = Utilities::Matrix3dFactory::Create(
                                  this->numOUDStates, this->numInterventions,
                                  this->numDemographicCombos)
                                  .constant(0);

    if (table.find("initial_block") == table.end()) {
        return transMat;
    }

    std::vector<std::string> t1 = table["initial_block"];
    std::string currentIntervention = t1[indices[0]];
    std::vector<std::string> keys;

    // reconstructing order of interventions based on column keys
    bool postFlag = false;
    int idx = 0;
    int offset = 0;
    std::vector<std::string> interventions = this->Config.getInterventions();
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
                postFlag =
                    true; // only assumes one "to_corresponding_post_trt column"
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
            std::vector<std::string> t2 = table[key];
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
Data::Matrix3d
DataLoader::createTransitionMatrix3d(std::vector<std::vector<int>> indicesVec,
                                     InputTable table,
                                     Data::Dimension dimension) {
    if (dimension == Data::INTERVENTION) {
        Matrix3d stackingMatrices =
            Utilities::Matrix3dFactory::Create(this->numOUDStates,
                                               this->numInterventions *
                                                   this->numInterventions,
                                               this->numDemographicCombos)
                .constant(0);
        for (int i = 0; i < indicesVec.size(); i++) {
            // assign to index + offset of numInterventions
            Eigen::array<Eigen::Index, 3> offsets = {i * this->numInterventions,
                                                     0, 0};
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
                this->numOUDStates * this->numOUDStates, this->numInterventions,
                this->numDemographicCombos)
                .constant(0);
        return stackingMatrices;
    }
    Matrix3d stackingMatrices = Utilities::Matrix3dFactory::Create(
                                    this->numOUDStates, this->numInterventions,
                                    this->numDemographicCombos)
                                    .constant(0);
    return stackingMatrices;
}

/// @brief
/// @param v
/// @param target
/// @return
std::vector<int> DataLoader::findIndices(std::vector<std::string> const &v,
                                         std::string target) {
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
DataLoader::getIndicesByIntervention(std::vector<std::string> col) {
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
    std::vector<int> ict, InputTable table,
    std::vector<std::vector<int>> indicesVec) {
    Matrix3dOverTime m3dot;

    for (int timestep : ict) {
        // get rid of the pointless columns for this iteration
        std::string str_timestep = "cycle" + std::to_string(timestep + 1);
        InputTable currentTimeTable = this->removeColumns(str_timestep, table);
        m3dot.insert(this->createTransitionMatrix3d(
                         indicesVec, currentTimeTable, Data::INTERVENTION),
                     timestep);
    }
    return m3dot;
}

/// @brief
/// @param table
/// @param key
/// @return
Matrix3d DataLoader::buildOverdoseTransitions(InputTable table,
                                              std::string key) {
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
                    std::string::npos) {
                    overdoseTransitionsCycle(intervention, oud_state, dem) =
                        0.0f;
                } else {
                    overdoseTransitionsCycle(intervention, oud_state, dem) =
                        std::stod(table[key][row]);
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
        this->reportingInterval = this->Config.getReportingInterval();
        this->costCategoryOutputs = this->Config.getCostCategoryOutputs();
    }
}

std::vector<std::string> DataLoader::getCostPerspectives() {
    if (!this->costSwitch) {
        return {};
    }
    return this->costPerspectives;
}

double DataLoader::getDiscountRate() {
    if (!this->costSwitch) {
        return 0.0;
    }
    return this->discountRate;
}

int DataLoader::getReportingInterval() {
    if (!this->costSwitch) {
        return 0;
    }
    return this->reportingInterval;
}

bool DataLoader::getCostCategoryOutputs() {
    if (!this->costSwitch) {
        return false;
    }
    return this->costCategoryOutputs;
}
