#ifndef MODEL_CALIBRATION_HPP_
#define MODEL_CALIBRATION_HPP_

#include <iostream>
#include <vector>
#include <unordered_map>

#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sources/logger.hpp>

#include "DataLoader.hpp"

namespace Calibration{
    using DataMatrix = std::vector<Eigen::Tensor<float, 3>>;
    using CalibrationOutputs = std::vector<std::unordered_map<std::string, DataMatrix>>;
    using CalibrationInputs = std::vector<std::unordered_map<std::string, DataMatrix>>;

    class ICalibrator{
    public:
        virtual ~ICalibrator();
        virtual void Calibrate();
        virtual void LoadInputData(DataLoader dataLoader); 
        virtual CalibrationOutputs GetCalibrationOutputs();
    };

    class Calibrator : public ICalibrator{
    private:
        boost::log::sources::logger lg;
        CalibrationInputs inputTables;

    public:
        Calibrator();
        Calibrator(CalibrationInputs inputs);
        void Calibrate() override;
        void LoadInputData(DataLoader dataLoader) override;
        CalibrationOutputs GetCalibrationOutputs() override;
    };
}

#endif