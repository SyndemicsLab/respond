#include "RespondDataBlock.hpp"
#include <DataManagement/DataManager.hpp>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>

namespace data {
    class RespondDataBlock : public virtual IRespondDataBlock {
    public:
        RespondDataBlock() {
            _dm = std::make_shared<datamanagement::DataManager>();
        }
        int LoadDatabase(const std::string &db) override {
            return _dm->ConnectToDatabase(db);
        }
        int LoadConfig(const std::string &confile) override {
            int rc = _dm->LoadConfig(confile);
            BuildDemographicCombinations();
            return rc;
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
            ParseIniLine(timesteps, data);
            return timesteps;
        }

        std::vector<std::string> GetInterventions() const override {
            return GetStringVectorFromConfig("state.interventions");
        }
        std::vector<std::string> GetBehaviors() const override {
            return GetStringVectorFromConfig("state.behaviors");
        }
        std::vector<std::string> GetDemographicTypes() const override {
            std::vector<std::string> dem_types = {};
            _dm->GetConfigSectionCategories("demographic", dem_types);
            return dem_types;
        }
        std::vector<std::string> GetDemographicCombinations() const override {
            return _demographic_combinations;
        }

        std::shared_ptr<Tensor3d> GetInitialCohort() const override {
            return _initial_cohort;
        }
        std::shared_ptr<Tensor3d> GetMigratingCohort() const override {
            return _migrating_cohort;
        }

        std::shared_ptr<Tensor3d> GetBehaviorTransitions() const override {}

        std::shared_ptr<Tensor3d> GetInterventionTransitions() const override {}

        std::shared_ptr<Tensor3d>
        GetBehaviorTransitionsAfterInterventionChange() const override {}

        std::shared_ptr<Tensor3d> GetOverdoseProbabilities() const override {}

        std::shared_ptr<Tensor3d>
        GetProbabilitiesOfOverdoseBeingFatal() const override {}

        std::shared_ptr<Tensor3d> GetStandardMortalityRatios() const override {}

        std::shared_ptr<Tensor3d> GetBackgroundMortalities() const override {}

    private:
        std::shared_ptr<datamanagement::DataManager> _dm;
        std::vector<std::string> _demographic_combinations;
        std::shared_ptr<Tensor3d> _initial_cohort = nullptr;
        std::shared_ptr<Tensor3d> _migrating_cohort = nullptr;
        std::shared_ptr<Tensor3d> _behavior_transitions = nullptr;
        std::shared_ptr<Tensor3d> _intervention_transitions = nullptr;
        std::shared_ptr<Tensor3d>
            _behavior_transitions_after_intervention_change = nullptr;
        std::shared_ptr<Tensor3d> _overdose_probabilities = nullptr;
        std::shared_ptr<Tensor3d> _probabilities_of_overdose_being_fatal =
            nullptr;
        std::shared_ptr<Tensor3d> _smrs = nullptr;
        std::shared_ptr<Tensor3d> _background_mortalities = nullptr;

        int _input_set = 1;
        int _parameter_set = 1;

        static int callback(void *storage, int count, char **data,
                            char **columns) {
            (*((std::vector<double> *)storage)).push_back(std::stod(data[0]));
            return 0;
        }

        Eigen::TensorMap<data::Tensor3d> TensorBuilder(const std::string &query,
                                                       int x, int y, int z) {
            std::string error;
            std::vector<double> data;
            int rc =
                _dm->SelectCustomCallback(query, this->callback, &data, error);
            if (rc != 0) {
                // error
            }
            if (data.empty()) {
                // error
            }

            Eigen::TensorMap<data::Tensor3d> res(data.data(), x, y, z);
            return res;
        }

        void LoadInitialCohort() {
            std::stringstream sql;
            sql << "SELECT counts FROM init_cohort WHERE input_set_id = ";
            sql << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "intervention ASC, behavior ASC;";
            std::string query = sql.str();

            data::Tensor3d temp = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
            // this->_initial_cohort = std::make_shared<data::Tensor3d>(temp);
            // Not sure if this works?
        }

        void LoadMigratingCohort(int year) {
            std::stringstream sql;
            sql << "SELECT weekly_cohort_size_change FROM entering_cohort ";
            sql << "WHERE input_set_id = " << _input_set;
            sql << " ORDER BY " << year;
            sql << " ASC, age_group ASC, sex ASC, race ASC, "
                   "intervention ASC, behavior ASC;";
            std::string query = sql.str();
            this->_migrating_cohort = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
        }

