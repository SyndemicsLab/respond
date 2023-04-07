#include "Simulation.hpp"


namespace Simulation{
    //////////////////////////////////////////////
    ///
    /// Simulation Public Implementation
    ///
    /////////////////////////////////////////////

    /// @brief Default Constructor
    Sim::Sim() : Sim::Sim(0, 0, 0, 0){}

    /// @brief Constructor for Simulation Object
    /// @param duration Total time for the entire simulation
    /// @param numOUDStates Total number of OUD states
    /// @param numTreatmentStates Total number of possible treatment states (2n+1) 
    /// @param numDemographics Total number of demographic combinations
    Sim::Sim(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics){
        boost::log::add_file_log("simulation.log");
        boost::log::core::get()->set_filter(
            boost::log::trivial::severity >= boost::log::trivial::info
        );

        BOOST_LOG(this->lg) << "Initialize Logging";
        const auto processor_count = std::thread::hardware_concurrency();
        Eigen::setNbThreads(processor_count);
        this->Duration = duration;
        this->currentTime = 0;
        this->numOUDStates = numOUDStates;
        this->numTreatmentStates = numTreatmentStates;
        this->numDemographics = numDemographics;
        this->state = this->CreateNewShapedTensor();
        this->state.setZero();
        this->transition = this->CreateNewShapedTensor();
        this->transition.setZero();
    }

    /// @brief Create a new Tensor shaped like the state data
    /// @return A New Empty Tensor of the same shape as the state data
    Eigen::Tensor<float, 3> Sim::CreateNewShapedTensor(){
        std::array<long int, 3> order = {0,0,0};
        order[OUD] = this->numOUDStates;
        order[TREATMENT] = this->numTreatmentStates;
        order[DEMOGRAPHIC_COMBO] = this->numDemographics; 
        Eigen::Tensor<float, 3> empty(order);
        return empty;
    }

    /// @brief 
    /// @param initialGroup 
    void Sim::LoadInitialGroup(Eigen::Tensor<float, 3> initialGroup){
        this->state = initialGroup;
        BOOST_LOG(this->lg) << "Initial Group Loaded";
    }

    /// @brief 
    /// @param enteringSamples 
    void Sim::LoadEnteringSamples(std::vector<Eigen::Tensor<float, 3>> enteringSamples){
        this->enteringSamples = enteringSamples;
        BOOST_LOG(this->lg) << "Entering Samples Loaded";
    }

    /// @brief 
    /// @param oudTransitions 
    void Sim::LoadOUDTransitions(std::vector<Eigen::Tensor<float, 3>> oudTransitions){
        this->oudTransitions = oudTransitions;
        BOOST_LOG(this->lg) << "OUD Transitions Loaded";
    }

    /// @brief 
    /// @param treatmentTransitions 
    void Sim::LoadTreatmentTransitions(std::vector<Eigen::Tensor<float, 3>> treatmentTransitions){
        this->treatmentTransitions = treatmentTransitions;
        BOOST_LOG(this->lg) << "Treatment Transitions Loaded";
    }

    /// @brief 
    /// @param overdoseTransitions 
    void Sim::LoadOverdoseTransitions(std::vector<Eigen::Tensor<float, 3>> overdoseTransitions){
        this->overdoseTransitions = overdoseTransitions;
        BOOST_LOG(this->lg) << "Overdose Transitions Loaded";
    }

    /// @brief 
    /// @param mortalityTransitions 
    void Sim::LoadMortalityTransitions(std::vector<Eigen::Tensor<float, 3>> mortalityTransitions){
        this->mortalityTransitions = mortalityTransitions;
        BOOST_LOG(this->lg) << "Mortality Transitions Loaded";
    }

    /// @brief 
    /// @return 
    DataMatrix Sim::GetEnteringSamples(){
        return this->enteringSamples;
    }

    /// @brief 
    /// @return 
    DataMatrix Sim::GetOUDTransitions(){
        return this->oudTransitions;
    }

    /// @brief 
    /// @return 
    DataMatrix Sim::GetTreatmentTransitions(){
        return this->treatmentTransitions;
    }
    
    /// @brief 
    /// @return 
    DataMatrix Sim::GetOverdoseTransitions(){
        return this->overdoseTransitions;
    }
    
    /// @brief 
    /// @return 
    DataMatrix Sim::GetMortalityTransitions(){
        return this->mortalityTransitions;
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
        Eigen::Tensor<float, 3> zeroMat = this->CreateNewShapedTensor();
        zeroMat.setZero();
        this->history.overdoseHistory.push_back(zeroMat);
        this->history.mortalityHistory.push_back(zeroMat);
        this->history.stateHistory.push_back(this->state);

        for(this->currentTime = 0; this->currentTime < this->Duration; this->currentTime++){
            std::string fmt_string = fmt::format("Running Timestep {}\n", this->currentTime);
            BOOST_LOG(this->lg) << fmt_string;
            this->state = this->step();
            this->history.stateHistory.push_back(this->state);
        }
    }

