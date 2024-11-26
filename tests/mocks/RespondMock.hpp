#ifndef MOCKRESPOND_HPP_
#define MOCKRESPOND_HPP_

#include "Respond.hpp"

namespace models {
    class MOCKRespond : public IRespond {
    public:
        MOCK_METHOD(bool, Run,
                    (const std::shared_ptr<data::IRespondDataStore> &x),
                    (override));
        MOCK_METHOD(std::shared_ptr<Eigen::VectorXd>, GetState, (),
                    (const, override));
        MOCK_METHOD((std::unordered_map<int, std::shared_ptr<Eigen::VectorXd>>),
                    GetSimulationHistory, (), (const, override));
    };
} // namespace models

#endif