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
    std::vector<Eigen::Tensor<float, 3>> transitionMatrices;
public:
    Simulation();
	Simulation(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics);
    ~Simulation();
    void LoadTransitionMatrices(std::vector<Eigen::Tensor<float, 3>>);
    void AddTransitionMatrix(Eigen::Tensor<float, 3> matrix);
	void Run();
};



#endif /* SRC_SIMULATION_HPP_ */
