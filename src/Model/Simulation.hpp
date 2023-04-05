/*
 * Simulation.hpp
 *
 *  Created on: Mar 29, 2023
 *      Author: matt
 */

#ifndef SRC_SIMULATION_HPP_
#define SRC_SIMULATION_HPP_

#include <iostream>
#include <cstdint>
#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <string>

namespace Simulation{
    typedef std::vector<Eigen::Tensor<float, 3>> DataMatrix;

    class ISim{
    public:
        virtual ~ISim(){};
        virtual void LoadInitialGroup(Eigen::Tensor<float, 3> initialGroup) = 0;
        virtual void LoadEnteringSamples(DataMatrix enteringSamples) = 0;
        virtual void LoadOUDTransitions(DataMatrix oudTransitions) = 0;
        virtual void LoadTreatmentTransitions(DataMatrix treatmentTransitions) = 0;
        virtual void LoadOverdoseTransitions(DataMatrix overdoseTransitions) = 0;
        virtual void LoadMortalityTransitions(DataMatrix mortalityTransitions) = 0;
        
        virtual DataMatrix GetEnteringSamples() = 0;
        virtual DataMatrix GetOUDTransitions() = 0;
        virtual DataMatrix GetTreatmentTransitions() = 0;
        virtual DataMatrix GetOverdoseTransitions() = 0;
        virtual DataMatrix GetMortalityTransitions() = 0;

        virtual void LoadTransitionModules(DataMatrix enteringSamples, 
            DataMatrix oudTransitions, 
            DataMatrix treatmentTransitions,
            DataMatrix overdoseTransitions,
            DataMatrix mortalityTransitions
        ) = 0;
        virtual void Run() = 0;
        virtual DataMatrix getHistory() = 0;
    };

    class Sim : public ISim {
    private:
        Eigen::Tensor<float, 3> state;
        Eigen::Tensor<float, 3> transition;
        uint16_t currentTime;
        uint8_t numOUDStates;
        uint8_t numTreatmentStates;
        uint16_t numDemographics;
        DataMatrix history;
        DataMatrix enteringSamples;
        DataMatrix oudTransitions;
        DataMatrix treatmentTransitions;
        DataMatrix overdoseTransitions;
        DataMatrix mortalityTransitions;
        Eigen::Tensor<float, 3> step();
        Eigen::Tensor<float, 3> addEnteringSamples(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyOUDTransitions(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyTreatmentTransitions(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyOverdoseTransitions(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyMortalityTransitions(Eigen::Tensor<float, 3> state);
    public:
        Sim();
        Sim(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics);
        virtual ~Sim() {};
        void LoadInitialGroup(Eigen::Tensor<float, 3> initialGroup) override;
        void LoadEnteringSamples(DataMatrix enteringSamples) override;
        void LoadOUDTransitions(DataMatrix oudTransitions) override;
        void LoadTreatmentTransitions(DataMatrix treatmentTransitions) override;
        void LoadOverdoseTransitions(DataMatrix overdoseTransitions) override;
        void LoadMortalityTransitions(DataMatrix mortalityTransitions) override;

        DataMatrix GetEnteringSamples() override;
        DataMatrix GetOUDTransitions() override;
        DataMatrix GetTreatmentTransitions() override;
        DataMatrix GetOverdoseTransitions() override;
        DataMatrix GetMortalityTransitions() override;

        void LoadTransitionModules(DataMatrix enteringSamples, 
            DataMatrix oudTransitions, 
            DataMatrix treatmentTransitions,
            DataMatrix overdoseTransitions,
            DataMatrix mortalityTransitions
        ) override;
        void Run() override;
        DataMatrix getHistory() override;
        uint16_t Duration;
    };
}

#endif /* SRC_SIMULATION_HPP_ */
