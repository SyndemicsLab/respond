#ifndef CONTROLLER_DATAWRITER_HPP_
#define CONTROLLER_DATAWRITER_HPP_

#include <iostream>
#include <fstream>
#include <string>
#include <ctime>
#include <vector>
#include <boost/filesystem.hpp>
#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#include <fmt/core.h>

#include "Simulation.hpp"
#include "DataTypes.hpp"


namespace Data {

    enum OutputType {
        FILE = 0,
        STRING = 1
    };

    class IDataWriter{
    public:
        virtual ~IDataWriter(){};
        virtual void AddHistory(History history) = 0;
        virtual void AddDirname(std::string dirname) = 0;
        virtual std::string getDirname() = 0;
        virtual void setInterventions(std::vector<std::string> interventions) = 0;
        virtual void setOUDStates(std::vector<std::string> oudStates) = 0;
        virtual void setDemographics(std::vector<std::vector<std::string>> demographics) = 0;
        virtual std::string Write(OutputType outputType) = 0;
    };

    class DataWriter : public IDataWriter{
    public:
        DataWriter();
        DataWriter(std::string dirname, 
        std::vector<std::string> interventions, 
        std::vector<std::string> oudStates, 
        std::vector<std::vector<std::string>> demographics,
        History history);
        void AddHistory(History history) override;
        void AddDirname(std::string dirname) override;
        std::string getDirname() override;
        void setInterventions(std::vector<std::string> interventions) override;
        void setOUDStates(std::vector<std::string> oudStates) override;
        void setDemographics(std::vector<std::vector<std::string>> demographics) override;
        std::string Write(OutputType outputType) override;
    private:
        std::vector<std::string> interventions;
        std::vector<std::string> oudStates;
        std::vector<std::vector<std::string>> demographics;
        std::string dirname;
        History history;
        std::string WriteColumnHeaders(int timesteps);
        void Writer(std::ostream &stream, Matrix3dOverTime historyToWrite);
    };
}

#endif