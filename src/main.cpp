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
    Data::DataLoader inputs(argv[1]);
    inputs.loadInitialSample("init_cohort.csv");
    inputs.loadEnteringSamples("entering_cohort.csv");
    inputs.loadOUDTransitionRates("oud_trans.csv");
    inputs.loadInterventionInitRates("block_init_effect.csv");
    inputs.loadInterventionTransitionRates("block_trans.csv");
    inputs.loadOverdoseRates("all_types_overdose.csv");
    inputs.loadFatalOverdoseRates("fatal_overdose.csv");
    inputs.loadMortalityRates("SMR.csv", "background_mortality.csv");
    Simulation::Sim sim(inputs);
    sim.Run();

    Data::CostLoader costLoader(argv[1]);
    costLoader.loadHealthcareUtilizationCost("healthcare_utilization_cost.csv");
    costLoader.loadOverdoseCost("overdose_cost.csv");
    costLoader.loadPharmaceuticalCost("pharmaceutical_cost.csv");
    costLoader.loadTreatmentUtilizationCost("treatment_utilization_cost.csv");

    Data::UtilityLoader utilityLoader(argv[1]);
    utilityLoader.loadBackgroundUtility("bg_utility.csv");
    utilityLoader.loadOUDUtility("oud_utility.csv");
    utilityLoader.loadSettingUtility("setting_utility.csv");

    Calculator::CostCalculator costCalculator(costLoader, utilityLoader, sim.getHistory());
    Data::Cost cost = costCalculator.calculateCost();
    Data::Utility util = costCalculator.calculateUtility();

    std::vector<std::vector<std::string>> demographics{
        { "10_14", "male" }, { "10_14", "female" },
        { "15_19", "male" }, { "15_19", "female" },
        { "20_24", "male" }, { "20_24", "female" }
        };

    Data::DataWriter writer("output",
        inputs.getInterventions(),
        inputs.getOUDStates(),
        demographics,
        sim.getHistory());
    writer.write(Data::FILE);
    writer.writeCost(Data::FILE, cost);
    writer.writeUtility(Data::FILE, util);
}
