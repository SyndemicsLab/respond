#include "RespondDataStoreMock.hpp"
#include "StateTransitionModelMock.hpp"
#include <synmodels/models/Respond.hpp>

#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <Eigen/Eigen>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <vector>

using ::testing::_;
using ::testing::Return;
using ::testing::ReturnRef;

class RespondTest : public ::testing::Test {
private:
    void RegisterLogger() {
        auto console_sink =
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        std::vector<spdlog::sink_ptr> sinks{console_sink};
        auto logger = std::make_shared<spdlog::logger>("main", sinks.begin(),
                                                       sinks.end());
        spdlog::register_logger(logger);
        spdlog::flush_every(std::chrono::seconds(3));
    }

protected:
    std::shared_ptr<synmodels::kernels::MOCKStateTransitionModel> mock_stm;
    std::shared_ptr<synmodels::data::MOCKRespondDataStore> mock_ds;
    std::shared_ptr<synmodels::models::Respond> test_respond;

    void SetUp() override {
        RegisterLogger();
        mock_stm =
            std::make_shared<synmodels::kernels::MOCKStateTransitionModel>();
        mock_ds = std::make_shared<synmodels::data::MOCKRespondDataStore>();
    }
    void TearDown() override { spdlog::drop("main"); }
};

TEST_F(RespondTest, Run) {
    Eigen::VectorXd empty = Eigen::VectorXd::Zero(4);
    EXPECT_CALL(*mock_ds, GetDuration()).WillOnce(Return(1));

    std::vector<int> hts = {1};
    EXPECT_CALL(*mock_ds, GetHistoryTimestepsToStore()).WillOnce(Return(hts));
    EXPECT_CALL(*mock_ds, GetInitialCohortState()).WillOnce(ReturnRef(empty));
    EXPECT_CALL(*mock_stm, SetState(_)).Times(1);

    Eigen::VectorXd vec(2);
    vec << 2, 4;
    EXPECT_CALL(*mock_ds, GetMigratingCohortState(_)).WillOnce(ReturnRef(vec));
    EXPECT_CALL(*mock_stm, AddState(_, _)).WillOnce(Return(empty));

    EXPECT_CALL(*mock_ds, GetOverdoseProbabilityState(_))
        .WillOnce(ReturnRef(vec));
    EXPECT_CALL(*mock_stm, MultiplyState(_, _))
        .WillOnce(Return(empty))
        .WillOnce(Return(empty))
        .WillOnce(Return(empty))
        .WillOnce(Return(empty));

    EXPECT_CALL(*mock_ds, GetOverdoseBeingFatalProbabilityState(_))
        .WillOnce(ReturnRef(vec));

    EXPECT_CALL(*mock_ds, GetStandardMortalityRatioState(_))
        .WillOnce(ReturnRef(vec));

    EXPECT_CALL(*mock_ds, GetBackgroundMortalityState(_))
        .WillOnce(ReturnRef(vec));
    test_respond = synmodels::models::Respond::Create(mock_stm);
    test_respond->Run(mock_ds);
}

TEST_F(RespondTest, GetState) {
    Eigen::VectorXd vec(4);
    vec << 2, 3, 4, 5;
    EXPECT_CALL(*mock_stm, GetState()).WillOnce(Return(vec));
    test_respond = synmodels::models::Respond::Create(mock_stm);
    ASSERT_TRUE(test_respond->GetState().isApprox(vec));
}

TEST_F(RespondTest, GetSimulationHistory) {
    Eigen::VectorXd empty = Eigen::VectorXd::Zero(4);
    EXPECT_CALL(*mock_ds, GetDuration()).WillRepeatedly(Return(5));

    std::vector<int> hts = {1, 3};
    EXPECT_CALL(*mock_ds, GetHistoryTimestepsToStore())
        .WillRepeatedly(Return(hts));

    EXPECT_CALL(*mock_ds, GetInitialCohortState())
        .WillRepeatedly(ReturnRef(empty));
    EXPECT_CALL(*mock_stm, SetState(_)).Times(1);

    Eigen::VectorXd vec(2);
    vec << 2, 4;
    EXPECT_CALL(*mock_ds, GetMigratingCohortState(_))
        .WillRepeatedly(ReturnRef(vec));
    EXPECT_CALL(*mock_stm, AddState(_, _)).WillRepeatedly(Return(empty));

    EXPECT_CALL(*mock_ds, GetOverdoseProbabilityState(_))
        .WillRepeatedly(ReturnRef(vec));
    EXPECT_CALL(*mock_stm, MultiplyState(_, _)).WillRepeatedly(Return(empty));

    EXPECT_CALL(*mock_ds, GetOverdoseBeingFatalProbabilityState(_))
        .WillRepeatedly(ReturnRef(vec));

    EXPECT_CALL(*mock_ds, GetStandardMortalityRatioState(_))
        .WillRepeatedly(ReturnRef(vec));

    EXPECT_CALL(*mock_ds, GetBackgroundMortalityState(_))
        .WillRepeatedly(ReturnRef(vec));

    Eigen::VectorXd state(2);
    state << 5, 9;
    EXPECT_CALL(*mock_stm, GetState()).WillRepeatedly(Return(state));

    test_respond = synmodels::models::Respond::Create(mock_stm);
    test_respond->Run(mock_ds);
    auto result = test_respond->GetSimulationHistory();
    ASSERT_EQ(result.size(), 2);
    for (const auto &r : result) {
        ASSERT_TRUE(state.isApprox(r.second));
        ASSERT_NE(std::find(hts.begin(), hts.end(), r.first), hts.end());
    }
}