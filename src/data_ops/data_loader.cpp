////////////////////////////////////////////////////////////////////////////////
// File: data_loader.cpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-11                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/data_loader_internals.hpp"

#include <respond/data_ops/matrix_3d_factory.hpp>

#include <cmath>
#include <stdexcept>
#include <unordered_map>

namespace respond::data_ops {
    Matrix3d DataLoaderImpl::LoadInitialSample(const std::string &file) {
        // INITIAL GROUP

        Data::IDataTablePtr initial_cohort = LoadDataTable(file);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        this->initial_sample = Matrix3dFactory::Create(
            number_behavior_states, number_intervention_states,
            number_demographic_combos);

        for (int intervention = 0; intervention < number_intervention_states;
             ++intervention) {

            std::string post_check_string = interventions[intervention];
            std::transform(post_check_string.begin(), post_check_string.end(),
                           post_check_string.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            if (post_check_string.rfind("post", 0) == 0) {
                continue;
            }

            std::unordered_map<std::string, std::string> select_where = {};
            select_where["block"] = interventions[intervention];
            Data::IDataTablePtr interventions_data_table =
                initial_cohort->selectWhere(select_where);

            for (int behavior = 0; behavior < number_behavior_states;
                 ++behavior) {
                select_where.clear();
                select_where["oud"] = behaviors[behavior];
                Data::IDataTablePtr oudDT =
                    interventions_data_table->selectWhere(select_where);
                std::vector<std::string> values = oudDT->getColumn("counts");
                if (values.size() == 0) {
                    respond::utils::LogWarning(
                        logger_name,
                        "init_cohort.csv counts column has no record "
                        "for " +
                            behaviors[behavior] + " and " +
                            interventions[intervention]);
                    continue;
                }

                Eigen::array<Eigen::Index, 3> offsets = {intervention, behavior,
                                                         0};
                Eigen::array<Eigen::Index, 3> extents = {
                    1, 1, number_demographic_combos};

                initial_sample.slice(offsets, extents) =
                    StrVecToMatrix3d(values, 1, 1, number_demographic_combos);
            }
        }
        return this->initial_sample;
    }

    Matrix4d DataLoaderImpl::LoadEnteringSamples(
        const std::string &file,
        const std::string &entering_sample_intervention,
        const std::string &entering_sample_behavior) {

        Data::IDataTablePtr enteringSamplesTable = LoadDataTable(file);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        auto itr = find(interventions.begin(), interventions.end(),
                        entering_sample_intervention);
        int esiIdx =
            (itr != interventions.end()) ? itr - interventions.begin() : 0;

        itr =
            find(behaviors.begin(), behaviors.end(), entering_sample_behavior);
        int esoIdx = (itr != behaviors.end()) ? itr - behaviors.begin() : 0;

        std::string columnPrefix = "cohort_size_change_";

        int startTime = 0;
        std::vector<int> entering_sample_change_times =
            GetConfig()->getIntVector(
                "simulation.entering_sample_change_times");
        for (int i = 0; i < entering_sample_change_times.size(); ++i) {
            int changepoint = entering_sample_change_times[i];
            std::string column = columnPrefix;
            if (i == 0) {
                column += "1_";
            } else {
                column +=
                    (std::to_string(entering_sample_change_times[i - 1] + 1) +
                     "_");
            }
            column += std::to_string(changepoint);
            std::vector<std::string> col =
                enteringSamplesTable->getColumn(column);

            Matrix3d enteringSample = data_ops::Matrix3dFactory::Create(
                number_behavior_states, number_intervention_states,
                number_demographic_combos);

            // Slice for setting matrix values. We select a single
            // value and set that constant
            Eigen::array<Eigen::Index, 3> offsets = {esiIdx, esoIdx, 0};
            Eigen::array<Eigen::Index, 3> extents = {1, 1,
                                                     number_demographic_combos};

            enteringSample.slice(offsets, extents) =
                StrVecToMatrix3d(col, 1, 1, number_demographic_combos);

            FillTime(startTime, changepoint, enteringSample,
                     this->entering_samples);
        }
        return this->entering_samples;
    }

    Matrix4d DataLoaderImpl::LoadEnteringSamples(const std::string &file) {
        // ENTERING GROUP Stratified by OUD

        Data::IDataTablePtr enteringCohort = LoadDataTable(file);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        std::string columnPrefix = "cohort_size_change_";
        int startTime = 0;
        std::vector<int> entering_sample_change_times =
            GetConfig()->getIntVector(
                "simulation.entering_sample_change_times");
        for (int i = 0; i < entering_sample_change_times.size(); ++i) {
            int changepoint = entering_sample_change_times[i];
            std::string column = columnPrefix;
            if (i == 0) {
                column += "1_";
            } else {
                column +=
                    (std::to_string(entering_sample_change_times[i - 1] + 1) +
                     "_");
            }
            column += std::to_string(changepoint);
            std::vector<std::string> col = enteringCohort->getColumn(column);

            Matrix3d enteringSample = data_ops::Matrix3dFactory::Create(
                number_behavior_states, number_intervention_states,
                number_demographic_combos);

            for (int intervention = 0;
                 intervention < number_intervention_states; ++intervention) {

                std::string post_check_string = interventions[intervention];
                std::transform(post_check_string.begin(),
                               post_check_string.end(),
                               post_check_string.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                if (post_check_string.rfind("post", 0) == 0) {
                    continue;
                }

                std::unordered_map<std::string, std::string> select_where = {};
                select_where["block"] = interventions[intervention];
                Data::IDataTablePtr interventions_data_table =
                    enteringCohort->selectWhere(select_where);

                for (int behavior = 0; behavior < number_behavior_states;
                     ++behavior) {
                    select_where.clear();
                    select_where["oud"] = behaviors[behavior];
                    Data::IDataTablePtr oudDT =
                        interventions_data_table->selectWhere(select_where);
                    std::vector<std::string> values = oudDT->getColumn(column);
                    if (values.size() == 0) {
                        respond::utils::LogWarning(
                            logger_name, "entering_cohort.csv " + column +
                                             " column has no record "
                                             "for " +
                                             behaviors[behavior] + " and " +
                                             interventions[intervention]);
                        continue;
                    }

                    Eigen::array<Eigen::Index, 3> offsets = {intervention,
                                                             behavior, 0};
                    Eigen::array<Eigen::Index, 3> extents = {
                        1, 1, number_demographic_combos};

                    enteringSample.slice(offsets, extents) = StrVecToMatrix3d(
                        values, 1, 1, number_demographic_combos);
                }
            }
            FillTime(startTime, changepoint, enteringSample,
                     this->entering_samples);
        }

        return this->entering_samples;
    }

    Matrix3d DataLoaderImpl::LoadOUDTransitionRates(const std::string &file) {

        Data::IDataTablePtr oudTransitionTable = LoadDataTable(file);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();
        // end dimensions of oudTransitionRates are number_intervention_states x
        // number_behavior_states^2 x demographics start with a vector of
        // StateTensor-sized Matrix3d objects and stack at the end
        Matrix3d tempOUDTransitions = data_ops::Matrix3dFactory::Create(
            number_behavior_states * number_behavior_states,
            number_intervention_states, number_demographic_combos);

        // Matrix3d objects in the vector are matched with the order that oud
        // states are specified in the Config file. the order represents the
        // "initial oud state"
        for (int initial_state = 0; initial_state < number_behavior_states;
             ++initial_state) {
            std::unordered_map<std::string, std::string> select_where = {};
            select_where["initial_oud"] = behaviors[initial_state];
            Data::IDataTablePtr oudSelectedTable =
                oudTransitionTable->selectWhere(select_where);
            for (int intervention = 0;
                 intervention < number_intervention_states; ++intervention) {
                select_where.clear();
                select_where["intervention"] = interventions[intervention];
                Data::IDataTablePtr interventionSelectedTable =
                    oudSelectedTable->selectWhere(select_where);
                for (int result_state = 0;
                     result_state < number_behavior_states; ++result_state) {

                    std::string column = behaviors[result_state];

                    std::vector<std::string> col =
                        interventionSelectedTable->getColumn(column);
                    if (col.size() == 0) {
                        respond::utils::LogWarning(
                            logger_name, "oud_trans.csv " + column +
                                             " column has no record for " +
                                             interventions[intervention]);
                        continue;
                    }

                    // Slice for setting matrix values. We select a single
                    // value and set that constant
                    Eigen::array<Eigen::Index, 3> offsets1 = {
                        intervention,
                        (initial_state * number_behavior_states) + result_state,
                        0};
                    Eigen::array<Eigen::Index, 3> extents = {
                        1, 1, number_demographic_combos};

                    tempOUDTransitions.slice(offsets1, extents) =
                        StrVecToMatrix3d(col, 1, 1, number_demographic_combos);
                }
            }
        }

        this->oud_transition_rates = std::move(tempOUDTransitions);
        return this->oud_transition_rates;
    }

    Matrix3d
    DataLoaderImpl::LoadInterventionInitRates(const std::string &file) {

        Data::IDataTablePtr interventionInitTable = LoadDataTable(file);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        Matrix3d tempinterventionInit = data_ops::Matrix3dFactory::Create(
            number_behavior_states * number_behavior_states,
            number_intervention_states, number_demographic_combos);

        for (int initial_state = 0; initial_state < number_behavior_states;
             ++initial_state) {
            std::unordered_map<std::string, std::string> select_where = {};
            select_where["initial_oud_state"] = behaviors[initial_state];
            Data::IDataTablePtr oudSelectedTable =
                interventionInitTable->selectWhere(select_where);
            for (int intervention = 0;
                 intervention < number_intervention_states; ++intervention) {
                select_where.clear();
                select_where["to_intervention"] = interventions[intervention];
                Data::IDataTablePtr interventionSelectedTable =
                    oudSelectedTable->selectWhere(select_where);
                for (int result_state = 0;
                     result_state < number_behavior_states; ++result_state) {

                    std::string column = behaviors[result_state];

                    std::vector<std::string> col =
                        interventionSelectedTable->getColumn(column);
                    if (col.size() == 0) {
                        respond::utils::LogWarning(
                            logger_name, "block_init_effect.csv " + column +
                                             " column has no "
                                             "record for " +
                                             interventions[intervention]);
                        continue;
                    }

                    // Slice for setting matrix values. We select a single
                    // value and set that constant
                    Eigen::array<Eigen::Index, 3> offsets1 = {
                        intervention,
                        (initial_state * number_behavior_states) + result_state,
                        0};
                    Eigen::array<Eigen::Index, 3> extents = {
                        1, 1, number_demographic_combos};

                    tempinterventionInit.slice(offsets1, extents) =
                        DoubleToMatrix3d(std::stod(col[0]), 1, 1,
                                         number_demographic_combos);
                }
            }
        }

        this->intervention_init_rates = std::move(tempinterventionInit);
        return this->intervention_init_rates;
    }

    Matrix4d
    DataLoaderImpl::LoadInterventionTransitionRates(const std::string &file) {

        // INTERVENTION TRANSITIONS
        Data::IDataTablePtr interventionTransitionTable = LoadDataTable(file);

        std::vector<int> intervention_change_times =
            GetConfig()->getIntVector("simulation.intervention_change_times");

        try {
            intervention_transition_rates = BuildTransitionRatesOverTime(
                intervention_change_times, interventionTransitionTable);
        } catch (const std::exception &e) {
            respond::utils::LogError(logger_name, e.what());
        }

        return intervention_transition_rates;
    }

    Matrix4d DataLoaderImpl::LoadOverdoseRates(const std::string &file) {

        // OVERDOSE
        Data::IDataTablePtr overdoseTransitionTable = LoadDataTable(file);

        int startTime = 0;
        std::vector<int> overdose_change_times =
            GetConfig()->getIntVector("simulation.overdose_change_times");
        for (int i = 0; i < overdose_change_times.size(); ++i) {
            int timestep = overdose_change_times[i];
            std::string odcolumn = "overdose_";
            if (i == 0) {
                odcolumn += "1_" + std::to_string(timestep);
            } else {
                odcolumn += std::to_string(overdose_change_times[i - 1] + 1) +
                            "_" + std::to_string(timestep);
            }
            std::vector<std::string> headers =
                overdoseTransitionTable->getHeaders();

            for (std::string header : headers) {
                if (header.find(odcolumn) != std::string::npos) {
                    Matrix3d temp = this->BuildOverdoseTransitions(
                        overdoseTransitionTable, header);
                    FillTime(startTime, timestep, temp, this->overdose_rates);
                }
            }
        }
        return this->overdose_rates;
    }

    Matrix4d DataLoaderImpl::LoadFatalOverdoseRates(const std::string &file) {

        std::vector<Matrix3d> tempFatalOverdoseTransitions;

        Data::IDataTablePtr fatalOverdoseTable = LoadDataTable(file);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        int startTime = 0;
        std::vector<int> overdose_change_times =
            GetConfig()->getIntVector("simulation.overdose_change_times");
        for (int i = 0; i < overdose_change_times.size(); ++i) {
            Matrix3d overdoseTransition = data_ops::Matrix3dFactory::Create(
                number_behavior_states, number_intervention_states,
                number_demographic_combos);

            int timestep = overdose_change_times[i];
            std::string fodColumn = "percent_overdoses_fatal_";
            if (i == 0) {
                fodColumn += "1_" + std::to_string(timestep);
            } else {
                fodColumn += std::to_string(overdose_change_times[i - 1] + 1) +
                             "_" + std::to_string(timestep);
            }

            std::vector<std::string> headers = fatalOverdoseTable->getHeaders();

            for (std::string header : headers) {
                if (header.find(fodColumn) != std::string::npos) {
                    Matrix3d temp = this->BuildFatalOverdoseTransitions(
                        fatalOverdoseTable, header);
                    FillTime(startTime, timestep, temp,
                             this->fatal_overdose_rates);
                }
            }
        }
        return this->fatal_overdose_rates;
    }

    Matrix3d DataLoaderImpl::LoadMortalityRates(const std::string &smrCSVName,
                                                const std::string &bgmCSVName) {

        // MORTALITY TRANSITIONS
        // mortality here refers to death from reasons other than oud and is
        // calculated by combining the SMR and background mortality calculation
        // to combine these into the mortality is 1-exp(log(1-bg_mort)*SMR)
        Data::IDataTablePtr temp = LoadDataTable(smrCSVName);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        std::vector<std::string> smrColumn = temp->getColumn("SMR");
        if (smrColumn.empty()) {
            respond::utils::LogError(logger_name,
                                     "SMR column not found in SMR.csv");
            return {};
        }

        // only stratified by the demographics, needs to be expanded for oud and
        // intervention

        temp = LoadDataTable(bgmCSVName);
        std::vector<std::string> backgroundMortalityColumn =
            temp->getColumn("death_prob");

        if (backgroundMortalityColumn.empty()) {
            respond::utils::LogError(
                logger_name,
                "death_prob column not found in background_mortality.csv");
            return {};
        }

        Matrix3d mortalityTransition = data_ops::Matrix3dFactory::Create(
            number_behavior_states, number_intervention_states,
            number_demographic_combos);
        // mortality is one element per stratum, no time variability
        int smrIndex = 0;
        for (int intervention = 0; intervention < number_intervention_states;
             ++intervention) {
            for (int dem = 0; dem < number_demographic_combos; dem++) {
                for (int oud = 0; oud < number_behavior_states; ++oud) {
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
        this->mortality_rates = mortalityTransition;
        return this->mortality_rates;
    }

    data_ops::Matrix3d
    DataLoaderImpl::BuildInterventionMatrix(Data::IDataTablePtr const &table,
                                            std::string interventionName,
                                            int timestep) {

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        std::vector<int> intervention_change_times =
            GetConfig()->getIntVector("simulation.intervention_change_times");

        data_ops::Matrix3d transMat = data_ops::Matrix3dFactory::Create(
            number_behavior_states, number_intervention_states,
            number_demographic_combos);

        std::unordered_map<std::string, std::string> selectConditions;

        for (int oudCtr = 0; oudCtr < number_behavior_states; ++oudCtr) {
            selectConditions.clear();
            selectConditions["oud"] = behaviors[oudCtr];
            selectConditions["initial_intervention"] = interventionName;
            Data::IDataTablePtr temp = table->selectWhere(selectConditions);
            if (temp->getShape().getNRows() == 0) {
                respond::utils::LogWarning(
                    logger_name,
                    "block_trans.csv counts column has error with " +
                        behaviors[oudCtr] + " and " + interventionName);
                continue;
            }

            for (int i = 0; i < number_intervention_states; ++i) {

                // Slice for setting matrix values. We select a single
                // value and set that constant
                Eigen::array<Eigen::Index, 3> offsets = {i, oudCtr, 0};

                Eigen::array<Eigen::Index, 3> extents = {
                    1, 1, number_demographic_combos};

                std::string header = interventions[i] + "_";

                auto it = std::find(intervention_change_times.begin(),
                                    intervention_change_times.end(), timestep);

                int idx = it - intervention_change_times.begin();

                if (idx == 0) {
                    header += "1_" + std::to_string(timestep);
                } else {
                    header +=
                        std::to_string(intervention_change_times[idx - 1] + 1) +
                        "_" + std::to_string(timestep);
                }

                std::vector<std::string> value = temp->getColumn(header);

                transMat.slice(offsets, extents) =
                    StrVecToMatrix3d(value, 1, 1, number_demographic_combos);
            }
        }
        return transMat;
    }

    data_ops::Matrix3d
    DataLoaderImpl::CreateTransitionMatrix3d(Data::IDataTablePtr const &table,
                                             data_ops::Dimension dimension,
                                             int timestep) {

        std::shared_ptr<Data::DataTable> dynaCast =
            std::dynamic_pointer_cast<Data::DataTable>(table);

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        Data::DataTable temp(*dynaCast);
        Data::IDataTablePtr tempPtr =
            std::make_shared<Data::DataTable>(std::move(temp));

        if (dimension == Dimension::kIntervention) {
            Matrix3d stackingMatrices = data_ops::Matrix3dFactory::Create(
                number_behavior_states,
                number_intervention_states * number_intervention_states,
                number_demographic_combos);
            for (int i = 0; i < number_intervention_states; i++) {
                // assign to index + offset of numInterventions
                Eigen::array<Eigen::Index, 3> offsets = {0, 0, 0};
                offsets[Dimension::kIntervention] =
                    i * number_intervention_states;
                offsets[Dimension::kOud] = i * 0;
                offsets[Dimension::kDemographicCombo] = 0;
                Eigen::array<Eigen::Index, 3> extents = {0, 0, 0};
                extents[Dimension::kIntervention] = number_intervention_states;
                extents[Dimension::kOud] = number_behavior_states;
                extents[Dimension::kDemographicCombo] =
                    number_demographic_combos;
                data_ops::Matrix3d temp =
                    BuildInterventionMatrix(table, interventions[i], timestep);
                stackingMatrices.slice(offsets, extents) = temp;
            }
            return stackingMatrices;

        } else if (dimension == Dimension::kOud) {
            Matrix3d stackingMatrices = data_ops::Matrix3dFactory::Create(
                number_behavior_states * number_behavior_states,
                number_intervention_states, number_demographic_combos);
            return stackingMatrices;
        }
        Matrix3d stackingMatrices = data_ops::Matrix3dFactory::Create(
            number_behavior_states, number_intervention_states,
            number_demographic_combos);
        return stackingMatrices;
    }

    Matrix4d DataLoaderImpl::BuildTransitionRatesOverTime(
        std::vector<int> const &ict, Data::IDataTablePtr const &table) {
        Matrix4d m3dot;
        int startTime = 0;
        for (int timestep : ict) {
            Matrix3d transMat = CreateTransitionMatrix3d(
                table, Dimension::kIntervention, timestep);
            FillTime(startTime, timestep, transMat, m3dot);
        }
        return m3dot;
    }

    Matrix3d
    DataLoaderImpl::BuildOverdoseTransitions(Data::IDataTablePtr const &table,
                                             const std::string &key) {

        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        Matrix3d overdoseTransitionsCycle = data_ops::Matrix3dFactory::Create(
            number_behavior_states, number_intervention_states,
            number_demographic_combos);

        int row = 0;
        for (int intervention = 0; intervention < number_intervention_states;
             ++intervention) {
            for (int dem = 0; dem < number_demographic_combos; ++dem) {
                for (int oud_state = 0; oud_state < number_behavior_states;
                     ++oud_state) {
                    if (behaviors[oud_state].find("Nonactive") !=
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

    Matrix3d DataLoaderImpl::BuildFatalOverdoseTransitions(
        Data::IDataTablePtr const &table, const std::string &key) {
        std::vector<std::string> behaviors =
            GetConfig()->getStringVector("state.ouds");
        size_t number_behavior_states = behaviors.size();
        size_t number_demographic_combos = GetDemographicCombos().size();
        std::vector<std::string> interventions =
            GetConfig()->getStringVector("state.interventions");
        size_t number_intervention_states = interventions.size();

        Matrix3d fatalOverdoseTransitionsCycle =
            data_ops::Matrix3dFactory::Create(number_behavior_states,
                                              number_intervention_states,
                                              number_demographic_combos);
        std::vector<std::string> col = table->getColumn(key);
        int row = 0;
        for (int dem = 0; dem < number_demographic_combos; ++dem) {
            if (row >= col.size()) {
                respond::utils::LogError(
                    logger_name,
                    "Invalid Number of Entries for single year of Fatal "
                    "Overdoses. Have " +
                        std::to_string(col.size()) + " and expected " +
                        std::to_string(dem));
                return fatalOverdoseTransitionsCycle;
            }
            // intervention, oud_state, dem
            Eigen::array<Eigen::Index, 3> offsets = {0, 0, 0};
            offsets[Dimension::kDemographicCombo] = dem;
            Eigen::array<Eigen::Index, 3> extents = {0, 0, 0};
            extents[Dimension::kIntervention] = number_intervention_states;
            extents[Dimension::kOud] = number_behavior_states;
            extents[Dimension::kDemographicCombo] = 1;
            fatalOverdoseTransitionsCycle.slice(offsets, extents)
                .setConstant(std::stod(col[row]));
            ++row;
        }

        return fatalOverdoseTransitionsCycle;
    }

    void DataLoaderImpl::FillTime(int &start, int const end, Matrix3d data,
                                  Matrix4d &storage) {
        while (start <= end) {
            storage.Insert(data, start);
            start++;
        }
    }

    Matrix3d DataLoaderImpl::StrVecToMatrix3d(std::vector<std::string> strVec,
                                              int matD1, int matD2, int matD3) {
        std::vector<double> doubleVector(strVec.size());
        std::transform(strVec.begin(), strVec.end(), doubleVector.begin(),
                       [](const std::string &val) { return std::stod(val); });

        Eigen::TensorMap<Eigen::Tensor<double, 3>> writingTensor(
            doubleVector.data(), matD1, matD2, matD3);

        Matrix3d ret = writingTensor;
        return ret;
    }

    Matrix3d DataLoaderImpl::DoubleToMatrix3d(double value, int matD1,
                                              int matD2, int matD3) {
        std::vector<double> rateVector(matD1 * matD2 * matD3);
        std::fill(rateVector.begin(), rateVector.end(), value);
        Eigen::TensorMap<Eigen::Tensor<double, 3>> writingTensor(
            rateVector.data(), matD1, matD2, matD3);

        Matrix3d ret = writingTensor;
        return ret;
    }
} // namespace respond::data_ops