    /// @brief 
    /// @return 
    History Sim::getHistory(){
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
        Eigen::Tensor<float, 3> enteringSamples = this->enteringSamples.at(this->currentTime);
        if(enteringSamples.dimensions() != state.dimensions()){
            std::string message = fmt::format("Entering Samples Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }

        // Slice is done because I need a copy of the state instead of the actual state reference
        std::array<long int, 3> offset = {0,0,0};
        std::array<long int, 3> extent = {state.dimensions()};
        Eigen::Tensor<float, 3> ret = state.slice(offset, extent);

        ret += enteringSamples;
        return ret;
    }

    // /// @brief Multiply OUD transition rates to the current state
    // /// @param state current state
    // /// @return new state with OUD transitions
    Eigen::Tensor<float, 3> Sim::multiplyOUDTransitions(Eigen::Tensor<float, 3> state){
        return this->multiplyTransitions(state, OUD);
    }

    // /// @brief Multiply Treatment Transitions with current state to produce change matrix
    // /// @param state current state
    // /// @return new state with treatment transitions
    Eigen::Tensor<float, 3> Sim::multiplyTreatmentTransitions(Eigen::Tensor<float, 3> state){
        return this->multiplyTransitions(state, TREATMENT);
    }

    Eigen::Tensor<float, 3> Sim::getTransitionFromDim(Dimension dim){
        switch(dim){
            case OUD:
                return this->oudTransitions.at(this->currentTime);
            case TREATMENT:
                return this->treatmentTransitions.at(this->currentTime);
            case DEMOGRAPHIC_COMBO:
            default:
                return this->CreateNewShapedTensor();
        }
    }

    /// @brief Helper function to multiply Transitions based on the provided dimension
    /// @param state current state
    /// @param dim dimension under investigation
    /// @return new state with transitions multiplied in
    Eigen::Tensor<float, 3> Sim::multiplyTransitions(Eigen::Tensor<float, 3> state, Dimension dim){
        Eigen::Tensor<float, 3> ret(state.dimensions());
        ret.setZero();

        Eigen::Tensor<float, 3> transition = this->getTransitionFromDim(dim);
        if(transition.dimension(dim) != pow(state.dimension(dim),2)){
            std::string message = fmt::format("Transition Dimensions does not equal the Square of State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }

        int transitionsInState = transition.dimension(dim) / state.dimension(dim);

        for(int i = 0; i < transitionsInState; i++){

            std::array<long int, 3> offsetTrans = {0,0,0};
            std::array<long int, 3> extentTrans = state.dimensions();
            offsetTrans[dim] = i*state.dimension(dim);

            std::array<long int, 3> offsetState = {0,0,0};
            std::array<long int, 3> extentState = state.dimensions();
            offsetState[dim] = i;
            extentState[dim] = 1;

            Eigen::Tensor<float, 3> slicedState = state.slice(offsetState, extentState); 

            std::array<long int, 3> bcast = {1,1,1};
            bcast[dim] = state.dimension(dim);

            Eigen::Tensor<float, 3> broadcastedTensor = slicedState.broadcast(bcast);
            Eigen::Tensor<float, 3> slicedTransition = transition.slice(offsetTrans, extentTrans);
            ret += (broadcastedTensor * slicedTransition);
        }        
        return ret;
    }


    /// @brief Calculate number of Overdoses
    /// @param state current state
    /// @return Number of Overdoses
    Eigen::Tensor<float, 3> Sim::multiplyOverdoseTransitions(Eigen::Tensor<float, 3> state){
        Eigen::Tensor<float, 3> overdoseMatrix = this->overdoseTransitions[this->currentTime];
        if(overdoseMatrix.dimensions() != state.dimensions()){
            std::string message = fmt::format("Overdose Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }
        Eigen::Tensor<float, 3> mult = overdoseMatrix * state;
        this->history.overdoseHistory.push_back(mult);
        return state;
    }

    /// @brief Calculate the amount of mortalities
    /// @param state current state
    /// @return New State with death calculated in
    Eigen::Tensor<float, 3> Sim::multiplyMortalityTransitions(Eigen::Tensor<float, 3> state){
        Eigen::Tensor<float, 3> mortalityMatrix = this->mortalityTransitions[this->currentTime];
        if(mortalityMatrix.dimensions() != state.dimensions()){
            std::string message = fmt::format("Mortality Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }
        Eigen::Tensor<float, 3> ret(state.dimensions());
        Eigen::Tensor<float, 3> mor = (state * mortalityMatrix);
        this->history.mortalityHistory.push_back(mor);
        ret = state - mor;
        return ret;
    }
}
