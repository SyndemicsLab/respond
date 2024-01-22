//===-- DataLoader.hpp - DataLoader class definition ------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the DataLoader class.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#ifndef DATA_DATALOADER_HPP_
#define DATA_DATALOADER_HPP_

#include <iostream>
#include <map>
#include <string>
#include <vector>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"

#include "Configuration.hpp"
#include "DataTypes.hpp"
#include "Loader.hpp"

namespace Data {
    class IDataLoader : public virtual ILoader {
    public:
        /// @brief Get the Directory Name
        /// @return String Directory Name
        virtual std::string getDirName() const = 0;

        /// @brief Get the Simulation Duration
        /// @return Integer Duration
        virtual int getDuration() const = 0;

        /// @brief Get Number of OUD States
        /// @return Integer Number of OUD States
        virtual int getNumOUDStates() const = 0;

        /// @brief Get Number of Integerventions
        /// @return Integer Number of Interventions
        virtual int getNumInterventions() const = 0;

        /// @brief Get the Number of Demographics
        /// @return Integer Number of Demographics
        virtual int getNumDemographics() const = 0;

        /// @brief Get the Number of Demographics
        /// @return Integer Number of Demographics
        virtual int getNumDemographicCombos() const = 0;

        /// @brief Get the Initial Sample
        /// @return Matrix3d Initial Sample
        virtual Matrix3d getInitialSample() const = 0;

        /// @brief Get the Entering Samples
        /// @return Matrix3dOverTime Entering Samples
        virtual Matrix3dOverTime getEnteringSamples() const = 0;

        /// @brief Get the OUD Transition Rates
        /// @return Matrix3d OUD Transition Rates
        virtual Matrix3d getOUDTransitionRates() const = 0;

        /// @brief Get the Intervention Transition Rates
        /// @return Matrix3dOverTime Intervention Transition Rates
        virtual Matrix3dOverTime getInterventionTransitionRates() const = 0;

        /// @brief Get the Overdose Rates
        /// @return Matrix3dOverTime Overdose Rates
        virtual Matrix3dOverTime getOverdoseRates() const = 0;

        /// @brief Get the Fatal Overdose Rates
        /// @return Matrix3dOverTime Fatal Overdose Rates
        virtual Matrix3dOverTime getFatalOverdoseRates() const = 0;

        /// @brief Get the Mortality Rates
        /// @return Matrix3d Mortality Rates
        virtual Matrix3d getMortalityRates() const = 0;

        /// @brief Get the Intervention Initialization Rates
        /// @return Matrix3d Intervention Initialization Rates
        virtual Matrix3d getInterventionInitRates() const = 0;

        /// @brief Get the Interventions
        /// @return Vector of Strings Interventions
        virtual std::vector<std::string> getInterventions() const = 0;

        /// @brief Get the OUD States
        /// @return Vector of Strings OUD States
        virtual std::vector<std::string> getOUDStates() const = 0;

        /// @brief Get the Aging Interval
        /// @return Integer Aging Interval
        virtual int getAgingInterval() const = 0;

        /// @brief Get the Age Group Shift
        /// @return Integer Age Group Shift
        virtual int getAgeGroupShift() const = 0;

        /// @brief Determine if cost analysis is on or off
        /// @return Boolean true if user config specifies to include cost
        /// analysis, otherwise false
        virtual bool getCostSwitch() const = 0;

        /// @brief Get the vector of cost perspectives if cost analysis is on,
        /// otherwise, get an empty vector
        /// @return Vector of Strings representing perspectives from which to
        /// consider costs
        virtual std::vector<std::string> getCostPerspectives() const = 0;

        /// @brief Get the decimal representation of the discount rate applied
        /// to cost, provided that cost analysis is enabled. Otherwise, get 0.0
        /// @return Double Discount Rate
        virtual double getDiscountRate() const = 0;

        /// @brief Get the bin size for cost reporting timesteps if cost
        /// analysis is enabled. Otherwise, get 0
        /// @return Integer cost reporting timestep bin size
        virtual std::vector<int> getCostUtilityOutputTimesteps() const = 0;

        /// @brief Get the user config variable specifying whether the cost
        /// outputs should be broken down by cost perspective. Always returns
        /// false if cost analysis is disabled.
        /// @return Boolean cost category breakdown switch
        virtual bool getCostCategoryOutputs() const = 0;

