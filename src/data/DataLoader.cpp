#include "DataLoader.hpp"

using namespace Data;

void printMatrix3d(Matrix3d toPrint);

/// @brief The default constructor for the object that processes input data
/// @param inputDir the name of the directory where input files are stored
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
    this->interventions = this->Config.GetInterventions();
    this->oudStates = this->Config.GetOUDStates();
    this->demographicCounts = this->Config.GetDemographicCounts();

    // SETTING SIMULATION CONSTANTS
    this->numInterventions   = this->interventions.size();
    this->numOUDStates       = this->oudStates.size();

    this->duration = this->Config.GetDuration();
    this->numDemographics = this->demographicCounts.size();
    this->numDemographicCombos = this->Config.GetNumDemographicCombos();
    this->agingInterval = this->Config.GetAgingInterval();
    // forcing the convention that age is the first demographic specified in the
    // simulation config; needed for aging the population
    this->ageGroupShift = this->numDemographicCombos/this->demographicCounts[0];

    std::vector<std::string> demographicCombos = this->Config.GetDemographicCombos();

    this->LoadInitialGroup("init_cohort.csv");
    this->LoadEnteringSamples("entering_cohort.csv");
    this->LoadOUDTransitions("oud_trans.csv");
    this->LoadInterventionTransitions("block_trans.csv");
    this->LoadOverdoseTransitions("all_types_overdose.csv");
    this->LoadFatalOverdoseTransitions("fatal_overdose.csv");
    this->LoadMortalityTransitions("SMR.csv");
}

/// @brief
/// @param csvName
void DataLoader::LoadInitialGroup(std::string csvName) {
    int nonPostInterventions = ((this->numInterventions - 1)/2 + 1);

    // INITIAL GROUP
    InputTable initialCohort = this->inputTables[csvName];
    initialGroup = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
    int row = 0;
    for (int intervention = 0; intervention < nonPostInterventions; ++intervention) {
        for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
            for (int oud_state = 0; oud_state < this->numOUDStates; ++oud_state) {
                initialGroup(intervention, oud_state, dem) =
                    std::stof(initialCohort["counts"][row]);
                ++row;
            }
        }
    }
}


/// @brief
/// @param csvName
void DataLoader::LoadEnteringSamples(std::string csvName) {
    // ENTERING SAMPLES
    InputTable enteringSamplesTable = inputTables[csvName];
    // new population enters the reference state, "currently Active Non-Injection
    // in No Treatment" aka the first state in interventions and oud states
    int enteringSampleIntervention = 0;
    int enteringSampleOUD          = 0;
    std::string columnPrefix = "number_of_new_comers_cycle";
    // generate each unique `Matrix3d enteringSamples`
    std::vector<int> changeTimes = this->Config.GetEnteringSampleChangeTimes();
    for (int timestep : changeTimes) {
        Matrix3d enteringSample = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
        for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
            for (int dem = 0; dem < this->numDemographicCombos; ++dem) {
                for (int oud = 0; oud < this->numOUDStates; ++oud) {
                    if ((intervention == enteringSampleIntervention) and (oud == enteringSampleOUD)) {
                        std::string column = columnPrefix + std::to_string(timestep);
                        enteringSample(intervention, oud, dem) =
                            std::stof(enteringSamplesTable[column][dem]);
                    }
                }
            }
        }
        this->enteringSamples.insert(enteringSample, timestep);
    }
}

/// @brief
/// @param csvName
void DataLoader::LoadOUDTransitions(std::string csvName) {
    // OUD TRANSITIONS
    InputTable oudTransitionTable = inputTables[csvName];
    // end dimensions of oudTransitions are this->numInterventions x this->numOUDStates^2 x demographics
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
                        std::stof(oudTransitionTable[column][this->numOUDStates*row+initial_state]);
                }
                ++row;
            }
        }
    }

    // stack the Matrix3d objects along the OUD axis
    oudTransitions = tempOUDTransitions[0];
    for (int i = 1; i < tempOUDTransitions.size(); ++i) {
        Matrix3d temp = oudTransitions.concatenate(tempOUDTransitions[i], Data::OUD).eval()
            .reshape(Matrix3d::Dimensions(this->numInterventions, (i+1) * this->numOUDStates, this->numDemographicCombos));
        oudTransitions = std::move(temp);
    }
}

InputTable DataLoader::RemoveColumns(std::string colString, InputTable ogTable) {
    InputTable res = ogTable;
    std::vector<std::string> keys;
    for(auto i : res) {
        if((i.first.find(colString) == std::string::npos) && (i.first.find("cycle") != std::string::npos)) {
            std::string temp = i.first;
            keys.push_back(temp);
        }
    }

    for(auto i : keys) {
        res.erase(i);
    }

    for(auto kv : res) {
        std::cout << kv.first << std::endl;
    }
    return res;
}

