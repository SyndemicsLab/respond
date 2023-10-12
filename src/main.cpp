//===-- main.cpp - Instruction class definition -----------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the declaration of the Instruction class, which is the
/// base class for all of the VM instructions.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include <cassert>
#include <iostream>

#include "CostLoader.hpp"
#include "DataLoader.hpp"
#include "DataWriter.hpp"
#include "PostSimulationCalculator.hpp"
#include "Simulation.hpp"

int main(int argc, char **argv) {
    if (argc != 4) {
        std::cerr << "Usage: " << argv[0]
                  << "[INPUT FOLDER] [RUN START] [RUN END]\n\n"
                  << "RESPOND, a compartmental simulation of healthcare in "
                     "communities with high-risk opioid use";
    }

    for (int i = std::stoi(argv[2]); i < std::stoi(argv[3]); i++) {
        std::string inputDir = argv[1];
        inputDir += "input" + std::to_string(i);
        Data::DataLoader inputs(inputDir);
        inputs.loadInitialSample("init_cohort.csv");
        inputs.loadEnteringSamples("entering_cohort.csv", "No_Treatment",
                                   "Active_Noninjection");
        inputs.loadOUDTransitionRates("oud_trans.csv");
        inputs.loadInterventionInitRates("block_init_effect.csv");
        inputs.loadInterventionTransitionRates("block_trans.csv");
        inputs.loadOverdoseRates("all_types_overdose.csv");
        inputs.loadFatalOverdoseRates("fatal_overdose.csv");
        inputs.loadMortalityRates("SMR.csv", "background_mortality.csv");
        Simulation::Sim sim(inputs);
        sim.Run();
        Data::History history = sim.getHistory();

        Data::CostLoader costLoader(inputDir);
        costLoader.loadHealthcareUtilizationCost(
            "healthcare_utilization_cost.csv");
        costLoader.loadOverdoseCost("overdose_cost.csv");
        costLoader.loadPharmaceuticalCost("pharmaceutical_cost.csv");
        costLoader.loadTreatmentUtilizationCost(
            "treatment_utilization_cost.csv");

        Data::UtilityLoader utilityLoader(inputDir);
        utilityLoader.loadBackgroundUtility("bg_utility.csv");
        utilityLoader.loadOUDUtility("oud_utility.csv");
        utilityLoader.loadSettingUtility("setting_utility.csv");

        Calculator::PostSimulationCalculator PostSimulationCalculator(
            costLoader, utilityLoader, history);
        Data::Cost cost = PostSimulationCalculator.calculateCost();

        Data::Utility util = PostSimulationCalculator.calculateUtility();

        std::vector<std::vector<std::string>> demographics =
            inputs.getConfiguration().getDemographicCombosVecOfVec();

        std::string outputDir = inputDir + "output" + std::to_string(i);
        Data::DataWriter writer(outputDir, inputs.getInterventions(),
                                inputs.getOUDStates(), demographics);

        writer.writeHistory(Data::FILE, history);
        writer.writeCost(Data::FILE, cost);
        writer.writeUtility(Data::FILE, util);
        std::cout << "Output " << std::to_string(i) << " Complete" << std::endl;
    }

    std::cout << "Simulation Complete! :)" << std::endl;
    return 0;
}
