#include "RespondDataBlock.hpp"
#include <DataManagement/DataManager.hpp>
#include <string>

namespace data {
    class RespondDataBlock : public virtual IRespondDataBlock {
    public:
        RespondDataBlock() {
            _dm = std::make_shared<datamanagement::DataManager>();
        }
        int LoadDatabase(std::string &const db) override {
            return _dm->ConnectToDatabase(db);
        }
        int LoadConfig(std::string &const confile) override {
            return _dm->LoadConfig(confile);
        }
        int GetSimulationDuration() const override {
            std::string data;
            int rc = _dm->GetFromConfig("simulation.duration", data);
            if (!data.empty()) {
                return std::stoi(data);
            }
            return 0;
        }
        std::vector<int> GetDataDimensions() const override {}
        bool GetStoreHistoryStatus() const override {
            std::string data;
            int rc = _dm->GetFromConfig("output.general_outputs", data);
            bool res = false;
            if (!data.empty()) {
                std::istringstream(data) >> res;
            }
            return res;
        }
        std::vector<int> GetHistoryTimestepsToStore() const override {
            std::string data;
            int rc = _dm->GetFromConfig("output.general_stats_output_timesteps",
                                        data);
            std::vector<int> timesteps = {};
            if (!data.empty()) {
                std::stringstream s(data);
                while (s.good()) {
                    std::string substr;
                    getline(s, substr, ',');
                    trim(substr);
                    timesteps.push_back(std::stoi(substr));
                }
            }
            return timesteps;
        }
        Tensor3d GetInitialCohort() const override {}
        Tensor3d GetMigratingCohort(int timestep) const override {}
        Tensor3d GetBehaviorTransitions(int timestep) const override {}
        Tensor3d GetInterventionTransitions(int timestep) const override {}
        Tensor3d GetBehaviorTransitionsAfterInterventionChange(
            int timestep) const override {}
        Tensor3d GetOverdoseProbabilities(int timestep) const override {}
        Tensor3d
        GetProbabilitiesOfOverdoseBeingFatal(int timestep) const override {}
        Tensor3d GetStandardMortalityRatios(int timestep) const override {}
        Tensor3d GetBackgroundMortalities(int timestep) const override {}
        Tensor3d GetStandardMortalityRatios(int timestep) const override {}

    private:
        std::shared_ptr<datamanagement::DataManager> _dm;

        void trim(std::string &str) const {
            while (str[0] == ' ')
                str.erase(str.begin());
            while (str[str.size() - 1] == ' ')
                str.pop_back();
        }
    };
} // namespace data