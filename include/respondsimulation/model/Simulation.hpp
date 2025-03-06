////////////////////////////////////////////////////////////////////////////////
// File: Simulation.hpp                                                       //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef MODEL_SIMULATION_HPP_
#define MODEL_SIMULATION_HPP_

#include <respondsimulation/data_ops/CostLoader.hpp>
#include <respondsimulation/data_ops/DataLoader.hpp>
#include <respondsimulation/data_ops/Matrix3dFactory.hpp>
#include <respondsimulation/data_ops/UtilityLoader.hpp>

#include <any>
#include <memory>

/// @brief Namespace defining all Simulation Operations
namespace Simulation {
    class IRunnable {
    public:
        virtual void
        run(std::shared_ptr<Matrixify::IDataLoader> const &dataloader,
            std::shared_ptr<Matrixify::ICostLoader> const &costloader,
            std::shared_ptr<Matrixify::IUtilityLoader> const &utilloader) = 0;
        virtual std::uint64_t getCurrentTime() const = 0;
        virtual void setDuration(std::uint64_t const time) = 0;
        virtual std::uint64_t getDuration() const = 0;
    };

    class ILoadable {
    public:
        virtual void
        setData(std::shared_ptr<Matrixify::IDataLoader> const &loader) = 0;
        virtual void
        setCost(std::shared_ptr<Matrixify::ICostLoader> const &loader) = 0;
        virtual void setUtility(
            std::shared_ptr<Matrixify::IUtilityLoader> const &loader) = 0;
    };

    /// @brief Interface for all Simulations
    class IRespond : public virtual IRunnable,
                     public virtual ILoadable,
                     public virtual Matrixify::ILoggable {
    public:
        virtual Matrixify::Matrix3d getState() const = 0;
        virtual std::shared_ptr<Matrixify::IDataLoader>
        getDataLoader() const = 0;
        virtual std::shared_ptr<Matrixify::ICostLoader>
        getCostLoader() const = 0;
        virtual std::shared_ptr<Matrixify::IUtilityLoader>
        getUtilityLoader() const = 0;
        virtual void ageUp() = 0;
        virtual Matrixify::History getHistory() const = 0;
    };

    /*! Concrete Class for Respond implementing the IRespond interface
     *  @image html png/RESPOND-StateMatrix.png
     *  @image latex pdf/RESPOND-StateMatrix.pdf "Multiplication" width=10cm
     */
    class Respond : public IRespond {
    public:
        /// @brief
        Respond() : Respond(nullptr) {}

        /// @brief
        /// @param dataLoader
        Respond(std::shared_ptr<Matrixify::IDataLoader> dataLoader);

        ~Respond() {};

        void
        run(std::shared_ptr<Matrixify::IDataLoader> const &dataloader = nullptr,
            std::shared_ptr<Matrixify::ICostLoader> const &costloader = nullptr,
            std::shared_ptr<Matrixify::IUtilityLoader> const &utilloader =
                nullptr) override;
        void ageUp() override;

        void setData(
            std::shared_ptr<Matrixify::IDataLoader> const &loader) override {
            this->dataLoader = loader;
        }
        void setCost(
            std::shared_ptr<Matrixify::ICostLoader> const &loader) override {
            this->costLoader = loader;
        }
        void setUtility(
            std::shared_ptr<Matrixify::IUtilityLoader> const &loader) override {
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

        Matrixify::History getHistory() const override { return this->history; }
        Matrixify::Matrix3d getState() const override { return this->state; }
        std::shared_ptr<Matrixify::IDataLoader> getDataLoader() const override {
            return this->dataLoader;
        }
        std::shared_ptr<Matrixify::ICostLoader> getCostLoader() const override {
            return this->costLoader;
        }
        std::shared_ptr<Matrixify::IUtilityLoader>
        getUtilityLoader() const override {
            return this->utilLoader;
        }
        std::shared_ptr<spdlog::logger> getLogger() const override {
            return this->logger;
        }

    private:
        Matrixify::Matrix3d state;
        std::uint64_t duration = 0;
        int currentTime = 0;
        Matrixify::History history;
        std::shared_ptr<Matrixify::IDataLoader> dataLoader = nullptr;
        std::shared_ptr<Matrixify::ICostLoader> costLoader = nullptr;
        std::shared_ptr<Matrixify::IUtilityLoader> utilLoader = nullptr;
        std::shared_ptr<spdlog::logger> logger = nullptr;

        Matrixify::Matrix3d step();

        void setupHistory();

        Matrixify::Matrix3d
        multUseAfterIntervention(Matrixify::Matrix3d const mat,
                                 int const intervention_idx) const;

        Matrixify::Matrix3d
        addEnteringSample(Matrixify::Matrix3d const mat) const;

        Matrixify::Matrix3d
        multBehaviorTransition(Matrixify::Matrix3d const mat) const;

        Matrixify::Matrix3d
        multInterventionTransition(Matrixify::Matrix3d const mat) const;

        Matrixify::Matrix3d multFODGivenOD(Matrixify::Matrix3d const mat) const;

        Matrixify::Matrix3d multOD(Matrixify::Matrix3d const mat) const;

        Matrixify::Matrix3d multMortality(Matrixify::Matrix3d const mat) const;

        inline Matrixify::Matrix3d createStandardMatrix3d() const {
            Matrixify::Matrix3d mat = Matrixify::Matrix3dFactory::Create(
                dataLoader->getNumOUDStates(),
                dataLoader->getNumInterventions(),
                dataLoader->getNumDemographicCombos());
            mat.setZero();
            return mat;
        }
    };
} // namespace Simulation
#endif // MODEL_SIMULATION_HPP_
