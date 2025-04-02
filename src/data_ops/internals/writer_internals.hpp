////////////////////////////////////////////////////////////////////////////////
// File: writer_internals.hpp                                                 //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-13                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-04-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_WRITERINTERNALS_HPP_
#define RESPOND_DATAOPS_WRITERINTERNALS_HPP_

#include <respond/data_ops/writer.hpp>

#include <algorithm>
#include <filesystem>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/data_types.hpp>
#include <respond/utils/logging.hpp>

#include "data_loader_internals.hpp"

namespace respond {
namespace data_ops {
class WriterImpl : public virtual Writer {
public:
    WriterImpl(const std::string &directory = "",
               const std::string &log_name = "console")
        : logger_name(log_name) {
        std::filesystem::path dir = directory;
        std::filesystem::path conf = dir / "sim.conf";
        if (!std::filesystem::exists(conf)) {
            respond::utils::LogError(
                log_name,
                "Unable to find a sim.conf file at the provided directory");
            throw std::invalid_argument("The Writer needs a sim.conf file.");
        }
        config = std::make_shared<Data::Config>(conf.string());
        BuildDemographicCombinations();
    }
    ~WriterImpl() = default;
    std::string WriteInputData(
        const DataLoader &data_loader, const std::string &directory = "",
        const OutputType output_type = OutputType::kString) const override;
    std::string WriteHistoryData(
        const History &history, const std::string &directory = "",
        const OutputType output_type = OutputType::kString) const override;
    std::string WriteCostData(
        const CostList &cost_list, const std::string &directory = "",
        const OutputType output_type = OutputType::kString) const override;
    std::string WriteUtilityData(
        const TimedMatrix3d &utilities, const std::string &directory = "",
        const OutputType output_type = OutputType::kString) const override;
    std::string WriteTotalsData(
        const Totals &totals, const std::string &directory = "",
        const OutputType output_type = OutputType::kString) const override;

private:
    Data::IConfigablePtr config;
    const std::string logger_name;
    std::vector<std::string> demographic_combinations;

    void Newline(std::stringstream &stream) const {
        std::string result = stream.str();
        if (!result.empty() && result.back() == ',') {
            result.pop_back(); // Remove the trailing comma
        }
        stream.str("");
        stream.clear(); // Clear the stream
        stream << result << std::endl;
    }

    std::string WriteTimedMatrix3dToOutput(
        const TimedMatrix3d &matrices,
        const std::vector<std::string> &interventions,
        const std::vector<std::string> &behaviors,
        const std::vector<std::string> &demographic_combinations, bool pivot,
        const std::string &path,
        OutputType output_type = OutputType::kString) const;

    std::string
    WriteContents(std::stringstream &stream, const std::string &filename,
                  OutputType output_type = OutputType::kString) const;

    std::string
    WriteTimedMatrix3d(const TimedMatrix3d &matrices,
                       const std::vector<std::string> &interventions,
                       const std::vector<std::string> &behaviors,
                       const std::vector<std::string> &demographic_combinations,
                       bool pivot) const;

    std::string WriteColumnHeaders(const std::vector<int> &timesteps,
                                   bool pivot) const;

    void BuildDemographicCombinations() {
        std::vector<std::string> demographics =
            config->getSectionCategories("demographic");
        std::vector<std::vector<std::string>> input_demographics;
        for (const auto &demographic : demographics) {
            input_demographics.push_back(
                config->getStringVector("demographic." + demographic));
        }
        std::vector<std::vector<std::string>> end_result;
        std::vector<std::string> running_total;
        RecursionHelper(end_result, running_total, input_demographics.begin(),
                        input_demographics.end());
        std::stringstream group;
        for (const auto &string_list : end_result) {
            group.clear();
            for (std::string st : string_list) {
                std::transform(st.begin(), st.end(), st.begin(),
                               [](unsigned char c) { return std::tolower(c); });
                group << " " << st;
            }
            demographic_combinations.push_back(group.str());
        }
    }

    void RecursionHelper(
        std::vector<std::vector<std::string>> &end_result,
        std::vector<std::string> &running_total,
        std::vector<std::vector<std::string>>::const_iterator current,
        std::vector<std::vector<std::string>>::const_iterator final) {
        if (current == final) {
            end_result.push_back(running_total);
            return;
        }
        const std::vector<std::string> &temp = *current;
        for (std::vector<std::string>::const_iterator it = temp.begin();
             it != temp.end(); it++) {
            running_total.push_back(*it);
            RecursionHelper(end_result, running_total, current + 1, final);
            running_total.pop_back();
        }
    }

    std::string WriteOUDTransitionRates(const DataLoader &data_loader,
                                        const std::string &directory,
                                        const OutputType output_type) const;

    std::string WriteInterventionInitRates(const DataLoader &data_loader,
                                           const std::string &directory,
                                           const OutputType output_type) const;

    std::string
    WriteInterventionTransitionRates(const DataLoader &data_loader,
                                     const std::string &directory,
                                     const OutputType output_type) const;

    std::string WriteOverdoseRates(const DataLoader &data_loader,
                                   const std::string &directory,
                                   const OutputType output_type) const;

    std::string WriteFatalOverdoseRates(const DataLoader &data_loader,
                                        const std::string &directory,
                                        const OutputType output_type) const;
};
} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_WRITERINTERNALS_HPP_