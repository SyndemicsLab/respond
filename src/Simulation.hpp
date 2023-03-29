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

class Simulation{
private:
	Eigen::Tensor<float, 3> state;
	Eigen::Tensor<float, 3> transition;
public:
	Simulation(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics);
	void Run();
};



#endif /* SRC_SIMULATION_HPP_ */