        /// @brief Get a boolean which represents whether to break down outputs
        /// by interventions or to group all interventions in a single output
        /// @return Boolean per intervention outputs
        virtual bool getPerInterventionPredictions() const = 0;

        /// @brief Get a boolean which represents whether to generate an output
        /// file that contains the entire state tensor object across all
        /// timesteps
        /// @return Boolean general outputs switch
        virtual bool getGeneralOutputsSwitch() const = 0;

        /// @brief Select which timesteps to provide general statistics output
        /// @return Vector of Integers representing the timesteps at which to
        /// print output
        virtual std::vector<int> getGeneralStatsOutputTimesteps() const = 0;

        /// @brief Load the Initial Sample from a File
        /// @param csvName Filename to the Initial Sample
        /// @return Matrix3d Initial Sample
        virtual Matrix3d loadInitialSample(std::string const &csvName) = 0;

        /// @brief Load the Entering Samples from a File
        /// @param csvName Filename to the Entering Samples
        /// @param enteringSampleIntervention The Intervention to Load Entering
        /// Samples Into
        /// @param enteringSampleOUD The OUD State to Load Entering Samples Into
        /// @return Matrix3dOverTime Entering Samples
        virtual Matrix3dOverTime
        loadEnteringSamples(std::string const &csvName,
                            std::string const &enteringSampleIntervention,
                            std::string const &enteringSampleOUD) = 0;

        /// @brief Load the OUD Transition Rates
        /// @param csvName Filename to the OUD Transition Rates
        /// @return Matrix3d OUD Transition Rates
        virtual Matrix3d loadOUDTransitionRates(std::string const &csvName) = 0;

        /// @brief Load the Intervention Initialization Rates
        /// @param csvName Filename to the Intervention Initialization Rates
        /// @return Matrix3d Intervention Initialization Rates
        virtual Matrix3d
        loadInterventionInitRates(std::string const &csvName) = 0;

        /// @brief Load the Intervention Transition Rates
        /// @param csvName Filename to the Intervention Transition Rates
        /// @return Matrix3dOverTime Intervention Transition Rates
        virtual Matrix3dOverTime
        loadInterventionTransitionRates(std::string const &csvName) = 0;

        /// @brief Load the Overdose Rates
        /// @param csvName Filename to the Overdose Rates
        /// @return Matrix3dOverTime Overdose Rates
        virtual Matrix3dOverTime
        loadOverdoseRates(std::string const &csvName) = 0;

        /// @brief Load the Fatal Overdose Rates
        /// @param csvName Filename to the Fatal Overdose Rates
        /// @return Matrix3dOverTime Fatal Overdose Rates
        virtual Matrix3dOverTime
        loadFatalOverdoseRates(std::string const &csvName) = 0;

        /// @brief Load the Mortality Rates
        /// @param smrCSVName Filename to the SMR File
        /// @param bgmCSVName Filename to the Background Mortality File
        /// @return Matrix3d Mortality Rates
        virtual Matrix3d loadMortalityRates(std::string const &smrCSVName,
                                            std::string const &bgmCSVName) = 0;
    };

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
    class DataLoader : public Loader, public IDataLoader {
    public:
        /// @brief The default constructor for DataLoader.
        /// This constructor initializes all members to zero or to empty
        /// objects.
        DataLoader();

        /// @brief A constructor for DataLoader that generates necessary model
        /// objects based on the contents of the provided input directory
        /// @param inputDir the name of the directory where input files are
        /// stored
        DataLoader(std::string const &inputDir,
                   std::shared_ptr<spdlog::logger> logger = {});

        /// @brief An alternative constructor for DataLoader for loading data
        /// when a Configuration object has already been created prior to
        /// specifying the directory containing tabular inputs
        /// @param config The object containing parameters read from an
        /// already-processed configuration file
        /// @param inputDir The name of the directory where input files are
        /// stored
        DataLoader(Configuration &config, std::string const &inputDir,
                   std::shared_ptr<spdlog::logger> logger = {});

        ~DataLoader(){};

        /// @brief
        /// @param configPath
        /// @return
        Configuration loadConfigurationFile(std::string const &configPath);

        virtual std::string getDirName() const { return dirName; }

        virtual int getDuration() const { return duration; }

        virtual int getNumOUDStates() const { return numOUDStates; }

