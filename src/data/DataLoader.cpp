#include "DataLoader.hpp"

using namespace Data;

/*********************************************************************
 *
 * Constructors
 *
 *********************************************************************/
DataLoader::DataLoader() {
    this->duration = 0;
    this->agingInterval = 0;
    this->ageGroupShift = 0;
    this->numOUDStates = 0;
    this->numInterventions = 0;
    this->numDemographics = 0;
    this->numDemographicCombos = 0;

    this->interventions = {};
    this->oudStates = {};
    this->demographicCounts = {};
    this->simulationParameters = {};

    this->Config;

    this->initialSample;
    this->enteringSamples;
    this->oudTransitionRates;
    this->interventionTransitionRates;
    this->overdoseRates;
    this->fatalOverdoseRates;
    this->mortalityRates;

    this->inputTables = {};
}

DataLoader::DataLoader(std::string inputDir) {
    // PROCESSING INPUT FILES
    // account for no trailing slash in the provided input directory
    std::string configPath = inputDir;
    if (configPath.back() != '/') {
        configPath.push_back('/');
    }
    configPath += "sim.conf";
    this->Config = readConfigFile(configPath);
    this->inputTables = readInputDir(inputDir);

    // SETTING STRING VECTORS FOR DATA WRITER
    this->interventions = this->Config.getInterventions();
    this->oudStates = this->Config.getOUDStates();
    this->demographicCounts = this->Config.getDemographicCounts();

    // SETTING SIMULATION CONSTANTS
    this->numInterventions   = this->interventions.size();
    this->numOUDStates       = this->oudStates.size();

    this->duration = this->Config.getDuration();
    this->numDemographics = this->demographicCounts.size();
    this->numDemographicCombos = this->Config.getNumDemographicCombos();
    this->agingInterval = this->Config.getAgingInterval();
    // forcing the convention that age is the first demographic specified in the
    // simulation config; needed for aging the population
    this->ageGroupShift = this->numDemographicCombos/this->demographicCounts[0];

    std::vector<std::string> demographicCombos = this->Config.getDemographicCombos();
}

DataLoader::DataLoader(Configuration config, std::string inputDir){
    this->Config = config;
    this->inputTables = this->readInputDir(inputDir);
    // SETTING STRING VECTORS FOR DATA WRITER
    this->interventions = this->Config.getInterventions();
    this->oudStates = this->Config.getOUDStates();
    this->demographicCounts = this->Config.getDemographicCounts();

    // SETTING SIMULATION CONSTANTS
    this->numInterventions   = this->interventions.size();
    this->numOUDStates       = this->oudStates.size();

    this->duration = this->Config.getDuration();
    this->numDemographics = this->demographicCounts.size();
    this->numDemographicCombos = this->Config.getNumDemographicCombos();
    this->agingInterval = this->Config.getAgingInterval();
}

/*********************************************************************
 *
 * Public Methods
 *
 *********************************************************************/

Configuration DataLoader::readConfigFile(std::string inputFile) {
    Configuration config(inputFile);
    return config;
}

InputTable DataLoader::readCSV(std::string inputFile) {
    using boost::tokenizer;

    std::string inputContents;
    std::ifstream inputStream(inputFile);
    InputTable toReturn;

    // use the headers of the csv table to create keys for the unordered map
    std::getline(inputStream, inputContents);
    std::vector<std::string> headerNames;
    tokenizer<boost::escaped_list_separator<char>> token(inputContents);
    for (tokenizer<boost::escaped_list_separator<char>>::iterator beg = token.begin();
         beg != token.end(); ++beg) {
        headerNames.push_back(*beg);
        toReturn[*beg] = {};
    }
    // populate the keys/headers with values
    int i = 0;
    while (std::getline(inputStream, inputContents)) {
        tokenizer<boost::escaped_list_separator<char>> token(inputContents);
        for (tokenizer<boost::escaped_list_separator<char>>::iterator beg = token.begin();
             beg != token.end(); ++beg) {
            toReturn[headerNames[i % headerNames.size()]].push_back(*beg);
            ++i;
        }
    }
    inputStream.close();
    return toReturn;
}

