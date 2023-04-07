/*
 * Simulation.hpp
 *
 *  Created on: Mar 29, 2023
 *      Author: matt
 */

#ifndef MODEL_SIMULATION_HPP_
#define MODEL_SIMULATION_HPP_

#include <iostream>
#include <stdexcept>
#include <cmath>
#include <cstdint>
#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>
#include <string>

#include <boost/log/core.hpp>
#include <boost/log/trivial.hpp>
#include <boost/log/expressions.hpp>
#include <boost/log/sinks/text_file_backend.hpp>
#include <boost/log/utility/setup/file.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/log/sources/severity_logger.hpp>
#include <boost/log/sources/record_ostream.hpp>
#include <boost/log/sources/logger.hpp>

#include <fmt/core.h>

namespace Simulation{
    using DataMatrix = std::vector<Eigen::Tensor<float, 3>>;

    typedef struct history{
        DataMatrix stateHistory;
        DataMatrix overdoseHistory;
        DataMatrix mortalityHistory;
    }History;

    enum Dimension {
        TREATMENT = 0,
        OUD = 1,
        DEMOGRAPHIC_COMBO = 2
    };

    class ISim{
    public:
        virtual ~ISim(){};
        virtual void LoadInitialGroup(Eigen::Tensor<float, 3> initialGroup) = 0;
        virtual void LoadEnteringSamples(DataMatrix enteringSamples) = 0;
        virtual void LoadOUDTransitions(DataMatrix oudTransitions) = 0;
        virtual void LoadTreatmentTransitions(DataMatrix treatmentTransitions) = 0;
        virtual void LoadOverdoseTransitions(DataMatrix overdoseTransitions) = 0;
        virtual void LoadMortalityTransitions(DataMatrix mortalityTransitions) = 0;

        virtual DataMatrix GetEnteringSamples() = 0;
        virtual DataMatrix GetOUDTransitions() = 0;
        virtual DataMatrix GetTreatmentTransitions() = 0;
        virtual DataMatrix GetOverdoseTransitions() = 0;
        virtual DataMatrix GetMortalityTransitions() = 0;

        virtual void LoadTransitionModules(
            DataMatrix enteringSamples,
            DataMatrix oudTransitions,
            DataMatrix treatmentTransitions,
            DataMatrix overdoseTransitions,
            DataMatrix mortalityTransitions
        ) = 0;
        virtual void Run() = 0;
        virtual History getHistory() = 0;
    };

    class Sim : public ISim {
    private:
        boost::log::sources::logger lg;

        Eigen::Tensor<float, 3> state;
        Eigen::Tensor<float, 3> transition;
        uint16_t currentTime;
        uint8_t numOUDStates;
        uint8_t numTreatmentStates;
        uint16_t numDemographics;
        History history;
        DataMatrix enteringSamples;
        DataMatrix oudTransitions;
        DataMatrix treatmentTransitions;
        DataMatrix overdoseTransitions;
        DataMatrix mortalityTransitions;
        Eigen::Tensor<float, 3> step();
        Eigen::Tensor<float, 3> addEnteringSamples(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyOUDTransitions(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyTreatmentTransitions(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyOverdoseTransitions(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> multiplyMortalityTransitions(Eigen::Tensor<float, 3> state);
        Eigen::Tensor<float, 3> getTransitionFromDim(Dimension dim);
        Eigen::Tensor<float, 3> multiplyTransitions(Eigen::Tensor<float, 3> state, Dimension dim);
    public:
        Eigen::Tensor<float, 3> CreateNewShapedTensor();

        Sim();
        Sim(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics);
        virtual ~Sim() {};
        void LoadInitialGroup(Eigen::Tensor<float, 3> initialGroup) override;
        void LoadEnteringSamples(DataMatrix enteringSamples) override;
        void LoadOUDTransitions(DataMatrix oudTransitions) override;
        void LoadTreatmentTransitions(DataMatrix treatmentTransitions) override;
        void LoadOverdoseTransitions(DataMatrix overdoseTransitions) override;
        void LoadMortalityTransitions(DataMatrix mortalityTransitions) override;

        DataMatrix GetEnteringSamples() override;
        DataMatrix GetOUDTransitions() override;
        DataMatrix GetTreatmentTransitions() override;
        DataMatrix GetOverdoseTransitions() override;
        DataMatrix GetMortalityTransitions() override;

        void LoadTransitionModules(
            DataMatrix enteringSamples,
            DataMatrix oudTransitions,
            DataMatrix treatmentTransitions,
            DataMatrix overdoseTransitions,
            DataMatrix mortalityTransitions
        ) override;
        void Run() override;
        History getHistory() override;
        uint16_t Duration;
    };
}
#endif /* MODEL_SIMULATION_HPP_ */
