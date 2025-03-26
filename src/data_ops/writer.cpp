////////////////////////////////////////////////////////////////////////////////
// File: Writer.cpp                                                           //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include "internals/writer_internals.hpp"

#include <fstream>
#include <regex>
#include <sstream>
#include <string>
#include <vector>

#include <respond/data_ops/data_types.hpp>
#include <respond/data_ops/matrices.hpp>
#include <respond/utils/logging.hpp>

namespace respond::data_ops {
std::string WriterImpl::WriteInputData(const DataLoader &data_loader,
                                       const std::string &directory,
                                       const OutputType output_type) const {
    std::stringstream result;
    result << WriteOUDTransitionRates(data_loader, directory, output_type)
           << std::endl
           << std::endl
           << WriteInterventionTransitionRates(data_loader, directory,
                                               output_type)
           << std::endl
           << std::endl
           << WriteInterventionInitRates(data_loader, directory, output_type)
           << std::endl
           << std::endl
           << WriteOverdoseRates(data_loader, directory, output_type)
           << std::endl
           << std::endl
           << WriteFatalOverdoseRates(data_loader, directory, output_type);
    return result.str();
}

std::string WriterImpl::WriteHistoryData(const History &history,
                                         const std::string &directory,
                                         const OutputType output_type) const {
    if (history.state_history.empty() || history.overdose_history.empty() ||
        history.fatal_overdose_history.empty() ||
        history.mortality_history.empty()) {
        // log error
        respond::utils::LogWarning(logger_name,
                                   "History Structure contains Empty Matrices! "
                                   "Unable to Write to File. Skipping...");
        return "failure";
    }
    std::vector<std::string> behaviors = config->getStringVector("state.ouds");
    std::vector<std::string> interventions =
        config->getStringVector("state.interventions");

    std::stringstream result;

    result << WriteTimedMatrix3dToOutput(history.state_history, interventions,
                                         behaviors, demographic_combinations,
                                         false, directory + "/stateHistory.csv",
                                         output_type)
           << std::endl
           << std::endl
           << WriteTimedMatrix3dToOutput(
                  history.overdose_history, interventions, behaviors,
                  demographic_combinations, false,
                  directory + "/overdoseHistory.csv", output_type)
           << std::endl
           << std::endl
           << WriteTimedMatrix3dToOutput(
                  history.fatal_overdose_history, interventions, behaviors,
                  demographic_combinations, false,
                  directory + "/fatalOverdoseHistory.csv", output_type)
           << std::endl
           << std::endl
           << WriteTimedMatrix3dToOutput(
                  history.mortality_history, interventions, behaviors,
                  demographic_combinations, false,
                  directory + "/mortalityHistory.csv", output_type)
           << std::endl
           << std::endl
           << WriteTimedMatrix3dToOutput(
                  history.intervention_admission_history, interventions,
                  behaviors, demographic_combinations, false,
                  directory + "/admissionsHistory.csv", output_type);

    return result.str();
}

std::string WriterImpl::WriteCostData(const CostList &cost_list,
                                      const std::string &directory,
                                      const OutputType output_type) const {
    if (cost_list.empty()) {
        respond::utils::LogWarning(
            logger_name,
            "Cost List is Empty! Unable to Write to File. Skipping...");
        return "failure";
    }
    std::vector<std::string> behaviors = config->getStringVector("state.ouds");
    size_t number_demographic_combos = demographic_combinations.size();
    std::vector<std::string> interventions =
        config->getStringVector("state.interventions");

    std::stringstream result;
    for (const Cost &cost : cost_list) {

        if (cost.healthcare_cost.empty() || cost.pharma_cost.empty() ||
            cost.fatal_overdose_cost.empty() ||
            cost.non_fatal_overdose_cost.empty() ||
            cost.treatment_cost.empty()) {
            respond::utils::LogWarning(
                logger_name, "Cost Contains Empty Matrices! Unable to "
                             "Write to File. Skipping...");
            return "failure";
        }

        result << WriteTimedMatrix3dToOutput(cost.healthcare_cost,
                                             interventions, behaviors,
                                             demographic_combinations, false,
                                             directory + "/healthcareCost-" +
                                                 cost.perspective + ".csv",
                                             output_type)
               << std::endl
               << std::endl
               << WriteTimedMatrix3dToOutput(
                      cost.pharma_cost, interventions, behaviors,
                      demographic_combinations, false,
                      directory + "/pharmaCost-" + cost.perspective + ".csv",
                      output_type)
               << std::endl
               << std::endl
               << WriteTimedMatrix3dToOutput(cost.fatal_overdose_cost,
                                             interventions, behaviors,
                                             demographic_combinations, false,
                                             directory + "/fatalOverdoseCost-" +
                                                 cost.perspective + ".csv",
                                             output_type)
               << std::endl
               << std::endl
               << WriteTimedMatrix3dToOutput(
                      cost.non_fatal_overdose_cost, interventions, behaviors,
                      demographic_combinations, false,
                      directory + "/nonFatalOverdoseCost-" + cost.perspective +
                          ".csv",
                      output_type)
               << std::endl
               << std::endl
               << WriteTimedMatrix3dToOutput(
                      cost.treatment_cost, interventions, behaviors,
                      demographic_combinations, false,
                      directory + "/treatmentCost-" + cost.perspective + ".csv",
                      output_type);
    }
    return result.str();
}

std::string WriterImpl::WriteUtilityData(const TimedMatrix3d &utilities,
                                         const std::string &directory,
                                         const OutputType output_type) const {
    if (utilities.empty()) {
        respond::utils::LogWarning(logger_name,
                                   "Utilities Matrix is Empty! Unable to "
                                   "Write to File. Skipping...");
        return "failure";
    }
    std::stringstream result;

    std::vector<std::string> behaviors = config->getStringVector("state.ouds");
    size_t number_demographic_combos = demographic_combinations.size();
    std::vector<std::string> interventions =
        config->getStringVector("state.interventions");

    result << WriteTimedMatrix3dToOutput(
        utilities, interventions, behaviors, demographic_combinations, false,
        directory + "/utilities.csv", output_type);
    return result.str();
}

std::string WriterImpl::WriteTotalsData(const Totals &totals,
                                        const std::string &directory,
                                        const OutputType outputType) const {
    if (totals.base_costs.empty() || totals.disc_costs.empty()) {
        respond::utils::LogWarning(logger_name,
                                   "Totals Matrices are Empty! Unable to "
                                   "Write to File. Skipping...");
        return "failure";
    }

    std::stringstream stream;

    stream << "Name,Base,Discount" << std::endl;
    for (int i = 0; i < totals.base_costs.size(); ++i) {
        stream << "Perspective " << i << "," << totals.base_costs[i] << ","
               << totals.disc_costs[i] << std::endl;
    }
    stream << "Life Years," << totals.base_life_years << ","
           << totals.disc_life_years << std::endl;
    stream << "Utility," << totals.base_utility << "," << totals.disc_utility
           << std::endl;
    return WriteContents(stream, directory + "/totals.csv", outputType);
}

std::string WriterImpl::WriteTimedMatrix3d(
    const TimedMatrix3d &matrices,
    const std::vector<std::string> &interventions,
    const std::vector<std::string> &behaviors,
    const std::vector<std::string> &demographic_combinations,
    bool pivot) const {
    std::stringstream stream;
    for (int i = 0; i < interventions.size(); ++i) {
        for (int b = 0; b < behaviors.size(); ++b) {
            for (int d = 0; d < demographic_combinations.size(); ++d) {
                std::string current_demographic = demographic_combinations[d];
                current_demographic = std::regex_replace(
                    current_demographic, std::regex("^ +| +$|( ) +"), "$1");
                std::replace(current_demographic.begin(),
                             current_demographic.end(), ' ', ',');
                if (!pivot) {
                    stream << interventions[i] << "," << behaviors[b] << ","
                           << current_demographic << ",";
                }

                for (const auto &kv : matrices) {
                    std::array<long int, 3> index = {0, 0, 0};
                    index[static_cast<int>(Dimension::kIntervention)] = i;
                    index[static_cast<int>(Dimension::kOud)] = b;
                    index[static_cast<int>(Dimension::kDemographicCombo)] = d;
                    double value = kv.second(index[0], index[1], index[2]);
                    if (pivot) {
                        stream << interventions[i] << "," << behaviors[b] << ","
                               << current_demographic << ","
                               << std::to_string(kv.first) << ","
                               << std::to_string(value) << std::endl;
                    } else {
                        stream << std::to_string(value) << ",";
                    }
                }
                if (!pivot) {
                    Newline(stream);
                }
            }
        }
    }
    return stream.str();
}

std::string WriterImpl::WriteTimedMatrix3dToOutput(
    const TimedMatrix3d &matrices,
    const std::vector<std::string> &interventions,
    const std::vector<std::string> &behaviors,
    const std::vector<std::string> &demographic_combinations, bool pivot,
    const std::string &path, OutputType output_type) const {
    std::stringstream stream;
    std::vector<int> timesteps;
    for (const auto &kv : matrices) {
        timesteps.push_back(kv.first);
    }
    stream << WriteColumnHeaders(timesteps, pivot) << std::endl
           << WriteTimedMatrix3d(matrices, interventions, behaviors,
                                 demographic_combinations, pivot);
    WriteContents(stream, path, output_type);
    return stream.str();
}

std::string
WriterImpl::WriteOUDTransitionRates(const DataLoader &data_loader,
                                    const std::string &directory,
                                    const OutputType output_type) const {
    std::stringstream stream;
    stream << "intervention,agegrp,race,sex,initial_oud,";

    std::vector<std::string> behaviors = config->getStringVector("state.ouds");
    std::vector<std::string> interventions =
        config->getStringVector("state.interventions");

    for (const std::string &behavior : behaviors) {
        stream << behavior << ",";
    }
    Newline(stream);

    for (int b = 0; b < behaviors.size(); ++b) {
        for (int i = 0; i < interventions.size(); ++i) {
            for (int d = 0; d < demographic_combinations.size(); ++d) {
                std::string temp = demographic_combinations[d];
                temp =
                    std::regex_replace(temp, std::regex("^ +| +$|( ) +"), "$1");
                std::replace(temp.begin(), temp.end(), ' ', ',');
                stream << interventions[i] << "," << temp << "," << behaviors[b]
                       << ",";
                for (int h = 0; h < behaviors.size(); ++h) {
                    std::array<long int, 3> index = {0, 0, 0};
                    index[static_cast<int>(Dimension::kIntervention)] = i;
                    index[static_cast<int>(Dimension::kOud)] =
                        (b * behaviors.size()) + h;
                    index[static_cast<int>(Dimension::kDemographicCombo)] = d;
                    double value = data_loader.GetOUDTransitionRates()(
                        index[0], index[1], index[2]);
                    stream << std::to_string(value) << ",";
                }
                Newline(stream);
            }
        }
    }
    return WriteContents(stream, directory + "/oud_trans.csv", output_type);
}

std::string
WriterImpl::WriteInterventionInitRates(const DataLoader &data_loader,
                                       const std::string &directory,
                                       const OutputType output_type) const {
    std::vector<std::string> behaviors = config->getStringVector("state.ouds");
    std::vector<std::string> interventions =
        config->getStringVector("state.interventions");

    std::stringstream stream;
    stream << "initial_oud_state,to_intervention,";
    for (std::string behavior : behaviors) {
        stream << behavior << ",";
    }
    Newline(stream);

    for (int b = 0; b < behaviors.size(); ++b) {
        for (int i = 0; i < interventions.size(); ++i) {
            stream << behaviors[b] << "," << interventions[i] << ",";
            for (int h = 0; h < behaviors.size(); ++h) {
                std::array<long int, 3> index = {0, 0, 0};
                index[static_cast<int>(Dimension::kIntervention)] = i;
                index[static_cast<int>(Dimension::kOud)] =
                    (b * behaviors.size()) + h;
                index[static_cast<int>(Dimension::kDemographicCombo)] = 0;
                double value = data_loader.GetInterventionInitRates()(
                    index[0], index[1], index[2]);
                stream << std::to_string(value) << ",";
            }
            Newline(stream);
        }
    }
    return WriteContents(stream, directory + "/block_init_effect.csv",
                         output_type);
}

std::string WriterImpl::WriteInterventionTransitionRates(
    const DataLoader &data_loader, const std::string &directory,
    const OutputType output_type) const {

    std::stringstream stream;

    stream << "agegrp,race,sex,oud,initial_intervention,";

    std::vector<int> timesteps =
        config->getIntVector("simulation.intervention_change_times");
    std::vector<std::string> behaviors = config->getStringVector("state.ouds");
    std::vector<std::string> interventions =
        config->getStringVector("state.interventions");

    int t_1 = 1;
    for (const int &t_2 : timesteps) {
        for (const std::string &col : interventions) {
            stream << col << "_" << std::to_string(t_1) << "_"
                   << std::to_string(t_2) << ",";
        }
        t_1 = t_2;
    }
    Newline(stream);

    // add zero index and pop off final change time to get matrix
    // indices
    timesteps.insert(timesteps.begin(), 0);
    timesteps.pop_back();

    for (long int d = 0; d < demographic_combinations.size(); d++) {
        for (int b = 0; b < behaviors.size(); b++) {
            for (int i = 0; i < interventions.size(); i++) {
                std::string temp = demographic_combinations[d];
                temp =
                    std::regex_replace(temp, std::regex("^ +| +$|( ) +"), "$1");
                std::replace(temp.begin(), temp.end(), ' ', ',');
                stream << temp << "," << behaviors[b] << "," << interventions[i]
                       << ",";
                for (int timestep : timesteps) {
                    for (int res = 0; res < interventions.size(); res++) {
                        std::array<long int, 3> index = {0, 0, 0};
                        index[static_cast<int>(Dimension::kIntervention)] =
                            (i * interventions.size()) + res;
                        index[static_cast<int>(Dimension::kOud)] = b;
                        index[static_cast<int>(Dimension::kDemographicCombo)] =
                            d;
                        double value =
                            data_loader.GetInterventionTransitionRates(
                                timestep)(index[0], index[1], index[2]);
                        stream << std::to_string(value) << ",";
                    }
                }
                Newline(stream);
            }
        }
    }
    return WriteContents(stream, directory + "/block_trans.csv", output_type);
}

std::string WriterImpl::WriteOverdoseRates(const DataLoader &data_loader,
                                           const std::string &directory,
                                           const OutputType output_type) const {
    std::stringstream stream;
    stream << "intervention,agegrp,race,sex,oud,";

    std::vector<std::string> behaviors = config->getStringVector("state.ouds");
    std::vector<std::string> interventions =
        config->getStringVector("state.interventions");

    std::vector<int> timesteps =
        config->getIntVector("simulation.overdose_change_times");
    int t_1 = 1;
    for (int t_2 : timesteps) {
        stream << "overdose_" << std::to_string(t_1) << "_"
               << std::to_string(t_2) << ",";
        t_1 = t_2;
    }

    Newline(stream);

    // add zero index and pop off final change time to get matrix
    // indices
    timesteps.insert(timesteps.begin(), 0);
    timesteps.pop_back();
    for (int i = 0; i < interventions.size(); i++) {
        for (long int d = 0; d < demographic_combinations.size(); d++) {
            for (int b = 0; b < behaviors.size(); b++) {
                std::string temp = demographic_combinations[d];
                temp =
                    std::regex_replace(temp, std::regex("^ +| +$|( ) +"), "$1");
                std::replace(temp.begin(), temp.end(), ' ', ',');

                stream << interventions[i] << "," << temp << "," << behaviors[b]
                       << ",";
                for (int timestep : timesteps) {
                    std::array<long int, 3> index = {0, 0, 0};
                    index[static_cast<int>(Dimension::kIntervention)] = i;
                    index[static_cast<int>(Dimension::kOud)] = b;
                    index[static_cast<int>(Dimension::kDemographicCombo)] = d;
                    double value = data_loader.GetOverdoseRates(timestep)(
                        index[0], index[1], index[2]);
                    stream << std::to_string(value) << ",";
                }
                Newline(stream);
            }
        }
    }
    return WriteContents(stream, directory + "/all_types_overdose.csv",
                         output_type);
}

std::string
WriterImpl::WriteFatalOverdoseRates(const DataLoader &data_loader,
                                    const std::string &directory,
                                    const OutputType output_type) const {
    std::stringstream stream;
    stream << "agegrp,race,sex,";

    std::vector<int> timesteps =
        config->getIntVector("simulation.overdose_change_times");
    int t_1 = 1;
    for (int t_2 : timesteps) {
        std::string c = "percent_overdoses_fatal_" + std::to_string(t_1) + "_" +
                        std::to_string(t_2);
        stream << c << ",";
        t_1 = t_2;
    }
    Newline(stream);

    // add zero index and pop off final change time to get matrix
    // indices
    timesteps.insert(timesteps.begin(), 0);
    timesteps.pop_back();
    for (long int d = 0; d < demographic_combinations.size(); d++) {
        std::string temp = demographic_combinations[d];
        temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"), "$1");
        std::replace(temp.begin(), temp.end(), ' ', ',');
        stream << temp << ",";
        for (int timestep : timesteps) {
            std::array<long int, 3> index = {0, 0, 0};
            index[static_cast<int>(Dimension::kDemographicCombo)] = d;
            double value = data_loader.GetFatalOverdoseRates(timestep)(
                index[0], index[1], index[2]);
            stream << std::to_string(value) << ",";
        }
        Newline(stream);
    }
    return WriteContents(stream, directory + "/fatal_overdose.csv",
                         output_type);
}

std::string WriterImpl::WriteContents(std::stringstream &stream,
                                      const std::string &path,
                                      OutputType output_type) const {
    if (output_type == OutputType::kFile) {
        std::filesystem::path p = (path.empty()) ? "temp.csv" : path;
        try {
            std::ofstream file(p.string());
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

std::string WriterImpl::WriteColumnHeaders(const std::vector<int> &timesteps,
                                           bool pivot) const {
    std::vector<std::string> demographics =
        config->getSectionCategories("demographic");
    std::stringstream ret;
    ret << "Interventions,OUD States,";
    for (std::string demographic : demographics) {
        ret << demographic << ",";
    }
    if (pivot) {
        ret << "time,value";
    } else {
        for (int timestep : timesteps) {
            ret << "t+" << std::to_string(timestep) << ",";
        }
    }
    std::string val = ret.str();
    if (!val.empty() && val.back() == ',') {
        val.pop_back(); // Remove the trailing comma
    }

    return val;
}

std::unique_ptr<Writer> Writer::Create(const Data::IConfigablePtr &cfg,
                                       const std::string &log_name) {
    return std::make_unique<WriterImpl>(cfg, log_name);
}
} // namespace respond::data_ops
