#ifndef RESPONDMOCK_HPP_
#define RESPONDMOCK_HPP_

#include "Respond.hpp"

namespace models {
    class RespondMock : public IRespond {
        MOCK_METHOD(bool, Run, (), (override));
        MOCK_METHOD(bool, Step, (), (override));
        MOCK_METHOD((std::shared_ptr<kernels::IStateTransitionModel>), GetModel,
                    (), (const, override));
        MOCK_METHOD((std::shared_ptr<data::IRespondDataStore>), GetDataStore,
                    (), (const, override));

        MOCK_METHOD((std::shared_ptr<std::vector<std::string>>),
                    GetInterventions, (), (const, override));
        MOCK_METHOD((std::shared_ptr<std::vector<std::string>>), GetBehaviors,
                    (), (const, override));

        MOCK_METHOD(void, SetInterventions,
                    (const(std::shared_ptr<std::vector<std::string>>) &),
                    (override));
        MOCK_METHOD(void, SetBehaviors,
                    (const(std::shared_ptr<std::vector<std::string>>) &),
                    (override));
    };
} // namespace models

#endif