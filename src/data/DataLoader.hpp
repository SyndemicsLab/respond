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
        /// @brief
        /// @return
        std::string getDirName() { return dirName; }
        /// @brief
        /// @return
        int getDuration() { return duration; }
        /// @brief
        /// @return
        int getNumOUDStates() { return numOUDStates; }
        /// @brief
        /// @return
        int getNumInterventions() { return numInterventions; }
        /// @brief
        /// @return
        int getNumDemographics() { return numDemographics; }
        /// @brief
        /// @return
        Matrix3d getInitialSample() { return initialSample; }
        /// @brief
        /// @return
        Matrix3dOverTime getEnteringSamples() { return enteringSamples; }
        /// @brief
        /// @return
        Matrix3d getOUDTransitionRates() { return oudTransitionRates; }
        /// @brief
        /// @return
        Matrix3dOverTime getInterventionTransitionRates() { return interventionTransitionRates; }
        /// @brief
        /// @return
        Matrix3dOverTime getOverdoseRates() { return overdoseRates; }
        /// @brief
        /// @return
        Matrix3dOverTime getFatalOverdoseRates() { return fatalOverdoseRates; }
        /// @brief
        /// @return
        Matrix3d getMortalityRates() { return mortalityRates; }
        /// @brief
        /// @return
        Matrix3d getInterventionInitRates() { return interventionInitRates; }
        /// @brief
        /// @return
        std::vector<std::string> getInterventions() { return interventions; }
        /// @brief
        /// @return
        std::vector<std::string> getOUDStates() { return oudStates; }
        /// @brief
        /// @return
        int getAgingInterval() { return agingInterval; }
        /// @brief
        /// @return
        int getAgeGroupShift() { return ageGroupShift; }

        // SETTERS
        /// @brief
        /// @param csvName
        /// @return
        Matrix3d loadInitialSample(std::string csvName);
        /// @brief
        /// @param csvName
        /// @return
        Matrix3dOverTime loadEnteringSamples(std::string csvName);
        /// @brief
        /// @param csvName
        /// @return
        Matrix3d loadOUDTransitionRates(std::string csvName);
        /// @brief
        /// @param csvName
        /// @return
        Matrix3d loadInterventionInitRates(std::string csvName);
        /// @brief
        /// @param csvName
        /// @return
        Matrix3dOverTime loadInterventionTransitionRates(std::string csvName);
        /// @brief
        /// @param csvName
        /// @return
        Matrix3dOverTime loadOverdoseRates(std::string csvName);
        /// @brief
        /// @param csvName
        /// @return
        Matrix3dOverTime loadFatalOverdoseRates(std::string csvName);
        /// @brief
        /// @param smrCSVName
        /// @param bgmCSVName
        /// @return
        Matrix3d loadMortalityRates(std::string smrCSVName, std::string bgmCSVName);

    private:
        InputTable removeColumns(std::string colString, InputTable ogTable);
        Matrix3d createTransitionMatrix3d(std::vector<std::vector<int>> indicesVec, InputTable table, Data::Dimension dimension);
        Matrix3d buildInterventionMatrix(std::vector<int> indices, InputTable table);
        Matrix3d buildOverdoseTransitions(InputTable table, std::string key);
        std::vector<int> findIndices(std::vector<std::string> const &v, std::string target);
        std::vector<std::vector<int>> getIndicesByIntervention(std::vector<std::string> col);
        Matrix3dOverTime buildTransitionRatesOverTime(std::vector<int> ict, InputTable table, std::vector<std::vector<int>> indicesVec);

        std::string dirName;
        int duration;
        int agingInterval;
        int ageGroupShift;
        int numOUDStates;
        int numInterventions;
        int numDemographics;
        int numDemographicCombos;

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
