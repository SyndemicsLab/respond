#include "api.hpp"

namespace API {
    RESPONDAPI::RESPONDAPI() {
        this->uploadBP = std::make_shared<crow::Blueprint>();
        this->setupUploadBP(uploadBP);
        this->downloadBP = std::make_shared<crow::Blueprint>();
        this->setupDownloadBP();
        this->calculateBP = std::make_shared<crow::Blueprint>();
        this->setupCalculateBP();
    }

    void RESPONDAPI::setupApp() {
        this->app.register_blueprint(*(this->uploadBP));
        this->app.register_blueprint(*(this->downloadBP));
        this->app.register_blueprint(*(this->calculateBP));

        CROW_ROUTE(this->app, "/run")
            .methods(crow::HTTPMethod::Post)(
                [&this->hist, &this->inputs](const crow::request &req) {
                    Simulation::Respond sim(inputs);
                    sim.Run();
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

    void RESPONDAPI::setupUploadBP(std::make_shared<crow::Blueprint> bp) {
        // Hoping this works? https://github.com/CrowCpp/Crow/issues/591
        CROW_BP_ROUTE(*bp, "/upload/config")
            .methods(crow::HTTPMethod::Post)(
                [&inputs, &costs, &utils](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);
                    inputs.loadConfigFile(uploaded_file_name);
                    costs.loadConfigFile(uploaded_file_name);
                    utils.loadConfigFile(uploaded_file_name);
                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/population")
            .methods(crow::HTTPMethod::Post)(
                [&inputs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);
                    inputs.loadInitialSample(uploaded_file_name);
                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/uptake")
            .methods(crow::HTTPMethod::Post)(
                [&inputs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);
                    std::string enteringSampleIntervention =
                        req.url_params.get("intervention");
                    std::string enteringSampleOUD = req.url_params.get("oud");

                    inputs.loadEnteringSamples(uploaded_file_name,
                                               enteringSampleIntervention,
                                               enteringSampleOUD);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/behavior")
            .methods(crow::HTTPMethod::Post)(
                [&inputs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    inputs.loadOUDTransitionRates(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/interventions")
            .methods(crow::HTTPMethod::Post)(
                [&inputs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    inputs.loadInterventionTransitionRates(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/conditionalbehavior")
            .methods(crow::HTTPMethod::Post)(
                [&inputs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    inputs.loadInterventionInitRates(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/overdoses")
            .methods(crow::HTTPMethod::Post)(
                [&inputs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    inputs.loadOverdoseRates(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/fataloverdoses")
            .methods(crow::HTTPMethod::Post)(
                [&inputs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    inputs.loadFatalOverdoseRates(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/rates/mortality")
            .methods(
                crow::HTTPMethod::Post)([&inputs](const crow::request &req) {
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

        CROW_BP_ROUTE(*bp, "/upload/costs/healthcare")
            .methods(crow::HTTPMethod::Post)(
                [&costs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    costs.loadHealthcareUtilizationCost(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/costs/overdoses")
            .methods(crow::HTTPMethod::Post)(
                [&costs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    costs.loadOverdoseCost(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/costs/pharmaceutical")
            .methods(crow::HTTPMethod::Post)(
                [&costs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    costs.loadPharmaceuticalCost(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/costs/treatment")
            .methods(crow::HTTPMethod::Post)(
                [&costs](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    costs.loadTreatmentUtilizationCost(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/utility/background")
            .methods(crow::HTTPMethod::Post)(
                [&utils](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    utils.loadBackgroundUtility(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/utility/behavior")
            .methods(crow::HTTPMethod::Post)(
                [&utils](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    utils.loadOUDUtility(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });

        CROW_BP_ROUTE(*bp, "/upload/utility/setting")
            .methods(crow::HTTPMethod::Post)(
                [&utils](const crow::request &req) {
                    std::string uploaded_file_name = readMessage(req);

                    utils.loadSettingUtility(uploaded_file_name);

                    return crow::response(crow::status::OK);
                });
    }

    void RESPONDAPI::setupDownloadBP(std::make_shared<crow::Blueprint> bp) {
        CROW_BP_ROUTE(*bp, "/download/outputs/history")
            .methods(crow::HTTPMethod::Get)([&hist, &inputs, &writer]() {
                writer.setInterventions(inputs.getInterventions());
                writer.setOUDStates(inputs.getOUDStates());
                writer.setDemographics(
                    inputs.getConfig().getDemographicCombosVecOfVec());
                writer.addDirname("output");
                writer.writeHistory(Matrixify::FILE, hist);
                crow::response response;
                response.set_static_file_info("output/stateHistory.csv");
                return response;
            });

        CROW_BP_ROUTE(*bp, "/download/outputs/cost")
            .methods(
                crow::HTTPMethod::Post)([&hist, &inputs, &costs, &utils,
                                         &writer](const crow::request &req) {
                Calculator::CostCalculator costCalculator(costs, utils, hist);
                Matrixify::Cost cost = costCalculator.calculateCost();

                writer.setInterventions(inputs.getInterventions());
                writer.setOUDStates(inputs.getOUDStates());
                writer.setDemographics(
                    inputs.getConfig().getDemographicCombosVecOfVec());
                writer.addDirname("output");

                writer.writeCost(Matrixify::STRING, cost);
                crow::response response;
                response.set_static_file_info("output/healthcareCost.csv");

                return response;
            });

        CROW_BP_ROUTE(*bp, "/download/outputs/utility")
            .methods(
                crow::HTTPMethod::Post)([&hist, &inputs, &costs, &utils,
                                         &writer](const crow::request &req) {
                Calculator::CostCalculator costCalculator(costs, utils, hist);
                Matrixify::Matrix4d utility = costCalculator.calculateUtility();

                writer.setInterventions(inputs.getInterventions());
                writer.setOUDStates(inputs.getOUDStates());
                writer.setDemographics(
                    inputs.getConfig().getDemographicCombosVecOfVec());
                writer.addDirname("output");

                writer.writeUtility(Matrixify::FILE, utility);
                crow::response response;
                response.set_static_file_info("output/backgroundUtility.csv");

                return response;
            });
    }

    void RESPONDAPI::setupCalculateBP(std::make_shared<crow::Blueprint> bp) {
        CROW_BP_ROUTE(*bp, "/calculate/cost")
            .methods(crow::HTTPMethod::Post)([&hist, &costs, &utils](
                                                 const crow::request &req) {
                Calculator::CostCalculator costCalculator(costs, utils, hist);
                Matrixify::Cost cost = costCalculator.calculateCost();

                return crow::response(crow::status::OK);
            });

        CROW_BP_ROUTE(*bp, "/calculate/utility")
            .methods(crow::HTTPMethod::Post)([&hist, &costs, &utils](
                                                 const crow::request &req) {
                Calculator::CostCalculator costCalculator(costs, utils, hist);
                Matrixify::Matrix4d utility = costCalculator.calculateUtility();

                return crow::response(crow::status::OK);
            });
    }

} // namespace API