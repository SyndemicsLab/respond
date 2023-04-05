#include "Simulation.hpp"


namespace Simulation{
    //////////////////////////////////////////////
    ///
    /// Simulation Public Implementation
    ///
    /////////////////////////////////////////////

    /// @brief Default Constructor
    Sim::Sim() : Sim(0, 0, 0, 0){}

    /// @brief Constructor for Simulation Object
    /// @param duration Total time for the entire simulation
    /// @param numOUDStates Total number of OUD states
    /// @param numTreatmentStates Total number of possible treatment states (2n+1) 
    /// @param numDemographics Total number of demographic combinations
    Sim::Sim(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics){
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
        this->Duration = duration;
        this->currentTime = 0;
        this->numOUDStates = numOUDStates;
        this->numTreatmentStates = numTreatmentStates;
        this->numTreatmentStates = numDemographics;
        this->state = Eigen::Tensor<float, 3>(numOUDStates, numTreatmentStates, numDemographics);
        this->transition = Eigen::Tensor<float, 3>(numOUDStates, numTreatmentStates, numDemographics);
    }

    /// @brief 
    /// @param enterintSamples 
    void Sim::LoadEnteringSamples(std::vector<Eigen::Tensor<float, 3>> enteringSamples){
        this->enteringSamples = enteringSamples;
    }

    /// @brief 
    /// @param oudTransitions 
    void Sim::LoadOUDTransitions(std::vector<Eigen::Tensor<float, 3>> oudTransitions){
        this->oudTransitions = oudTransitions;
    }

    /// @brief 
    /// @param treatmentTransitions 
    void Sim::LoadTreatmentTransitions(std::vector<Eigen::Tensor<float, 3>> treatmentTransitions){
        this->treatmentTransitions = treatmentTransitions;
    }

    /// @brief 
    /// @param overdoseTransitions 
    void Sim::LoadOverdoseTransitions(std::vector<Eigen::Tensor<float, 3>> overdoseTransitions){
        this->overdoseTransitions = overdoseTransitions;
    }

    /// @brief 
    /// @param mortalityTransitions 
    void Sim::LoadMortalityTransitions(std::vector<Eigen::Tensor<float, 3>> mortalityTransitions){
        this->mortalityTransitions = mortalityTransitions;
    }

    /// @brief 
    /// @param enteringSamples 
    /// @param oudTransitions 
    /// @param treatmentTransitions 
    /// @param overdoseTransitions 
    /// @param mortalityTransitions 
    void Sim::LoadTransitionModules(std::vector<Eigen::Tensor<float, 3>> enteringSamples, 
        std::vector<Eigen::Tensor<float, 3>> oudTransitions, 
        std::vector<Eigen::Tensor<float, 3>> treatmentTransitions,
        std::vector<Eigen::Tensor<float, 3>> overdoseTransitions,
        std::vector<Eigen::Tensor<float, 3>> mortalityTransitions
    ){
        this->LoadEnteringSamples(enteringSamples);
        this->LoadOUDTransitions(oudTransitions);
        this->LoadTreatmentTransitions(treatmentTransitions);
        this->LoadOverdoseTransitions(overdoseTransitions);
        this->LoadMortalityTransitions(mortalityTransitions);
    }

    /// @brief Driving Method
    void Sim::Run(){
        for(uint16_t i = 0; i < this->Duration; i++){
            this->history.push_back(this->state);
            // this->state = getNewState(this->state, this->transitionMatrices);
        }
    }

    /// @brief 
    /// @return 
    std::vector<Eigen::Tensor<float, 3>> Sim::getHistory(){
        return this->history;
    }

    ////////////////////////////////////////////////////////
    ///
    /// Simulation Private Implementation
    ///
    ////////////////////////////////////////////////////////

