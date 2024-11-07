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
        matrixify::IDataLoaderPtr inputs;
        matrixify::ICostLoaderPtr costs;
        matrixify::IUtilityLoaderPtr utils;
        matrixify::HistoryWriter historyWriter;
        matrixify::CostWriter costWriter;
        matrixify::UtilityWriter utilityWriter;

        matrixify::History hist;

        void setupUpload();
        void setupDownload();
        void setupCalculate();
        std::string readMessage(const crow::request &req);
    };
} // namespace API

#endif