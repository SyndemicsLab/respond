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

#include "DataTypes.hpp"
#include "Loader.hpp"

namespace Matrixify {
    class IDataLoader : public virtual ILoader {
    public:
        /// @brief Get the Initial Sample
        /// @return Matrix3d Initial Sample
        virtual Matrix3d getInitialSample() const = 0;

        /// @brief Get the Entering Samples
        /// @return Matrix4d Entering Samples
        virtual Matrix4d getEnteringSamples() const = 0;

        /// @brief Get the OUD Transition Rates
        /// @return Matrix3d OUD Transition Rates
        virtual Matrix3d getOUDTransitionRates() const = 0;

        /// @brief Get the Intervention Transition Rates
        /// @return Matrix4d Intervention Transition Rates
        virtual Matrix4d getInterventionTransitionRates() const = 0;

        /// @brief Get the Overdose Rates
        /// @return Matrix4d Overdose Rates
        virtual Matrix4d getOverdoseRates() const = 0;

        /// @brief Get the Fatal Overdose Rates
        /// @return Matrix4d Fatal Overdose Rates
        virtual Matrix4d getFatalOverdoseRates() const = 0;

        /// @brief Get the Mortality Rates
        /// @return Matrix3d Mortality Rates
        virtual Matrix3d getMortalityRates() const = 0;

        /// @brief Get the Intervention Initialization Rates
        /// @return Matrix3d Intervention Initialization Rates
        virtual Matrix3d getInterventionInitRates() const = 0;

        /// @brief Load the Initial Sample from a File
        /// @param csvName Filename to the Initial Sample
        /// @return Matrix3d Initial Sample
        virtual Matrix3d loadInitialSample(std::string const &csvName) = 0;

        /// @brief Load the Entering Samples from a File
        /// @param csvName Filename to the Entering Samples
        /// @param enteringSampleIntervention The Intervention to Load Entering
        /// Samples Into
        /// @param enteringSampleOUD The OUD State to Load Entering Samples Into
        /// @return Matrix4d Entering Samples
        virtual Matrix4d
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
        /// @return Matrix4d Intervention Transition Rates
        virtual Matrix4d
        loadInterventionTransitionRates(std::string const &csvName) = 0;

        /// @brief Load the Overdose Rates
        /// @param csvName Filename to the Overdose Rates
        /// @return Matrix4d Overdose Rates
        virtual Matrix4d loadOverdoseRates(std::string const &csvName) = 0;

        /// @brief Load the Fatal Overdose Rates
        /// @param csvName Filename to the Fatal Overdose Rates
        /// @return Matrix4d Fatal Overdose Rates
        virtual Matrix4d loadFatalOverdoseRates(std::string const &csvName) = 0;

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
        DataLoader(Data::IConfigurationPtr &config, std::string const &inputDir,
                   std::shared_ptr<spdlog::logger> logger = {});

        ~DataLoader(){};

        virtual Matrix3d getInitialSample() const { return initialSample; }

        virtual Matrix4d getEnteringSamples() const { return enteringSamples; }

        virtual Matrix3d getOUDTransitionRates() const {
            return oudTransitionRates;
        }

        virtual Matrix4d getInterventionTransitionRates() const {
            return interventionTransitionRates;
        }

        virtual Matrix4d getOverdoseRates() const { return overdoseRates; }

        virtual Matrix4d getFatalOverdoseRates() const {
            return fatalOverdoseRates;
        }

        virtual Matrix3d getMortalityRates() const { return mortalityRates; }

        virtual Matrix3d getInterventionInitRates() const {
            return interventionInitRates;
        }

        virtual Matrix3d loadInitialSample(std::string const &csvName);

        virtual Matrix4d
        loadEnteringSamples(std::string const &csvName,
                            std::string const &enteringSampleIntervention,
                            std::string const &enteringSampleOUD);

        virtual Matrix3d loadOUDTransitionRates(std::string const &csvName);

        virtual Matrix3d loadInterventionInitRates(std::string const &csvName);

        virtual Matrix4d
        loadInterventionTransitionRates(std::string const &csvName);

        virtual Matrix4d loadOverdoseRates(std::string const &csvName);

        virtual Matrix4d loadFatalOverdoseRates(std::string const &csvName);

        virtual Matrix3d loadMortalityRates(std::string const &smrCSVName,
                                            std::string const &bgmCSVName);

    private:
        void fillTime(int &start, int const end, Matrix3d data,
                      Matrix4d &storage);

        Matrix3d strVecToMatrix3d(std::vector<std::string> strVec, int matD1,
                                  int matD2, int matD3);

        Matrix3d doubleToMatrix3d(double value, int matD1, int matD2,
                                  int matD3);

        /// @brief
        /// @param table
        /// @param dimension
        /// @return
        Matrix3d createTransitionMatrix3d(Data::IDataTablePtr const &table,
                                          Matrixify::Dimension dimension,
                                          int timestep);

        /// @brief
        /// @param indices
        /// @param table
        /// @return
        Matrix3d buildInterventionMatrix(Data::IDataTablePtr const &table,
                                         std::string interventionName,
                                         int timestep);

        /// @brief
        /// @param table
        /// @param key
        /// @return
        Matrix3d buildOverdoseTransitions(Data::IDataTablePtr const &table,
                                          std::string const &key);

        /// @brief
        /// @param ict
        /// @param table
        /// @return
        Matrix4d buildTransitionRatesOverTime(std::vector<int> const &ict,
                                              Data::IDataTablePtr const &table);

        std::vector<int> demographicCounts;
        std::map<std::string, std::vector<int>> simulationParameters;

        Matrix3d initialSample;
        Matrix4d enteringSamples;
        Matrix3d oudTransitionRates;
        Matrix3d interventionInitRates;
        Matrix4d interventionTransitionRates;
        Matrix4d overdoseRates;
        Matrix4d fatalOverdoseRates;
        Matrix3d mortalityRates;
    };
} // namespace Matrixify
#endif // DATA_DATALOADER_HPP_
