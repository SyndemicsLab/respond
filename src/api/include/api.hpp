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
        Matrixify::IDataLoaderPtr inputs;
        Matrixify::ICostLoaderPtr costs;
        Matrixify::IUtilityLoaderPtr utils;
        Matrixify::HistoryWriter historyWriter;
        Matrixify::CostWriter costWriter;
        Matrixify::UtilityWriter utilityWriter;

        Matrixify::History hist;

        void setupUpload();
        void setupDownload();
        void setupCalculate();
        std::string readMessage(const crow::request &req);
    };
} // namespace API

#endif