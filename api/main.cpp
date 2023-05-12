#include "crow.h"
#include "DataLoader.hpp"
#include "DataWriter.hpp"
#include "Simulation.hpp"
#include "CostLoader.hpp"
#include "CostCalculator.hpp"

int main(int argc, char** argv){
    crow::SimpleApp app;

    Data::DataLoader inputs;
    Data::CostLoader costs;
    Data::UtilityLoader utils;

    CROW_ROUTE(app, "/config")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/initCohort")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/enterCohort")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/oudTrans")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/interventionTrans")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/interventionInits")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/overdoseRates")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/fatalOverdoseRates")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/mortalityRates")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/run")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/healthcareCost")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/overdoseCost")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/pharmaCost")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/treatmentCost")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/backgroundUtil")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/oudUtil")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/settingUtil")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/calculateCost")([](){
        return "Hello World";
    });

    CROW_ROUTE(app, "/calculateUtil")([](){
        return "Hello World";
    });

    /*
    Data::DataLoader inputs(argv[1]);
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

    Data::CostLoader costLoader(argv[1]);
    costLoader.loadHealthcareUtilizationCost("healthcare_utilization_cost.csv");
    costLoader.loadOverdoseCost("overdose_cost.csv");
    costLoader.loadPharmaceuticalCost("pharmaceutical_cost.csv");
    costLoader.loadTreatmentUtilizationCost("treatment_utilization_cost.csv");

    Data::UtilityLoader utilityLoader(argv[1]);
    utilityLoader.loadBackgroundUtility("bg_utility.csv");
    utilityLoader.loadOUDUtility("oud_utility.csv");
    utilityLoader.loadSettingUtility("setting_utility.csv");

    Calculator::CostCalculator costCalculator(costLoader, utilityLoader, history);
    Data::Cost cost = costCalculator.calculateCost();
    Data::Utility util = costCalculator.calculateUtility();

    std::vector<std::vector<std::string>> demographics = inputs.getConfiguration().getDemographicCombosVecOfVec();
    
    Data::DataWriter writer("output", inputs.getInterventions(), inputs.getOUDStates(), demographics);

    writer.writeHistory(Data::FILE, history);
    writer.writeCost(Data::FILE, cost);
    writer.writeUtility(Data::FILE, util); 
    */

    app.port(18080).multithreaded().run();
}