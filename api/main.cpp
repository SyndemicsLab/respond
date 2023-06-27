#include "CostCalculator.hpp"
#include "CostLoader.hpp"
#include "DataLoader.hpp"
#include "DataWriter.hpp"
#include "Simulation.hpp"
#include "crow.h"

int main(int argc, char **argv) {
    crow::SimpleApp app;

    Data::DataLoader inputs;
    Data::CostLoader costs;
    Data::UtilityLoader utils;
    Data::DataWriter writer;

    Data::History hist;

    // I hate this route and lambda. I really want to clean it up when I have
    // time
    CROW_ROUTE(app, "/upload")
        .methods(crow::HTTPMethod::Post)([&inputs, &costs,
                                          &utils](const crow::request &req) {
            std::string config_file;
            std::string init_cohort;
            std::string entering_cohort;
            std::string oud_trans;
            std::string intervention_trans;
            std::string intervention_inits;
            std::string od_rates;
            std::string fod_rates;
            std::string bgm_rates;
            std::string smr_rates;
            std::string health_costs;
            std::string od_costs;
            std::string pharma_costs;
            std::string treat_costs;
            std::string bg_utils;
            std::string setting_utils;
            std::string oud_utils;

            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("ConfigFile" == part_name) {
                    // Get file name
                    config_file =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(config_file);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("InitCohortFile" == part_name) {
                    // Get file name
                    init_cohort =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(init_cohort);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("EnteringCohortFile" == part_name) {
                    // Get file name
                    entering_cohort =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(entering_cohort);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("OUDTransFile" == part_name) {
                    // Get file name
                    oud_trans = part_value.headers.find("Content-Disposition")
                                    ->second.params["filename"];

                    std::ofstream out_file(oud_trans);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("InterventionTransFile" == part_name) {
                    // Get file name
                    intervention_trans =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(intervention_trans);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("InterventionInitsFile" == part_name) {
                    // Get file name
                    intervention_inits =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(intervention_inits);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("OverdoseRates" == part_name) {
                    // Get file name
                    od_rates = part_value.headers.find("Content-Disposition")
                                   ->second.params["filename"];

                    std::ofstream out_file(od_rates);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("FatalOverdoseRates" == part_name) {
                    // Get file name
                    fod_rates = part_value.headers.find("Content-Disposition")
                                    ->second.params["filename"];

                    std::ofstream out_file(fod_rates);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("BackgroundMortRates" == part_name) {
                    // Get file name
                    bgm_rates = part_value.headers.find("Content-Disposition")
                                    ->second.params["filename"];

                    std::ofstream out_file(bgm_rates);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("SMRRates" == part_name) {
                    // Get file name
                    smr_rates = part_value.headers.find("Content-Disposition")
                                    ->second.params["filename"];

                    std::ofstream out_file(smr_rates);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("HealthCosts" == part_name) {
                    // Get file name
                    health_costs =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(health_costs);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("OverdoseCosts" == part_name) {
                    // Get file name
                    od_costs = part_value.headers.find("Content-Disposition")
                                   ->second.params["filename"];

                    std::ofstream out_file(od_costs);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("PharmaCosts" == part_name) {
                    // Get file name
                    pharma_costs =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(pharma_costs);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("TreatmentCosts" == part_name) {
                    // Get file name
                    treat_costs =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(treat_costs);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("BackgroundUtilities" == part_name) {
                    // Get file name
                    bg_utils = part_value.headers.find("Content-Disposition")
                                   ->second.params["filename"];

                    std::ofstream out_file(bg_utils);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("SettingUtilities" == part_name) {
                    // Get file name
                    setting_utils =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(setting_utils);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("OUDUtilities" == part_name) {
                    // Get file name
                    oud_utils = part_value.headers.find("Content-Disposition")
                                    ->second.params["filename"];

                    std::ofstream out_file(oud_utils);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadConfigurationFile(config_file);
            inputs.loadInitialSample(init_cohort);

            std::string enteringSampleIntervention =
                req.url_params.get("intervention");
            std::string enteringSampleOUD = req.url_params.get("oud");
            inputs.loadEnteringSamples(
                entering_cohort, enteringSampleIntervention, enteringSampleOUD);

            inputs.loadOUDTransitionRates(oud_trans);
            inputs.loadInterventionTransitionRates(intervention_trans);
            inputs.loadInterventionInitRates(intervention_inits);
            inputs.loadOverdoseRates(od_rates);
            inputs.loadFatalOverdoseRates(fod_rates);
            inputs.loadMortalityRates(smr_rates, bgm_rates);

            costs.loadConfigurationFile(config_file);
            costs.loadHealthcareUtilizationCost(health_costs);
            costs.loadOverdoseCost(od_costs);
            costs.loadPharmaceuticalCost(pharma_costs);
            costs.loadTreatmentUtilizationCost(treat_costs);

            utils.loadConfigurationFile(config_file);
            utils.loadBackgroundUtility(bg_utils);
            utils.loadOUDUtility(oud_utils);
            utils.loadSettingUtility(setting_utils);

            return crow::response(crow::status::OK);
        });

    // Hoping this works? https://github.com/CrowCpp/Crow/issues/591
    CROW_ROUTE(app, "/upload/config")
        .methods(crow::HTTPMethod::Post)(
            [&inputs, &costs, &utils](const crow::request &req) {
                std::string uploaded_file_name;
                crow::multipart::message file_message(req);
                for (auto [part_name, part_value] : file_message.part_map) {
                    if ("InputFile" == part_name) {
                        // Get file name
                        uploaded_file_name =
                            part_value.headers.find("Content-Disposition")
                                ->second.params["filename"];

                        std::ofstream out_file(uploaded_file_name);
                        if (!out_file) {
                            continue;
                        }
                        out_file << part_value.body;
                        out_file.close();
                    }
                }

                inputs.loadConfigurationFile(uploaded_file_name);
                costs.loadConfigurationFile(uploaded_file_name);
                utils.loadConfigurationFile(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

    CROW_ROUTE(app, "/upload/initCohort")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadInitialSample(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/enterCohort")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }
            std::string enteringSampleIntervention =
                req.url_params.get("intervention");
            std::string enteringSampleOUD = req.url_params.get("oud");

            inputs.loadEnteringSamples(uploaded_file_name,
                                       enteringSampleIntervention,
                                       enteringSampleOUD);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/oudTrans")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadOUDTransitionRates(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/interventionTrans")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadInterventionTransitionRates(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/interventionInits")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadInterventionInitRates(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/overdoseRates")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadOverdoseRates(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/fatalOverdoseRates")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadFatalOverdoseRates(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/mortalityRates")
        .methods(crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
            std::string uploaded_file_name;
            std::string smr_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                } else if ("SMRFile" == part_name) {
                    // Get file name
                    smr_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(smr_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            inputs.loadMortalityRates(uploaded_file_name, smr_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/healthcareCost")
        .methods(crow::HTTPMethod::Post)([&costs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            costs.loadHealthcareUtilizationCost(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/overdoseCost")
        .methods(crow::HTTPMethod::Post)([&costs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            costs.loadOverdoseCost(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/pharmaCost")
        .methods(crow::HTTPMethod::Post)([&costs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            costs.loadPharmaceuticalCost(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/treatmentCost")
        .methods(crow::HTTPMethod::Post)([&costs](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            costs.loadTreatmentUtilizationCost(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/backgroundUtil")
        .methods(crow::HTTPMethod::Post)([&utils](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            utils.loadBackgroundUtility(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/oudUtil")
        .methods(crow::HTTPMethod::Post)([&utils](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            utils.loadOUDUtility(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/upload/settingUtil")
        .methods(crow::HTTPMethod::Post)([&utils](const crow::request &req) {
            std::string uploaded_file_name;
            crow::multipart::message file_message(req);
            for (auto [part_name, part_value] : file_message.part_map) {
                if ("InputFile" == part_name) {
                    // Get file name
                    uploaded_file_name =
                        part_value.headers.find("Content-Disposition")
                            ->second.params["filename"];

                    std::ofstream out_file(uploaded_file_name);
                    if (!out_file) {
                        continue;
                    }
                    out_file << part_value.body;
                    out_file.close();
                }
            }

            utils.loadSettingUtility(uploaded_file_name);

            return crow::response(crow::status::OK);
        });

    CROW_ROUTE(app, "/run")
        .methods(crow::HTTPMethod::Post)(
            [&hist, &inputs](const crow::request &req) {
                Simulation::Sim sim(inputs);
                sim.Run();
                hist = sim.getHistory();
                return crow::response(crow::status::OK);
            });

    CROW_ROUTE(app, "/calculateCost")
        .methods(crow::HTTPMethod::Post)(
            [&hist, &costs, &utils](const crow::request &req) {
                Calculator::CostCalculator costCalculator(costs, utils, hist);
                Data::Cost cost = costCalculator.calculateCost();

                return crow::response(crow::status::OK);
            });

    CROW_ROUTE(app, "/calculateUtil")
        .methods(crow::HTTPMethod::Post)(
            [&hist, &costs, &utils](const crow::request &req) {
                Calculator::CostCalculator costCalculator(costs, utils, hist);
                Data::Utility utility = costCalculator.calculateUtility();

                return crow::response(crow::status::OK);
            });

    CROW_ROUTE(app, "/download/writeHistory")
        .methods(crow::HTTPMethod::Get)([&hist, &inputs, &writer]() {
            writer.setInterventions(inputs.getInterventions());
            writer.setOUDStates(inputs.getOUDStates());
            writer.setDemographics(
                inputs.getConfiguration().getDemographicCombosVecOfVec());
            writer.addDirname("output");
            writer.writeHistory(Data::FILE, hist);
            crow::response response;
            response.set_static_file_info("output/stateHistory.csv");
            return response;
        });

    CROW_ROUTE(app, "/download/writeCost")
        .methods(crow::HTTPMethod::Post)([&hist, &inputs, &costs, &utils,
                                          &writer](const crow::request &req) {
            Calculator::CostCalculator costCalculator(costs, utils, hist);
            Data::Cost cost = costCalculator.calculateCost();

            writer.setInterventions(inputs.getInterventions());
            writer.setOUDStates(inputs.getOUDStates());
            writer.setDemographics(
                inputs.getConfiguration().getDemographicCombosVecOfVec());
            writer.addDirname("output");

            writer.writeCost(Data::STRING, cost);
            crow::response response;
            response.set_static_file_info("output/healthcareCost.csv");

            return response;
        });

    CROW_ROUTE(app, "/download/writeUtility")
        .methods(crow::HTTPMethod::Post)([&hist, &inputs, &costs, &utils,
                                          &writer](const crow::request &req) {
            Calculator::CostCalculator costCalculator(costs, utils, hist);
            Data::Utility utility = costCalculator.calculateUtility();

            writer.setInterventions(inputs.getInterventions());
            writer.setOUDStates(inputs.getOUDStates());
            writer.setDemographics(
                inputs.getConfiguration().getDemographicCombosVecOfVec());
            writer.addDirname("output");

            writer.writeUtility(Data::FILE, utility);
            crow::response response;
            response.set_static_file_info("output/backgroundUtility.csv");

            return response;
        });

    app.port(18080).multithreaded().run();
}