std::unordered_map<std::string, InputTable> DataLoader::readInputDir(std::string inputDir) {
    std::string inputDirFixed = inputDir;
    std::unordered_map<std::string, InputTable> toReturn;
    // account for no trailing slash in the provided input directory
    if (inputDirFixed.back() != '/') {
        inputDirFixed.push_back('/');
    }

    for (std::string inputFile: INPUT_FILES) {
        toReturn[inputFile] = readCSV(inputDirFixed + inputFile);
    }
    return toReturn;
}

Matrix3d DataLoader::loadInitialSample(std::string csvName) {
    int nonPostInterventions = ((this->numInterventions - 1)/2 + 1);

    // INITIAL GROUP
    if(this->inputTables.find(csvName) == this->inputTables.end()){
        return Utilities::Matrix3dFactory::Create(0, 0, 0).constant(0);
    }
    InputTable initialCohort = this->inputTables[csvName];
    this->initialSample = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
    int row = 0;
    for (int intervention = 0; intervention < nonPostInterventions; ++intervention) {
        for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
            for (int oud_state = 0; oud_state < this->numOUDStates; ++oud_state) {
                this->initialSample(intervention, oud_state, dem) =
                    std::stod(initialCohort["counts"][row]);
                ++row;
            }
        }
    }
    return this->initialSample;
}

Matrix3dOverTime DataLoader::loadEnteringSamples(std::string csvName) {
    // ENTERING SAMPLES
    InputTable enteringSamplesTable = inputTables[csvName];
    // new population enters the reference state, "currently Active Non-Injection
    // in No Treatment" aka the first state in interventions and oud states
    int enteringSampleIntervention = 0;
    int enteringSampleOUD          = 0;
    std::string columnPrefix = "number_of_new_comers_cycle";
    // generate each unique `Matrix3d enteringSamples`
    std::vector<int> changeTimes = this->Config.getEnteringSampleChangeTimes();
    for (int timestep : changeTimes) {
        Matrix3d enteringSample = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
        for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                for (int oud = 0; oud < this->numOUDStates; ++oud) {
                    if ((intervention == enteringSampleIntervention) and (oud == enteringSampleOUD)) {
                        std::string column = columnPrefix + std::to_string(timestep);
                        enteringSample(intervention, oud, dem) =
                            std::stod(enteringSamplesTable[column][dem]);
                    }
                }
            }
        }
        this->enteringSamples.insert(enteringSample, timestep);
    }
    return this->enteringSamples;
}

Matrix3d DataLoader::loadOUDTransitionRates(std::string csvName) {
    // OUD TRANSITIONS
    InputTable oudTransitionTable = inputTables[csvName];
    // end dimensions of oudTransitionRates are this->numInterventions x this->numOUDStates^2 x demographics
    // start with a vector of StateTensor-sized Matrix3d objects and stack at the end
    std::vector<Matrix3d> tempOUDTransitions;
    for (int i = 0; i < this->numOUDStates; ++i) {
        tempOUDTransitions.push_back(Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos));
    }
    // Matrix3d objects in the vector are matched with the order that oud states are
    // specified in the Config file. the order represents the "initial oud state"
    for (int initial_state = 0; initial_state < this->numOUDStates; ++initial_state) {
        int row = 0;
        for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                for (int result_state = 0; result_state < this->numOUDStates; ++result_state) {
                    std::string column = "to_" + this->oudStates[result_state];
                    tempOUDTransitions[initial_state](intervention, result_state, dem) =
                        std::stod(oudTransitionTable[column][this->numOUDStates*row+initial_state]);
                }
                ++row;
            }
        }
    }

    // stack the Matrix3d objects along the OUD axis
    this->oudTransitionRates = tempOUDTransitions[0];
    for (int i = 1; i < tempOUDTransitions.size(); ++i) {
        Matrix3d temp = this->oudTransitionRates.concatenate(tempOUDTransitions[i], Data::OUD).eval()
            .reshape(Matrix3d::Dimensions(this->numInterventions, (i+1) * this->numOUDStates, this->numDemographicCombos));
        this->oudTransitionRates = std::move(temp);
    }
    return this->oudTransitionRates;
}

