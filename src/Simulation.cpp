#include "Simulation.hpp"

/// @brief Callback function used during Transition Table SELECT statement execution
/// @param objData object data pointer passed in through keyword "this"
/// @param argc count of number of columns returned
/// @param rowData Record Data
/// @param colNames Names of the columns
/// @return error code if necessary
static int callback(void* objData, int argc, char** rowData, char** colNames){
    Simulation* sim = (Simulation*) objData;
    // Eigen::Tensor<float, 3> convertedMatrix = convert from rowData
    // sim->AddTransitionMatrix(convertedMatrix);
}

/// @brief A Private timestep function used to execute one multiplcation of the two 3D matrices and return the resulting state
/// @param state Current demographic vector for the treatment and oud under scrutiny
/// @param transition Transition probabilities matrix
/// @return Resulting State Matrix
Eigen::Tensor<float, 3> timestepPass(Eigen::Tensor<float, 3> state, Eigen::Tensor<float, 3> transition){
    Eigen::Tensor<float, 3> ret = state;
    if (state.dimension(0) != transition.dimension(0) 
    && state.dimension(1) != transition.dimension(1) 
    && state.dimension(2) != transition.dimension(2)){
        uint8_t x = state.dimension(0)/transition.dimension(0);
        uint8_t y = state.dimension(1)/transition.dimension(1);
        uint8_t z = state.dimension(2)/transition.dimension(2);
        Eigen::array<int, 3> bcast({x, y, z});
        ret = state.broadcast(bcast);
    }
    ret *= transition;
    return ret;
}

/// @brief Method restricted to file for getting the new state
/// @param state Full State Tensor x(OUD) by y(Treatment) by z(Demographics)
/// @return New State Vector after a timestep
Eigen::Tensor<float, 3> getNewState(Eigen::Tensor<float, 3> state, std::vector<Eigen::Tensor<float, 3>> transitionMatrices){
    Eigen::Tensor<float, 3> newState(state.dimension(0), state.dimension(1), state.dimension(2));
    int oudStates = state.dimension(0);
    int treatments = state.dimension(1);
    int counter = 0; // forgive me, I know there is an easier way my brain is just fried
    // loop through each oud/treatment state combo - O((2N+1)*M)
    for (int i = 0; i < oudStates; i++){
        for (int j = 0; j < treatments; j++){
            std::array<int, 3> offset = {i, j, 0};
            std::array<int, 3> extent = {1, 1, state.dimension(2)};
            Eigen::Tensor<float, 3> transition = timestepPass(state.slice(offset, extent), transitionMatrices[counter]); // get the counts that changed
            newState = state + transition; // Add the moving counts to the current counts
            newState.slice(offset, extent) -= transition.sum(Eigen::array<int, 2>({0, 1})); // Subtract the moved counts from the current state
            counter++;
        }
    }
}

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
    this->numOUDStates = numOUDStates;
    this->numTreatmentStates = numTreatmentStates;
    this->numTreatmentStates = numDemographics;
    this->state = new Eigen::Tensor<float, 3>(numOUDStates, numTreatmentStates, numDemographics);
    this->transition = new Eigen::Tensor<float, 3>(numOUDStates, numTreatmentStates, numDemographics);
    // this->LoadTransitionMatrices("transitions.db");
}

/// @brief Driving Method
void Simulation::Run(){
    for(uint16_t i = 0; i < duration; i++){
        this->history.push_back(this->state);
        this->state = getNewState(this->state, this->transitionMatrices);
    }
}

/// @brief Method used to load transition table data from sqlite3 Database
/// @param path database filepath
void Simulation::LoadTransitionMatrices(std::vector<Eigen::Tensor<float, 3>> transitionMatrices){
    // std::string query = "SELECT * FROM transitions";
    // sqlite3 *db;
    // int rc = sqlite3_open(path, &db);
    // if(rc <= 0){
    //     //log error
    //     return;
    // }
    // sqlite3_exec(db, query.c_str(), callback, this, NULL);
    this->transitionMatrices = transitionMatrices;
}

/// @brief Setter for pushing on a transition matrix to the simulation vector
/// @param matrix Matrix to push into vector
void Simulation::AddTransitionMatrix(Eigen::Tensor<float, 3> matrix){
    this->transitionMatrices.push_back(matrix);
}

std::vector<Eigen::Tensor<float, 3>> getHistory(){
    return this->history;
}

/// @brief Default Destructor
Simulation::~Simulation(){
    // delete this->state;
    // delete this->transition;
    // while(!this->history.empty()){
    //     delete this->history.pop_back();
    // }
}

