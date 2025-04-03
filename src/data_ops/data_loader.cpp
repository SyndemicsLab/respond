////////////////////////////////////////////////////////////////////////////////
// File: data_loader.cpp                                                      //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-27                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/data_loader_internals.hpp"

#include <algorithm>
#include <cstddef>
#include <sstream>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include <Eigen/Eigen>
#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/matrices.hpp>
#include <respond/utils/logging.hpp>

namespace respond {
namespace data_ops {
Matrix3d DataLoaderImpl::LoadInitialSample(const std::string &file) {
    // INITIAL GROUP

    Data::IDataTablePtr initial_cohort = LoadDataTable(file);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    initial_sample =
        CreateMatrix3d(number_intervention_states, number_behavior_states,
                       number_demographic_combos);

    for (int i = 0; i < number_intervention_states; ++i) {
        std::string post_check_string = interventions[i];
        std::transform(post_check_string.begin(), post_check_string.end(),
                       post_check_string.begin(),
                       [](unsigned char c) { return std::tolower(c); });
        if (post_check_string.rfind("post", 0) == 0) {
            continue;
        }

        std::unordered_map<std::string, std::string> select_where = {};
        select_where["block"] = interventions[i];
        Data::IDataTablePtr interventions_data_table =
            initial_cohort->selectWhere(select_where);

        for (int b = 0; b < number_behavior_states; ++b) {
            select_where.clear();
            select_where["oud"] = behaviors[b];
            Data::IDataTablePtr behavior_data_table =
                interventions_data_table->selectWhere(select_where);
            std::vector<std::string> values =
                behavior_data_table->getColumn("counts");
            if (values.size() == 0) {
                respond::utils::LogWarning(
                    logger_name, "init_cohort.csv counts column has no record "
                                 "for " +
                                     behaviors[b] + " and " + interventions[i]);
                continue;
            }

            Eigen::array<Eigen::Index, 3> offsets = {i, b, 0};
            Eigen::array<Eigen::Index, 3> extent = {1, 1,
                                                    number_demographic_combos};

            initial_sample.slice(offsets, extent) =
                StrVecToMatrix3d(values, 1, 1, number_demographic_combos);
        }
    }
    return initial_sample;
}

TimedMatrix3d DataLoaderImpl::LoadEnteringSamples(
    const std::string &file, const std::string &entering_sample_intervention,
    const std::string &entering_sample_behavior) {

    Data::IDataTablePtr entering_samples_table = LoadDataTable(file);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    auto itr = find(interventions.begin(), interventions.end(),
                    entering_sample_intervention);
    int interventions_index =
        (itr != interventions.end()) ? itr - interventions.begin() : 0;

    itr = find(behaviors.begin(), behaviors.end(), entering_sample_behavior);
    int behaviors_index =
        (itr != behaviors.end()) ? itr - behaviors.begin() : 0;

    std::string column_prefix = "cohort_size_change_";
    std::stringstream column;

    int start_time = 0;
    std::vector<int> entering_sample_change_times =
        GetConfig()->getIntVector("simulation.entering_sample_change_times");
    for (int i = 0; i < entering_sample_change_times.size(); ++i) {
        int changepoint = entering_sample_change_times[i];
        column.clear();
        column << column_prefix
               << ((i == 0) ? "1_"
                            : (std::to_string(
                                   entering_sample_change_times[i - 1] + 1) +
                               "_"))
               << std::to_string(changepoint);
        std::vector<std::string> col =
            entering_samples_table->getColumn(column.str());

        Matrix3d entering_sample =
            CreateMatrix3d(number_intervention_states, number_behavior_states,
                           number_demographic_combos);

        // Slice for setting matrix values. We select a single
        // value and set that constant
        Eigen::array<Eigen::Index, 3> offsets = {interventions_index,
                                                 behaviors_index, 0};
        Eigen::array<Eigen::Index, 3> extent = {1, 1,
                                                number_demographic_combos};

        entering_sample.slice(offsets, extent) =
            StrVecToMatrix3d(col, 1, 1, number_demographic_combos);

        FillTime(start_time, changepoint, entering_sample, entering_samples);
    }
    return entering_samples;
}

TimedMatrix3d DataLoaderImpl::LoadEnteringSamples(const std::string &file) {
    Data::IDataTablePtr entering_cohort = LoadDataTable(file);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    std::string column_prefix = "cohort_size_change_";
    std::stringstream column;
    int start_time = 0;
    std::vector<int> entering_sample_change_times =
        GetConfig()->getIntVector("simulation.entering_sample_change_times");
    for (int e = 0; e < entering_sample_change_times.size(); ++e) {
        int changepoint = entering_sample_change_times[e];
        column.clear();
        column << column_prefix
               << ((e == 0) ? "1_"
                            : (std::to_string(
                                   entering_sample_change_times[e - 1] + 1) +
                               "_"))
               << std::to_string(changepoint);
        std::vector<std::string> col = entering_cohort->getColumn(column.str());

        Matrix3d entering_sample =
            CreateMatrix3d(number_intervention_states, number_behavior_states,
                           number_demographic_combos);

        for (int i = 0; i < number_intervention_states; ++i) {

            std::string post_check_string = interventions[i];
            std::transform(post_check_string.begin(), post_check_string.end(),
                           post_check_string.begin(),
                           [](unsigned char c) { return std::tolower(c); });
            if (post_check_string.rfind("post", 0) == 0) {
                continue;
            }

            std::unordered_map<std::string, std::string> select_where = {};
            select_where["block"] = interventions[i];
            Data::IDataTablePtr interventions_data_table =
                entering_cohort->selectWhere(select_where);

            for (int b = 0; b < number_behavior_states; ++b) {
                select_where.clear();
                select_where["oud"] = behaviors[b];
                Data::IDataTablePtr behavior_data_table =
                    interventions_data_table->selectWhere(select_where);
                std::vector<std::string> values =
                    behavior_data_table->getColumn(column.str());
                if (values.size() == 0) {
                    respond::utils::LogWarning(
                        logger_name, "entering_cohort.csv " + column.str() +
                                         " column has no record "
                                         "for " +
                                         behaviors[b] + " and " +
                                         interventions[i]);
                    continue;
                }

                Eigen::array<Eigen::Index, 3> offsets = {i, b, 0};
                Eigen::array<Eigen::Index, 3> extent = {
                    1, 1, number_demographic_combos};

                entering_sample.slice(offsets, extent) =
                    StrVecToMatrix3d(values, 1, 1, number_demographic_combos);
            }
        }
        FillTime(start_time, changepoint, entering_sample, entering_samples);
    }
    return entering_samples;
}

Matrix3d DataLoaderImpl::LoadOUDTransitionRates(const std::string &file) {

    Data::IDataTablePtr behavior_transitions_table = LoadDataTable(file);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();
    // end dimensions of oudTransitionRates are number_intervention_states x
    // number_behavior_states^2 x demographics start with a vector of
    // StateTensor-sized Matrix3d objects and stack at the end
    Matrix3d running_transitions =
        CreateMatrix3d(number_intervention_states,
                       number_behavior_states * number_behavior_states,
                       number_demographic_combos);

    // Matrix3d objects in the vector are matched with the order that oud
    // states are specified in the Config file. the order represents the
    // "initial oud state"
    for (int b = 0; b < number_behavior_states; ++b) {
        std::unordered_map<std::string, std::string> select_where = {};
        select_where["initial_oud"] = behaviors[b];
        Data::IDataTablePtr selected_behavior_table =
            behavior_transitions_table->selectWhere(select_where);
        for (int i = 0; i < number_intervention_states; ++i) {
            select_where.clear();
            select_where["intervention"] = interventions[i];
            Data::IDataTablePtr selected_intervention_table =
                selected_behavior_table->selectWhere(select_where);
            for (int result_state = 0; result_state < number_behavior_states;
                 ++result_state) {

                std::string column = behaviors[result_state];

                std::vector<std::string> col =
                    selected_intervention_table->getColumn(column);
                if (col.size() == 0) {
                    respond::utils::LogWarning(
                        logger_name, "oud_trans.csv " + column +
                                         " column has no record for " +
                                         interventions[i]);
                    continue;
                }

                // Slice for setting matrix values. We select a single
                // value and set that constant
                Eigen::array<Eigen::Index, 3> offset = {
                    i, (b * number_behavior_states) + result_state, 0};
                Eigen::array<Eigen::Index, 3> extent = {
                    1, 1, number_demographic_combos};

                running_transitions.slice(offset, extent) =
                    StrVecToMatrix3d(col, 1, 1, number_demographic_combos);
            }
        }
    }
    oud_transition_rates = std::move(running_transitions);
    return oud_transition_rates;
}

Matrix3d DataLoaderImpl::LoadInterventionInitRates(const std::string &file) {

    Data::IDataTablePtr intervention_init_table = LoadDataTable(file);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    Matrix3d running_transitions =
        CreateMatrix3d(number_intervention_states,
                       number_behavior_states * number_behavior_states,
                       number_demographic_combos);

    for (int b = 0; b < number_behavior_states; ++b) {
        std::unordered_map<std::string, std::string> select_where = {};
        select_where["initial_oud_state"] = behaviors[b];
        Data::IDataTablePtr selected_behavior_table =
            intervention_init_table->selectWhere(select_where);
        for (int i = 0; i < number_intervention_states; ++i) {
            select_where.clear();
            select_where["to_intervention"] = interventions[i];
            Data::IDataTablePtr selected_intervention_table =
                selected_behavior_table->selectWhere(select_where);
            for (int result_state = 0; result_state < number_behavior_states;
                 ++result_state) {

                std::string column = behaviors[result_state];

                std::vector<std::string> col =
                    selected_intervention_table->getColumn(column);
                if (col.size() == 0) {
                    respond::utils::LogWarning(
                        logger_name, "block_init_effect.csv " + column +
                                         " column has no "
                                         "record for " +
                                         interventions[i]);
                    continue;
                }

                // Slice for setting matrix values. We select a single
                // value and set that constant
                Eigen::array<Eigen::Index, 3> offset = {
                    i, (b * number_behavior_states) + result_state, 0};
                Eigen::array<Eigen::Index, 3> extent = {
                    1, 1, number_demographic_combos};

                running_transitions.slice(offset, extent) = DoubleToMatrix3d(
                    std::stod(col[0]), 1, 1, number_demographic_combos);
            }
        }
    }

    intervention_init_rates = std::move(running_transitions);
    return intervention_init_rates;
}

TimedMatrix3d
DataLoaderImpl::LoadInterventionTransitionRates(const std::string &file) {
    Data::IDataTablePtr intervention_transitions_table = LoadDataTable(file);

    std::vector<int> intervention_change_times =
        GetConfig()->getIntVector("simulation.intervention_change_times");

    try {
        intervention_transition_rates = BuildTransitionRatesOverTime(
            intervention_change_times, intervention_transitions_table);
    } catch (const std::exception &e) {
        respond::utils::LogError(logger_name, e.what());
    }

    return intervention_transition_rates;
}

TimedMatrix3d DataLoaderImpl::LoadOverdoseRates(const std::string &file) {

    Data::IDataTablePtr overdose_transitions_table = LoadDataTable(file);

    int start_time = 0;
    std::vector<int> overdose_change_times =
        GetConfig()->getIntVector("simulation.overdose_change_times");
    for (int i = 0; i < overdose_change_times.size(); ++i) {
        int timestep = overdose_change_times[i];
        std::stringstream column;
        column << "overdose_"
               << ((i == 0)
                       ? "1"
                       : (std::to_string(overdose_change_times[i - 1] + 1)))
               << "_" << std::to_string(timestep);

        std::vector<std::string> headers =
            overdose_transitions_table->getHeaders();

        for (std::string header : headers) {
            if (header.find(column.str()) != std::string::npos) {
                Matrix3d temp = BuildOverdoseTransitions(
                    overdose_transitions_table, header);
                FillTime(start_time, timestep, temp, overdose_rates);
            }
        }
    }
    return overdose_rates;
}

TimedMatrix3d DataLoaderImpl::LoadFatalOverdoseRates(const std::string &file) {
    Data::IDataTablePtr fatal_overdose_table = LoadDataTable(file);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    int start_time = 0;
    std::vector<int> overdose_change_times =
        GetConfig()->getIntVector("simulation.overdose_change_times");
    for (int i = 0; i < overdose_change_times.size(); ++i) {
        int timestep = overdose_change_times[i];
        std::stringstream column;
        column << "percent_overdoses_fatal_"
               << ((i == 0)
                       ? "1"
                       : (std::to_string(overdose_change_times[i - 1] + 1)))
               << "_" << std::to_string(timestep);

        std::vector<std::string> headers = fatal_overdose_table->getHeaders();

        for (std::string header : headers) {
            if (header.find(column.str()) != std::string::npos) {
                Matrix3d temp =
                    BuildFatalOverdoseTransitions(fatal_overdose_table, header);
                FillTime(start_time, timestep, temp, fatal_overdose_rates);
            }
        }
    }
    return fatal_overdose_rates;
}

Matrix3d DataLoaderImpl::LoadMortalityRates(
    const std::string &smr_file, const std::string &background_mortality_file) {

    // MORTALITY TRANSITIONS
    // mortality here refers to death from reasons other than oud and is
    // calculated by combining the SMR and background mortality calculation
    // to combine these into the mortality is 1-exp(log(1-bg_mort)*SMR)
    Data::IDataTablePtr temp = LoadDataTable(smr_file);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    std::vector<std::string> smr_column = temp->getColumn("SMR");
    if (smr_column.empty()) {
        respond::utils::LogError(logger_name,
                                 "SMR column not found in SMR.csv");
        return {};
    }

    // only stratified by the demographics, needs to be expanded for oud and
    // intervention

    temp = LoadDataTable(background_mortality_file);
    std::vector<std::string> background_mortality_column =
        temp->getColumn("death_prob");

    if (background_mortality_column.empty()) {
        respond::utils::LogError(
            logger_name,
            "death_prob column not found in background_mortality.csv");
        return {};
    }

    Matrix3d mortality_transitions =
        CreateMatrix3d(number_intervention_states, number_behavior_states,
                       number_demographic_combos);
    // mortality is one element per stratum, no time variability
    int smr_index = 0;
    for (int i = 0; i < number_intervention_states; ++i) {
        for (int d = 0; d < number_demographic_combos; d++) {
            for (int oud = 0; oud < number_behavior_states; ++oud) {
                if (background_mortality_column.size() > d &&
                    smr_column.size() > smr_index) {
                    mortality_transitions(i, oud, d) =
                        1 -
                        exp(log(1 - std::stod(background_mortality_column[d])) *
                            std::stod(smr_column[smr_index]));
                    smr_index++;
                } else {
                    mortality_transitions(i, oud, d) = 0.0;
                }
            }
        }
    }
    mortality_rates = mortality_transitions;
    return mortality_rates;
}

Matrix3d
DataLoaderImpl::BuildInterventionMatrix(const Data::IDataTablePtr &table,
                                        std::string name, int timestep) {

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    std::vector<int> intervention_change_times =
        GetConfig()->getIntVector("simulation.intervention_change_times");

    Matrix3d transition_matrix =
        CreateMatrix3d(number_intervention_states, number_behavior_states,
                       number_demographic_combos);

    std::unordered_map<std::string, std::string> select_where;

    for (int b = 0; b < number_behavior_states; ++b) {
        select_where.clear();
        select_where["oud"] = behaviors[b];
        select_where["initial_intervention"] = name;
        Data::IDataTablePtr temp = table->selectWhere(select_where);
        if (temp->getShape().getNRows() == 0) {
            respond::utils::LogWarning(
                logger_name, "block_trans.csv counts column has error with " +
                                 behaviors[b] + " and " + name);
            continue;
        }

        for (int i = 0; i < number_intervention_states; ++i) {

            // Slice for setting matrix values. We select a single
            // value and set that constant
            Eigen::array<Eigen::Index, 3> offsets = {i, b, 0};

            Eigen::array<Eigen::Index, 3> extent = {1, 1,
                                                    number_demographic_combos};

            auto it = std::find(intervention_change_times.begin(),
                                intervention_change_times.end(), timestep);

            int idx = it - intervention_change_times.begin();

            std::stringstream column;
            column << interventions[i] << "_"
                   << ((idx == 0)
                           ? "1"
                           : (std::to_string(
                                 intervention_change_times[idx - 1] + 1)))
                   << "_" << std::to_string(timestep);

            std::vector<std::string> value = temp->getColumn(column.str());

            transition_matrix.slice(offsets, extent) =
                StrVecToMatrix3d(value, 1, 1, number_demographic_combos);
        }
    }
    return transition_matrix;
}

Matrix3d
DataLoaderImpl::CreateTransitionMatrix3d(const Data::IDataTablePtr &table,
                                         Dimension dimension, int timestep) {

    std::shared_ptr<Data::DataTable> data_table =
        std::dynamic_pointer_cast<Data::DataTable>(table);

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    Data::DataTable temp(*data_table);
    Data::IDataTablePtr tempPtr =
        std::make_shared<Data::DataTable>(std::move(temp));

    if (dimension == Dimension::kIntervention) {
        Matrix3d matrix_stack = CreateMatrix3d(
            number_intervention_states * number_intervention_states,
            number_behavior_states, number_demographic_combos);
        for (int i = 0; i < number_intervention_states; i++) {
            // assign to index + offset of numInterventions
            Eigen::array<Eigen::Index, 3> offsets = {0, 0, 0};
            offsets[static_cast<int>(Dimension::kIntervention)] =
                i * number_intervention_states;
            offsets[static_cast<int>(Dimension::kOud)] = i * 0;
            offsets[static_cast<int>(Dimension::kDemographicCombo)] = 0;
            Eigen::array<Eigen::Index, 3> extent = {0, 0, 0};
            extent[static_cast<int>(Dimension::kIntervention)] =
                number_intervention_states;
            extent[static_cast<int>(Dimension::kOud)] = number_behavior_states;
            extent[static_cast<int>(Dimension::kDemographicCombo)] =
                number_demographic_combos;
            Matrix3d temp =
                BuildInterventionMatrix(table, interventions[i], timestep);
            matrix_stack.slice(offsets, extent) = temp;
        }
        return matrix_stack;

    } else if (dimension == Dimension::kOud) {
        Matrix3d matrix_stack =
            CreateMatrix3d(number_intervention_states,
                           number_behavior_states * number_behavior_states,
                           number_demographic_combos);
        return matrix_stack;
    }
    Matrix3d matrix_stack =
        CreateMatrix3d(number_intervention_states, number_behavior_states,
                       number_demographic_combos);
    return matrix_stack;
}

TimedMatrix3d
DataLoaderImpl::BuildTransitionRatesOverTime(const std::vector<int> &ict,
                                             const Data::IDataTablePtr &table) {
    TimedMatrix3d matrices;
    int start_time = 0;
    for (int timestep : ict) {
        Matrix3d transition_matrix =
            CreateTransitionMatrix3d(table, Dimension::kIntervention, timestep);
        FillTime(start_time, timestep, transition_matrix, matrices);
    }
    return matrices;
}

Matrix3d
DataLoaderImpl::BuildOverdoseTransitions(Data::IDataTablePtr const &table,
                                         const std::string &key) {

    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    Matrix3d overdose_transitions =
        CreateMatrix3d(number_intervention_states, number_behavior_states,
                       number_demographic_combos);

    // This should be translated into a matrix slice instead
    auto col = (table->getColumn(key));
    int row = 0;
    for (int i = 0; i < number_intervention_states; ++i) {
        for (int d = 0; d < number_demographic_combos; ++d) {
            for (int b = 0; b < number_behavior_states; ++b) {
                if (row >= col.size()) {
                    continue;
                }
                overdose_transitions(i, b, d) = std::stod(col[row]);
                ++row;
            }
        }
    }
    return overdose_transitions;
}

Matrix3d
DataLoaderImpl::BuildFatalOverdoseTransitions(Data::IDataTablePtr const &table,
                                              const std::string &key) {
    std::vector<std::string> behaviors =
        GetConfig()->getStringVector("state.ouds");
    int number_behavior_states = behaviors.size();
    int number_demographic_combos = GetDemographicCombos().size();
    std::vector<std::string> interventions =
        GetConfig()->getStringVector("state.interventions");
    int number_intervention_states = interventions.size();

    Matrix3d fatal_overdose_transitions =
        CreateMatrix3d(number_intervention_states, number_behavior_states,
                       number_demographic_combos);
    std::vector<std::string> col = table->getColumn(key);
    int row = 0;
    for (int d = 0; d < number_demographic_combos; ++d) {
        if (row >= col.size()) {
            respond::utils::LogError(
                logger_name,
                "Invalid Number of Entries for single year of Fatal "
                "Overdoses. Have " +
                    std::to_string(col.size()) + " and expected " +
                    std::to_string(d));
            return fatal_overdose_transitions;
        }
        // intervention, oud_state, dem
        Eigen::array<Eigen::Index, 3> offsets = {0, 0, 0};
        offsets[static_cast<int>(Dimension::kDemographicCombo)] = d;
        Eigen::array<Eigen::Index, 3> extent = {0, 0, 0};
        extent[static_cast<int>(Dimension::kIntervention)] =
            number_intervention_states;
        extent[static_cast<int>(Dimension::kOud)] = number_behavior_states;
        extent[static_cast<int>(Dimension::kDemographicCombo)] = 1;
        fatal_overdose_transitions.slice(offsets, extent)
            .setConstant(std::stod(col[row]));
        ++row;
    }
    return fatal_overdose_transitions;
}

void DataLoaderImpl::FillTime(int &start, const int end, Matrix3d data,
                              TimedMatrix3d &storage) {
    for (; start <= end; start++) {
        storage[start] = data;
    }
}

Matrix3d
DataLoaderImpl::StrVecToMatrix3d(std::vector<std::string> string_vector,
                                 int first_dimension, int second_dimension,
                                 int third_dimension) {
    std::vector<double> double_vector(string_vector.size());
    std::transform(string_vector.begin(), string_vector.end(),
                   double_vector.begin(),
                   [](const std::string &val) { return std::stod(val); });

    Eigen::TensorMap<Eigen::Tensor<double, 3>> writer_tensor(
        double_vector.data(), first_dimension, second_dimension,
        third_dimension);

    Matrix3d ret = writer_tensor;
    return ret;
}

Matrix3d DataLoaderImpl::DoubleToMatrix3d(double value, int first_dimension,
                                          int second_dimension,
                                          int third_dimension) {
    std::vector<double> transition_vector(first_dimension * second_dimension *
                                          third_dimension);
    std::fill(transition_vector.begin(), transition_vector.end(), value);
    Eigen::TensorMap<Eigen::Tensor<double, 3>> writer_tensor(
        transition_vector.data(), first_dimension, second_dimension,
        third_dimension);

    Matrix3d ret = writer_tensor;
    return ret;
}

std::unique_ptr<DataLoader> DataLoader::Create(const std::string &directory,
                                               const std::string &log_name) {
    return std::make_unique<DataLoaderImpl>(directory, log_name);
}
} // namespace data_ops
} // namespace respond