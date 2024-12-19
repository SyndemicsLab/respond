#ifndef MOCKRESPOND_HPP_
#define MOCKRESPOND_HPP_

#include "gmock/gmock.h"
#include "gtest/gtest.h"
#include <Eigen/Eigen>
#include <synmodels/models/Respond.hpp>

namespace synmodels::models {
    class MOCKRespond : public Respond {
    public:
        MOCK_METHOD(bool, Run, (const RDATASTORE_POINTER &), (override));
        MOCK_METHOD((Eigen::VectorXd const), GetState, (), (const, override));
        MOCK_METHOD((HISTORY_MAP const &), GetSimulationHistory, (),
                    (const, override));
    };
} // namespace synmodels::models

#endif