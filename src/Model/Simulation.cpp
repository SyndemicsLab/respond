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
    /// @param numInterventions Total number of possible interventions 
    /// @param numDemographics Total number of demographic combinations
    Sim::Sim(uint16_t duration, uint8_t numOUDStates, uint8_t numInterventions, uint16_t numDemographics){
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
        this->numInterventions = numInterventions;
        this->numDemographics = numDemographics;
        this->state = this->CreateNewShapedTensor();
        this->state.setZero();
        this->transition = this->CreateNewShapedTensor();
        this->transition.setZero();
    }

    /// @brief Constructor to handle DataLoader input to Simulation
    /// @param dataLoader DataLoader object filled ready to Run
    Sim::Sim(Data::DataLoader dataLoader){
        // Stub for DataLoader
    }

    /// @brief Create a new Tensor shaped like the state data
    /// @return A New Empty Tensor of the same shape as the state data
    Data::Matrix3d Sim::CreateNewShapedTensor(){
        std::array<long int, 3> order = {0,0,0};
        order[Data::OUD] = this->numOUDStates;
        order[Data::INTERVENTION] = this->numInterventions;
        order[Data::DEMOGRAPHIC_COMBO] = this->numDemographics; 
        Data::Matrix3d empty(order);
        return empty;
    }

    /// @brief Setter for Initial Group Parameter
    /// @param initialGroup 3 Dimensional Matrix describing the Initial Population
    void Sim::LoadInitialGroup(Data::Matrix3d initialGroup){
        this->state = initialGroup;
        BOOST_LOG(this->lg) << "Initial Group Loaded";
    }

    /// @brief Setter for Entering Samples Group
    /// @param enteringSamples 3 Dimensional Matrix over Time describing the Entering Samples
    void Sim::LoadEnteringSamples(Data::Matrix3dOverTime enteringSamples){
        this->enteringSamples = enteringSamples;
        BOOST_LOG(this->lg) << "Entering Samples Loaded";
    }

    /// @brief Setter for OUD Transitions
    /// @param oudTransitions 3 Dimensional Matrix over Time describing the OUD Transition Percentages
    void Sim::LoadOUDTransitions(Data::Matrix3d oudTransitions){
        this->oudTransitions = oudTransitions;
        BOOST_LOG(this->lg) << "OUD Transitions Loaded";
    }

    /// @brief Setter for Intervention Transitions
    /// @param InterventionTransitions 3 Dimensional Matrix over Time describing the Intervention Transition Percentages
    void Sim::LoadInterventionTransitions(Data::Matrix3dOverTime interventionTransitions){
        this->interventionTransitions = interventionTransitions;
        BOOST_LOG(this->lg) << "Intervention Transitions Loaded";
    }

    void Sim::LoadFatalOverdoseTransitions(Data::Matrix3dOverTime fatalOverdoseTransitions){
        this->fatalOverdoseTransitions = fatalOverdoseTransitions;
        BOOST_LOG(this->lg) << "Fatal Overdose Transitions Loaded";
    }

    /// @brief Setter for Overdose Rates
    /// @param overdoseTransitions 3 Dimensional Matrix over Time describing the Overdose Percentages
    void Sim::LoadOverdoseTransitions(Data::Matrix3dOverTime overdoseTransitions){
        this->overdoseTransitions = overdoseTransitions;
        BOOST_LOG(this->lg) << "Overdose Transitions Loaded";
    }

    /// @brief Setter for Mortality Rates
    /// @param mortalityTransitions 3 Dimensional Matrix over Time describing the Mortality Percentages
    void Sim::LoadMortalityTransitions(Data::Matrix3d mortalityTransitions){
        this->mortalityTransitions = mortalityTransitions;
        BOOST_LOG(this->lg) << "Mortality Transitions Loaded";
    }

    /// @brief Setter for all data using a DataLoader object
    void Sim::Load(Data::DataLoader dataLoader){
        // Stub for Loading
    }

    /// @brief Getter for Entering Samples Group
    /// @return 3 Dimensional Matrix over Time describing the Entering Samples
    Data::Matrix3dOverTime Sim::GetEnteringSamples(){
        return this->enteringSamples;
    }

    /// @brief Getter for OUD Transition Percentages
    /// @return 3 Dimensional Matrix over Time describing the OUD Transition Percentages
    Data::Matrix3d Sim::GetOUDTransitions(){
        return this->oudTransitions;
    }

    /// @brief Getter for Intervention Transition Percentages
    /// @return 3 Dimensional Matrix over Time describing the Intervention Transition Percentages
    Data::Matrix3dOverTime Sim::GetInterventionTransitions(){
        return this->interventionTransitions;
    }

    Data::Matrix3dOverTime Sim::GetFatalOverdoseTransitions(){
        return this->fatalOverdoseTransitions;
    }
    
    /// @brief Getter for Overdose Rates
    /// @return 3 Dimensional Matrix over Time describing the Overdose Percentages
    Data::Matrix3dOverTime Sim::GetOverdoseTransitions(){
        return this->overdoseTransitions;
    }
    
    /// @brief Getter for Mortality Rates
    /// @return 3 Dimensional Matrix over Time describing the Mortality Percentages
    Data::Matrix3d Sim::GetMortalityTransitions(){
        return this->mortalityTransitions;
    }

    /// @brief Convenience function to Load all Matrices at Once
    /// @param enteringSamples 3 Dimensional Matrix over Time describing the Entering Samples
    /// @param oudTransitions 3 Dimensional Matrix over Time describing the OUD Transition Percentages
    /// @param interventionTransitions 3 Dimensional Matrix over Time describing the Intervention Transition Percentages
    /// @param overdoseTransitions 3 Dimensional Matrix over Time describing the Overdose Percentages
    /// @param mortalityTransitions 3 Dimensional Matrix over Time describing the Mortality Percentages
    void Sim::LoadTransitionModules(Data::Matrix3dOverTime enteringSamples, 
        Data::Matrix3d oudTransitions, 
        Data::Matrix3dOverTime interventionTransitions,
        Data::Matrix3dOverTime fatalOverdoseTransitions,
        Data::Matrix3dOverTime overdoseTransitions,
        Data::Matrix3d mortalityTransitions
    ){
        this->LoadEnteringSamples(enteringSamples);
        this->LoadOUDTransitions(oudTransitions);
        this->LoadInterventionTransitions(interventionTransitions);
        this->LoadFatalOverdoseTransitions(fatalOverdoseTransitions);
        this->LoadOverdoseTransitions(overdoseTransitions);
        this->LoadMortalityTransitions(mortalityTransitions);
    }

    /// @brief Core method used to run the simulation
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

    /// @brief Getter for entire stored history
    /// @return History struct containing Overdoses, Mortality, and State Object at each timestep
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
        Data::Matrix3d transitionedState = this->multiplyInterventionTransitions(
            this->multiplyOUDTransitions(
                    this->addEnteringSamples(this->state)
                )
            );
        
        Data::Matrix3d overdoses = this->multiplyOverdoseTransitions(transitionedState);
        Data::Matrix3d fatalOverdoses = this->multiplyFatalOverdoseTransitions(overdoses);
        Data::Matrix3d mortalities = this->multiplyMortalityTransitions(transitionedState);

        return (transitionedState - (mortalities + fatalOverdoses));
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
        return this->multiplyTransitions(state, Data::OUD);
    }

    /// @brief Multiply Intervention Transitions with current state to produce change matrix
    /// @param state current state
    /// @return new state with Intervention transitions
    Data::Matrix3d Sim::multiplyInterventionTransitions(Data::Matrix3d state){
        return this->multiplyTransitions(state, Data::INTERVENTION);
    }

    Data::Matrix3d Sim::getTransitionFromDim(Data::Dimension dim){
        switch(dim){
            case Data::OUD:
                return this->oudTransitions;
            case Data::INTERVENTION:
                return this->interventionTransitions.at(this->currentTime);
            case Data::DEMOGRAPHIC_COMBO:
            default:
                return this->CreateNewShapedTensor();
        }
    }

    /// @brief Helper function to multiply Transitions based on the provided dimension
    /// @param state current state
    /// @param dim dimension under investigation
    /// @return new state with transitions multiplied in
    Data::Matrix3d Sim::multiplyTransitions(Data::Matrix3d state, Data::Dimension dim){
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

    Data::Matrix3d Sim::multiplyFatalOverdoseTransitions(Data::Matrix3d state){
        Data::Matrix3d fatalOverdoseMatrix = this->fatalOverdoseTransitions[this->currentTime];
        if(fatalOverdoseMatrix.dimensions() != state.dimensions()){
            std::string message = fmt::format("Fatal Overdose Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }
        Data::Matrix3d mult = fatalOverdoseMatrix * state;
        return mult;
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
        return mult;
    }

    /// @brief Calculate the amount of mortalities
    /// @param state current state
    /// @return New State with death calculated in
    Data::Matrix3d Sim::multiplyMortalityTransitions(Data::Matrix3d state){
        Data::Matrix3d mortalityMatrix = this->mortalityTransitions;
        if(mortalityMatrix.dimensions() != state.dimensions()){
            std::string message = fmt::format("Mortality Dimensions does not equal the State Dimensions at timestep {}", this->currentTime);
            throw std::invalid_argument(message);
        }
        Data::Matrix3d ret(state.dimensions());
        Data::Matrix3d mor = (state * mortalityMatrix);
        this->history.mortalityHistory.push_back(mor);
        return mor;
    }
}
