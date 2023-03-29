#include "Simulation.hpp"

Simulation::Simulation(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics){
    for(uint16_t i = 0; i < duration; i++){
        
    }
}

void Simulation::Run(){

}

Eigen::Tensor<float, 3> timestepPass(Eigen::Tensor<float, 3> state, Eigen::Tensor<float, 3> transition){

}