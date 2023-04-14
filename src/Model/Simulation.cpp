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

    Sim::Sim(Data::DataLoader dataLoader){
        // Stub for DataLoader
    }

    /// @brief Create a new Tensor shaped like the state data
    /// @return A New Empty Tensor of the same shape as the state data
    Data::Matrix3d Sim::CreateNewShapedTensor(){
        std::array<long int, 3> order = {0,0,0};
        order[OUD] = this->numOUDStates;
        order[TREATMENT] = this->numTreatmentStates;
        order[DEMOGRAPHIC_COMBO] = this->numDemographics; 
        Data::Matrix3d empty(order);
        return empty;
    }

    /// @brief 
    /// @param initialGroup 
    void Sim::LoadInitialGroup(Data::Matrix3d initialGroup){
        this->state = initialGroup;
        BOOST_LOG(this->lg) << "Initial Group Loaded";
    }

    /// @brief 
    /// @param enteringSamples 
    void Sim::LoadEnteringSamples(Data::Matrix3dOverTime enteringSamples){
        this->enteringSamples = enteringSamples;
        BOOST_LOG(this->lg) << "Entering Samples Loaded";
    }

    /// @brief 
    /// @param oudTransitions 
    void Sim::LoadOUDTransitions(Data::Matrix3dOverTime oudTransitions){
        this->oudTransitions = oudTransitions;
        BOOST_LOG(this->lg) << "OUD Transitions Loaded";
    }

    /// @brief 
    /// @param treatmentTransitions 
    void Sim::LoadTreatmentTransitions(Data::Matrix3dOverTime treatmentTransitions){
        this->treatmentTransitions = treatmentTransitions;
        BOOST_LOG(this->lg) << "Treatment Transitions Loaded";
    }

    /// @brief 
    /// @param overdoseTransitions 
    void Sim::LoadOverdoseTransitions(Data::Matrix3dOverTime overdoseTransitions){
        this->overdoseTransitions = overdoseTransitions;
        BOOST_LOG(this->lg) << "Overdose Transitions Loaded";
    }

    /// @brief 
    /// @param mortalityTransitions 
    void Sim::LoadMortalityTransitions(Data::Matrix3dOverTime mortalityTransitions){
        this->mortalityTransitions = mortalityTransitions;
        BOOST_LOG(this->lg) << "Mortality Transitions Loaded";
    }

    void Sim::Load(Data::DataLoader dataLoader){
        // Stub for Loading
    }

    /// @brief 
    /// @return 
    Data::Matrix3dOverTime Sim::GetEnteringSamples(){
        return this->enteringSamples;
    }

    /// @brief 
    /// @return 
    Data::Matrix3dOverTime Sim::GetOUDTransitions(){
        return this->oudTransitions;
    }

    /// @brief 
    /// @return 
    Data::Matrix3dOverTime Sim::GetTreatmentTransitions(){
        return this->treatmentTransitions;
    }
    
    /// @brief 
    /// @return 
    Data::Matrix3dOverTime Sim::GetOverdoseTransitions(){
        return this->overdoseTransitions;
    }
    
    /// @brief 
    /// @return 
    Data::Matrix3dOverTime Sim::GetMortalityTransitions(){
        return this->mortalityTransitions;
    }

    /// @brief 
    /// @param enteringSamples 
    /// @param oudTransitions 
    /// @param treatmentTransitions 
    /// @param overdoseTransitions 
    /// @param mortalityTransitions 
    void Sim::LoadTransitionModules(Data::Matrix3dOverTime enteringSamples, 
        Data::Matrix3dOverTime oudTransitions, 
        Data::Matrix3dOverTime treatmentTransitions,
        Data::Matrix3dOverTime overdoseTransitions,
        Data::Matrix3dOverTime mortalityTransitions
    ){
        this->LoadEnteringSamples(enteringSamples);
        this->LoadOUDTransitions(oudTransitions);
        this->LoadTreatmentTransitions(treatmentTransitions);
        this->LoadOverdoseTransitions(overdoseTransitions);
        this->LoadMortalityTransitions(mortalityTransitions);
    }

    /// @brief Driving Method
    void Sim::Run(){
        Data::Matrix3d zeroMat = this->CreateNewShapedTensor();
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
    Data::History Sim::getHistory(){
        return this->history;
    }

    ////////////////////////////////////////////////////////
    ///
    /// Simulation Private Implementation
    ///
    ////////////////////////////////////////////////////////

    /// @brief Standard timestep method making a step through each module
    /// @return NextState Matrix
    Data::Matrix3d Sim::step(){
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
    Data::Matrix3d Sim::addEnteringSamples(Data::Matrix3d state){
        Data::Matrix3d enteringSamples = this->enteringSamples.at(this->currentTime);
        if(enteringSamples.dimensions() != state.dimensions()){
            std::string message = fmt::format("Entering Samples Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }

        // Slice is done because I need a copy of the state instead of the actual state reference
        std::array<long int, 3> offset = {0,0,0};
        std::array<long int, 3> extent = {state.dimensions()};
        Data::Matrix3d ret = state.slice(offset, extent);

        ret += enteringSamples;
        return ret;
    }

    /// @brief Multiply OUD transition rates to the current state
    /// @param state current state
    /// @return new state with OUD transitions
    Data::Matrix3d Sim::multiplyOUDTransitions(Data::Matrix3d state){
        return this->multiplyTransitions(state, OUD);
    }

    /// @brief Multiply Treatment Transitions with current state to produce change matrix
    /// @param state current state
    /// @return new state with treatment transitions
    Data::Matrix3d Sim::multiplyTreatmentTransitions(Data::Matrix3d state){
        return this->multiplyTransitions(state, TREATMENT);
    }

    Data::Matrix3d Sim::getTransitionFromDim(Dimension dim){
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
    Data::Matrix3d Sim::multiplyTransitions(Data::Matrix3d state, Dimension dim){
        Data::Matrix3d ret(state.dimensions());
        ret.setZero();

        Data::Matrix3d transition = this->getTransitionFromDim(dim);
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

            Data::Matrix3d slicedState = state.slice(offsetState, extentState); 

            std::array<long int, 3> bcast = {1,1,1};
            bcast[dim] = state.dimension(dim);

            Data::Matrix3d broadcastedTensor = slicedState.broadcast(bcast);
            Data::Matrix3d slicedTransition = transition.slice(offsetTrans, extentTrans);
            ret += (broadcastedTensor * slicedTransition);
        }        
        return ret;
    }


    /// @brief Calculate number of Overdoses
    /// @param state current state
    /// @return Number of Overdoses
    Data::Matrix3d Sim::multiplyOverdoseTransitions(Data::Matrix3d state){
        Data::Matrix3d overdoseMatrix = this->overdoseTransitions[this->currentTime];
        if(overdoseMatrix.dimensions() != state.dimensions()){
            std::string message = fmt::format("Overdose Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }
        Data::Matrix3d mult = overdoseMatrix * state;
        this->history.overdoseHistory.push_back(mult);
        return state;
    }

    /// @brief Calculate the amount of mortalities
    /// @param state current state
    /// @return New State with death calculated in
    Data::Matrix3d Sim::multiplyMortalityTransitions(Data::Matrix3d state){
        Data::Matrix3d mortalityMatrix = this->mortalityTransitions[this->currentTime];
        if(mortalityMatrix.dimensions() != state.dimensions()){
            std::string message = fmt::format("Mortality Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }
        Data::Matrix3d ret(state.dimensions());
        Data::Matrix3d mor = (state * mortalityMatrix);
        this->history.mortalityHistory.push_back(mor);
        ret = state - mor;
        return ret;
    }
}
