#ifndef API_API_HPP_
#define API_API_HPP_

#include "CostLoader.hpp"
#include "DataLoader.hpp"
#include "UtilityLoader.hpp"
#include "Writer.hpp"
#include "crow.h"
#include <string>

namespace API {
    class RESPONDAPI {
    public:
        RESPONDAPI();
        crow::SimpleApp app;
        void setupApp();
        void runApp();

    private:
        std::shared_ptr<crow::Blueprint> uploadBP;
        std::shared_ptr<crow::Blueprint> downloadBP;
        std::shared_ptr<crow::Blueprint> calculateBP;

        Matrixify::IDataLoaderPtr inputs;
        Matrixify::ICostLoaderPtr costs;
        Matrixify::IUtilityLoaderPtr utils;
        Matrixify::Writer writer;

        Matrixify::History hist;

        void setupUploadBP(std::make_shared<crow::Blueprint> bp);
        void setupDownloadBP(std::make_shared<crow::Blueprint> bp);
        void setupCalculateBP(std::make_shared<crow::Blueprint> bp);
        std::string readMessage(const crow::request &req);
    };
} // namespace API

#endif