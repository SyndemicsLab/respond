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

#include "DataTypes.hpp"
#include "DataLoader.hpp"

namespace Simulation{
    enum Dimension {
        TREATMENT = 0,
        OUD = 1,
        DEMOGRAPHIC_COMBO = 2
    };

    class ISim{
    public:
        virtual ~ISim(){};
        virtual void LoadInitialGroup(Data::Matrix3d initialGroup) = 0;
        virtual void LoadEnteringSamples(Data::Matrix3dOverTime enteringSamples) = 0;
        virtual void LoadOUDTransitions(Data::Matrix3dOverTime oudTransitions) = 0;
        virtual void LoadTreatmentTransitions(Data::Matrix3dOverTime treatmentTransitions) = 0;
        virtual void LoadOverdoseTransitions(Data::Matrix3dOverTime overdoseTransitions) = 0;
        virtual void LoadMortalityTransitions(Data::Matrix3dOverTime mortalityTransitions) = 0;
        virtual void Load(Data::DataLoader dataLoader) = 0;

        virtual Data::Matrix3dOverTime GetEnteringSamples() = 0;
        virtual Data::Matrix3dOverTime GetOUDTransitions() = 0;
        virtual Data::Matrix3dOverTime GetTreatmentTransitions() = 0;
        virtual Data::Matrix3dOverTime GetOverdoseTransitions() = 0;
        virtual Data::Matrix3dOverTime GetMortalityTransitions() = 0;

        virtual void LoadTransitionModules(
            Data::Matrix3dOverTime enteringSamples,
            Data::Matrix3dOverTime oudTransitions,
            Data::Matrix3dOverTime treatmentTransitions,
            Data::Matrix3dOverTime overdoseTransitions,
            Data::Matrix3dOverTime mortalityTransitions
        ) = 0;
        virtual void Run() = 0;
        virtual Data::History getHistory() = 0;
    };

    class Sim : public ISim {
    private:
        boost::log::sources::logger lg;

        Data::Matrix3d state;
        Data::Matrix3d transition;
        uint16_t currentTime;
        uint8_t numOUDStates;
        uint8_t numTreatmentStates;
        uint16_t numDemographics;
        Data::History history;
        Data::Matrix3dOverTime enteringSamples;
        Data::Matrix3dOverTime oudTransitions;
        Data::Matrix3dOverTime treatmentTransitions;
        Data::Matrix3dOverTime overdoseTransitions;
        Data::Matrix3dOverTime mortalityTransitions;
        Data::Matrix3d step();
        Data::Matrix3d addEnteringSamples(Data::Matrix3d state);
        Data::Matrix3d multiplyOUDTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyTreatmentTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyOverdoseTransitions(Data::Matrix3d state);
        Data::Matrix3d multiplyMortalityTransitions(Data::Matrix3d state);
        Data::Matrix3d getTransitionFromDim(Dimension dim);
        Data::Matrix3d multiplyTransitions(Data::Matrix3d state, Dimension dim);
    public:
        Data::Matrix3d CreateNewShapedTensor();

        Sim();
        Sim(uint16_t duration, uint8_t numOUDStates, uint8_t numTreatmentStates, uint16_t numDemographics);
        ~Sim() {};
        Sim(Data::DataLoader dataLoader);
        void LoadInitialGroup(Data::Matrix3d initialGroup) override;
        void LoadEnteringSamples(Data::Matrix3dOverTime enteringSamples) override;
        void LoadOUDTransitions(Data::Matrix3dOverTime oudTransitions) override;
        void LoadTreatmentTransitions(Data::Matrix3dOverTime treatmentTransitions) override;
        void LoadOverdoseTransitions(Data::Matrix3dOverTime overdoseTransitions) override;
        void LoadMortalityTransitions(Data::Matrix3dOverTime mortalityTransitions) override;
        void Load(Data::DataLoader dataLoader) override;

        Data::Matrix3dOverTime GetEnteringSamples() override;
        Data::Matrix3dOverTime GetOUDTransitions() override;
        Data::Matrix3dOverTime GetTreatmentTransitions() override;
        Data::Matrix3dOverTime GetOverdoseTransitions() override;
        Data::Matrix3dOverTime GetMortalityTransitions() override;

        void LoadTransitionModules(
            Data::Matrix3dOverTime enteringSamples,
            Data::Matrix3dOverTime oudTransitions,
            Data::Matrix3dOverTime treatmentTransitions,
            Data::Matrix3dOverTime overdoseTransitions,
            Data::Matrix3dOverTime mortalityTransitions
        ) override;
        void Run() override;
        Data::History getHistory() override;
        uint16_t Duration;
    };
}
#endif /* MODEL_SIMULATION_HPP_ */
