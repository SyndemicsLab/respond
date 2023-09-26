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

#define FMT_HEADER_ONLY

#include "DataLoader.hpp"
#include "DataTypes.hpp"
#include "Matrix3dFactory.hpp"
#include <Eigen/Eigen>
#include <algorithm>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/sources/logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <cmath>
#include <cstdint>
#include <fmt/core.h>
#include <iostream>
#include <numeric>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>

/// @brief Namespace defining all Simulation Operations
namespace Simulation {
    /// @brief Interface for all Simulations
    class ISim {
    public:
        virtual ~ISim(){};

        /// @brief Load the Initial Sample
        /// @param initialSample Matrix3d of InitialSample
        virtual void loadInitialSample(Data::Matrix3d initialSample) = 0;

        /// @brief Load the Entering Samples
        /// @param enteringSamples Matrix3dOverTime of Entering Samples
        virtual void
        loadEnteringSamples(Data::Matrix3dOverTime enteringSamples) = 0;

        /// @brief Load the OUD Transition Rates
        /// @param oudTransitionRates Matrix3d of OUD Transition Rates
        virtual void
        loadOUDTransitionRates(Data::Matrix3d oudTransitionRates) = 0;

        /// @brief Load the Intervention Initialization Rates
        /// @param interventionInitRates Matrix3d of Intervention Initialization
        /// Rates
        virtual void
        loadInterventionInitRates(Data::Matrix3d interventionInitRates) = 0;

        /// @brief Load the Intervention Transition Rates
        /// @param interventionTransitionRates Matrix3dOverTime of Intervention
        /// Transition Rates
        virtual void loadInterventionTransitionRates(
            Data::Matrix3dOverTime interventionTransitionRates) = 0;

        /// @brief Load the Overdose Rates
        /// @param overdoseRates Matrix3dOverTime of Overdose Rates
        virtual void
        loadOverdoseRates(Data::Matrix3dOverTime overdoseRates) = 0;

        /// @brief Load the Fatal Overdose Rates
        /// @param fatalOverdoseRates Matrix3dOverTime of Fatal Overdose Rates
        virtual void
        loadFatalOverdoseRates(Data::Matrix3dOverTime fatalOverdoseRates) = 0;

        /// @brief Load the Mortality Rates
        /// @param mortalityRates Matrix3d of Mortality Rates
        virtual void loadMortalityRates(Data::Matrix3d mortalityRates) = 0;

        /// @brief Load Data to the Simulation
        /// @param dataLoader dataLoader containing necessary data for
        /// Simulations
        virtual void Load(Data::DataLoader dataLoader) = 0;

        /// @brief Load Aging Parameters
        /// @param shift Integer Value to offset aging
        /// @param interval Integer Size of Age Group
        virtual void LoadAgingParameters(int shift, int interval) = 0;

        /// @brief Get the Entering Samples
        /// @return Matrix3dOverTime of Entering Samples
        virtual Data::Matrix3dOverTime GetEnteringSamples() = 0;

        /// @brief Get the OUD Transitions
        /// @return Matrix3d of OUD Transitions
        virtual Data::Matrix3d GetOUDTransitions() = 0;

        /// @brief Get the Intervention Transitions
        /// @return Matrix3dOverTime of Intervention Transitions
        virtual Data::Matrix3dOverTime GetInterventionTransitions() = 0;

        /// @brief Get the Overdose Transactions
        /// @return Matrix3dOverTime of Overdose Transitions
        virtual Data::Matrix3dOverTime GetOverdoseTransitions() = 0;

        /// @brief Get the Fatal Overdose Transitions
        /// @return Matrix3dOverTime of Fatal Overdose Transitions
        virtual Data::Matrix3dOverTime GetFatalOverdoseTransitions() = 0;

        /// @brief Get the Mortality Transitions
        /// @return Matrix3d of Mortality Transitions
        virtual Data::Matrix3d GetMortalityTransitions() = 0;

        /// @brief Load all Modules
        /// @param enteringSamples Matrix3dOverTime of Entering Samples
        /// @param oudTransitionRates Matrix3d of OUD Transition Rates
        /// @param interventionInitRates Matrix3d of Intervention Initialization
        /// Rates
        /// @param interventionTransitionRates Matrix3dOverTime of Intervention
        /// Transition Rates
        /// @param fatalOverdoseRates Matrix3dOverTime of Fatal Overdose Rates
        /// @param overdoseRates Matrix3dOverTime of Overdose Rates
        /// @param mortalityRates Matrix3d of Mortality Rates
        virtual void LoadTransitionModules(
            Data::Matrix3dOverTime enteringSamples,
            Data::Matrix3d oudTransitionRates,
            Data::Matrix3d interventionInitRates,
            Data::Matrix3dOverTime interventionTransitionRates,
            Data::Matrix3dOverTime fatalOverdoseRates,
            Data::Matrix3dOverTime overdoseRates,
            Data::Matrix3d mortalityRates) = 0;

