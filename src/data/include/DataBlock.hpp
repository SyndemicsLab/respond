#ifndef DATA_DATABLOCK_HPP_
#define DATA_DATABLOCK_HPP_

#include <cstdint>
#include <string>
#include <vector>

namespace data {
    class IDataBlock {
    public:
        virtual int LoadDatabase(const std::string &db) = 0;
        virtual int LoadConfig(const std::string &confile) = 0;
        virtual int GetSimulationDuration() const = 0;
        virtual std::vector<int> GetDataDimensions() const = 0;
        virtual bool GetStoreHistoryStatus() const = 0;
        virtual std::vector<int> GetHistoryTimestepsToStore() const = 0;
    };
} // namespace data

#endif