        virtual int getNumInterventions() const { return numInterventions; }

        virtual int getNumDemographics() const { return numDemographics; }

        virtual int getNumDemographicCombos() const {
            return numDemographicCombos;
        }

        virtual Matrix3d getInitialSample() const { return initialSample; }

        virtual Matrix3dOverTime getEnteringSamples() const {
            return enteringSamples;
        }

        virtual Matrix3d getOUDTransitionRates() const {
            return oudTransitionRates;
        }

        virtual Matrix3dOverTime getInterventionTransitionRates() const {
            return interventionTransitionRates;
        }

        virtual Matrix3dOverTime getOverdoseRates() const {
            return overdoseRates;
        }

        virtual Matrix3dOverTime getFatalOverdoseRates() const {
            return fatalOverdoseRates;
        }

        virtual Matrix3d getMortalityRates() const { return mortalityRates; }

        virtual Matrix3d getInterventionInitRates() const {
            return interventionInitRates;
        }

        virtual std::vector<std::string> getInterventions() const {
            return interventions;
        }

        virtual std::vector<std::string> getOUDStates() const {
            return oudStates;
        }

        virtual int getAgingInterval() const { return agingInterval; }

        virtual int getAgeGroupShift() const { return ageGroupShift; }

        virtual bool getCostSwitch() const { return costSwitch; }

        virtual std::vector<std::string> getCostPerspectives() const;

        virtual double getDiscountRate() const;

        virtual std::vector<int> getCostUtilityOutputTimesteps() const;

        virtual bool getCostCategoryOutputs() const;

        virtual bool getPerInterventionPredictions() const {
            return perInterventionPredictions;
        }

        virtual bool getGeneralOutputsSwitch() const {
            return generalOutputsSwitch;
        }

        virtual std::vector<int> getGeneralStatsOutputTimesteps() const {
            return generalStatsOutputTimesteps;
        }

        virtual Matrix3d loadInitialSample(std::string const &csvName);

        virtual Matrix3dOverTime
        loadEnteringSamples(std::string const &csvName,
                            std::string const &enteringSampleIntervention,
                            std::string const &enteringSampleOUD);

        virtual Matrix3d loadOUDTransitionRates(std::string const &csvName);

        virtual Matrix3d loadInterventionInitRates(std::string const &csvName);

        virtual Matrix3dOverTime
        loadInterventionTransitionRates(std::string const &csvName);

        virtual Matrix3dOverTime loadOverdoseRates(std::string const &csvName);

        virtual Matrix3dOverTime
        loadFatalOverdoseRates(std::string const &csvName);

        virtual Matrix3d loadMortalityRates(std::string const &smrCSVName,
                                            std::string const &bgmCSVName);

    private:
        /// @brief
        /// @param colString
        /// @param ogTable
        /// @return
        InputTable removeColumns(std::string const &colString,
                                 InputTable const &ogTable);

        /// @brief
        /// @param indicesVec
        /// @param table
        /// @param dimension
        /// @return
        Matrix3d createTransitionMatrix3d(
            std::vector<std::vector<int>> const &indicesVec,
            InputTable const &table, Data::Dimension dimension);

        /// @brief
        /// @param indices
        /// @param table
        /// @return
        Matrix3d buildInterventionMatrix(std::vector<int> const &indices,
                                         InputTable const &table);

        /// @brief
        /// @param table
        /// @param key
        /// @return
        Matrix3d buildOverdoseTransitions(InputTable const &table,
                                          std::string const &key);

        /// @brief
        /// @param v
        /// @param target
        /// @return
        std::vector<int> findIndices(std::vector<std::string> const &v,
                                     std::string const &target);

        /// @brief
        /// @param col
        /// @return
        std::vector<std::vector<int>>
        getIndicesByIntervention(std::vector<std::string> const &col);

        /// @brief
        /// @param ict
        /// @param table
        /// @param indicesVec
        /// @return
        Matrix3dOverTime buildTransitionRatesOverTime(
            std::vector<int> const &ict, InputTable const &table,
            std::vector<std::vector<int>> const &indicesVec);

        /// @brief
        void populateCostParameters();

        std::shared_ptr<spdlog::logger> logger;
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
        std::vector<int> costUtilityOutputTimesteps;
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
} // namespace Data
#endif // DATA_DATALOADER_HPP_
