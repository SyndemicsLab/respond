#ifndef DATA_DATABLOCK_HPP_
#define DATA_DATABLOCK_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace data {
    class IDataBlock {
    public:
        virtual int LoadData(const int input_set, const int parameter_set,
                             const int year = 2015, const std::string &db = "",
                             const std::string &confile = "") = 0;
        virtual int ConnectToDatabase(const std::string &db) = 0;
        virtual int LoadConfig(const std::string &confile) = 0;
        virtual int GetSimulationDuration() const = 0;
        virtual std::vector<int> GetDataDimensions() const = 0;
        virtual bool GetStoreHistoryStatus() const = 0;
        virtual std::vector<int> GetHistoryTimestepsToStore() const = 0;
    };
} // namespace data

#endif