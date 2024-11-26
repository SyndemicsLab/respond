#ifndef DATA_RESPONDDataStore_HPP_
#define DATA_RESPONDDataStore_HPP_

#include <memory>
#include <unordered_map>
#include <vector>

namespace Eigen {
    template <typename _Scalar, int _Rows, int _Cols, int _Options,
              int _MaxRows, int _MaxCols>
    class Matrix;
    using MatrixXd = Matrix<double, -1, -1, 0, -1, -1>;
    using VectorXd = Matrix<double, -1, 1, 0, -1, 1>;
} // namespace Eigen

namespace data {
    class IRespondDataStore {
    public:
        virtual int GetDuration() const = 0;
        virtual bool GetStoreHistoryStatus() const = 0;
        virtual std::vector<int> GetHistoryTimestepsToStore() const = 0;

        virtual void SetDuration(const int &) = 0;
        virtual void SetStoreHistoryStatus(const bool &) = 0;
        virtual void SetHistoryTimestepsToStore(const std::vector<int> &) = 0;

        virtual std::shared_ptr<Eigen::VectorXd>
        GetInitialCohortState() const = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        GetMigratingCohortState(const int &timestep) const = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        GetBehaviorTransitions(const int &timestep) const = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        GetInterventionTransitions(const int &timestep) const = 0;
        virtual std::shared_ptr<Eigen::MatrixXd>
        GetBehaviorAfterInterventionTransitions(const int &timestep) const = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        GetOverdoseProbabilityState(const int &timestep) const = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        GetOverdoseBeingFatalProbabilityState(const int &timestep) const = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        GetStandardMortalityRatioState(const int &timestep) const = 0;
        virtual std::shared_ptr<Eigen::VectorXd>
        GetBackgroundMortalityState(const int &timestep) const = 0;

        virtual void
        SetInitialCohortState(std::shared_ptr<Eigen::VectorXd> &) = 0;
        virtual void SetMigratingCohortState(std::shared_ptr<Eigen::VectorXd> &,
                                             const int &timestep) = 0;
        virtual void SetBehaviorTransitions(std::shared_ptr<Eigen::MatrixXd> &,
                                            const int &timestep) = 0;
        virtual void
        SetInterventionTransitions(std::shared_ptr<Eigen::MatrixXd> &,
                                   const int &timestep) = 0;
        virtual void SetBehaviorAfterInterventionTransitions(
            std::shared_ptr<Eigen::MatrixXd> &, const int &timestep) = 0;
        virtual void
        SetOverdoseProbabilityState(std::shared_ptr<Eigen::VectorXd> &,
                                    const int &timestep) = 0;
        virtual void SetOverdoseBeingFatalProbabilityState(
            std::shared_ptr<Eigen::VectorXd> &, const int &timestep) = 0;
        virtual void
        SetStandardMortalityRatioState(std::shared_ptr<Eigen::VectorXd> &,
                                       const int &timestep) = 0;
        virtual void
        SetBackgroundMortalityState(std::shared_ptr<Eigen::VectorXd> &,
                                    const int &timestep) = 0;
    };

    class RespondDataStoreFactory {
    public:
        static std::shared_ptr<IRespondDataStore> MakeDataStore();
    };
} // namespace data

#endif