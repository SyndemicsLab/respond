////////////////////////////////////////////////////////////////////////////////
// File: Writer.cpp                                                           //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-17                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-24                                                  //
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
               << " "
               << WriteInterventionTransitionRates(data_loader, directory,
                                                   output_type)
               << " "
               << WriteInterventionInitRates(data_loader, directory,
                                             output_type)
               << " " << WriteOverdoseRates(data_loader, directory, output_type)
               << " "
               << WriteFatalOverdoseRates(data_loader, directory, output_type);
        return result.str();
    }

    std::string
    WriterImpl::WriteHistoryData(const History &history,
                                 const std::string &directory,
                                 const OutputType output_type) const {
        if (history.state_history.empty() || history.overdose_history.empty() ||
            history.fatal_overdose_history.empty() ||
            history.mortality_history.empty()) {
            // log error
            respond::utils::LogWarning(
                logger_name, "History Structure contains Empty Matrices! "
                             "Unable to Write to File. Skipping...");
            return "failure";
        }
        std::vector<std::string> behaviors =
            config->getStringVector("state.ouds");
        size_t number_demographic_combos = demographic_combinations.size();
        std::vector<std::string> interventions =
            config->getStringVector("state.interventions");

        std::stringstream result;

        result << WriteTimedMatrix3dToFile(history.state_history, interventions,
                                           behaviors, demographic_combinations,
                                           false,
                                           directory + "/stateHistory.csv")
               << " "
               << WriteTimedMatrix3dToFile(history.overdose_history,
                                           interventions, behaviors,
                                           demographic_combinations, false,
                                           directory + "/overdoseHistory.csv")
               << " "
               << WriteTimedMatrix3dToFile(
                      history.fatal_overdose_history, interventions, behaviors,
                      demographic_combinations, false,
                      directory + "/fatalOverdoseHistory.csv")
               << " "
               << WriteTimedMatrix3dToFile(history.mortality_history,
                                           interventions, behaviors,
                                           demographic_combinations, false,
                                           directory + "/mortalityHistory.csv")
               << " "
               << WriteTimedMatrix3dToFile(
                      history.intervention_admission_history, interventions,
                      behaviors, demographic_combinations, false,
                      directory + "/admissionsHistory.csv");

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
        std::vector<std::string> behaviors =
            config->getStringVector("state.ouds");
        size_t number_demographic_combos = demographic_combinations.size();
        std::vector<std::string> interventions =
            config->getStringVector("state.interventions");

        std::stringstream result;
        for (Cost cost : cost_list) {

            if (cost.healthcare_cost.empty() || cost.pharma_cost.empty() ||
                cost.fatal_overdose_cost.empty() ||
                cost.non_fatal_overdose_cost.empty() ||
                cost.treatment_cost.empty()) {
                respond::utils::LogWarning(
                    logger_name, "Cost Contains Empty Matrices! Unable to "
                                 "Write to File. Skipping...");
                return "failure";
            }

            result << WriteTimedMatrix3dToFile(cost.healthcare_cost,
                                               interventions, behaviors,
                                               demographic_combinations, false,
                                               directory + "/healthcareCost-" +
                                                   cost.perspective + ".csv")
                   << " "
                   << WriteTimedMatrix3dToFile(cost.pharma_cost, interventions,
                                               behaviors,
                                               demographic_combinations, false,
                                               directory + "/pharmaCost-" +
                                                   cost.perspective + ".csv")
                   << " "
                   << WriteTimedMatrix3dToFile(
                          cost.fatal_overdose_cost, interventions, behaviors,
                          demographic_combinations, false,
                          directory + "/fatalOverdoseCost-" + cost.perspective +
                              ".csv")
                   << " "
                   << WriteTimedMatrix3dToFile(
                          cost.non_fatal_overdose_cost, interventions,
                          behaviors, demographic_combinations, false,
                          directory + "/nonFatalOverdoseCost-" +
                              cost.perspective + ".csv")
                   << " "
                   << WriteTimedMatrix3dToFile(cost.treatment_cost,
                                               interventions, behaviors,
                                               demographic_combinations, false,
                                               directory + "/treatmentCost-" +
                                                   cost.perspective + ".csv");
        }
        return result.str();
    }

    std::string
    WriterImpl::WriteUtilityData(const TimedMatrix3d &utilities,
                                 const std::string &directory,
                                 const OutputType output_type) const {
        if (utilities.empty()) {
            respond::utils::LogWarning(logger_name,
                                       "Utilities Matrix is Empty! Unable to "
                                       "Write to File. Skipping...");
            return "failure";
        }
        std::stringstream result;

        std::vector<std::string> behaviors =
            config->getStringVector("state.ouds");
        size_t number_demographic_combos = demographic_combinations.size();
        std::vector<std::string> interventions =
            config->getStringVector("state.interventions");

        result << WriteTimedMatrix3dToFile(utilities, interventions, behaviors,
                                           demographic_combinations, false,
                                           directory + "/utilities.csv");
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

        stream << "Name, Base, Discount" << std::endl;
        for (int i = 0; i < totals.base_costs.size(); ++i) {
            stream << "Perspective " << i << ", " << totals.base_costs[i]
                   << ", " << totals.disc_costs[i] << std::endl;
        }
        stream << "Life Years, " << totals.base_life_years << ", "
               << totals.disc_life_years << std::endl;
        stream << "Utility, " << totals.base_utility << ", "
               << totals.disc_utility << std::endl;
        return WriteContents(stream, directory + "/totals.csv",
                             OutputType::kFile);
    }

    std::string WriterImpl::WriteTimedMatrix3d(
        const TimedMatrix3d &matrices,
        const std::vector<std::string> &interventions,
        const std::vector<std::string> &behaviors,
        const std::vector<std::string> &demographic_combinations,
        bool pivot) const {
        std::stringstream stream;
        for (long int i = 0; i < interventions.size(); i++) {
            for (long int b = 0; b < behaviors.size(); b++) {
                for (long int d = 0; d < demographic_combinations.size(); d++) {
                    std::string current_demographic =
                        demographic_combinations[d];
                    current_demographic = std::regex_replace(
                        current_demographic, std::regex("^ +| +$|( ) +"), "$1");
                    std::replace(current_demographic.begin(),
                                 current_demographic.end(), ' ', ',');
                    if (!pivot) {
                        stream << interventions[i] << "," << behaviors[b] << ","
                               << current_demographic << ",";
                    }

                    for (auto kv : matrices) {
                        std::array<long int, 3> index = {0, 0, 0};
                        index[static_cast<int>(Dimension::kIntervention)] = i;
                        index[static_cast<int>(Dimension::kOud)] = b;
                        index[static_cast<int>(Dimension::kDemographicCombo)] =
                            d;
                        double value = kv.second(index[0], index[1], index[2]);
                        if (pivot) {
                            stream << interventions[i] << "," << behaviors[b]
                                   << "," << current_demographic << ","
                                   << std::to_string(kv.first) << ","
                                   << std::to_string(value) << std::endl;
                        } else {
                            stream << std::to_string(value) << ",";
                        }
                    }
                    if (!pivot) {
                        stream << std::endl;
                    }
                }
            }
        }
        return stream.str();
    }

    std::string WriterImpl::WriteTimedMatrix3dToFile(
        const TimedMatrix3d &matrices,
        const std::vector<std::string> &interventions,
        const std::vector<std::string> &behaviors,
        const std::vector<std::string> &demographic_combinations, bool pivot,
        const std::string &path) const {
        std::stringstream stream;
        std::vector<int> timesteps;
        for (auto kv : matrices) {
            timesteps.push_back(kv.first);
        }
        stream << WriteColumnHeaders(timesteps, pivot)
               << WriteTimedMatrix3d(matrices, interventions, behaviors,
                                     demographic_combinations, pivot);
        WriteContents(stream, path, OutputType::kFile);
        return stream.str();
    }

    std::string
    WriterImpl::WriteOUDTransitionRates(const DataLoader &data_loader,
                                        const std::string &directory,
                                        const OutputType output_type) const {
        std::stringstream stream;
        stream << "intervention,agegrp,race,sex,initial_oud,";

        std::vector<std::string> behaviors =
            config->getStringVector("state.ouds");
        std::vector<std::string> interventions =
            config->getStringVector("state.interventions");

        for (const std::string &behavior : behaviors) {
            stream << behavior << (&behavior != &behaviors.back()) ? "," : "\n";
        }

        for (long int b = 0; b < behaviors.size(); ++b) {
            for (long int i = 0; i < interventions.size(); ++i) {
                for (long int d = 0; d < demographic_combinations.size(); ++d) {
                    std::string temp = demographic_combinations[d];
                    temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"),
                                              "$1");
                    std::replace(temp.begin(), temp.end(), ' ', ',');
                    stream << interventions[i] << "," << temp << ","
                           << behaviors[b] << ",";
                    for (long int h = 0; h < behaviors.size(); h++) {
                        std::array<long int, 3> index = {0, 0, 0};
                        index[static_cast<int>(Dimension::kIntervention)] = i;
                        index[static_cast<int>(Dimension::kOud)] =
                            (b * behaviors.size()) + h;
                        index[static_cast<int>(Dimension::kDemographicCombo)] =
                            d;
                        stream << std::to_string(
                                      data_loader.GetOUDTransitionRates()(
                                          index[0], index[1], index[2]))
                               << ",";
                    }
                    stream << std::endl;
                }
            }
        }
        return WriteContents(stream, directory + "/oud_trans.csv", output_type);
    }

    std::string
    WriterImpl::WriteInterventionInitRates(const DataLoader &data_loader,
                                           const std::string &directory,
                                           const OutputType output_type) const {
        std::vector<std::string> behaviors =
            config->getStringVector("state.ouds");
        std::vector<std::string> interventions =
            config->getStringVector("state.interventions");

        std::stringstream stream;
        stream << "initial_oud_state,to_intervention";
        for (std::string behavior : behaviors) {
            stream << behavior << (&behavior != &behaviors.back()) ? "," : "\n";
        }

        for (int b = 0; b < behaviors.size(); ++b) {
            for (int i = 0; i < interventions.size(); ++i) {
                stream << behaviors[b] << "," << interventions[i] << ",";
                for (int h = 0; h < behaviors.size(); ++h) {
                    std::array<long int, 3> index = {0, 0, 0};
                    index[static_cast<int>(Dimension::kIntervention)] = i;
                    index[static_cast<int>(Dimension::kOud)] =
                        (b * behaviors.size()) + h;
                    index[static_cast<int>(Dimension::kDemographicCombo)] = 0;
                    stream << std::to_string(
                                  data_loader.GetInterventionInitRates()(
                                      index[0], index[1], index[2]))
                           << ",";
                }
                stream << std::endl;
            }
        }
        return WriteContents(stream, directory + "/block_init_effect.csv",
                             output_type);
    }

    std::string WriterImpl::WriteInterventionTransitionRates(
        const DataLoader &data_loader, const std::string &directory,
        const OutputType output_type) const {

        std::stringstream stream;

        stream << "agegrp,race,sex,oud,initial_intervention";

        std::vector<int> timesteps =
            config->getIntVector("simulation.intervention_change_times");
        std::vector<std::string> behaviors =
            config->getStringVector("state.ouds");
        std::vector<std::string> interventions =
            config->getStringVector("state.interventions");

        int t_1 = 1;
        for (const int &t_2 : timesteps) {
            for (const std::string &col : interventions) {
                std::string c =
                    col + "_" + std::to_string(t_1) + "_" + std::to_string(t_2);
                stream << c
                       << ((&t_2 != &timesteps.back() &&
                            &col != &interventions.back())
                               ? ","
                               : "\n");
            }
            t_1 = t_2;
        }

        // add zero index and pop off final change time to get matrix
        // indices
        timesteps.insert(timesteps.begin(), 0);
        timesteps.pop_back();

        for (long int d = 0; d < demographic_combinations.size(); d++) {
            for (int b = 0; b < behaviors.size(); b++) {
                for (int i = 0; i < interventions.size(); i++) {
                    std::string temp = demographic_combinations[d];
                    temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"),
                                              "$1");
                    std::replace(temp.begin(), temp.end(), ' ', ',');
                    stream << temp << "," << behaviors[b] << ","
                           << interventions[i] << ",";
                    for (int timestep : timesteps) {
                        for (int res = 0; res < interventions.size(); res++) {
                            std::array<long int, 3> index = {0, 0, 0};
                            index[static_cast<int>(Dimension::kIntervention)] =
                                (i * interventions.size()) + res;
                            index[static_cast<int>(Dimension::kOud)] = b;
                            index[static_cast<int>(
                                Dimension::kDemographicCombo)] = d;
                            stream << std::to_string(
                                          data_loader
                                              .GetInterventionTransitionRates(
                                                  timestep)(index[0], index[1],
                                                            index[2]))
                                   << ",";
                        }
                    }
                    stream << std::endl;
                }
            }
        }
        return WriteContents(stream, directory + "/block_trans.csv",
                             output_type);
    }

    std::string
    WriterImpl::WriteOverdoseRates(const DataLoader &data_loader,
                                   const std::string &directory,
                                   const OutputType output_type) const {
        std::stringstream stream;
        stream << "intervention,agegrp,race,sex,oud";

        std::vector<std::string> behaviors =
            config->getStringVector("state.ouds");
        std::vector<std::string> interventions =
            config->getStringVector("state.interventions");

        std::vector<int> timesteps =
            config->getIntVector("simulation.overdose_change_times");
        int t_1 = 1;
        for (int t_2 : timesteps) {
            std::string c =
                "overdose_" + std::to_string(t_1) + "_" + std::to_string(t_2);
            stream << c << ((&t_2 != &timesteps.back()) ? "," : "\n");
            t_1 = t_2;
        }

        // add zero index and pop off final change time to get matrix
        // indices
        timesteps.insert(timesteps.begin(), 0);
        timesteps.pop_back();
        for (int i = 0; i < interventions.size(); i++) {
            for (long int d = 0; d < demographic_combinations.size(); d++) {
                for (int b = 0; b < behaviors.size(); b++) {
                    std::string temp = demographic_combinations[d];
                    temp = std::regex_replace(temp, std::regex("^ +| +$|( ) +"),
                                              "$1");
                    std::replace(temp.begin(), temp.end(), ' ', ',');

                    stream << interventions[i] << "," << temp << ","
                           << behaviors[b] << ",";
                    for (int timestep : timesteps) {
                        std::array<long int, 3> index = {0, 0, 0};
                        index[static_cast<int>(Dimension::kIntervention)] = i;
                        index[static_cast<int>(Dimension::kOud)] = b;
                        index[static_cast<int>(Dimension::kDemographicCombo)] =
                            d;
                        stream << std::to_string(data_loader.GetOverdoseRates(
                                      timestep)(index[0], index[1], index[2]))
                               << ",";
                    }
                    stream << std::endl;
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
        stream << "agegrp,race,sex";

        std::vector<int> timesteps =
            config->getIntVector("simulation.overdose_change_times");
        int t_1 = 1;
        for (int t_2 : timesteps) {
            std::string c = "percent_overdoses_fatal_" + std::to_string(t_1) +
                            "_" + std::to_string(t_2);
            stream << c << ((&t_2 != &timesteps.back()) ? "," : "\n");
            t_1 = t_2;
        }

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
                stream << std::to_string(data_loader.GetFatalOverdoseRates(
                              timestep)(index[0], index[1], index[2]))
                       << ",";
            }
            stream << std::endl;
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

    std::string
    WriterImpl::WriteColumnHeaders(const std::vector<int> &timesteps,
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

        return ret.str();
    }

    std::unique_ptr<Writer> Writer::Create(const Data::IConfigablePtr &cfg,
                                           const std::string &log_name) {
        return std::make_unique<WriterImpl>(cfg, log_name);
    }
} // namespace respond::data_ops