Data::Matrix3d DataLoader::BuildInterventionMatrix(std::vector<int> indices, InputTable table) {
    Data::Matrix3d transMat = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
    std::string currentIntervention = table["initial_block"][indices[0]];
    std::vector<std::string> keys;

    // reconstructing order of interventions based on column keys
    bool postFlag = false;
    for(auto inter : this->Config.GetInterventions()) {
        for(auto kv : table) {
            if(kv.first.find(inter) != std::string::npos) {
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
    }

    for(int i = 0; i < keys.size(); i++) {
        int interventionOffset = i;
        // int interventionOffset = (keys[i].find("post") != std::string::npos) ? i : i + indices[0];
        // std::cout << keys[i] << " " << interventionOffset << std::endl;

        std::string key = keys[i];
        int oudIdx = 0;
        int demIdx = 0;
        for (int idx : indices) {
            std::string val = table[key][idx];
            transMat(interventionOffset, oudIdx, demIdx) = std::stof(val);
            oudIdx++;
            if(oudIdx%this->numOUDStates == 0) {
                oudIdx = 0;
                demIdx++;
            }
        }

    }
    return transMat;
}

Data::Matrix3d DataLoader::BuildMatrixFromTransitionData(std::vector<std::vector<int>> indicesVec, InputTable table, Data::Dimension dimension) {
    if(dimension == Data::INTERVENTION) {
        Matrix3d stackingMatrices = Utilities::Matrix3dFactory::Create(this->numOUDStates,
        this->numInterventions * this->numInterventions,
        this->numDemographicCombos).constant(0);
        for (int i=0; i < indicesVec.size(); i++) {
            // assign to index + offset of numInterventions
            Eigen::array<Eigen::Index, 3> offsets = {i * this->numInterventions, 0,0};
            Eigen::array<Eigen::Index, 3> extents = {this->numInterventions, this->numOUDStates, this->numDemographicCombos};
            stackingMatrices.slice(offsets, extents) = this->BuildInterventionMatrix(indicesVec[i], table);
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

std::vector<int> DataLoader::FindIndices(std::vector<std::string> const &v, std::string target) {
    std::vector<int> indices;
    auto it = v.begin();
    while ((it = std::find_if(it, v.end(), [&] (std::string const &e) { return e.find(target) != std::string::npos; })) != v.end()) {
        indices.push_back(std::distance(v.begin(), it));
        it++;
    }
    return indices;
}

std::vector<int> DataLoader::FindIndicesExactMatch(std::vector<std::string> const &v, std::string target) {
    std::vector<int> indices;
    auto it = v.begin();
    while ((it = std::find_if(it, v.end(), [&] (std::string const &e) { return e.compare(target) == 0; })) != v.end()) {
        indices.push_back(std::distance(v.begin(), it));
        it++;
    }
    return indices;
}

std::vector<std::vector<int>> DataLoader::GetIndicesVector(std::vector<std::string> col) {
    std::vector<std::vector<int>> indicesVec;
    for(std::string in : this->Config.GetInterventions()) {
        indicesVec.push_back(this->FindIndicesExactMatch(col, in));
    }
    return indicesVec;
}

Matrix3dOverTime DataLoader::CalcInterventionTransitions(std::vector<int> ict, InputTable table, std::vector<std::vector<int>> indicesVec) {
    Matrix3dOverTime m3dot;

    for (int timestep: ict) {
        // Get rid of the pointless columns for this iteration
        std::string str_timestep = "cycle" + std::to_string(timestep);
        InputTable currentTimeTable = this->RemoveColumns(str_timestep, table);
        m3dot.insert(this->BuildMatrixFromTransitionData(indicesVec, currentTimeTable, Data::INTERVENTION), timestep);
    }
    return m3dot;
}

/// @brief
/// @param csvName
void DataLoader::LoadInterventionTransitions(std::string csvName) {

    // INTERVENTION TRANSITIONS
    InputTable interventionTransitionTable = inputTables[csvName];
    std::vector<int> ict = this->Config.GetInterventionChangeTimes();

    std::vector<std::vector<int>> indicesVec = this->GetIndicesVector(interventionTransitionTable["initial_block"]);
    this->interventionTransitions = this->CalcInterventionTransitions(ict, interventionTransitionTable, indicesVec);

}

Matrix3d DataLoader::BuildOverdoseTransitions(InputTable table, std::string key) {
    std::vector<std::string> oudStates = this->Config.GetOUDStates();

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
                    std::stof(table[key][row]);
                    ++row;
                }
            }
        }
    }
    return overdoseTransitionsCycle;
}

/// @brief
/// @param csvName
void DataLoader::LoadOverdoseTransitions(std::string csvName) {
    // OVERDOSE
    InputTable overdoseTransitionTable = this->inputTables[csvName];
    std::vector<int> oct = this->Config.GetOverdoseChangeTimes();

    for(auto timestep : oct) {
        std::string str_timestep = "cycle" + std::to_string(timestep);
        InputTable currentTimeTable = this->RemoveColumns(str_timestep, overdoseTransitionTable);
        for(auto kv : currentTimeTable) {
            std::string str = kv.first;
            if(str.find("overdose_cycle") != std::string::npos) {
                this->overdoseTransitions.insert(this->BuildOverdoseTransitions(currentTimeTable, str), timestep);
            }
        }
    }
}

void DataLoader::LoadFatalOverdoseTransitions(std::string csvName) {
    InputTable fatalOverdoseTable      = inputTables[csvName];
    std::vector<Matrix3d> tempFatalOverdoseTransitions;
    for (int timestep: this->Config.GetOverdoseChangeTimes()) {
        Matrix3d overdoseTransition = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos).constant(0);
        // fatal overdose is a constant across all strata
        std::string fodColumn = "fatal_to_all_types_overdose_ratio_cycle" + std::to_string(timestep);
        this->fatalOverdoseTransitions.insert(
            Utilities::Matrix3dFactory::Create(
                this->numOUDStates, this->numInterventions, this->numDemographicCombos
            ).constant(std::stof(fatalOverdoseTable[fodColumn][0])), timestep
        );
    }
}

