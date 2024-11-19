#include "RespondDataBlock.hpp"
#include <DataManagement/DataManager.hpp>
#include <cmath>
#include <string>
#include <unsupported/Eigen/CXX11/Tensor>

namespace data {
    class RespondDataBlock : public virtual IRespondDataBlock {
    public:
        RespondDataBlock() {
            _dm = std::make_shared<datamanagement::DataManager>();
        }
        int ConnectToDatabase(const std::string &db) override {
            return _dm->ConnectToDatabase(db);
        }
        int LoadConfig(const std::string &confile) override {
            int rc = _dm->LoadConfig(confile);
            BuildDemographicCombinations();
            return rc;
        }
        int LoadData(const int input_set, const int parameter_set,
                     const int year, const std::string &db,
                     const std::string &confile) override {
            if (!db.empty()) {
                ConnectToDatabase(db);
            }
            if (!confile.empty()) {
                LoadConfig(confile);
            }
            _input_set = input_set;
            _parameter_set = parameter_set;
            LoadInitialCohort();
            LoadMigratingCohort(year);
            LoadBehaviorTransitions(year);
            LoadInterventionTransitions(year);
            LoadBehaviorTransitionsAfterInterventionChange(year);
            LoadOverdoseProbabilities(year);
            LoadProbabilitiesOfOverdoseBeingFatal(year);
            LoadStandardMortalityRatios(year);
            LoadBackgroundMortalities(year);
            return 0;
        }
        int GetSimulationDuration() const override {
            std::string data;
            int rc = _dm->GetFromConfig("simulation.duration", data);
            if (!data.empty()) {
                return std::stoi(data);
            }
            return 0;
        }
        std::vector<int> GetDataDimensions() const override {
            int x = GetInterventions().size();
            int y = GetBehaviors().size();
            int z = GetDemographicCombinations().size();
            std::vector<int> res = {x, y, z};
            return res;
        }

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

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetInitialCohort() const override {
            return _initial_cohort;
        }
        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetMigratingCohort() const override {
            return _migrating_cohort;
        }

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetBehaviorTransitions() const override {
            return _behavior_transitions;
        }

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetInterventionTransitions() const override {
            return _intervention_transitions;
        }

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetBehaviorTransitionsAfterInterventionChange() const override {
            return _behavior_transitions_after_intervention_change;
        }

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetOverdoseProbabilities() const override {
            return _overdose_probabilities;
        }

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetProbabilitiesOfOverdoseBeingFatal() const override {
            return _probabilities_of_overdose_being_fatal;
        }

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetStandardMortalityRatios() const override {
            return _smrs;
        }

        std::shared_ptr<std::vector<Eigen::MatrixXd>>
        GetBackgroundMortalities() const override {
            return _background_mortalities;
        }

    private:
        std::shared_ptr<datamanagement::DataManager> _dm;
        std::vector<std::string> _demographic_combinations;
        std::shared_ptr<std::vector<Eigen::MatrixXd>> _initial_cohort = nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>> _migrating_cohort =
            nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>> _behavior_transitions =
            nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>>
            _intervention_transitions = nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>>
            _behavior_transitions_after_intervention_change = nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>> _overdose_probabilities =
            nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>>
            _probabilities_of_overdose_being_fatal = nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>> _smrs = nullptr;
        std::shared_ptr<std::vector<Eigen::MatrixXd>> _background_mortalities =
            nullptr;

        int _input_set = 1;
        int _parameter_set = 1;

        static int callback(void *storage, int count, char **data,
                            char **columns) {
            (*((std::vector<double> *)storage)).push_back(std::stod(data[0]));
            return 0;
        }

        Eigen::TensorMap<std::vector<Eigen::MatrixXd>>
        TensorBuilder(const std::string &query, int x, int y, int z) {
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

            Eigen::TensorMap<std::vector<Eigen::MatrixXd>> res(data.data(), x,
                                                               y, z);
            return res;
        }

