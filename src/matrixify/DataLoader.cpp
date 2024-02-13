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
        : Loader(inputDir, logger) {
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
    Matrix3d DataLoader::loadInitialSample(std::string const &csvName) {
        // INITIAL GROUP

        Data::IDataTablePtr initialCohort = loadTable(csvName);

        this->initialSample = Matrixify::Matrix3dFactory::Create(
            getNumOUDStates(), getNumInterventions(),
            getNumDemographicCombos());

        for (int intervention = 0; intervention < getNumInterventions();
             ++intervention) {

            std::string testStr = interventions[intervention];
            std::transform(testStr.begin(), testStr.end(), testStr.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            if (testStr.rfind("post", 0) == 0) {
                continue;
            }

            std::unordered_map<std::string, std::string>
                interventionSelectCriteria = {};
            interventionSelectCriteria["block"] =
                this->interventions[intervention];
            Data::IDataTablePtr interventionDT =
                initialCohort->selectWhere(interventionSelectCriteria);

            for (int oudState = 0; oudState < getNumOUDStates(); ++oudState) {
                std::unordered_map<std::string, std::string> oudSelectCriteria =
                    {};
                oudSelectCriteria["oud"] = this->oudStates[oudState];
                Data::IDataTablePtr oudDT =
                    interventionDT->selectWhere(oudSelectCriteria);
                std::vector<std::string> values = oudDT->getColumn("counts");
                if (values.size() == 0) {
                    logger->warn("counts column has no record for {} and {}",
                                 this->oudStates[oudState],
                                 this->interventions[intervention]);
                    continue;
                }

                Eigen::array<Eigen::Index, 3> offsets = {intervention, oudState,
                                                         0};
                Eigen::array<Eigen::Index, 3> extents = {
                    1, 1, getNumDemographicCombos()};

                initialSample.slice(offsets, extents) =
                    strVecToMatrix3d(values, 1, 1, getNumDemographicCombos());
            }
        }
        return this->initialSample;
    }

    Matrix4d DataLoader::loadEnteringSamples(
        std::string const &csvName,
        std::string const &enteringSampleIntervention,
        std::string const &enteringSampleOUD) {

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

            Matrix3d enteringSample = Matrixify::Matrix3dFactory::Create(
                getNumOUDStates(), getNumInterventions(),
                getNumDemographicCombos());

            // Slice for setting matrix values. We select a single
            // value and set that constant
            Eigen::array<Eigen::Index, 3> offsets = {esiIdx, esoIdx, 0};
            Eigen::array<Eigen::Index, 3> extents = {1, 1,
                                                     getNumDemographicCombos()};

            enteringSample.slice(offsets, extents) =
                strVecToMatrix3d(col, 1, 1, getNumDemographicCombos());

            fillTime(startTime, changepoint, enteringSample,
                     this->enteringSamples);
        }
        return this->enteringSamples;
    }

    Matrix3d DataLoader::loadOUDTransitionRates(std::string const &csvName) {

        Data::IDataTablePtr oudTransitionTable = loadTable(csvName);
        // end dimensions of oudTransitionRates are getNumInterventions() x
        // getNumOUDStates()^2 x demographics start with a vector of
        // StateTensor-sized Matrix3d objects and stack at the end
        Matrix3d tempOUDTransitions = Matrixify::Matrix3dFactory::Create(
            getNumOUDStates() * getNumOUDStates(), getNumInterventions(),
            getNumDemographicCombos());

        // Matrix3d objects in the vector are matched with the order that oud
        // states are specified in the Config file. the order represents the
        // "initial oud state"
        for (int initial_state = 0; initial_state < getNumOUDStates();
             ++initial_state) {
            std::unordered_map<std::string, std::string> oudSelectCriteria = {};
            oudSelectCriteria["initial_status"] =
                this->oudStates[initial_state];
            Data::IDataTablePtr oudSelectedTable =
                oudTransitionTable->selectWhere(oudSelectCriteria);
            for (int intervention = 0; intervention < getNumInterventions();
                 ++intervention) {
                std::unordered_map<std::string, std::string>
                    interventionSelectCriteria = {};
                interventionSelectCriteria["block"] =
                    this->interventions[intervention];
                Data::IDataTablePtr interventionSelectedTable =
                    oudSelectedTable->selectWhere(interventionSelectCriteria);
                for (int result_state = 0; result_state < getNumOUDStates();
                     ++result_state) {

                    std::string column = "to_" + this->oudStates[result_state];

                    std::vector<std::string> col =
                        interventionSelectedTable->getColumn(column);
                    if (col.size() == 0) {
                        logger->warn("{} column has no record for {}", column,
                                     this->interventions[intervention]);
                        continue;
                    }

                    // Slice for setting matrix values. We select a single
                    // value and set that constant
                    Eigen::array<Eigen::Index, 3> offsets1 = {
                        intervention,
                        (initial_state * getNumOUDStates()) + result_state, 0};
                    Eigen::array<Eigen::Index, 3> extents = {
                        1, 1, getNumDemographicCombos()};

                    tempOUDTransitions.slice(offsets1, extents) =
                        strVecToMatrix3d(col, 1, 1, getNumDemographicCombos());
                }
            }
        }

        this->oudTransitionRates = std::move(tempOUDTransitions);
        return this->oudTransitionRates;
    }

    Matrix3d DataLoader::loadInterventionInitRates(std::string const &csvName) {

        Data::IDataTablePtr interventionInitTable = loadTable(csvName);

        Matrix3d tempinterventionInit = Matrixify::Matrix3dFactory::Create(
            getNumOUDStates() * getNumOUDStates(), getNumInterventions(),
            getNumDemographicCombos());

        // Matrix3d objects in the vector are matched with the order that
        // oud states are specified in the Config file. the order represents
        // the "initial oud state"
        std::vector<std::string> oudStates = getOUDStates();
        std::vector<std::string> interventions = getInterventions();
        for (int i = 0; i < getNumOUDStates(); ++i) {
            std::unordered_map<std::string, std::string> selectConditions;
            selectConditions["initial_oud_state"] = oudStates[i];

            for (int j = 0; j < getNumInterventions(); ++j) {
                selectConditions["to_intervention"] = interventions[j];
                Data::IDataTablePtr temp =
                    interventionInitTable->selectWhere(selectConditions);
                std::vector<std::string> col =
                    temp->getColumn("retention_rate");

                if (col.empty()) {
                    logger->error("Rention Rate Not Found in "
                                  "block_trans.csv for {} {}",
                                  oudStates[i], interventions[j]);
                    throw std::out_of_range(interventions[j] +
                                            " not in block_trans.csv");
                }

                double retentionRate = std::stod(col[0]);
                double transitionRate = 1 - retentionRate;

                // Slice for setting matrix values. We select a single value and
                // set that constant
                Eigen::array<Eigen::Index, 3> offsets1 = {j, i, 0};

                Eigen::array<Eigen::Index, 3> offsets2 = {
                    j, i + getNumOUDStates(), 0};

                Eigen::array<Eigen::Index, 3> extents = {
                    1, 1, getNumDemographicCombos()};

                tempinterventionInit.slice(offsets1, extents) =
                    doubleToMatrix3d(retentionRate, 1, 1,
                                     getNumDemographicCombos());

                tempinterventionInit.slice(offsets2, extents) =
                    doubleToMatrix3d(transitionRate, 1, 1,
                                     getNumDemographicCombos());
            }
        }

        this->interventionInitRates = std::move(tempinterventionInit);
        return this->interventionInitRates;
    }

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

    Matrix4d DataLoader::loadOverdoseRates(std::string const &csvName) {

        // OVERDOSE
        Data::IDataTablePtr overdoseTransitionTable = loadTable(csvName);

        int startTime = 0;
        for (auto timestep : this->overdoseChangeTimes) {
            std::vector<std::string> headers =
                overdoseTransitionTable->getHeaders();

            for (std::string header : headers) {
                if (header.find("all_types_overdose_cycle" +
                                std::to_string(timestep)) !=
                    std::string::npos) {
                    Matrix3d temp = this->buildOverdoseTransitions(
                        overdoseTransitionTable, header);
                    fillTime(startTime, timestep, temp, this->overdoseRates);
                }
            }
        }
        return this->overdoseRates;
    }

    Matrix4d DataLoader::loadFatalOverdoseRates(std::string const &csvName) {

        std::vector<Matrix3d> tempFatalOverdoseTransitions;

        Data::IDataTablePtr fatalOverdoseTable = loadTable(csvName);

        int startTime = 0;
        for (int timestep : this->overdoseChangeTimes) {
            Matrix3d overdoseTransition = Matrixify::Matrix3dFactory::Create(
                getNumOUDStates(), getNumInterventions(),
                getNumDemographicCombos());

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
                getNumDemographicCombos());

            double t = (col.size() > 0) ? std::stod(col[0]) : 0.0;
            temp = temp.constant(t);

            fillTime(startTime, timestep, temp, this->fatalOverdoseRates);
        }
        return this->fatalOverdoseRates;
    }

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

    Matrixify::Matrix3d
    DataLoader::buildInterventionMatrix(Data::IDataTablePtr const &table,
                                        std::string interventionName,
                                        int timestep) {
        Matrixify::Matrix3d transMat = Matrixify::Matrix3dFactory::Create(
            getNumOUDStates(), getNumInterventions(),
            getNumDemographicCombos());

        std::unordered_map<std::string, std::string> selectConditions;

        for (int oudCtr = 0; oudCtr < getNumOUDStates(); ++oudCtr) {
            selectConditions.clear();
            selectConditions["oud"] = oudStates[oudCtr];
            selectConditions["initial_block"] = interventionName;
            Data::IDataTablePtr temp = table->selectWhere(selectConditions);
            if (temp->getShape().getNRows() == 0) {
                continue;
            }

            for (int interven = 0; interven < getNumInterventions();
                 ++interven) {

                // Slice for setting matrix values. We select a single
                // value and set that constant
                Eigen::array<Eigen::Index, 3> offsets = {interven, oudCtr, 0};

                Eigen::array<Eigen::Index, 3> extents = {
                    1, 1, getNumDemographicCombos()};

                std::string header = "";
                if (interventions[interven].find("Post") != std::string::npos) {
                    if (interventions[interven].find(interventionName) !=
                        std::string::npos) {
                        header = "to_corresponding_post_trt_cycle" +
                                 std::to_string(timestep);
                        std::vector<std::string> value =
                            temp->getColumn(header);

                        transMat.slice(offsets, extents) = strVecToMatrix3d(
                            value, 1, 1, getNumDemographicCombos());

                    } else {
                        transMat.slice(offsets, extents) = doubleToMatrix3d(
                            0.0, 1, 1, getNumDemographicCombos());
                    }
                } else {
                    header = "to_" + interventions[interven] + "_cycle" +
                             std::to_string(timestep);
                    std::vector<std::string> value = temp->getColumn(header);

                    transMat.slice(offsets, extents) = strVecToMatrix3d(
                        value, 1, 1, getNumDemographicCombos());
                }
            }
        }
        return transMat;
    }

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
            Matrix3d stackingMatrices = Matrixify::Matrix3dFactory::Create(
                getNumOUDStates(),
                getNumInterventions() * getNumInterventions(),
                getNumDemographicCombos());
            for (int i = 0; i < getNumInterventions(); i++) {
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
            Matrix3d stackingMatrices = Matrixify::Matrix3dFactory::Create(
                getNumOUDStates() * getNumOUDStates(), getNumInterventions(),
                getNumDemographicCombos());
            return stackingMatrices;
        }
        Matrix3d stackingMatrices = Matrixify::Matrix3dFactory::Create(
            getNumOUDStates(), getNumInterventions(),
            getNumDemographicCombos());
        return stackingMatrices;
    }

    Matrix4d
    DataLoader::buildTransitionRatesOverTime(std::vector<int> const &ict,
                                             Data::IDataTablePtr const &table) {
        Matrix4d m3dot;
        int startTime = 0;
        for (int timestep : ict) {
            Matrix3d transMat = this->createTransitionMatrix3d(
                table, Matrixify::INTERVENTION, timestep);
            fillTime(startTime, timestep, transMat, m3dot);
        }
        return m3dot;
    }

    Matrix3d
    DataLoader::buildOverdoseTransitions(Data::IDataTablePtr const &table,
                                         std::string const &key) {
        Matrix3d overdoseTransitionsCycle = Matrixify::Matrix3dFactory::Create(
            getNumOUDStates(), getNumInterventions(),
            getNumDemographicCombos());

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

    void DataLoader::fillTime(int &start, int const end, Matrix3d data,
                              Matrix4d &storage) {
        while (start <= end) {
            storage.insert(data, start);
            start++;
        }
    }

    Matrix3d DataLoader::strVecToMatrix3d(std::vector<std::string> strVec,
                                          int matD1, int matD2, int matD3) {
        std::vector<double> doubleVector(strVec.size());
        std::transform(strVec.begin(), strVec.end(), doubleVector.begin(),
                       [](const std::string &val) { return std::stod(val); });

        Eigen::TensorMap<Eigen::Tensor<double, 3>> writingTensor(
            doubleVector.data(), matD1, matD2, matD3);

        Matrix3d ret = writingTensor;
        return ret;
    }

    Matrix3d DataLoader::doubleToMatrix3d(double value, int matD1, int matD2,
                                          int matD3) {
        std::vector<double> rateVector(matD1 * matD2 * matD3);
        std::fill(rateVector.begin(), rateVector.end(), value);
        Eigen::TensorMap<Eigen::Tensor<double, 3>> writingTensor(
            rateVector.data(), matD1, matD2, matD3);

        Matrix3d ret = writingTensor;
        return ret;
    }
} // namespace Matrixify