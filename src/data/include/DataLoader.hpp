#ifndef DATA_DATALOADER_HPP_
#define DATA_DATALOADER_HPP_

#include <boost/tokenizer.hpp>
#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <unordered_map>
#include <vector>
#include <map>
#include <fmt/core.h>

#include "Configuration.hpp"
#include "DataTypes.hpp"
#include "Loader.hpp"
#include "Matrix3dFactory.hpp"
#include "Matrix3dPrinter.hpp"

namespace Data {
    /*!
     * @brief The object that processes model inputs
     *
     * DataLoader handles the primary configuration file, `sim.conf`, and the
     * tabular inputs necessary to run the model.
     *
     * Updated **[2023-04-28]**
     *
     * **Required Input Tables:**
     * - `sim.conf`
     * - `all_types_overdose.csv`
     * - `background_mortality.csv`
     * - `block_init_effect.csv`
     * - `block_trans.csv`
     * - `entering_cohort.csv`
     * - `fatal_overdose.csv`
     * - `init_cohort.csv`
     * - `oud_trans.csv`
     * - `SMR.csv`
     *
     * **Optional Input Tables:**
     * - `bg_utility.csv`
     * - `healthcare_utilization_cost.csv`
     * - `oud_utility.csv`
     * - `overdose_cost.csv`
     * - `pharmaceutical_cost.csv`
     * - `setting_utility.csv`
     * - `treatment_utilization_cost.csv`
     */
    class DataLoader : public Loader {
    public:
        // CONSTRUCTORS
        /// @brief The default constructor for DataLoader.
        ///
        /// This constructor initializes all members to zero or to empty objects.
        DataLoader();
        /// @brief A constructor for DataLoader that generates necessary model
        /// objects based on the contents of the provided input directory
        /// @param inputDir the name of the directory where input files are stored
        DataLoader(std::string inputDir);
        /// @brief An alternative constructor for DataLoader for loading data
        /// when a Configuration object has already been created prior to
        /// specifying the directory containing tabular inputs
        /// @param config The object containing parameters read from an
        /// already-processed configuration file
        /// @param inputDir The name of the directory where input files are stored
        DataLoader(Configuration config, std::string inputDir);
        ~DataLoader() {};

        // PUBLIC METHODS
        // GETTERS
        Configuration loadConfigurationFile(std::string configPath);
        /// @brief Get the Directory Name
        /// @return String Directory Name
        std::string getDirName() { return dirName; }
        /// @brief Get the Simulation Duration
        /// @return Integer Duration
        int getDuration() { return duration; }
        /// @brief Get Number of OUD States
        /// @return Integer Number of OUD States
        int getNumOUDStates() { return numOUDStates; }
        /// @brief Get Number of Integerventions
        /// @return Integer Number of Interventions
        int getNumInterventions() { return numInterventions; }
        /// @brief Get the Number of Demographics
        /// @return Integer Number of Demographics
        int getNumDemographics() { return numDemographics; }
        /// @brief Get the Initial Sample
        /// @return Matrix3d Initial Sample
        Matrix3d getInitialSample() { return initialSample; }
        /// @brief Get the Entering Samples
        /// @return Matrix3dOverTime Entering Samples
        Matrix3dOverTime getEnteringSamples() { return enteringSamples; }
        /// @brief Get the OUD Transition Rates
        /// @return Matrix3d OUD Transition Rates
        Matrix3d getOUDTransitionRates() { return oudTransitionRates; }
        /// @brief Get the Intervention Transition Rates
        /// @return Matrix3dOverTime Intervention Transition Rates
        Matrix3dOverTime getInterventionTransitionRates() { return interventionTransitionRates; }
        /// @brief Get the Overdose Rates
        /// @return Matrix3dOverTime Overdose Rates
        Matrix3dOverTime getOverdoseRates() { return overdoseRates; }
        /// @brief Get the Fatal Overdose Rates
        /// @return Matrix3dOverTime Fatal Overdose Rates
        Matrix3dOverTime getFatalOverdoseRates() { return fatalOverdoseRates; }
        /// @brief Get the Mortality Rates
        /// @return Matrix3d Mortality Rates
        Matrix3d getMortalityRates() { return mortalityRates; }
        /// @brief Get the Intervention Initialization Rates
        /// @return Matrix3d Intervention Initialization Rates
        Matrix3d getInterventionInitRates() { return interventionInitRates; }
        /// @brief Get the Interventions
        /// @return Vector of Strings Interventions
        std::vector<std::string> getInterventions() { return interventions; }
        /// @brief Get the OUD States
        /// @return Vector of Strings OUD States
        std::vector<std::string> getOUDStates() { return oudStates; }
        /// @brief Get the Aging Interval
        /// @return Integer Aging Interval
        int getAgingInterval() { return agingInterval; }
        /// @brief Get the Age Group Shift
        /// @return Integer Age Group Shift
        int getAgeGroupShift() { return ageGroupShift; }
        /// @brief
        /// @return
        bool getCostSwitch() { return costSwitch; }
        /// @brief
        /// @return
        std::vector<std::string> getCostPerspectives();