Matrix3d DataLoader::loadInterventionInitRates(std::string csvName) {
    // OUD TRANSITIONS
    InputTable interventionInitTable = inputTables[csvName];
    // end dimensions of oudTransitionRates are this->numInterventions x this->numOUDStates^2 x demographics
    // start with a vector of StateTensor-sized Matrix3d objects and stack at the end
    std::vector<Matrix3d> tempinterventionInit;
    int activeNonActiveOffset = this->numOUDStates/2;
    for (int i = 0; i < this->numOUDStates; ++i) {
        tempinterventionInit.push_back(Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0));
    }
    // Matrix3d objects in the vector are matched with the order that oud states are
    // specified in the Config file. the order represents the "initial oud state"
    for (int initial_state = 0; initial_state < this->numOUDStates; ++initial_state) {
        std::string currentOUDState = this->oudStates[initial_state];
        auto iterator = std::find(
            interventionInitTable["initial_oud_state"].begin(),
            interventionInitTable["initial_oud_state"].end(),
            currentOUDState
        );

        int idx = 0;
        if(iterator != interventionInitTable["initial_oud_state"].end()){
            idx = iterator - interventionInitTable["initial_oud_state"].begin();
        }

        int nonactiveFlag = false;
        if(currentOUDState.find("Nonactive") != std::string::npos){
            nonactiveFlag = true;
        }
        else{
            nonactiveFlag = false;
        }

        for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
            std::string currentIntervention = this->interventions[intervention];
            std::string dash("-");
            std::string period(".");
            size_t pos = currentIntervention.find(dash);
            if(pos != std::string::npos){
                currentIntervention.replace(pos, dash.length(), period);
            }
            currentIntervention = "to_" + currentIntervention;

            double tableVal = std::stod(interventionInitTable[currentIntervention][idx]);
            double oppVal = 1 - tableVal;
            int oppValIdx = (nonactiveFlag) ? idx - activeNonActiveOffset : idx + activeNonActiveOffset;
            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                tempinterventionInit[initial_state](intervention, idx, dem) = tableVal;
                tempinterventionInit[initial_state](intervention, oppValIdx, dem) = oppVal;
            }
        }
    }

    // stack the Matrix3d objects along the OUD axis
    this->interventionInitRates = tempinterventionInit[0];
    for (int i = 1; i < tempinterventionInit.size(); ++i) {
        Matrix3d temp = this->interventionInitRates.concatenate(tempinterventionInit[i], Data::OUD).eval()
            .reshape(Matrix3d::Dimensions(this->numInterventions, (i+1) * this->numOUDStates, this->numDemographicCombos));
        this->interventionInitRates = std::move(temp);
    }

    // std::cout << this->interventionInitRates << std::endl;
    std::cout << std::endl;
    Utilities::Matrix3dPrinter::Print(this->interventionInitRates, std::cout);

    return this->interventionInitRates;
}

Matrix3dOverTime DataLoader::loadInterventionTransitionRates(std::string csvName) {

    // INTERVENTION TRANSITIONS
    InputTable interventionTransitionTable = inputTables[csvName];
    std::vector<int> ict = this->Config.getInterventionChangeTimes();

    std::vector<std::vector<int>> indicesVec = this->getIndicesByIntervention(interventionTransitionTable["initial_block"]);
    this->interventionTransitionRates = this->buildTransitionRatesOverTime(ict, interventionTransitionTable, indicesVec);
    return this->interventionTransitionRates;
}

Matrix3dOverTime DataLoader::loadOverdoseRates(std::string csvName) {
    // OVERDOSE
    InputTable overdoseTransitionTable = this->inputTables[csvName];
    std::vector<int> oct = this->Config.getOverdoseChangeTimes();

    for(auto timestep : oct) {
        std::string str_timestep = "cycle" + std::to_string(timestep);
        InputTable currentTimeTable = this->removeColumns(str_timestep, overdoseTransitionTable);
        for(auto kv : currentTimeTable) {
            std::string str = kv.first;
            if(str.find("overdose_cycle") != std::string::npos) {
                this->overdoseRates.insert(this->buildOverdoseTransitions(currentTimeTable, str), timestep);
            }
        }
    }
    return this->overdoseRates;
}

