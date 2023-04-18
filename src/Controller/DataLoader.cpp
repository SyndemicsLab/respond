#include "DataLoader.hpp"

using namespace Data;

/// @brief
/// @param
DataLoader::DataLoader(const std::string& inputDir): dirName(inputDir) {
    // PROCESSING INPUT FILES
    // account for no trailing slash in the provided input directory
    std::string configPath = inputDir;
    if (configPath.back() != '/') {
        configPath.push_back('/');
    }
    configPath += "sim.conf";
    Configuration config = readConfigFile(configPath);
    std::unordered_map<std::string, InputTable> inputTables = readInputDir(inputDir);

    // SETTING STRING VECTORS FOR DATA WRITER
    this->interventions = config.interventions;
    this->oudStates = config.oud_states;

    // SETTING SIMULATION CONSTANTS
    const int& interventions   = config.interventions.size();
    const int& oud_states      = config.oud_states.size();
    const int& age_groups      = config.age_groups.size();
    const int& sexes           = config.sex.size();

    // needed for some post-intervention processes
    const int& nonPostInterventions = ((interventions - 1)/2 + 1);

    duration = config.duration;
    numOUDStates = oud_states;
    numInterventions = interventions;
    numDemographics = age_groups * sexes;

    // INITIAL GROUP
    InputTable& initialCohort = inputTables["init_cohort.csv"];
    initialGroup = Matrix3d(interventions, oud_states, age_groups * sexes).constant(0);
    int row = 0;
    for (int intervention = 0; intervention < nonPostInterventions; ++intervention) {
        for (int age = 0; age < age_groups; ++age) {
            for (int sex = 0; sex < sexes; ++sex) {
                for (int oud_state = 0; oud_state < oud_states; ++oud_state) {
                    initialGroup(intervention, oud_state, (age * sexes) + sex) =
                        std::stof(initialCohort["counts"][row]);
                    ++row;
                }
            }
        }
    }

    // ENTERING SAMPLES
    InputTable& enteringSamplesTable = inputTables["entering_cohort.csv"];
    // new population enters the reference state, "currently Active Non-Injection
    // in No Treatment" aka the first state in interventions and oud states
    int enteringSampleIntervention = 0;
    int enteringSampleOUD          = 0;
    std::vector<Matrix3d> tempSamples;
    std::string columnPrefix = "number_of_new_comers_cycle";
    // generate each unique `Matrix3d enteringSamples`
    for (int& timestep: config.entering_sample_change_times) {
        Matrix3d enteringSample(interventions, oud_states, age_groups * sexes);
        for (int intervention = 0; intervention < interventions; ++intervention) {
            for (int age = 0; age < age_groups; ++age) {
                for (int sex = 0; sex < sexes; ++sex) {
                    for (int oud = 0; oud < oud_states; ++oud) {
                        if ((intervention == enteringSampleIntervention) and (oud == enteringSampleOUD)) {
                            std::string column = columnPrefix + std::to_string(timestep);
                            int demographic = age * sexes + sex;
                            enteringSample(intervention, oud, demographic) =
                                std::stof(enteringSamplesTable[column][demographic]);
                        }
                        else {
                            enteringSample(intervention, oud, age * sexes + sex) = 0;
                        }
                    }
                }
            }
        }
        tempSamples.push_back(enteringSample);
    }

    // create copies as appropriate, based on the timesteps in config
    for (int i = 0; i < tempSamples.size(); ++i) {
        int tmpSize = enteringSamples.size();
        for (int j = tmpSize; j < config.entering_sample_change_times[i]; ++j) {
            enteringSamples.push_back(tempSamples[i]);
        }
    }

    // OUD TRANSITIONS
    InputTable& oudTransitionTable = inputTables["oud_trans.csv"];
    // end dimensions of oudTransitions are interventions x oud_states^2 x demographics
    // start with a vector of StateTensor-sized Matrix3d objects and stack at the end
    std::vector<Matrix3d> tempOUDTransitions;
    for (int i = 0; i < oud_states; ++i) {
        tempOUDTransitions.push_back(Matrix3d(interventions, oud_states, age_groups * sexes));
    }
    // Matrix3d objects in the vector are matched with the order that oud states are
    // specified in the config file. the order represents the "initial oud state"
    for (int initial_state = 0; initial_state < oud_states; ++initial_state) {
        int row = 0;
        for (int intervention = 0; intervention < interventions; ++intervention) {
            for (int age = 0; age < age_groups; ++age) {
                for (int sex = 0; sex < sexes; ++sex) {
                    for (int result_state = 0; result_state < oud_states; ++result_state) {
                        std::string column = "to_" + config.oud_states[result_state];
                        tempOUDTransitions[initial_state](intervention, result_state, (age * sexes) + sex) =
                            std::stof(oudTransitionTable[column][oud_states*row+initial_state]);
                    }
                    ++row;
                }
            }
        }
    }

    // stack the Matrix3d objects along the OUD axis
    oudTransitions = tempOUDTransitions[0];
    for (int i = 1; i < tempOUDTransitions.size(); ++i) {
        Matrix3d temp = oudTransitions.concatenate(tempOUDTransitions[i], Data::OUD).eval()
            .reshape(Matrix3d::Dimensions(interventions, (i+1) * oud_states, age_groups * sexes));
        oudTransitions = std::move(temp);
    }

    // INTERVENTION TRANSITIONS
    InputTable& interventionTransitionTable = inputTables["block_trans.csv"];
    // the first depth of vectors represents the time-variation
    // the internal set of vectors represents the transition tensors for each initial intervention state
    std::vector<std::vector<Matrix3d>> tempInterventionTransitions;
    for (int& timestep: config.intervention_change_times) {
        // create the blank state matrices for each initial intervention
        std::vector<Matrix3d> interventionTransition;
        for (int i = 0; i < interventions; ++i) {
            interventionTransition.push_back(Matrix3d(interventions, oud_states, age_groups * sexes).constant(0));
        }
        for (int age = 0; age < age_groups; ++age) {
            for (int sex = 0; sex < sexes; ++sex) {
                for (int oud = 0; oud < oud_states; ++oud) {
                    for (int initial_inter = 0; initial_inter < interventions; ++initial_inter) {
                        // row indicates the initial state, the same row is used across all initial
                        // interventions because columns are broken into to_<result_inter>
                        int row = age * sexes * oud_states * interventions + sex * oud_states * interventions
                            + oud * interventions + initial_inter;
                        // iterate over the to_<result_inter> columns for the current timestep
                        // referencing nonPostInterventions due to post-treatments being condensed to
                        // `corresponding_post_trt`
                        for (int result_inter = 0; result_inter <= nonPostInterventions; ++result_inter) {
                            // the if statements below ensure that post-intervention states work out
                            // correctly, with a special check to avoid overwriting values originating
                            // from No Treatment
                            int toState;
                            std::string toStateString;
                            if (result_inter < nonPostInterventions) {
                                // considering entering a "normal"/non-post intervention state
                                toState       = result_inter;
                                toStateString = config.interventions[result_inter];
                            }
                            else {
                                // considering post-treatment states
                                // if the current intervention is greater than result_inter aka nonPostInterventions
                                // it is a post-intervention state already
                                // special handling for coming from No Treatment
                                if (initial_inter == 0) {
                                    toState = result_inter;
                                }
                                else {
                                    toState = (initial_inter >= result_inter) ?
                                        initial_inter : result_inter + initial_inter - 1;
                                }
                                // post-treatments are grouped into "corresponding_post_trt"
                                toStateString = "corresponding_post_trt";
                            }
                            std::string column = "to_" + toStateString + "_cycle"
                                + std::to_string(timestep);
                            interventionTransition[row % interventions](toState, oud, (age * sexes) + sex) =
                                std::stof(interventionTransitionTable[column][row]);
                        }
                    }
                }
            }
        }
        tempInterventionTransitions.push_back(interventionTransition);
    }
    // stack the intervention Matrix3d objects along the intervention axis
    std::vector<Matrix3d> interventionTransitionHolder;
    for (std::vector<Matrix3d>& intervention: tempInterventionTransitions) {
        Matrix3d tempIntervention = intervention[0];
        for (int i = 1; i < intervention.size(); ++i) {
            Matrix3d temp = tempIntervention.concatenate(intervention[i], Data::INTERVENTION).eval()
                .reshape(Matrix3d::Dimensions((i+1) * interventions, oud_states, age_groups * sexes));
            tempIntervention = std::move(temp);
        }
        interventionTransitionHolder.push_back(tempIntervention);
    }
    // create copies as appropriate, based on the timesteps in the config
    for (int i = 0; i < interventionTransitionHolder.size(); ++i) {
        int tmpSize = interventionTransitions.size();
        for (int j = tmpSize; j < config.intervention_change_times[i]; ++j) {
            interventionTransitions.push_back(interventionTransitionHolder[i]);
        }
    }

    // OVERDOSE
    InputTable& overdoseTransitionTable = inputTables["all_types_overdose.csv"];
    InputTable& fatalOverdoseTable      = inputTables["fatal_overdose.csv"];
    std::vector<Matrix3d> tempOverdoseTransitions;
    std::vector<Matrix3d> tempFatalOverdoseTransitions;
    for (int& timestep: config.overdose_change_times) {
        Matrix3d overdoseTransition = Matrix3d(interventions, oud_states, age_groups * sexes).constant(0);
        // fatal overdose is a constant across all strata
        std::string fodColumn = "fatal_to_all_types_overdose_ratio_cycle" + std::to_string(timestep);
        Matrix3d fatalOverdose = Matrix3d(interventions, oud_states, age_groups * sexes)
            .constant(std::stof(fatalOverdoseTable[fodColumn][0]));
        // regular overdose is straightforward to convert to the state tensor, one element
        // per stratum per timestep
        int row = 0;
        for (int intervention = 0; intervention < interventions; ++intervention) {
            for (int age = 0; age < age_groups; ++age) {
                for (int sex = 0; sex < sexes; ++sex) {
                    for (int oud = 0; oud < oud_states; ++oud) {
                        // only active oud states can overdose
                        if (oud < oud_states/2) {
                            std::string column = "all_types_overdose_cycle" + std::to_string(timestep);
                            overdoseTransition(intervention, oud, (age * sexes) + sex) = stof(overdoseTransitionTable[column][row]);
                            ++row;
                        }
                    }
                }
            }
        }
        tempOverdoseTransitions.push_back(overdoseTransition);
        tempFatalOverdoseTransitions.push_back(fatalOverdose);
    }
    // create copies as appropriate, based on the timesteps in the config
    for (int i = 0; i < tempOverdoseTransitions.size(); ++i) {
        // only use one temporary variable, because regular overdose and fatal overdose both have the same change timesteps
        int tmpOverdoseSize = overdoseTransitions.size();
        for (int j = tmpOverdoseSize; j < config.overdose_change_times[i]; ++j) {
            overdoseTransitions.push_back(tempOverdoseTransitions[i]);
            fatalOverdoseTransitions.push_back(tempFatalOverdoseTransitions[i]);
        }
    }

    // MORTALITY TRANSITIONS
    // mortality here refers to death from reasons other than oud and is calculated
    // by combining the SMR and background mortality
    // calculation to combine these into the mortality is 1-exp(log(1-bg_mort)*SMR)
    std::vector<std::string> smrColumn                 = inputTables["SMR.csv"]["SMR"];
    // only stratified by the demographics, needs to be expanded for oud and intervention
    std::vector<std::string> backgroundMortalityColumn = inputTables["background_mortality.csv"]["death_prob"];
    std::vector<std::string> backgroundMortality;
    Matrix3d mortalityTransition(interventions, oud_states, age_groups * sexes);
    // make the dimensions match between SMR and background_mortality
    for (int intervention = 0; intervention < interventions; ++intervention) {
        for (int oud = 0; oud < oud_states; ++oud) {
            backgroundMortality.insert(backgroundMortality.end(), backgroundMortalityColumn.begin(),
                                       backgroundMortalityColumn.end());
        }
    }
    // mortality is one element per stratum, no time variability
    for (int intervention = 0; intervention < interventions; ++intervention) {
        for (int age = 0; age < age_groups; ++age) {
            for (int sex = 0; sex < sexes; ++sex) {
                for (int oud = 0; oud < oud_states; ++oud) {
                    int index =
                        intervention * age_groups * sexes * oud_states +
                        age * sexes * oud_states +
                        sex * oud_states + oud;
                    mortalityTransition(intervention, oud, (age * sexes) + sex) =
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
Configuration DataLoader::readConfigFile(const std::string& inputFile) {
    return configure(inputFile);
}

/// @brief Read a CSV-formatted file into a map object where the headers are
/// keys and the rest of the columns are stored as vectors of strings
/// @param inputFile path to the CSV to be read
/// @return A map object containing columns stored as key-value pairs
InputTable DataLoader::readCSV(const std::string& inputFile) {
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
std::unordered_map<std::string, InputTable> DataLoader::readInputDir(const std::string& inputDir) {
    std::string inputDirFixed = inputDir;
    std::unordered_map<std::string, InputTable> toReturn;
    // account for no trailing slash in the provided input directory
    if (inputDirFixed.back() != '/') {
        inputDirFixed.push_back('/');
    }

    for (std::string& inputFile: INPUT_FILES) {
        toReturn[inputFile] = readCSV(inputDirFixed + inputFile);
    }
    return toReturn;
}

/// @brief A convenience function useful for viewing `Matrix3d` objects as 2d
/// slices along the demographics axis -- used for printf debugging
/// @param toPrint the `Matrix3d` object to print
void printMatrix3d(Matrix3d& toPrint) {
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
