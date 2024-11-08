#include "StateTransitionModel.hpp"
#include "RespondDataBlock.hpp"
#include "Tensor3dFactory.hpp"

namespace model {
    class StateTransitionModel : public virtual IStateTransitionModel {
    public:
        StateTransitionModel(uint16_t x, uint16_t y, uint16_t z,
                             bool store_history = true) {
            _state = std::make_shared<data::Tensor3d>(
                std::move(data::Tensor3dFactory::Create(x, y, z)));
        }
        StateTransitionModel(bool store_history = true)
            : StateTransitionModel(0, 0, 0, store_history) {}

        bool Step() override {
            auto operations = GetStepOperations();
            for (auto op : operations) {
                op(_state, _data);
            }
            if (_store_history) {
                data::Tensor3d state = *_state;
                _history.push_back(state);
            }
        }

        int GetCurrentTime() const override { return _timestep; }
        int GetDuration() const override { return _duration; }
        int LoadDatabase(std::string &const db_file) override {
            return _data->LoadDatabase(db_file);
        }
        int LoadConfig(std::string &const conf_file) override {
            int rc = _data->LoadConfig(conf_file);
            _duration = _data->GetSimulationDuration();
            _store_history = _data->GetStoreHistoryStatus();
            if (_store_history) {
                _stored_history_timepoints =
                    _data->GetHistoryTimestepsToStore();
            }
            auto dims = _data->GetDataDimensions();
            if (dims.size() < 2) {
                return -1;
            }
            _state = std::make_shared<data::Tensor3d>(std::move(
                data::Tensor3dFactory::Create(dims[0], dims[1], dims[2])));
            return rc;
        }
        void AppendStepOperation(
            std::function<void(std::shared_ptr<data::Tensor3d>,
                               std::shared_ptr<data::IDataBlock>)>
                func) override {
            _operations.push_back(func);
        }
        std::vector<std::function<void(std::shared_ptr<data::Tensor3d>,
                                       std::shared_ptr<data::IDataBlock>)>>
        GetStepOperations() const override {
            return _operations;
        }
        std::vector<data::Tensor3d> GetStateHistory() const override {
            return _history;
        }
        std::shared_ptr<data::Tensor3d> GetCurrentState() const override {
            return _state;
        }

    private:
        std::shared_ptr<data::Tensor3d> _state;
        bool _store_history = true;
        std::vector<int> _stored_history_timepoints = {};
        std::vector<data::Tensor3d> _history;
        std::shared_ptr<data::IDataBlock> _data;
        std::vector<std::function<void(std::shared_ptr<data::Tensor3d>,
                                       std::shared_ptr<data::IDataBlock>)>>
            _operations = {};
        int _timestep = 0;
        int _duration = 0;
    };
} // namespace model