    /// @brief Standard timestep method making a step through each module
    /// @return NextState Matrix
    Eigen::Tensor<float, 3> Sim::step(){
        return this->multiplyMortalityTransitions(
            this->multiplyOverdoseTransitions(
                this->multiplyTreatmentTransitions(
                    this->multiplyOUDTransitions(
                        this->addEnteringSamples(this->state)
                    )
                )
            )
        );
    }

    /// @brief Add Entering samples to current state
    /// @param state the tensor we will add the entering sample to
    /// @return Resulting tensor with added samples to the state
    Eigen::Tensor<float, 3> Sim::addEnteringSamples(Eigen::Tensor<float, 3> state){
        std::array<uint16_t, 3> offset = {0,0,0};
        std::array<uint16_t, 3> extent = {state.dimension(0), state.dimension(1), state.dimension(2)};
        Eigen::Tensor<float, 3> ret = state.slice(offset, extent);
        ret += this->enteringSamples[this->currentTime];
        return ret;
    }

    /// @brief Multiply OUD transition rates to the current state
    /// @param state current state
    /// @return new state with OUD transitions
    Eigen::Tensor<float, 3> Sim::multiplyOUDTransitions(Eigen::Tensor<float, 3> state){
        Eigen::Tensor<float, 3> ret(state.dimension(0), state.dimension(1), state.dimension(2));
        ret.setZero();
        for(int i = 0; i < state.dimension(1); i++){
            std::array<uint16_t, 3> offsetTrans = {0,i*state.dimension(1),0};
            std::array<uint16_t, 3> extentTrans = {state.dimension(0),(i*state.dimension(1))+state.dimension(1),state.dimension(2)};

            std::array<uint16_t, 3> offsetState = {0,i,0};
            std::array<uint16_t, 3> extentState = {state.dimension(0), 1, state.dimension(2)};
            Eigen::Tensor<float, 3> slicedState = state.slice(offsetState, extentState);

            std::array<uint16_t, 3> bcast = {1,state.dimension(1),1};

            ret += (slicedState.broadcast(bcast) * this->oudTransitions[this->currentTime].slice(offsetTrans, extentTrans));
        }
        return ret;
    }

    /// @brief Multiply Treatment Transitions with current state to produce change matrix
    /// @param state current state
    /// @return new state with treatment transitions
    Eigen::Tensor<float, 3> Sim::multiplyTreatmentTransitions(Eigen::Tensor<float, 3> state){
        Eigen::Tensor<float, 3> ret(state.dimension(0), state.dimension(1), state.dimension(2));
        ret.setZero();
        for(int i = 0; i < state.dimension(0); i++){
            std::array<uint16_t, 3> offsetTrans = {i*state.dimension(0),0,0};
            std::array<uint16_t, 3> extentTrans = {(i*state.dimension(0))+state.dimension(0), state.dimension(1),state.dimension(2)};

            std::array<uint16_t, 3> offsetState = {i,0,0};
            std::array<uint16_t, 3> extentState = {1, state.dimension(1), state.dimension(2)};

            Eigen::Tensor<float, 3> slicedState = state.slice(offsetState, extentState);

            std::array<uint16_t, 3> bcast = {state.dimension(0),1,1};

            ret += (slicedState.broadcast(bcast) * this->treatmentTransitions[this->currentTime].slice(offsetTrans, extentTrans));
        }
        return ret;
    }

    /// @brief Calculate number of Overdoses
    /// @param state current state
    /// @return Number of Overdoses
    Eigen::Tensor<float, 3> Sim::multiplyOverdoseTransitions(Eigen::Tensor<float, 3> state){
        return state;
    }

    /// @brief Calculate the amount of mortalities
    /// @param state current state
    /// @return New State with death calculated in
    Eigen::Tensor<float, 3> Sim::multiplyMortalityTransitions(Eigen::Tensor<float, 3> state){
        Eigen::Tensor<float, 3> ret(state.dimension(0), state.dimension(1), state.dimension(2));
        ret = state - (state * this->mortalityTransitions[this->currentTime]);
        return ret;
    }
}
