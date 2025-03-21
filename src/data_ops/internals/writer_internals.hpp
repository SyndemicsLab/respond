////////////////////////////////////////////////////////////////////////////////
// File: writer_internals.hpp                                                 //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-13                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-17                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_WRITERINTERNALS_HPP_
#define RESPOND_DATAOPS_WRITERINTERNALS_HPP_

#include <respond/data_ops/writer.hpp>

#include <algorithm>
#include <memory>
#include <sstream>
#include <string>
#include <vector>

#include <datamanagement/DataManagement.hpp>

#include <respond/data_ops/data_types.hpp>
#include <respond/utils/logging.hpp>

#include "data_loader_internals.hpp"

namespace respond::data_ops {
    class WriterImpl : public virtual Writer {
    public:
        WriterImpl(const Data::IConfigablePtr &cfg,
                   const std::string &log_name = "console")
            : config(cfg), logger_name(log_name) {}
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
        const Data::IConfigablePtr config;
        const std::string logger_name;
        std::vector<std::string> demographic_combinations;

        std::string WriteTimedMatrix3dToFile(
            const TimedMatrix3d &matrices,
            const std::vector<std::string> &interventions,
            const std::vector<std::string> &behaviors,
            const std::vector<std::string> &demographic_combinations,
            bool pivot, const std::string &path) const;

        std::string
        WriteContents(std::stringstream &stream, const std::string &filename,
                      OutputType output_type = OutputType::kString) const;

        std::string WriteTimedMatrix3d(
            const TimedMatrix3d &matrices,
            const std::vector<std::string> &interventions,
            const std::vector<std::string> &behaviors,
            const std::vector<std::string> &demographic_combinations,
            bool pivot) const;

        std::string WriteColumnHeaders(const std::vector<int> &timesteps,
                                       bool pivot) const;

        void BuildDemographicCombinations() {
            std::vector<std::string> demographics =
                config->getSectionCategories("demographic");
            std::vector<std::vector<std::string>> inputDemographicVals;
            for (std::string demographic : demographics) {
                inputDemographicVals.push_back(
                    config->getStringVector("demographic." + demographic));
            }
            std::vector<std::vector<std::string>> temp1;
            std::vector<std::string> temp2;
            RecursionHelper(temp1, temp2, inputDemographicVals.begin(),
                            inputDemographicVals.end());
            std::stringstream group;
            for (std::vector<std::string> strList : temp1) {
                group.clear();
                for (std::string st : strList) {
                    std::transform(
                        st.begin(), st.end(), st.begin(),
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

        std::string
        WriteInterventionInitRates(const DataLoader &data_loader,
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
} // namespace respond::data_ops

#endif // RESPOND_DATAOPS_WRITERINTERNALS_HPP_