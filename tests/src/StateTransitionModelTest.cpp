#include "StateTransitionModel.hpp"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/spdlog.h"
#include <Eigen/Eigen>
#include <gtest/gtest.h>

#include <iostream>
#include <memory>
#include <vector>

class StateTransitionModelTest : public ::testing::Test {
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
    std::shared_ptr<kernels::IStateTransitionModel> test_stm;
    void SetUp() override {
        RegisterLogger();
        test_stm =
            kernels::StateTransitionModelFactory::MakeStateTransitionModel();
    }
    void TearDown() override { spdlog::drop("main"); }
};

TEST_F(StateTransitionModelTest, State) {
    Eigen::VectorXd vec(4);
    vec << 2, 4, 6, 8;
    std::shared_ptr<Eigen::VectorXd> vec_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec));
    test_stm->SetState(vec_ptr);
    EXPECT_TRUE(test_stm->GetState()->isApprox(*vec_ptr));
}

TEST_F(StateTransitionModelTest, Transitions) {
    Eigen::MatrixXd mat(2, 2);
    mat << 2, 4, 6, 8;
    std::shared_ptr<Eigen::MatrixXd> mat_ptr =
        std::make_shared<Eigen::MatrixXd>(std::move(mat));
    test_stm->SetTransitions(mat_ptr);
    EXPECT_TRUE(test_stm->GetTransitions()->isApprox(*mat_ptr));
}

TEST_F(StateTransitionModelTest, TransitionNotInPlace) {
    Eigen::VectorXd vec(2);
    vec << 1, 2;
    Eigen::MatrixXd mat(2, 2);
    mat << 2, 4, 6, 8;
    Eigen::VectorXd expected = mat * vec;
    std::shared_ptr<Eigen::VectorXd> vec_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec));
    std::shared_ptr<Eigen::MatrixXd> mat_ptr =
        std::make_shared<Eigen::MatrixXd>(std::move(mat));
    test_stm->SetState(vec_ptr);
    test_stm->SetTransitions(mat_ptr);
    auto result = test_stm->Transition(false);
    EXPECT_TRUE(result->isApprox(expected));
}

TEST_F(StateTransitionModelTest, TransitionInPlace) {
    Eigen::VectorXd vec(2);
    vec << 1, 2;
    Eigen::MatrixXd mat(2, 2);
    mat << 2, 4, 6, 8;
    Eigen::VectorXd expected = mat * vec;
    std::shared_ptr<Eigen::VectorXd> vec_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec));
    std::shared_ptr<Eigen::MatrixXd> mat_ptr =
        std::make_shared<Eigen::MatrixXd>(std::move(mat));
    test_stm->SetState(vec_ptr);
    test_stm->SetTransitions(mat_ptr);
    test_stm->Transition(true);
    EXPECT_TRUE(test_stm->GetState()->isApprox(expected));
}

TEST_F(StateTransitionModelTest, AddNotInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    Eigen::VectorXd vec2(2);
    vec2 << 2, 4;
    Eigen::VectorXd expected = vec1 + vec2;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    std::shared_ptr<Eigen::VectorXd> vec2_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec2));
    test_stm->SetState(vec1_ptr);
    EXPECT_TRUE(test_stm->AddState(vec2_ptr, false)->isApprox(expected));
}

TEST_F(StateTransitionModelTest, AddInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    Eigen::VectorXd vec2(2);
    vec2 << 2, 4;
    Eigen::VectorXd expected = vec1 + vec2;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    std::shared_ptr<Eigen::VectorXd> vec2_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec2));
    test_stm->SetState(vec1_ptr);
    test_stm->AddState(vec2_ptr, true);
    auto result = test_stm->GetState();
    EXPECT_TRUE(result->isApprox(expected));
}

TEST_F(StateTransitionModelTest, SubtractNotInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    Eigen::VectorXd vec2(2);
    vec2 << 2, 4;
    Eigen::VectorXd expected = vec1 - vec2;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    std::shared_ptr<Eigen::VectorXd> vec2_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec2));
    test_stm->SetState(vec1_ptr);
    EXPECT_TRUE(test_stm->SubtractState(vec2_ptr, false)->isApprox(expected));
}

TEST_F(StateTransitionModelTest, SubtractInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    Eigen::VectorXd vec2(2);
    vec2 << 2, 4;
    Eigen::VectorXd expected = vec1 - vec2;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    std::shared_ptr<Eigen::VectorXd> vec2_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec2));
    test_stm->SetState(vec1_ptr);
    test_stm->SubtractState(vec2_ptr, true);
    auto result = test_stm->GetState();
    EXPECT_TRUE(result->isApprox(expected));
}

TEST_F(StateTransitionModelTest, MultiplyNotInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    Eigen::VectorXd vec2(2);
    vec2 << 2, 4;
    Eigen::VectorXd expected = vec1.cwiseProduct(vec2);
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    std::shared_ptr<Eigen::VectorXd> vec2_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec2));
    test_stm->SetState(vec1_ptr);
    EXPECT_TRUE(test_stm->MultiplyState(vec2_ptr, false)->isApprox(expected));
}

TEST_F(StateTransitionModelTest, MultiplyInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    Eigen::VectorXd vec2(2);
    vec2 << 2, 4;
    Eigen::VectorXd expected = vec1.cwiseProduct(vec2);
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    std::shared_ptr<Eigen::VectorXd> vec2_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec2));
    test_stm->SetState(vec1_ptr);
    test_stm->MultiplyState(vec2_ptr, true);
    auto result = test_stm->GetState();
    EXPECT_TRUE(result->isApprox(expected));
}

TEST_F(StateTransitionModelTest, ScalarMultiplyNotInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    double scalar = 2.5;
    Eigen::VectorXd expected = vec1 * scalar;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    test_stm->SetState(vec1_ptr);
    EXPECT_TRUE(
        test_stm->ScalarMultiplyState(scalar, false)->isApprox(expected));
}

TEST_F(StateTransitionModelTest, ScalarMultiplyInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    double scalar = 2.5;
    Eigen::VectorXd expected = vec1 * scalar;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    test_stm->SetState(vec1_ptr);
    test_stm->ScalarMultiplyState(scalar, true);
    auto result = test_stm->GetState();
    EXPECT_TRUE(result->isApprox(expected));
}

TEST_F(StateTransitionModelTest, DivideNotInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    double scalar = 2.5;
    Eigen::VectorXd expected = vec1 / scalar;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    test_stm->SetState(vec1_ptr);
    EXPECT_TRUE(test_stm->DivideState(scalar, false)->isApprox(expected));
}

TEST_F(StateTransitionModelTest, DivideInPlace) {
    Eigen::VectorXd vec1(2);
    vec1 << 1, 2;
    double scalar = 2.5;
    Eigen::VectorXd expected = vec1 / scalar;
    std::shared_ptr<Eigen::VectorXd> vec1_ptr =
        std::make_shared<Eigen::VectorXd>(std::move(vec1));
    test_stm->SetState(vec1_ptr);
    test_stm->DivideState(scalar, true);
    auto result = test_stm->GetState();
    EXPECT_TRUE(result->isApprox(expected));
}
