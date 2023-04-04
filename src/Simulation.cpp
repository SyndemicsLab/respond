#include "Simulation.hpp"


//////////////////////////////////////////////
///
/// Simulation Public Implementation
///
/////////////////////////////////////////////

/// @brief Default Constructor
Simulation::Simulation() : Simulation(0, 0, 0, 0){}

/// @brief Constructor for Simulation Object
/// @param duration Total time for the entire simulation
/// @param numOUDStates Total number of OUD states
/// @param numTreatmentStates Total number of possible treatment states (2n+1) 
/// @param numDemographics Total number of demographic combinations
Simulation::Simulation(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics){
    const auto processor_count = std::thread::hardware_concurrency();
	Eigen::setNbThreads(processor_count);
    this->duration = duration;
    this->currentTime = 0;
    this->numOUDStates = numOUDStates;
    this->numTreatmentStates = numTreatmentStates;
    this->numTreatmentStates = numDemographics;
    this->state = Eigen::Tensor<float, 3>(numOUDStates, numTreatmentStates, numDemographics);
    this->transition = Eigen::Tensor<float, 3>(numOUDStates, numTreatmentStates, numDemographics);
    // this->LoadTransitionMatrices("transitions.db");
}

/// @brief Default Destructor
Simulation::~Simulation(){
    delete this->state;
    delete this->transition;
}

/// @brief 
/// @param enterintSamples 
void Simulation::LoadEnteringSamples(std::vector<Eigen::Tensor<float, 3>> enteringSamples){
    this->enteringSamples = enteringSamples;
}

/// @brief 
/// @param oudTransitions 
void Simulation::LoadOUDTransitions(std::vector<Eigen::Tensor<float, 3>> oudTransitions){
    this->oudTransitions = oudTransitions;
}

/// @brief 
/// @param treatmentTransitions 
void Simulation::LoadTreatmentTransitions(std::vector<Eigen::Tensor<float, 3>> treatmentTransitions){
    this->treatmentTransitions = treatmentTransitions;
}

/// @brief 
/// @param overdoseTransitions 
void Simulation::LoadOverdoseTransitions(std::vector<Eigen::Tensor<float, 3>> overdoseTransitions){
    this->overdoseTransitions = overdoseTransitions;
}

/// @brief 
/// @param mortalityTransitions 
void Simulation::LoadMortalityTransitions(std::vector<Eigen::Tensor<float, 3>> mortalityTransitions){
    this->mortalityTransitions = mortalityTransitions;
}

/// @brief 
/// @param enteringSamples 
/// @param oudTransitions 
/// @param treatmentTransitions 
/// @param overdoseTransitions 
/// @param mortalityTransitions 
void Simulation::LoadTransitionModules(std::vector<Eigen::tensor<float, 3>> enteringSamples, 
    std::vector<Eigen::tensor<float, 3>> oudTransitions, 
    std::vector<Eigen::tensor<float, 3>> treatmentTransitions,
    std::vector<Eigen::tensor<float, 3>> overdoseTransitions,
    std::vector<Eigen::tensor<float, 3>> mortalityTransitions
){
    this->LoadEnteringSamples(enteringSamples);
    this->LoadOUDTransitions(oudTransitions);
    this->LoadTreatmentTransitions(treatmentTransitions);
    this->LoadOverdoseTransitions(overdoseTransitions);
    this->LoadMortalityTransitions(mortalityTransitions);
}

/// @brief Driving Method
void Simulation::Run(){
    for(uint16_t i = 0; i < duration; i++){
        this->history.push_back(this->state);
        this->state = getNewState(this->state, this->transitionMatrices);
    }
}

/// @brief 
/// @return 
std::vector<Eigen::Tensor<float, 3>> Simulation::getHistory(){
    return this->history;
}

////////////////////////////////////////////////////////
///
/// Simulation Private Implementation
///
////////////////////////////////////////////////////////

/// @brief Standard timestep method making a step through each module
/// @return NextState Matrix
Eigen::Tensor<float, 3> Simulation::step(){
    return this.multiplyMortalityTransitions(
        this.multiplyOverdoseTransitions(
            this.multiplyTreatmentTransitions(
                this.multiplyOUDTransitions(
                    this.addEnteringSamples(this.state)
                )
            )
        )
    );
}

/// @brief Add Entering samples to current state
/// @param state the tensor we will add the entering sample to
/// @return Resulting tensor with added samples to the state
Eigen::Tensor<float, 3> Simulation::addEnteringSamples(Eigen::Tensor<float, 3> state){
    Eigen::Tensor<float, 3> ret(state.dimension(0), state.dimension(1), state.dimension(2));
    ret.setValues(state);
    ret += this->enteringSamples[this->currentTime];
    return ret;
}

/// @brief Multiply OUD transition rates to the current state
/// @param state current state
/// @return new state with OUD transitions
Eigen::Tensor<float, 3> Simulation::multiplyOUDTransitions(Eigen::Tensor<float, 3> state){
    Eigen::Tensor<float, 3> ret(state.dimension(0), state.dimension(1), state.dimension(2));
    ret.setZeros();
    for(int i = 0; i < state.dimension(1); i++){
        std::array<uint16_t, 3> offset = {0,i*state.dimension(1),0};
        std::array<uint16_t, 3> extent = {state.dimension(0),(i*state.dimension(1))+state.dimension(1),state.dimension(2)};
        Eigen::Tensor<float, 3> sliced = state.slice({0,i,0}, {state.dimension(0), 1, state.dimension(2)});
        ret += (sliced.broadcast({1,state.dimension(1),1}) * this->oudTransitions[this->currentTime].slice(offset, extent));
    }
    return ret;
}

/// @brief Multiply Treatment Transitions with current state to produce change matrix
/// @param state current state
/// @return new state with treatment transitions
Eigen::Tensor<float, 3> Simulation::multiplyTreatmentTransitions(Eigen::Tensor<float, 3> state){
    Eigen::Tensor<float, 3> ret(state.dimension(0), state.dimension(1), state.dimension(2));
    ret.setZeros();
    for(int i = 0; i < state.dimension(0); i++){
        std::array<uint16_t, 3> offset = {i*state.dimension(0),0,0};
        std::array<uint16_t, 3> extent = {(i*state.dimension(0))+state.dimension(0), state.dimension(1),state.dimension(2)};
        Eigen::Tensor<float, 3> sliced = state.slice({i,0,0}, {1, state.dimension(1), state.dimension(2)});
        ret += (sliced.broadcast({state.dimension(0),1,1}) * this->treatmentTransitions[this->currentTime].slice(offset, extent));
    }
    return ret;
}

/// @brief Calculate number of Overdoses
/// @param state current state
/// @return Number of Overdoses
Eigen::Tensor<float, 3> Simulation::multiplyOverdoseTransitions(Eigen::Tensor<float, 3> state){
    return state;
}

/// @brief Calculate the amount of mortalities
/// @param state current state
/// @return New State with death calculated in
Eigen::Tensor<float, 3> Simulation::multiplyMortalityTransitions(Eigen::Tensor<float, 3> state){
    Eigen::Tensor<float, 3> ret(state.dimension(0), state.dimension(1), state.dimension(2));
    ret = state - (state * this->mortalityTransitions[this->currentTime]);
    return ret;
}