Matrix3dOverTime DataLoader::loadFatalOverdoseRates(std::string csvName) {
    InputTable fatalOverdoseTable      = inputTables[csvName];
    std::vector<Matrix3d> tempFatalOverdoseTransitions;
    for (int timestep: this->Config.getOverdoseChangeTimes()) {
        Matrix3d overdoseTransition = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
        // fatal overdose is a constant across all strata
        std::string fodColumn = "fatal_to_all_types_overdose_ratio_cycle" + std::to_string(timestep);
        this->fatalOverdoseRates.insert(
            Utilities::Matrix3dFactory::Create(
                this->numOUDStates, this->numInterventions, this->numDemographicCombos
            ).constant(std::stod(fatalOverdoseTable[fodColumn][0])), timestep
        );
    }
    return this->fatalOverdoseRates;
}

Matrix3d DataLoader::loadMortalityRates(std::string smrCSVName, std::string bgmCSVName) {
    // MORTALITY TRANSITIONS
    // mortality here refers to death from reasons other than oud and is calculated
    // by combining the SMR and background mortality
    // calculation to combine these into the mortality is 1-exp(log(1-bg_mort)*SMR)
    std::vector<std::string> smrColumn                 = inputTables[smrCSVName]["SMR"];
    // only stratified by the demographics, needs to be expanded for oud and intervention
    std::vector<std::string> backgroundMortalityColumn = inputTables[bgmCSVName]["death_prob"];
    Matrix3d mortalityTransition = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos);
    // mortality is one element per stratum, no time variability
    int smrIndex = 0;
    for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
        for (int dem = 0; dem < this->numDemographicCombos; dem++) {
            for (int oud = 0; oud < this->numOUDStates; ++oud) {

                mortalityTransition(intervention, oud, dem) =
                    1 - exp(log(1 - std::stod(backgroundMortalityColumn[dem]))*std::stod(smrColumn[smrIndex]));
                smrIndex++;
            }
        }
    }
    this->mortalityRates = mortalityTransition;
    return this->mortalityRates;
}

/*********************************************************************
 *
 * Private Methods
 *
 *********************************************************************/

/// @brief
/// @param colString
/// @param ogTable
/// @return
InputTable DataLoader::removeColumns(std::string colString, InputTable ogTable) {
    InputTable res = ogTable;
    std::vector<std::string> keys;
    for(auto i : res) {
        if((i.first.find(colString) == std::string::npos) && (i.first.find("cycle") != std::string::npos)) {
            std::string temp = i.first;
            keys.push_back(temp);
        }
    }

    for(auto i : keys) { res.erase(i); }

    return res;
}

/// @brief
/// @param indices
/// @param table
/// @return
Data::Matrix3d DataLoader::buildInterventionMatrix(std::vector<int> indices, InputTable table) {
    Data::Matrix3d transMat = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
    std::string currentIntervention = table["initial_block"][indices[0]];
    std::vector<std::string> keys;

    // reconstructing order of interventions based on column keys
    bool postFlag = false;
    int idx = 0;
    int offset = 0;
    for(auto inter : this->Config.getInterventions()){
        for(auto kv : table){
            if(kv.first.find(inter) != std::string::npos){
                std::string key = kv.first;
                keys.push_back(key);
            }
            else if((inter.find("Post") != std::string::npos) &&
                    (kv.first.find("post") != std::string::npos) &&
                    !postFlag) {
                std::string key = kv.first;
                keys.push_back(key);
                postFlag = true; // only assumes one "to_corresponding_post_trt column"
            }
        }
        if(currentIntervention.compare(inter) == 0){ offset = idx; }
        idx++;
    }

    for(int i = 0; i < keys.size(); i++) {
        int interventionOffset = i;

        if(currentIntervention.compare("No_Treatment") == 0){ interventionOffset = i; }
        else if(keys[i].find("post") != std::string::npos){
            interventionOffset = i+offset-1;
            if(currentIntervention.find("Post") != std::string::npos){ interventionOffset -= 2; }
        }

        std::string key = keys[i];
        int oudIdx = 0;
        int demIdx = 0;
        for (int idx : indices) {
            std::string val = table[key][idx];
            transMat(interventionOffset, oudIdx, demIdx) = std::stod(val);
            oudIdx++;
            if(oudIdx%this->numOUDStates == 0) {
                oudIdx = 0;
                demIdx++;
            }
        }

    }
    return transMat;
}

