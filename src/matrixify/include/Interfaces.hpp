
#ifndef DATA_INTERFACELOADERS_HPP_
#define DATA_INTERFACELOADERS_HPP_

#include "DataTypes.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <DataManagement.hpp>

namespace Matrixify {

    class IFileable {
    public:
        virtual void setDirname(const std::string) = 0;
        virtual std::string getDirname() const = 0;
    };

    class IReadable {
    public:
        virtual Data::IDataTablePtr readCSV(std::string const &) = 0;
        virtual std::unordered_map<std::string, Data::IDataTablePtr>
        readInputDir(std::string const &) = 0;
        virtual Data::IDataTablePtr loadTable(std::string const &filename) = 0;
    };

    /// @brief Type used to determine the output expected, used primarily in
    /// testing
    enum WriteType { FILE = 0, STRING = 1 };

    class IWritable {
    public:
        virtual void setWriteType(const WriteType) = 0;
        virtual WriteType getWriteType() const = 0;

    private:
        WriteType writeType;
    };

    class ILoggable {
    public:
        virtual void setLogger(const std::shared_ptr<spdlog::logger>) = 0;
        virtual std::shared_ptr<spdlog::logger> getLogger() const = 0;

    private:
        std::shared_ptr<spdlog::logger> logger;
    };

    class IOutputable {
    public:
        virtual bool getPerInterventionPredictions() const = 0;
        virtual bool getGeneralOutputsSwitch() const = 0;
        virtual std::vector<int> getGeneralStatsOutputTimesteps() const = 0;
    };

    class IConfigable {
    public:
        virtual bool loadConfigFile(std::string const &configPath) = 0;
        virtual bool loadConfigPtr(Data::IConfigablePtr configPtr) = 0;
        virtual Data::IConfigablePtr getConfig() const = 0;
    };

    class ISimulatable {
    public:
        virtual int getDuration() const = 0;
        virtual bool getEnteringCohortToggle() const = 0;
        virtual std::vector<int> getEnteringSampleChangeTimes() const = 0;
        virtual std::vector<int> getOverdoseChangeTimes() const = 0;
    };

    class IInterventionable {
    public:
        virtual std::vector<std::string> getInterventions() const = 0;
        virtual void setInterventions(const std::vector<std::string>) = 0;
        virtual int getNumInterventions() const = 0;
        virtual std::vector<int> getInterventionChangeTimes() const = 0;
        virtual void setInterventionChangeTimes(const std::vector<int>) = 0;
    };

    class IBehaviorable {
    public:
        virtual std::vector<std::string> getOUDStates() const = 0;
        virtual int getNumOUDStates() const = 0;
        virtual void setOUDStates(const std::vector<std::string>) = 0;
    };

    class IAgeable {
    public:
        virtual int getAgeGroupShift() const = 0;
        virtual int getAgingInterval() const = 0;
        virtual std::vector<std::string> getAgeGroupBins() const = 0;
    };

    class IDemographicable {
    public:
        virtual std::vector<std::string> getDemographics() const = 0;
        virtual int getNumDemographics() const = 0;
        virtual std::vector<std::string> getDemographicCombos() const = 0;
        virtual int getNumDemographicCombos() const = 0;
        virtual void setDemographics(const std::vector<std::string>) = 0;
        virtual void setDemographicCombos(const std::vector<std::string>) = 0;
    };

    class IStratifiable : public virtual IInterventionable,
                          public virtual IBehaviorable,
                          public virtual IDemographicable {};

    class ICostable {
    public:
        virtual bool getCostSwitch() const = 0;
        virtual std::vector<std::string> getCostPerspectives() const = 0;
        virtual double getDiscountRate() const = 0;
        virtual bool getCostCategoryOutputs() const = 0;
        virtual std::vector<int> getCostUtilityOutputTimesteps() const = 0;
    };
} // namespace Matrixify

#endif