/*
 * Simulation.hpp
 *
 *  Created on: Mar 29, 2023
 *      Author: matt
 */

#ifndef MODEL_SIMULATION_HPP_
#define MODEL_SIMULATION_HPP_

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>
#include <cmath>
#include <cstdint>
#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <fmt/core.h>
#include <iostream>
#include <stdexcept>
#include <string>

#include "DataTypes.hpp"
#include "DataLoader.hpp"
#include "Matrix3dFactory.hpp"

/// @brief Namespace defining all Simulation Operations
namespace Simulation{
    class ISim{
    public:
        virtual ~ISim(){};
        virtual void loadInitialSample(Data::Matrix3d initialSample) = 0;
        virtual void loadEnteringSamples(Data::Matrix3dOverTime enteringSamples) = 0;
        virtual void loadOUDTransitionRates(Data::Matrix3d oudTransitionRates) = 0;
        virtual void loadInterventionTransitionRates(Data::Matrix3dOverTime interventionTransitionRates) = 0;
        virtual void loadOverdoseRates(Data::Matrix3dOverTime overdoseRates) = 0;
        virtual void loadFatalOverdoseRates(Data::Matrix3dOverTime fatalOverdoseRates) = 0;
        virtual void loadMortalityRates(Data::Matrix3d mortalityRates) = 0;
        virtual void Load(Data::DataLoader dataLoader) = 0;
        virtual void LoadAgingParameters(int shift, int interval) = 0;

        virtual Data::Matrix3dOverTime GetEnteringSamples() = 0;
        virtual Data::Matrix3d GetOUDTransitions() = 0;
        virtual Data::Matrix3dOverTime GetInterventionTransitions() = 0;
        virtual Data::Matrix3dOverTime GetOverdoseTransitions() = 0;
        virtual Data::Matrix3dOverTime GetFatalOverdoseTransitions() = 0;
        virtual Data::Matrix3d GetMortalityTransitions() = 0;

        virtual void LoadTransitionModules(
            Data::Matrix3dOverTime enteringSamples,
            Data::Matrix3d oudTransitionRates,
            Data::Matrix3dOverTime interventionTransitionRates,
            Data::Matrix3dOverTime fatalOverdoseRates,
            Data::Matrix3dOverTime overdoseRates,
            Data::Matrix3d mortalityRates
        ) = 0;
        virtual void Run() = 0;
        virtual void raisePopulationAge() = 0;
        virtual Data::History getHistory() = 0;
    };


    /*! Concrete Class for Sim implementing the ISim interface
     *  \image html png/RESPOND-StateMatrix.png
     *  \image latex pdf/RESPOND-StateMatrix.pdf "Multiplication" width=10cm
     */
    class Sim : public ISim {
    public:
        Sim();
        Sim(int duration, int numOUDStates, int numInterventions, int numDemographics);
        ~Sim() {};
        Sim(Data::DataLoader dataLoader);
        void loadInitialSample(Data::Matrix3d initialSample) override;
        void loadEnteringSamples(Data::Matrix3dOverTime enteringSamples) override;
        void loadOUDTransitionRates(Data::Matrix3d oudTransitionRates) override;
        void loadInterventionTransitionRates(Data::Matrix3dOverTime interventionTransitionRates) override;
        void loadOverdoseRates(Data::Matrix3dOverTime overdoseRates) override;
        void loadFatalOverdoseRates(Data::Matrix3dOverTime fatalOverdoseRates) override;
        void loadMortalityRates(Data::Matrix3d mortalityRates) override;
        void Load(Data::DataLoader dataLoader) override;
        void LoadAgingParameters(int shift, int interval) override;

        Data::Matrix3dOverTime GetEnteringSamples() override;
        Data::Matrix3d GetOUDTransitions() override;
        Data::Matrix3dOverTime GetInterventionTransitions() override;
        Data::Matrix3dOverTime GetOverdoseTransitions() override;
        Data::Matrix3dOverTime GetFatalOverdoseTransitions() override;
        Data::Matrix3d GetMortalityTransitions() override;

        void LoadTransitionModules(
            Data::Matrix3dOverTime enteringSamples,
            Data::Matrix3d oudTransitionRates,
            Data::Matrix3dOverTime interventionTransitionRates,
            Data::Matrix3dOverTime fatalOverdoseRates,
            Data::Matrix3dOverTime overdoseRates,
            Data::Matrix3d mortalityRates
        ) override;
        void Run() override;
        void raisePopulationAge() override;
        Data::History getHistory() override;
        int Duration;
        int agingInterval;
        int ageGroupShift;
        bool agingSwitch;

    private:
        boost::log::sources::logger lg;

        Data::Matrix3d state;
        Data::Matrix3d transition;
        int currentTime;
        int numOUDStates;
        int numInterventions;
        int numDemographics;
        Data::History history;
        Data::Matrix3dOverTime enteringSamples;
        Data::Matrix3d oudTransitionRates;
        Data::Matrix3dOverTime interventionTransitionRates;
        Data::Matrix3dOverTime fatalOverdoseRates;
        Data::Matrix3dOverTime overdoseRates;
        Data::Matrix3d mortalityRates;
        Data::Matrix3d step();
        Data::Matrix3d addEnteringSamples(Data::Matrix3d state);
        Data::Matrix3d multiplyOUDTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyInterventionTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyFatalOverdoseTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyOverdoseTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyMortalityTransitions(Data::Matrix3d state);
        Data::Matrix3d getTransitionFromDim(Data::Dimension dim);
        Data::Matrix3d multiplyTransitions(Data::Matrix3d state, Data::Dimension dim);
    };
}
#endif /* MODEL_SIMULATION_HPP_ */