        void LoadInitialCohort() {
            std::stringstream sql;
            sql << "SELECT counts FROM init_cohort WHERE input_set_id = ";
            sql << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "intervention ASC, behavior ASC;";
            std::string query = sql.str();

            std::vector<Eigen::MatrixXd> temp = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
            this->_initial_cohort =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }

        void LoadMigratingCohort(int year) {
            std::stringstream sql;
            sql << "SELECT weekly_cohort_size_change FROM entering_cohort ";
            sql << "INNER JOIN years on years.id = entering_cohort.year ";
            sql << "WHERE input_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY entering_cohort.year ASC, age_group ASC, sex "
                   "ASC, race ASC, "
                   "intervention ASC, behavior ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
            this->_migrating_cohort =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }

        void LoadBehaviorTransitions(int year) {
            std::stringstream sql;
            sql << "SELECT transition_probability FROM behavior_transitions ";
            sql << "INNER JOIN years on years.id = behavior_transitions.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "intervention ASC, start_behavior ASC, end_behavior ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp =
                TensorBuilder(query, GetInterventions().size(),
                              std::pow(GetBehaviors().size(), 2),
                              GetDemographicCombinations().size());
            this->_behavior_transitions =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }
        void LoadInterventionTransitions(int year) {
            std::stringstream sql;
            sql << "SELECT transition_probability FROM "
                   "intervention_transitions ";
            sql << "INNER JOIN years on years.id = "
                   "intervention_transitions.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "behavior ASC, start_intervention ASC, end_intervention "
                   "ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp = TensorBuilder(
                query, std::pow(GetInterventions().size(), 2),
                GetBehaviors().size(), GetDemographicCombinations().size());
            this->_intervention_transitions =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }
        void LoadBehaviorTransitionsAfterInterventionChange(int year) {
            std::stringstream sql;
            sql << "SELECT retention_probability FROM "
                   "behavior_transition_after_intervention_change ";
            sql << "INNER JOIN years on years.id = "
                   "behavior_transition_after_intervention_change.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, "
                   "initial_behavior ASC, new_intervention ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp =
                TensorBuilder(query, GetInterventions().size(),
                              std::pow(GetBehaviors().size(), 2),
                              GetDemographicCombinations().size());
            this->_behavior_transitions_after_intervention_change =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }
        void LoadOverdoseProbabilities(int year) {
            std::stringstream sql;
            sql << "SELECT percent_pop_overdoses FROM "
                   "overdoses ";
            sql << "INNER JOIN years on years.id = overdoses.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "behavior ASC, intervention ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
            this->_migrating_cohort =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }
        void LoadProbabilitiesOfOverdoseBeingFatal(int year) {
            std::stringstream sql;
            sql << "SELECT percent_overdoses_fatal FROM "
                   "overdoses ";
            sql << "INNER JOIN years on years.id = overdoses.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "behavior ASC, intervention ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
            this->_migrating_cohort =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }
        void LoadStandardMortalityRatios(int year) {
            std::stringstream sql;
            sql << "SELECT smr FROM "
                   "smr ";
            sql << "INNER JOIN years on years.id = smr.year ";
            sql << "WHERE input_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race "
                   "ASC, behavior ASC, intervention ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
            this->_migrating_cohort =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
        }
        void LoadBackgroundMortalities(int year) {
            std::stringstream sql;
            sql << "SELECT death_probability FROM "
                   "background_mortality ";
            sql << "INNER JOIN years on years.id = background_mortality.year ";
            sql << "WHERE input_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC;";
            std::string query = sql.str();
            std::vector<Eigen::MatrixXd> temp = TensorBuilder(
                query, GetInterventions().size(), GetBehaviors().size(),
                GetDemographicCombinations().size());
            this->_migrating_cohort =
                std::make_shared<std::vector<Eigen::MatrixXd>>(temp);
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

    std::shared_ptr<IRespondDataBlock>
    RespondDataBlockFactory::MakeDataBlock() {
        std::shared_ptr<IRespondDataBlock> res =
            std::make_shared<RespondDataBlock>();
        return res;
    }
} // namespace data