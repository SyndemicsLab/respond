#include <iostream>

#include "DataLoader.hpp"
#include "DataWriter.hpp"
#include "Simulation.hpp"
#include "CostLoader.hpp"
#include "CostCalculator.hpp"

int main(int argc, char** argv) {
    using std::cout, std::cerr;
    if (argc != 3) {
        cerr << "Usage: " << argv[0]
             << "[INPUT FOLDER] [RUN NUMBER]\n\n"
             << "RESPOND, a compartmental simulation of healthcare in communities with high-risk opioid use";
    }
    
    for(int i=0; i < std::stoi(argv[2]); i++){
        std::string inputDir = argv[1];
        inputDir += "input" + std::to_string(i+1);
        Data::DataLoader inputs(inputDir);
        inputs.loadInitialSample("init_cohort.csv");
        inputs.loadEnteringSamples("entering_cohort.csv", "No_Treatment", "Active_Noninjection");
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
        costLoader.loadHealthcareUtilizationCost("healthcare_utilization_cost.csv");
        costLoader.loadOverdoseCost("overdose_cost.csv");
        costLoader.loadPharmaceuticalCost("pharmaceutical_cost.csv");
        costLoader.loadTreatmentUtilizationCost("treatment_utilization_cost.csv");

        Data::UtilityLoader utilityLoader(inputDir);
        utilityLoader.loadBackgroundUtility("bg_utility.csv");
        utilityLoader.loadOUDUtility("oud_utility.csv");
        utilityLoader.loadSettingUtility("setting_utility.csv");

        Calculator::CostCalculator costCalculator(costLoader, utilityLoader, history);
        Data::Cost cost = costCalculator.calculateCost();
        Data::Utility util = costCalculator.calculateUtility();

        std::vector<std::vector<std::string>> demographics = inputs.getConfiguration().getDemographicCombosVecOfVec();
        
        std::string outputDir = "output" + std::to_string(i+1);
        Data::DataWriter writer(outputDir, inputs.getInterventions(), inputs.getOUDStates(), demographics);

        writer.writeHistory(Data::FILE, history);
        writer.writeCost(Data::FILE, cost);
        writer.writeUtility(Data::FILE, util);
    }

    
}