        void LoadBehaviorTransitions(int timestep) {
            std::stringstream sql;
            sql << "SELECT transition_probability FROM behavior_transitions ";
            sql << "WHERE parameter_set_id = " << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "intervention ASC, start_behavior ASC, end_behavior ASC;";
            std::string query = sql.str();
            this->_behavior_transitions = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
        }
        void LoadInterventionTransitions(int timestep) {
            std::stringstream sql;
            sql << "SELECT transition_probability FROM "
                   "intervention_transitions ";
            sql << "WHERE parameter_set_id = " << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "behavior ASC, start_intervention ASC, end_intervention "
                   "ASC;";
            std::string query = sql.str();
            this->_intervention_transitions = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
        }
        void LoadBehaviorTransitionsAfterInterventionChange(int timestep) {
            std::stringstream sql;
            sql << "SELECT retention_probability FROM "
                   "behavior_transition_after_intervention_change ";
            sql << "WHERE parameter_set_id = " << _input_set;
            sql << " ORDER BY year ASC, "
                   "initial_behavior ASC, new_intervention ASC;";
            std::string query = sql.str();
            this->_behavior_transitions_after_intervention_change =
                TensorBuilder(query, GetInterventions().size(),
                              GetBehaviors().size(),
                              GetDemographicCombinations().size());
        }
        void LoadOverdoseProbabilities(int timestep) {
            std::stringstream sql;
            sql << "SELECT percent_pop_overdoses FROM "
                   "overdoses ";
            sql << "WHERE parameter_set_id = " << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "behavior ASC, intervention ASC;";
            std::string query = sql.str();
            this->_migrating_cohort = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
        }
        void LoadProbabilitiesOfOverdoseBeingFatal(int timestep) {
            std::stringstream sql;
            sql << "SELECT percent_overdoses_fatal FROM "
                   "overdoses ";
            sql << "WHERE parameter_set_id = " << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "behavior ASC, intervention ASC;";
            std::string query = sql.str();
            this->_migrating_cohort = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
        }
        void LoadStandardMortalityRatios(int timestep) {
            std::stringstream sql;
            sql << "SELECT smr FROM "
                   "smr ";
            sql << "WHERE input_set_id = " << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "behavior ASC, intervention ASC;";
            std::string query = sql.str();
            this->_migrating_cohort = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
        }
        void LoadBackgroundMortalities(int timestep) {
            std::stringstream sql;
            sql << "SELECT death_probability FROM "
                   "background_mortality ";
            sql << "WHERE input_set_id = " << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC;";
            std::string query = sql.str();
            this->_migrating_cohort = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
        }

        std::vector<std::string>
        GetStringVectorFromConfig(const std::string &key) const {
            std::string data;
            int rc = _dm->GetFromConfig(key, data);
            std::vector<std::string> values = {};
            ParseIniLine(values, data);
            return values;
        }

        void ParseIniLine(std::vector<std::string> &vec,
                          const std::string &data) const {
            if (!data.empty()) {
                std::stringstream s(data);
                while (s.good()) {
                    std::string substr;
                    getline(s, substr, ',');
                    trim(substr);
                    vec.push_back(substr);
                }
            }
        }

        void ParseIniLine(std::vector<int> &vec,
                          const std::string &data) const {
            if (!data.empty()) {
                std::stringstream s(data);
                while (s.good()) {
                    std::string substr;
                    getline(s, substr, ',');
                    trim(substr);
                    vec.push_back(std::stoi(substr));
                }
            }
        }

        void trim(std::string &str) const {
            while (str[0] == ' ')
                str.erase(str.begin());
            while (str[str.size() - 1] == ' ')
                str.pop_back();
        }

        void BuildDemographicCombinations() {
            _demographic_combinations = {};
            std::vector<std::string> dem_types = GetDemographicTypes();
            std::vector<std::vector<std::string>> dem_list;
            for (std::string &dem : dem_types) {
                std::string key = "demographic." + dem;
                dem_list.push_back(GetStringVectorFromConfig(key));
            }

            std::vector<std::vector<std::string>> final_result;
            std::vector<std::string> running_tally;
            RecursiveDemographicBuilder(final_result, running_tally,
                                        dem_list.begin(), dem_list.end());
            for (std::vector<std::string> str_vec : final_result) {
                std::string group = "";
                for (std::string st : str_vec) {
                    std::transform(
                        st.begin(), st.end(), st.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    group += " " + st;
                }
                _demographic_combinations.push_back(group);
            }
        }

        void RecursiveDemographicBuilder(
            std::vector<std::vector<std::string>> &final_result,
            std::vector<std::string> &current_result,
            std::vector<std::vector<std::string>>::const_iterator current_in,
            std::vector<std::vector<std::string>>::const_iterator final_in)
            const {
            if (current_in == final_in) {
                final_result.push_back(current_result);
                return;
            }
            const std::vector<std::string> &temp = *current_in;
            for (std::vector<std::string>::const_iterator it = temp.begin();
                 it != temp.end(); it++) {
                current_result.push_back(*it);
                RecursiveDemographicBuilder(final_result, current_result,
                                            current_in + 1, final_in);
                current_result.pop_back();
            }
        }
    };
} // namespace data