        // SETTERS
        /// @brief Load the Initial Sample from a File
        /// @param csvName Filename to the Initial Sample
        /// @return Matrix3d Initial Sample
        Matrix3d loadInitialSample(std::string csvName);

        /// @brief Load the Entering Samples from a File
        /// @param csvName Filename to the Entering Samples
        /// @param enteringSampleIntervention The Intervention to Load Entering Samples Into
        /// @param enteringSampleOUD The OUD State to Load Entering Samples Into
        /// @return Matrix3dOverTime Entering Samples
        Matrix3dOverTime loadEnteringSamples(std::string csvName, std::string enteringSampleIntervention, std::string enteringSampleOUD);

        /// @brief Load the OUD Transition Rates
        /// @param csvName Filename to the OUD Transition Rates
        /// @return Matrix3d OUD Transition Rates
        Matrix3d loadOUDTransitionRates(std::string csvName);

        /// @brief Load the Intervention Initialization Rates
        /// @param csvName Filename to the Intervention Initialization Rates
        /// @return Matrix3d Intervention Initialization Rates
        Matrix3d loadInterventionInitRates(std::string csvName);

        /// @brief Load the Intervention Transition Rates
        /// @param csvName Filename to the Intervention Transition Rates
        /// @return Matrix3dOverTime Intervention Transition Rates
        Matrix3dOverTime loadInterventionTransitionRates(std::string csvName);

        /// @brief Load the Overdose Rates
        /// @param csvName Filename to the Overdose Rates
        /// @return Matrix3dOverTime Overdose Rates
        Matrix3dOverTime loadOverdoseRates(std::string csvName);

        /// @brief Load the Fatal Overdose Rates
        /// @param csvName Filename to the Fatal Overdose Rates
        /// @return Matrix3dOverTime Fatal Overdose Rates
        Matrix3dOverTime loadFatalOverdoseRates(std::string csvName);

        /// @brief Load the Mortality Rates
        /// @param smrCSVName Filename to the SMR File
        /// @param bgmCSVName Filename to the Background Mortality File
        /// @return Matrix3d Mortality Rates
        Matrix3d loadMortalityRates(std::string smrCSVName, std::string bgmCSVName);

    private:
        InputTable removeColumns(std::string colString, InputTable ogTable);
        Matrix3d createTransitionMatrix3d(std::vector<std::vector<int>> indicesVec, InputTable table, Data::Dimension dimension);
        Matrix3d buildInterventionMatrix(std::vector<int> indices, InputTable table);
        Matrix3d buildOverdoseTransitions(InputTable table, std::string key);
        std::vector<int> findIndices(std::vector<std::string> const &v, std::string target);
        std::vector<std::vector<int>> getIndicesByIntervention(std::vector<std::string> col);
        Matrix3dOverTime buildTransitionRatesOverTime(std::vector<int> ict, InputTable table, std::vector<std::vector<int>> indicesVec);
        void populateCostParameters();

        std::string dirName;
        int duration;
        int agingInterval;
        int ageGroupShift;
        int numOUDStates;
        int numInterventions;
        int numDemographics;
        int numDemographicCombos;
        // cost parameters
        bool costSwitch;
        std::vector<std::string> costPerspectives;
        double discountRate;
        int reportingInterval;
        bool costCategoryOutputs;
        // output parameters
        bool perInterventionPredictions;
        bool generalOutputsSwitch;
        std::vector<int> generalStatsOutputTimesteps;

        std::vector<std::string> interventions;
        std::vector<std::string> oudStates;
        std::vector<int> demographicCounts;
        std::map<std::string, std::vector<int>> simulationParameters;

        Matrix3d initialSample;
        Matrix3dOverTime enteringSamples;
        Matrix3d oudTransitionRates;
        Matrix3d interventionInitRates;
        Matrix3dOverTime interventionTransitionRates;
        Matrix3dOverTime overdoseRates;
        Matrix3dOverTime fatalOverdoseRates;
        Matrix3d mortalityRates;
    };
}
#endif // DATA_DATALOADER_HPP_
