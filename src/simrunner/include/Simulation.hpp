//===-- Simulation.hpp - Simulation class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Simulation class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef MODEL_SIMULATION_HPP_
#define MODEL_SIMULATION_HPP_

#include "DataLoader.hpp"
#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"

/// @brief Namespace defining all Simulation Operations
namespace Simulation {
    /// @brief Interface for all Simulations
    class ISim {
    public:
        virtual ~ISim(){};

        /// @brief Load the Initial Sample
        /// @param initialSample Matrix3d of InitialSample
        virtual void
        loadInitialSample(Matrixify::Matrix3d const &initialSample) = 0;

        /// @brief Load the Entering Samples
        /// @param enteringSamples Matrix4d of Entering Samples
        virtual void
        loadEnteringSamples(Matrixify::Matrix4d const &enteringSamples) = 0;

        /// @brief Load the OUD Transition Rates
        /// @param oudTransitionRates Matrix3d of OUD Transition Rates
        virtual void loadOUDTransitionRates(
            Matrixify::Matrix3d const &oudTransitionRates) = 0;

        /// @brief Load the Intervention Initialization Rates
        /// @param interventionInitRates Matrix3d of Intervention Initialization
        /// Rates
        virtual void loadInterventionInitRates(
            Matrixify::Matrix3d const &interventionInitRates) = 0;

        /// @brief Load the Intervention Transition Rates
        /// @param interventionTransitionRates Matrix4d of Intervention
        /// Transition Rates
        virtual void loadInterventionTransitionRates(
            Matrixify::Matrix4d const &interventionTransitionRates) = 0;

        /// @brief Load the Overdose Rates
        /// @param overdoseRates Matrix4d of Overdose Rates
        virtual void
        loadOverdoseRates(Matrixify::Matrix4d const &overdoseRates) = 0;

        /// @brief Load the Fatal Overdose Rates
        /// @param fatalOverdoseRates Matrix4d of Fatal Overdose Rates
        virtual void loadFatalOverdoseRates(
            Matrixify::Matrix4d const &fatalOverdoseRates) = 0;

        /// @brief Load the Mortality Rates
        /// @param mortalityRates Matrix3d of Mortality Rates
        virtual void
        loadMortalityRates(Matrixify::Matrix3d const &mortalityRates) = 0;

        /// @brief Load Matrixify to the Simulation
        /// @param dataLoader dataLoader containing necessary data for
        /// Simulations
        virtual void
        Load(std::shared_ptr<Matrixify::IDataLoader> const dataLoader) = 0;

        /// @brief Load Aging Parameters
        /// @param shift Integer Value to offset aging
        /// @param interval Integer Size of Age Group
        virtual void LoadAgingParameters(int const &shift,
                                         int const &interval) = 0;

        /// @brief Get the Entering Samples
        /// @return Matrix4d of Entering Samples
        virtual Matrixify::Matrix4d GetEnteringSamples() const = 0;

        /// @brief Get the OUD Transitions
        /// @return Matrix3d of OUD Transitions
        virtual Matrixify::Matrix3d GetOUDTransitions() const = 0;

        /// @brief Get the Intervention Transitions
        /// @return Matrix4d of Intervention Transitions
        virtual Matrixify::Matrix4d GetInterventionTransitions() const = 0;

        /// @brief Get the Overdose Transactions
        /// @return Matrix4d of Overdose Transitions
        virtual Matrixify::Matrix4d GetOverdoseTransitions() const = 0;

        /// @brief Get the Fatal Overdose Transitions
        /// @return Matrix4d of Fatal Overdose Transitions
        virtual Matrixify::Matrix4d GetFatalOverdoseTransitions() const = 0;

        /// @brief Get the Mortality Transitions
        /// @return Matrix3d of Mortality Transitions
        virtual Matrixify::Matrix3d GetMortalityTransitions() const = 0;

        /// @brief Load all Modules
        /// @param enteringSamples Matrix4d of Entering Samples
        /// @param oudTransitionRates Matrix3d of OUD Transition Rates
        /// @param interventionInitRates Matrix3d of Intervention Initialization
        /// Rates
        /// @param interventionTransitionRates Matrix4d of Intervention
        /// Transition Rates
        /// @param fatalOverdoseRates Matrix4d of Fatal Overdose Rates
        /// @param overdoseRates Matrix4d of Overdose Rates
        /// @param mortalityRates Matrix3d of Mortality Rates
        virtual void LoadTransitionModules(
            Matrixify::Matrix4d const &enteringSamples,
            Matrixify::Matrix3d const &oudTransitionRates,
            Matrixify::Matrix3d const &interventionInitRates,
            Matrixify::Matrix4d const &interventionTransitionRates,
            Matrixify::Matrix4d const &fatalOverdoseRates,
            Matrixify::Matrix4d const &overdoseRates,
            Matrixify::Matrix3d const &mortalityRates) = 0;

        /// @brief Core Run Function
        virtual void Run() = 0;

        /// @brief Helper used to Raise Population Age according to cycle
        virtual void raisePopulationAge() = 0;

        /// @brief Return the History generated in the Simulation
        /// @return History struct
        virtual Matrixify::History getHistory() const = 0;
    };

    /*! Concrete Class for Sim implementing the ISim interface
     *  @image html png/RESPOND-StateMatrix.png
     *  @image latex pdf/RESPOND-StateMatrix.pdf "Multiplication" width=10cm
     */
    class Sim : public ISim {
    public:
        /// @brief
        Sim();

        /// @brief
        /// @param duration
        /// @param numOUDStates
        /// @param numInterventions
        /// @param numDemographics
        Sim(int duration, int numOUDStates, int numInterventions,
            int numDemographics);
        ~Sim(){};

