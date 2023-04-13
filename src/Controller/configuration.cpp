#include "configuration.hpp"

Configuration::Configuration(const po::variables_map& vm) {
    interventions = parseStringVector(vm["simulation.interventions"].as<std::string>());
    age_groups = parseStringVector(vm["simulation.age_groups"].as<std::string>());
    sex = parseStringVector(vm["simulation.sex"].as<std::string>());
    oud_states = parseStringVector(vm["simulation.oud_states"].as<std::string>());
    duration = vm["simulation.duration"].as<int>();
    aging_interval = vm["simulation.aging_interval"].as<int>();
    intervention_change_times =
        parseIntVector(vm["simulation.intervention_change_times"].as<std::string>());
    entering_sample_change_times =
        parseIntVector(vm["simulation.entering_sample_change_times"].as<std::string>());
    overdose_change_times =
        parseIntVector(vm["simulation.overdose_change_times"].as<std::string>());
    cost_analysis = vm["cost.cost_analysis"].as<bool>();
    cost_perspectives = parseStringVector(vm["cost.cost_perspectives"].as<std::string>());
    discount_rate = vm["cost.discount_rate"].as<double>();
    reporting_interval = vm["outputs.reporting_interval"].as<int>();
    per_intervention_predictions = vm["outputs.per_intervention_predictions"].as<bool>();
    general_outputs = vm["outputs.general_outputs"].as<bool>();
    general_stats_output_timesteps =
        parseIntVector(vm["outputs.general_stats_output_timesteps"].as<std::string>());
    cost_category_outputs = vm["outputs.cost_category_outputs"].as<bool>();
}

Configuration configure(const std::string& inputFile) {
    std::ifstream configFile(inputFile);

    po::options_description config("Configuration");
// clang-format off
    config.add_options()
        ("simulation.interventions", po::value<std::string>(),
         "A comma-separated list of the intervention states to be used")
        ("simulation.age_groups", po::value<std::string>(),
         "A comma-separated list of age groups")
        ("simulation.sex", po::value<std::string>(),
         "A comma-separated list of sexes")
        ("simulation.oud_states", po::value<std::string>(),
         "A comma-separated list of opioid use disorder (OUD) states")
        ("simulation.duration", po::value<int>(),
         "The number of timesteps (weeks) to be simulated")
        ("simulation.aging_interval", po::value<int>(),
         "The number of weeks between age groups shifting upward")
        ("simulation.intervention_change_times", po::value<std::string>(),
         "A comma-separated list of the timesteps at which intervention entrance proportions change")
        ("simulation.entering_sample_change_times", po::value<std::string>(),
         "A comma-separated list of the timesteps at which the entering sample distribution changes")
        ("simulation.overdose_change_times", po::value<std::string>(),
         "A comma-separated list of the timesteps at which overdose proportions change")
        ("cost.cost_analysis", po::value<bool>(),
         "True/False -- Include cost analysis in the simulation")
        ("cost.cost_perspectives", po::value<std::string>(),
         "A comma-separated list of the perspectives from which to analyze the cost contributions")
        ("cost.discount_rate", po::value<double>(),
         "A decimal value of the rate to discount costs")
        ("outputs.reporting_interval", po::value<int>(),
         "An integer specifying the timestep interval between output of predictions")
        ("outputs.per_intervention_predictions", po::value<bool>(),
         "True/False -- Break down predictions by intervention")
        ("outputs.general_outputs", po::value<bool>(),
         "True/False -- Generate a general_outputs file")
        ("outputs.general_stats_output_timesteps", po::value<std::string>(),
         "A comma-separated list of the timesteps at which general_stats output should be written")
        ("outputs.cost_category_outputs", po::value<bool>(),
         "True/False -- Break down cost predictions by category")
        ;
// clang-format on
    po::variables_map vm;
    po::store(po::parse_config_file(configFile, config, true), vm);
    configFile.close();
    po::notify(vm);

    return Configuration(vm);
}