/// @brief
/// @param csvName
void DataLoader::LoadMortalityTransitions(std::string csvName) {

    // MORTALITY TRANSITIONS
    // mortality here refers to death from reasons other than oud and is calculated
    // by combining the SMR and background mortality
    // calculation to combine these into the mortality is 1-exp(log(1-bg_mort)*SMR)
    std::vector<std::string> smrColumn                 = inputTables[csvName]["SMR"];
    // only stratified by the demographics, needs to be expanded for oud and intervention
    std::vector<std::string> backgroundMortalityColumn = inputTables["background_mortality.csv"]["death_prob"];
    std::vector<std::string> backgroundMortality;
    Matrix3d mortalityTransition = Utilities::Matrix3dFactory::Create(this->numOUDStates, this->numInterventions, this->numDemographicCombos);
    // make the dimensions match between SMR and background_mortality
    for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
        for (int oud = 0; oud < this->numOUDStates; ++oud) {
            backgroundMortality.insert(backgroundMortality.end(), backgroundMortalityColumn.begin(),
                                       backgroundMortalityColumn.end());
        }
    }
    // mortality is one element per stratum, no time variability
    for (int intervention = 0; intervention < this->numInterventions; ++intervention) {
        for (int age = 0; age < this->demographicCounts.at(0); ++age) {
            for (int sex = 0; sex < this->demographicCounts.at(1); ++sex) {
                for (int oud = 0; oud < this->numOUDStates; ++oud) {
                    int index =
                        intervention * this->numDemographicCombos * this->numOUDStates +
                        age * this->demographicCounts.at(1) * this->numOUDStates +
                        sex * this->numOUDStates + oud;
                    mortalityTransition(intervention, oud, (age * this->demographicCounts.at(1)) + sex) =
                        1 - exp(log(1 - std::stof(backgroundMortality[index]))*std::stof(smrColumn[index]));
                }
            }
        }
    }
    mortalityTransitions = mortalityTransition;
}

/// @brief Reads a configuration file to a Configuration object
/// @param inputFile path to the configuration file to be read
/// @return
Configuration DataLoader::readConfigFile(std::string inputFile) {
    Configuration config(inputFile);
    return config;
}

/// @brief Read a CSV-formatted file into a map object where the headers are
/// keys and the rest of the columns are stored as vectors of strings
/// @param inputFile path to the CSV to be read
/// @return A map object containing columns stored as key-value pairs
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

/// @brief Reads typical RESPOND input files from the provided input directory
/// @param inputDir the directory from which to read input files
/// @return an unordered map whose keys are table names and values are
/// CSV/InputTables
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

/// @brief A convenience function useful for viewing `Matrix3d` objects as 2d
/// slices along the demographics axis -- used for printf debugging
/// @param toPrint the `Matrix3d` object to print
void printMatrix3d(Matrix3d toPrint) {
    Matrix3d::Dimensions dim = toPrint.dimensions();
    for (int i = 0; i < dim[2]; ++i) {
        // intervention iterated along columns
        for (int j = 0; j < dim[0]; ++j) {
            // oud state is the row
            for (int k = 0; k < dim[1]; ++k) {
                std::cout << toPrint(j,k,i) << " ";
            }
            std::cout << std::endl;
        }
        if (i != (dim[2] - 1)) {
            std::cout << "===========================" << std::endl;
        }
    }
}
