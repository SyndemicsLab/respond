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

class Simulation{
private:
	Eigen::Tensor<float, 3> state;
	Eigen::Tensor<float, 3> transition;
    uint16_t duration;
    uint8_t numOUDStates;
    uint8_t numTreatmentStates;
    uint16_t numDemographics;
    std::vector<Eigen::Tensor<float, 3>> history;
    std::vector<Eigen::Tensor<float, 3>> enteringSamples;
    std::vector<Eigen::Tensor<float, 3>> oudTransitions;
    std::vector<Eigen::Tensor<float, 3>> treatmentTransitions;
    std::vector<Eigen::Tensor<float, 3>> overdoseTransitions;
    std::vector<Eigen::Tensor<float, 3>> fatalOverdoseTransitions;
    std::vector<Eigen::Tensor<float, 3>> mortalityTransitions;
    Eigen::Tensor<float, 3> step();
    Eigen::Tensor<float, 3> multiplyEnteringSamples(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyOUDTransitions(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyTreatmentTransitions(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyOverdoseTransitions(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyFatalOverdoseTransitions(Eigen::Tensor<float, 3> state);
    Eigen::Tensor<float, 3> multiplyMortalityTransitions(Eigen::Tensor<float, 3> state);
public:
    Simulation();
	Simulation(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics);
    ~Simulation();
    void LoadEnteringSamples(std::vector<Eigen::Tensor<float, 3>> enterintSamples);
    void LoadOUDTransitions(std::vector<Eigen::Tensor<float, 3>> oudTransitions);
    void LoadTreatmentTransitions(std::vector<Eigen::Tensor<float, 3>> treatmentTransitions);
    void LoadOverdoseTransitions(std::vector<Eigen::Tensor<float, 3>> overdoseTransitions);
    void LoadFatalOverdoseTransitions(std::vector<Eigen::Tensor<float, 3>> fatalOverdoseTransitions);
    void LoadMortalityTransitions(std::vector<Eigen::Tensor<float, 3>> mortalityTransitions);
    void LoadTransitionModules(std::vector<Eigen::Tensor<float, 3>> enteringSamples, 
        std::vector<Eigen::Tensor<float, 3>> oudTransitions, 
        std::vector<Eigen::Tensor<float, 3>> treatmentTransitions,
        std::vector<Eigen::Tensor<float, 3>> overdoseTransitions,
        std::vector<Eigen::Tensor<float, 3>> fatalOverdoseTransitions,
        std::vector<Eigen::Tensor<float, 3>> mortalityTransitions
    );
	void Run();
    std::vector<Eigen::Tensor<float, 3>> getHistory();
};



#endif /* SRC_SIMULATION_HPP_ */