/// @brief
/// @param indicesVec
/// @param table
/// @param dimension
/// @return
Data::Matrix3d DataLoader::createTransitionMatrix3d(std::vector<std::vector<int>> indicesVec, InputTable table, Data::Dimension dimension) {
    if(dimension == Data::INTERVENTION) {
        Matrix3d stackingMatrices = Utilities::Matrix3dFactory::Create(this->numOUDStates,
        this->numInterventions * this->numInterventions,
        this->numDemographicCombos).constant(0);
        for (int i=0; i < indicesVec.size(); i++) {
            // assign to index + offset of numInterventions
            Eigen::array<Eigen::Index, 3> offsets = {i * this->numInterventions, 0,0};
            Eigen::array<Eigen::Index, 3> extents = {this->numInterventions, this->numOUDStates, this->numDemographicCombos};
            stackingMatrices.slice(offsets, extents) = this->buildInterventionMatrix(indicesVec[i], table);
        }
        return stackingMatrices;

    }
    else if(dimension == Data::OUD) {
        Matrix3d stackingMatrices = Utilities::Matrix3dFactory::Create(this->numOUDStates * this->numOUDStates,
            this->numInterventions,
            this->numDemographicCombos).constant(0);
        return stackingMatrices;
    }
    Matrix3d stackingMatrices = Utilities::Matrix3dFactory::Create(this->numOUDStates,
        this->numInterventions,
        this->numDemographicCombos).constant(0);
    return stackingMatrices;
}

/// @brief
/// @param v
/// @param target
/// @return
std::vector<int> DataLoader::findIndices(std::vector<std::string> const &v, std::string target) {
    std::vector<int> indices;
    auto it = v.begin();
    while ((it = std::find_if(it, v.end(), [&] (std::string const &e) { return e.compare(target) == 0; })) != v.end()) {
        indices.push_back(std::distance(v.begin(), it));
        it++;
    }
    return indices;
}

/// @brief
/// @param col
/// @return
std::vector<std::vector<int>> DataLoader::getIndicesByIntervention(std::vector<std::string> col) {
    std::vector<std::vector<int>> indicesVec;
    for(std::string in : this->Config.getInterventions()) {
        indicesVec.push_back(this->findIndices(col, in));
    }
    return indicesVec;
}

/// @brief
/// @param ict
/// @param table
/// @param indicesVec
/// @return
Matrix3dOverTime DataLoader::buildTransitionRatesOverTime(std::vector<int> ict, InputTable table, std::vector<std::vector<int>> indicesVec) {
    Matrix3dOverTime m3dot;

    for (int timestep: ict) {
        // get rid of the pointless columns for this iteration
        std::string str_timestep = "cycle" + std::to_string(timestep);
        InputTable currentTimeTable = this->removeColumns(str_timestep, table);
        m3dot.insert(this->createTransitionMatrix3d(indicesVec, currentTimeTable, Data::INTERVENTION), timestep);
    }
    return m3dot;
}

/// @brief
/// @param table
/// @param key
/// @return
Matrix3d DataLoader::buildOverdoseTransitions(InputTable table, std::string key) {
    std::vector<std::string> oudStates = this->Config.getOUDStates();

    Matrix3d overdoseTransitionsCycle = Utilities::Matrix3dFactory::Create(
        this->numOUDStates,
        this->numInterventions,
        this->numDemographicCombos).constant(0);

    int row = 0;
    for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
        for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
            for (int oud_state = 0; oud_state < this->numOUDStates; ++oud_state) {
                if(oudStates[oud_state].find("Nonactive") != std::string::npos) {
                    overdoseTransitionsCycle(intervention, oud_state, dem) = 0.0f;
                }
                else{
                    overdoseTransitionsCycle(intervention, oud_state, dem) =
                    std::stod(table[key][row]);
                    ++row;
                }
            }
        }
    }
    return overdoseTransitionsCycle;
}
