#include "RespondDataBlock.hpp"
#include <DataManagement/DataManager.hpp>
#include <Eigen/Eigen>
#include <cmath>
#include <string>

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
        std::vector<std::vector<std::string>>
        GetDemographicCombinations() const override {
            return _demographic_combinations;
        }

        std::shared_ptr<ModelsVec> GetInitialCohort() const override {
            return _initial_cohort;
        }
        std::shared_ptr<ModelsVec> GetMigratingCohort() const override {
            return _migrating_cohort;
        }

        std::shared_ptr<ModelsVec> GetBehaviorTransitions() const override {
            return _behavior_transitions;
        }

        std::shared_ptr<ModelsVec> GetInterventionTransitions() const override {
            return _intervention_transitions;
        }

        std::shared_ptr<ModelsVec>
        GetBehaviorTransitionsAfterInterventionChange() const override {
            return _behavior_transitions_after_intervention_change;
        }

        std::shared_ptr<ModelsVec> GetOverdoseProbabilities() const override {
            return _overdose_probabilities;
        }

        std::shared_ptr<ModelsVec>
        GetProbabilitiesOfOverdoseBeingFatal() const override {
            return _probabilities_of_overdose_being_fatal;
        }

        std::shared_ptr<ModelsVec> GetStandardMortalityRatios() const override {
            return _smrs;
        }

        std::shared_ptr<ModelsVec> GetBackgroundMortalities() const override {
            return _background_mortalities;
        }

    private:
        const std::string INNERJOIN_DEMOGRAPHIC_STRING =
            "INNER JOIN age_group AS ag ON ag.id = ec.age_group INNER JOIN sex "
            "AS s ON s.id = ec.sex INNER JOIN race AS r ON r.id = ec.race ";

        std::shared_ptr<datamanagement::DataManager> _dm;
        std::vector<std::vector<std::string>> _demographic_combinations;
        std::shared_ptr<ModelsVec> _initial_cohort = nullptr;
        std::shared_ptr<ModelsVec> _migrating_cohort = nullptr;
        std::shared_ptr<ModelsVec> _behavior_transitions = nullptr;
        std::shared_ptr<ModelsVec> _intervention_transitions = nullptr;
        std::shared_ptr<ModelsVec>
            _behavior_transitions_after_intervention_change = nullptr;
        std::shared_ptr<ModelsVec> _overdose_probabilities = nullptr;
        std::shared_ptr<ModelsVec> _probabilities_of_overdose_being_fatal =
            nullptr;
        std::shared_ptr<ModelsVec> _smrs = nullptr;
        std::shared_ptr<ModelsVec> _background_mortalities = nullptr;

        int _input_set = 1;
        int _parameter_set = 1;

        static int callback(void *storage, int count, char **data,
                            char **columns) {
            (*((std::vector<double> *)storage)).push_back(std::stod(data[0]));
            return 0;
        }

        std::string BuildDemographicWhereClause(
            std::vector<std::string> demographic_combination) {
            std::stringstream clause;
            clause << "WHERE ag.age_group = " << demographic_combination[0];
            clause << " AND s.sex = " << demographic_combination[1];
            clause << " AND r.race = " << demographic_combination[2];
            return clause.str();
        }

        Eigen::MatrixXd MatrixBuilder(const std::string &query, int x, int y) {
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

            Eigen::MatrixXd res =
                Eigen::Map<Eigen::MatrixXd>(data.data(), x, y);
            return res;
        }

        void LoadInitialCohort() {
            std::stringstream sql;
            sql << "SELECT counts FROM init_cohort WHERE input_set_id = ";
            sql << _input_set;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "intervention ASC, behavior ASC;";
            std::string query = sql.str();

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

            ModelsVec temp = {};

            for (int i = 0; i < GetDemographicCombinations().size(); ++i) {
                Eigen::MatrixXd res = Eigen::Map<Eigen::MatrixXd>(
                    data.data(), GetInterventions().size(),
                    GetBehaviors().size());
                std::shared_ptr<Eigen::MatrixXd> res_ptr =
                    std::make_shared<Eigen::MatrixXd>(res);
                temp.push_back(res_ptr);
            }

            this->_initial_cohort = std::make_shared<ModelsVec>(temp);
        }

        void LoadMigratingCohort(int year) {
            std::stringstream sql;
            sql << "SELECT weekly_cohort_size_change ";
            sql << "FROM entering_cohort AS ec ";
            sql << "INNER JOIN years AS y ON y.id = ec.year ";
            sql << INNERJOIN_DEMOGRAPHIC_STRING;
            sql << BuildDemographicWhereClause({});
            sql << " AND input_set_id = " << _input_set;
            sql << " AND y.year = " << year;
            sql << " ORDER BY ec.year ASC, ec.age_group ASC, ec.sex "
                   "ASC, ec.race ASC, ec.intervention ASC, ec.behavior ASC;";
            std::string query = sql.str();
            Eigen::MatrixXd temp = MatrixBuilder(
                query, GetInterventions().size(), GetBehaviors().size());
            this->_migrating_cohort = std::make_shared<ModelsVec>(temp);
        }

        void LoadBehaviorTransitions(int year) {
            std::stringstream sql;
            sql << "SELECT transition_probability ";
            sql << "FROM behavior_transitions as bt";
            sql << "INNER JOIN years on years.id = behavior_transitions.year ";
            sql << INNERJOIN_DEMOGRAPHIC_STRING;
            sql << BuildDemographicWhereClause({});
            sql << " AND parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY year ASC, age_group ASC, sex ASC, race ASC, "
                   "intervention ASC, start_behavior ASC, end_behavior ASC;";
            std::string query = sql.str();
            Eigen::MatrixXd temp =
                MatrixBuilder(query, GetInterventions().size(),
                              std::pow(GetBehaviors().size(), 2));
            this->_behavior_transitions = std::make_shared<ModelsVec>(temp);
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
            Eigen::MatrixXd temp =
                MatrixBuilder(query, std::pow(GetInterventions().size(), 2),
                              GetBehaviors().size());
            this->_intervention_transitions = std::make_shared<ModelsVec>(temp);
        }
        void LoadBehaviorTransitionsAfterInterventionChange(int year) {
            std::stringstream sql;
            sql << "SELECT retention_probability FROM "
                   "behavior_transition_after_intervention_change AS btaic ";
            sql << "INNER JOIN years ON years.id = "
                   "behavior_transition_after_intervention_change.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY btaic.year ASC, "
                   "initial_behavior ASC, new_intervention ASC;";
            std::string query = sql.str();
            Eigen::MatrixXd temp =
                MatrixBuilder(query, GetInterventions().size(),
                              std::pow(GetBehaviors().size(), 2));
            this->_behavior_transitions_after_intervention_change =
                std::make_shared<ModelsVec>(temp);
        }
        void LoadOverdoseProbabilities(int year) {
            std::stringstream sql;
            sql << "SELECT percent_pop_overdoses FROM "
                   "overdoses ";
            sql << "INNER JOIN years on years.id = overdoses.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY overdoses.year ASC, age_group ASC, sex ASC, race "
                   "ASC, behavior ASC, intervention ASC;";
            std::string query = sql.str();
            Eigen::MatrixXd temp = MatrixBuilder(
                query, GetInterventions().size(), GetBehaviors().size());
            this->_migrating_cohort = std::make_shared<ModelsVec>(temp);
        }
        void LoadProbabilitiesOfOverdoseBeingFatal(int year) {
            std::stringstream sql;
            sql << "SELECT percent_overdoses_fatal FROM "
                   "overdoses ";
            sql << "INNER JOIN years on years.id = overdoses.year ";
            sql << "WHERE parameter_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY overdoses.year ASC, age_group ASC, sex ASC, race "
                   "ASC, behavior ASC, intervention ASC;";
            std::string query = sql.str();
            Eigen::MatrixXd temp = MatrixBuilder(
                query, GetInterventions().size(), GetBehaviors().size());
            this->_migrating_cohort = std::make_shared<ModelsVec>(temp);
        }
        void LoadStandardMortalityRatios(int year) {
            std::stringstream sql;
            sql << "SELECT smr FROM "
                   "smr ";
            sql << "INNER JOIN years on years.id = smr.year ";
            sql << "WHERE input_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY smr.year ASC, age_group ASC, sex ASC, race "
                   "ASC, behavior ASC, intervention ASC;";
            std::string query = sql.str();
            Eigen::MatrixXd temp = MatrixBuilder(
                query, GetInterventions().size(), GetBehaviors().size());
            this->_migrating_cohort = std::make_shared<ModelsVec>(temp);
        }
        void LoadBackgroundMortalities(int year) {
            std::stringstream sql;
            sql << "SELECT death_probability FROM "
                   "background_mortality ";
            sql << "INNER JOIN years on years.id = background_mortality.year ";
            sql << "WHERE input_set_id = " << _input_set
                << " AND years.year = " << year;
            sql << " ORDER BY background_mortality.year ASC, age_group ASC, "
                   "sex ASC, race ASC;";
            std::string query = sql.str();
            Eigen::MatrixXd temp = MatrixBuilder(
                query, GetInterventions().size(), GetBehaviors().size());
            this->_migrating_cohort = std::make_shared<ModelsVec>(temp);
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
                std::vector<std::string> group = {};
                for (std::string st : str_vec) {
                    std::transform(
                        st.begin(), st.end(), st.begin(),
                        [](unsigned char c) { return std::tolower(c); });
                    group.push_back(st);
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