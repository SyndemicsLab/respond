////////////////////////////////////////////////////////////////////////////////
// File: simulation_internals.hpp                                             //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_MODEL_SIMULATIONINTERNALS_HPP_
#define RESPOND_MODEL_SIMULATIONINTERNALS_HPP_

#include <respond/model/simulation.hpp>

namespace respond::model {
    class RespondImpl : public Respond {
    public:
        /// @brief
        RespondImpl();

        /// @brief
        /// @param dataLoader
        RespondImpl(std::shared_ptr<data_ops::IDataLoader> dataLoader);

        ~RespondImpl() {};

        void
        run(std::shared_ptr<data_ops::IDataLoader> const &dataloader = nullptr,
            std::shared_ptr<data_ops::ICostLoader> const &costloader = nullptr,
            std::shared_ptr<data_ops::IUtilityLoader> const &utilloader =
                nullptr) override;
        void ageUp() override;

        void
        setData(std::shared_ptr<data_ops::IDataLoader> const &loader) override {
            this->dataLoader = loader;
        }
        void
        setCost(std::shared_ptr<data_ops::ICostLoader> const &loader) override {
            this->costLoader = loader;
        }
        void setUtility(
            std::shared_ptr<data_ops::IUtilityLoader> const &loader) override {
            this->utilLoader = loader;
        }

        void setLogger(std::shared_ptr<spdlog::logger> const logger) override {
            this->logger = logger;
        }
        void setDuration(std::uint64_t const time) override {
            this->duration = time;
        }

        std::uint64_t getCurrentTime() const override {
            return this->currentTime;
        }
        std::uint64_t getDuration() const override { return this->duration; }

        data_ops::History getHistory() const override { return this->history; }
        data_ops::Matrix3d getState() const override { return this->state; }
        std::shared_ptr<data_ops::IDataLoader> getDataLoader() const override {
            return this->dataLoader;
        }
        std::shared_ptr<data_ops::ICostLoader> getCostLoader() const override {
            return this->costLoader;
        }
        std::shared_ptr<data_ops::IUtilityLoader>
        getUtilityLoader() const override {
            return this->utilLoader;
        }
        std::shared_ptr<spdlog::logger> getLogger() const override {
            return this->logger;
        }

    private:
        data_ops::Matrix3d state;
        std::uint64_t duration = 0;
        int currentTime = 0;
        data_ops::History history;
        std::shared_ptr<data_ops::IDataLoader> dataLoader = nullptr;
        std::shared_ptr<data_ops::ICostLoader> costLoader = nullptr;
        std::shared_ptr<data_ops::IUtilityLoader> utilLoader = nullptr;
        std::shared_ptr<spdlog::logger> logger = nullptr;

        data_ops::Matrix3d step();

        void setupHistory();

        data_ops::Matrix3d
        multUseAfterIntervention(data_ops::Matrix3d const mat,
                                 int const intervention_idx) const;

        data_ops::Matrix3d
        addEnteringSample(data_ops::Matrix3d const mat) const;

        data_ops::Matrix3d
        multBehaviorTransition(data_ops::Matrix3d const mat) const;

        data_ops::Matrix3d
        multInterventionTransition(data_ops::Matrix3d const mat) const;

        data_ops::Matrix3d multFODGivenOD(data_ops::Matrix3d const mat) const;

        data_ops::Matrix3d multOD(data_ops::Matrix3d const mat) const;

        data_ops::Matrix3d multMortality(data_ops::Matrix3d const mat) const;

        inline data_ops::Matrix3d createStandardMatrix3d() const {
            data_ops::Matrix3d mat = data_ops::Matrix3dFactory::Create(
                dataLoader->getNumOUDStates(),
                dataLoader->getNumInterventions(),
                dataLoader->getNumDemographicCombos());
            mat.setZero();
            return mat;
        }
    };
} // namespace respond::model

#endif // RESPOND_MODEL_SIMULATIONINTERNALS_HPP_