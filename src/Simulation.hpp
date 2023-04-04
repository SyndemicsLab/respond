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

class ISimulation{
    virtual ~Simulation(){};
    virtual void LoadEnteringSamples(std::vector<Eigen::Tensor<float, 3>> enteringSamples) = 0;
    virtual void LoadOUDTransitions(std::vector<Eigen::Tensor<float, 3>> oudTransitions) = 0;
    virtual void LoadTreatmentTransitions(std::vector<Eigen::Tensor<float, 3>> treatmentTransitions) = 0;
    virtual void LoadOverdoseTransitions(std::vector<Eigen::Tensor<float, 3>> overdoseTransitions) = 0;
    virtual void LoadMortalityTransitions(std::vector<Eigen::Tensor<float, 3>> mortalityTransitions) = 0;
    virtual void LoadTransitionModules(std::vector<Eigen::Tensor<float, 3>> enteringSamples, 
        std::vector<Eigen::Tensor<float, 3>> oudTransitions, 
        std::vector<Eigen::Tensor<float, 3>> treatmentTransitions,
        std::vector<Eigen::Tensor<float, 3>> overdoseTransitions,
        std::vector<Eigen::Tensor<float, 3>> mortalityTransitions
    ) = 0;
	virtual void Run() = 0;
    virtual std::vector<Eigen::Tensor<float, 3>> getHistory() = 0;
}

class Simulation : public ISimulation {
private:
	Eigen::Tensor<float, 3> state;
	Eigen::Tensor<float, 3> transition;
    uint16_t duration;
    uint16_t currentTime;
    uint8_t numOUDStates;
    uint8_t numTreatmentStates;
    uint16_t numDemographics;
    std::vector<Eigen::Tensor<float, 3>> history;
    std::vector<Eigen::Tensor<float, 3>> enteringSamples;
    std::vector<Eigen::Tensor<float, 3>> oudTransitions;
    std::vector<Eigen::Tensor<float, 3>> treatmentTransitions;
    std::vector<Eigen::Tensor<float, 3>> overdoseTransitions;
    std::vector<Eigen::Tensor<float, 3>> mortalityTransitions;
    Eigen::Tensor<float, 3> step();
    Eigen::Tensor<float, 3> addEnteringSamples(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyOUDTransitions(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyTreatmentTransitions(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyOverdoseTransitions(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyMortalityTransitions(Eigen::Tensor<float, 3> state);
public:
    Simulation();
	Simulation(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics);
    override ~Simulation();
    override void LoadEnteringSamples(std::vector<Eigen::Tensor<float, 3>> enteringSamples);
    override void LoadOUDTransitions(std::vector<Eigen::Tensor<float, 3>> oudTransitions);
    override void LoadTreatmentTransitions(std::vector<Eigen::Tensor<float, 3>> treatmentTransitions);
    override void LoadOverdoseTransitions(std::vector<Eigen::Tensor<float, 3>> overdoseTransitions);
    override void LoadMortalityTransitions(std::vector<Eigen::Tensor<float, 3>> mortalityTransitions);
    override void LoadTransitionModules(std::vector<Eigen::Tensor<float, 3>> enteringSamples, 
        std::vector<Eigen::Tensor<float, 3>> oudTransitions, 
        std::vector<Eigen::Tensor<float, 3>> treatmentTransitions,
        std::vector<Eigen::Tensor<float, 3>> overdoseTransitions,
        std::vector<Eigen::Tensor<float, 3>> mortalityTransitions
    );
	override void Run();
    override std::vector<Eigen::Tensor<float, 3>> getHistory();
};



#endif /* SRC_SIMULATION_HPP_ */