        /// @brief Core Run Function
        virtual void Run() = 0;

        /// @brief Helper used to Raise Population Age according to cycle
        virtual void raisePopulationAge() = 0;

        /// @brief Return the History generated in the Simulation
        /// @return History struct
        virtual Data::History getHistory() = 0;
    };

    /*! Concrete Class for Sim implementing the ISim interface
     *  @image html png/RESPOND-StateMatrix.png
     *  @image latex pdf/RESPOND-StateMatrix.pdf "Multiplication" width=10cm
     */
    class Sim : public ISim {
    public:
        // CONSTRUCTORS
        Sim();
        Sim(int duration, int numOUDStates, int numInterventions,
            int numDemographics);
        ~Sim(){};
        Sim(Data::DataLoader dataLoader);

        // PUBLIC METHODS
        void Run() override;
        void raisePopulationAge() override;
        Data::History getHistory() override;

        // SETTERS
        void loadInitialSample(Data::Matrix3d initialSample) override;
        void
        loadEnteringSamples(Data::Matrix3dOverTime enteringSamples) override;
        void loadOUDTransitionRates(Data::Matrix3d oudTransitionRates) override;
        void loadInterventionInitRates(
            Data::Matrix3d interventionInitRates) override;
        void loadInterventionTransitionRates(
            Data::Matrix3dOverTime interventionTransitionRates) override;
        void loadOverdoseRates(Data::Matrix3dOverTime overdoseRates) override;
        void loadFatalOverdoseRates(
            Data::Matrix3dOverTime fatalOverdoseRates) override;
        void loadMortalityRates(Data::Matrix3d mortalityRates) override;
        void Load(Data::DataLoader dataLoader) override;
        void LoadAgingParameters(int shift, int interval) override;

        void LoadTransitionModules(
            Data::Matrix3dOverTime enteringSamples,
            Data::Matrix3d oudTransitionRates,
            Data::Matrix3d interventionInitRates,
            Data::Matrix3dOverTime interventionTransitionRates,
            Data::Matrix3dOverTime fatalOverdoseRates,
            Data::Matrix3dOverTime overdoseRates,
            Data::Matrix3d mortalityRates) override;

        // GETTERS
        Data::Matrix3dOverTime GetEnteringSamples() override;
        Data::Matrix3d GetOUDTransitions() override;
        Data::Matrix3dOverTime GetInterventionTransitions() override;
        Data::Matrix3dOverTime GetOverdoseTransitions() override;
        Data::Matrix3dOverTime GetFatalOverdoseTransitions() override;
        Data::Matrix3d GetMortalityTransitions() override;

        // PUBLIC MEMBER OBJECTS
        int Duration;
        int agingInterval;
        int ageGroupShift;
        bool agingSwitch;

    private:
        boost::log::sources::logger lg;

        bool interventionInitState = false;

        Data::Matrix3d state;
        Data::Matrix3d transition;
        int currentTime;
        int numOUDStates;
        int numInterventions;
        int numDemographics;
        int numDemographicCombos;
        std::vector<int> reportingInterval;
        Data::Matrix3d runningOverdoses;
        Data::Matrix3d runningMortality;
        Data::Matrix3d runningFOD;
        Data::History history;
        Data::Matrix3dOverTime enteringSamples;
        Data::Matrix3d oudTransitionRates;
        Data::Matrix3d interventionInitRates;
        Data::Matrix3dOverTime interventionTransitionRates;
        Data::Matrix3dOverTime fatalOverdoseRates;
        Data::Matrix3dOverTime overdoseRates;
        Data::Matrix3d mortalityRates;
        Data::Matrix3d step();
        Data::Matrix3d
        multiplyInterventionInit(Data::Matrix3d interventionState, int i);
        Data::Matrix3d addEnteringSamples(Data::Matrix3d state);
        Data::Matrix3d multiplyOUDTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyInterventionTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyFatalOverdoseTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyOverdoseTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyMortalityTransitions(Data::Matrix3d state);
        Data::Matrix3d getTransitionFromDim(Data::Dimension dim);
        Data::Matrix3d multiplyTransitions(Data::Matrix3d state,
                                           Data::Dimension dim);
    };
} // namespace Simulation
#endif // MODEL_SIMULATION_HPP_
