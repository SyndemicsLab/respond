#ifndef DATA_DATABLOCK_HPP_
#define DATA_DATABLOCK_HPP_

#include <cstdint>
#include <string>

namespace data {
    class IDataBlock {
    public:
        virtual int LoadDatabase(std::string &const db) = 0;
        virtual int LoadConfig(std::string &const confile) = 0;
        virtual int GetSimulationDuration() const = 0;
        virtual std::vector<int> GetDataDimensions() const = 0;
        virtual bool GetStoreHistoryStatus() const = 0;
        virtual std::vector<int> GetHistoryTimestepsToStore() const = 0;
    };
} // namespace data

#endif