        /// @brief
        /// @param dataLoader
        Sim(std::shared_ptr<Matrixify::IDataLoader> dataLoader);

        /// @brief
        void Run() override;

        /// @brief
        void raisePopulationAge() override;

        /// @brief
        /// @return
        Matrixify::History getHistory() const override { return this->history; }

        /// @brief
        /// @param initialSample
        void
        loadInitialSample(Matrixify::Matrix3d const &initialSample) override;

        /// @brief
        /// @param enteringSamples
        void loadEnteringSamples(
            Matrixify::Matrix4d const &enteringSamples) override;

        /// @brief
        /// @param oudTransitionRates
        void loadOUDTransitionRates(
            Matrixify::Matrix3d const &oudTransitionRates) override;

        /// @brief
        /// @param interventionInitRates
        void loadInterventionInitRates(
            Matrixify::Matrix3d const &interventionInitRates) override;

        /// @brief
        /// @param interventionTransitionRates
        void loadInterventionTransitionRates(
            Matrixify::Matrix4d const &interventionTransitionRates) override;

        /// @brief
        /// @param overdoseRates
        void
        loadOverdoseRates(Matrixify::Matrix4d const &overdoseRates) override;

        /// @brief
        /// @param fatalOverdoseRates
        void loadFatalOverdoseRates(
            Matrixify::Matrix4d const &fatalOverdoseRates) override;

        /// @brief
        /// @param mortalityRates
        void
        loadMortalityRates(Matrixify::Matrix3d const &mortalityRates) override;

        /// @brief
        /// @param dataLoader
        void
        Load(std::shared_ptr<Matrixify::IDataLoader> const dataLoader) override;

        /// @brief
        /// @param shift
        /// @param interval
        void LoadAgingParameters(int const &shift,
                                 int const &interval) override;

        /// @brief
        /// @param enteringSamples
        /// @param oudTransitionRates
        /// @param interventionInitRates
        /// @param interventionTransitionRates
        /// @param fatalOverdoseRates
        /// @param overdoseRates
        /// @param mortalityRates
        void LoadTransitionModules(
            Matrixify::Matrix4d const &enteringSamples,
            Matrixify::Matrix3d const &oudTransitionRates,
            Matrixify::Matrix3d const &interventionInitRates,
            Matrixify::Matrix4d const &interventionTransitionRates,
            Matrixify::Matrix4d const &fatalOverdoseRates,
            Matrixify::Matrix4d const &overdoseRates,
            Matrixify::Matrix3d const &mortalityRates) override;

        /// @brief
        /// @return
        Matrixify::Matrix4d GetEnteringSamples() const override {
            return this->enteringSamples;
        }

        /// @brief
        /// @return
        Matrixify::Matrix3d GetOUDTransitions() const override {
            return this->oudTransitionRates;
        }

        /// @brief
        /// @return
        Matrixify::Matrix4d GetInterventionTransitions() const override {
            return this->interventionTransitionRates;
        }

        /// @brief
        /// @return
        Matrixify::Matrix4d GetOverdoseTransitions() const override {
            return this->overdoseRates;
        }

        /// @brief
        /// @return
        Matrixify::Matrix4d GetFatalOverdoseTransitions() const override {
            return this->fatalOverdoseRates;
        }

        /// @brief
        /// @return
        Matrixify::Matrix3d GetMortalityTransitions() const override {
            return this->mortalityRates;
        }

        // PUBLIC MEMBER OBJECTS
        int Duration;
        int agingInterval;
        int ageGroupShift;
        bool agingSwitch;

    private:
        bool interventionInitState = false;

        Matrixify::Matrix3d state;
        Matrixify::Matrix3d transition;
        int currentTime;
        int numOUDStates;
        int numInterventions;
        int numDemographics;
        int numDemographicCombos;
        Matrixify::History history;
        Matrixify::Matrix4d enteringSamples;
        Matrixify::Matrix3d oudTransitionRates;
        Matrixify::Matrix3d interventionInitRates;
        Matrixify::Matrix4d interventionTransitionRates;
        Matrixify::Matrix4d fatalOverdoseRates;
        Matrixify::Matrix4d overdoseRates;
        Matrixify::Matrix3d mortalityRates;

        /// @brief
        /// @return
        Matrixify::Matrix3d step();

        /// @brief
        /// @param interventionState
        /// @param i
        /// @return
        Matrixify::Matrix3d
        multiplyInterventionInit(Matrixify::Matrix3d interventionState, int i);

        /// @brief
        /// @param state
        /// @return
        Matrixify::Matrix3d addEnteringSamples(Matrixify::Matrix3d state);

        /// @brief
        /// @param state
        /// @return
        Matrixify::Matrix3d multiplyOUDTransitions(Matrixify::Matrix3d state);

        /// @brief
        /// @param state
        /// @return
        Matrixify::Matrix3d
        multiplyInterventionTransitions(Matrixify::Matrix3d state);

        /// @brief
        /// @param state
        /// @return
        Matrixify::Matrix3d
        multiplyFatalOverdoseTransitions(Matrixify::Matrix3d state);

        /// @brief
        /// @param state
        /// @return
        Matrixify::Matrix3d
        multiplyOverdoseTransitions(Matrixify::Matrix3d state);

        /// @brief
        /// @param state
        /// @return
        Matrixify::Matrix3d
        multiplyMortalityTransitions(Matrixify::Matrix3d state);

        /// @brief
        /// @param dim
        /// @return
        Matrixify::Matrix3d getTransitionFromDim(Matrixify::Dimension dim);

        /// @brief
        /// @param state
        /// @param dim
        /// @return
        Matrixify::Matrix3d multiplyTransitions(Matrixify::Matrix3d state,
                                                Matrixify::Dimension dim);
    };
} // namespace Simulation
#endif // MODEL_SIMULATION_HPP_
