#include "api.hpp"
#include "PostsimulationCalculator.hpp"
#include "simulation.hpp"

namespace API {
    RESPONDAPI::RESPONDAPI() {
        this->setupUpload();
        this->setupDownload();
        this->setupCalculate();
    }

    void RESPONDAPI::setupApp() {
        this->inputs = std::make_shared<matrixify::DataLoader>();
        this->costs = std::make_shared<matrixify::CostLoader>();
        this->utils = std::make_shared<matrixify::UtilityLoader>();

        CROW_ROUTE(this->app, "/run")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                simulation::Respond sim(inputs);
                sim.run();
                hist = sim.getHistory();
                return crow::response(crow::status::OK);
            });
    }

    void RESPONDAPI::runApp() { app.port(18080).multithreaded().run(); }

    std::string RESPONDAPI::readMessage(const crow::request &req) {
        std::string uploaded_file_name = "";
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
        return uploaded_file_name;
    }

    void RESPONDAPI::setupUpload() {
        // Hoping this works? https://github.com/CrowCpp/Crow/issues/591
        CROW_ROUTE(this->app, "/upload/config")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);
                inputs->loadConfigFile(uploaded_file_name);
                costs->loadConfigFile(uploaded_file_name);
                utils->loadConfigFile(uploaded_file_name);
                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/population")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);
                inputs->loadInitialSample(uploaded_file_name);
                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/uptake")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);
                std::string enteringSampleIntervention =
                    req.url_params.get("intervention");
                std::string enteringSampleOUD = req.url_params.get("oud");

                inputs->loadEnteringSamples(uploaded_file_name,
                                            enteringSampleIntervention,
                                            enteringSampleOUD);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/behavior")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                inputs->loadOUDTransitionRates(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/interventions")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                inputs->loadInterventionTransitionRates(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/conditionalbehavior")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                inputs->loadInterventionInitRates(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/overdoses")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                inputs->loadOverdoseRates(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/fataloverdoses")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                inputs->loadFatalOverdoseRates(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/rates/mortality")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
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

                inputs->loadMortalityRates(uploaded_file_name, smr_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/costs/healthcare")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                costs->loadHealthcareUtilizationCost(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/costs/overdoses")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                costs->loadOverdoseCost(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/costs/pharmaceutical")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                costs->loadPharmaceuticalCost(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/costs/treatment")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                costs->loadTreatmentUtilizationCost(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/utility/background")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                utils->loadBackgroundUtility(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/utility/behavior")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                utils->loadOUDUtility(uploaded_file_name);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/upload/utility/setting")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                std::string uploaded_file_name = readMessage(req);

                utils->loadSettingUtility(uploaded_file_name);

                return crow::response(crow::status::OK);
            });
    }

    void RESPONDAPI::setupDownload() {
        CROW_ROUTE(this->app, "/download/outputs/history")
            .methods(crow::HTTPMethod::Get)([this]() {
                historyWriter.setInterventions(inputs->getInterventions());
                historyWriter.setOUDStates(inputs->getOUDStates());
                historyWriter.setDemographics(inputs->getDemographics());
                historyWriter.setDirname("output");
                historyWriter.writeHistory(hist);
                crow::response response;
                response.set_static_file_info("output/stateHistory.csv");
                return response;
            });

        CROW_ROUTE(this->app, "/download/outputs/cost")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                Calculator::PostsimulationCalculator costCalculator(hist);
                matrixify::CostList costList =
                    costCalculator.calculateCosts(this->costs);

                costWriter.setInterventions(inputs->getInterventions());
                costWriter.setOUDStates(inputs->getOUDStates());
                costWriter.setDemographics(inputs->getDemographics());
                costWriter.setDirname("output");

                costWriter.writeCosts(costList);
                crow::response response;
                response.set_static_file_info("output/healthcareCost.csv");

                return response;
            });

        CROW_ROUTE(this->app, "/download/outputs/utility")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                Calculator::PostsimulationCalculator costCalculator(hist);
                matrixify::Matrix4d utility = costCalculator.calculateUtilities(
                    this->utils, Calculator::UTILITY_TYPE::MIN);

                utilityWriter.setInterventions(inputs->getInterventions());
                utilityWriter.setOUDStates(inputs->getOUDStates());
                utilityWriter.setDemographics(inputs->getDemographics());
                utilityWriter.setDirname("output");

                utilityWriter.writeUtilities(utility);
                crow::response response;
                response.set_static_file_info("output/backgroundUtility.csv");

                return response;
            });
    }

    void RESPONDAPI::setupCalculate() {
        CROW_ROUTE(this->app, "/calculate/cost")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                Calculator::PostsimulationCalculator costCalculator(hist);
                matrixify::CostList cost =
                    costCalculator.calculateCosts(this->costs);

                return crow::response(crow::status::OK);
            });

        CROW_ROUTE(this->app, "/calculate/utility")
            .methods(crow::HTTPMethod::Post)([this](const crow::request &req) {
                Calculator::PostsimulationCalculator costCalculator(hist);
                matrixify::Matrix4d utility = costCalculator.calculateUtilities(
                    this->utils, Calculator::UTILITY_TYPE::MIN);

                return crow::response(crow::status::OK);
            });
    }

} // namespace API