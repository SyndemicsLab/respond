#ifndef MODELS_RESPONDIMPL_HPP_
#define MODELS_RESPONDIMPL_HPP_

#include <Eigen/Eigen>
#include <synmodels/models/Respond.hpp>

namespace synmodels::models {
    class RespondImpl : public virtual Respond {
    public:
        RespondImpl(STMODEL_POINTER model = nullptr);
        ~RespondImpl() = default;
        bool Run(const RDATASTORE_POINTER &data_store) override;
        Eigen::VectorXd const GetState() const override;
        HISTORY_MAP const &GetSimulationHistory() const override;

    private:
        int timestep = 0;
        STMODEL_POINTER _model;
        RDATASTORE_POINTER _data_store;
        HISTORY_MAP _history = {};

        bool Step();
        STMODEL_POINTER AddMigratingCohort() const;
        STMODEL_POINTER Transition() const;
        STMODEL_POINTER MultiplyOverdoseProbabilities() const;
        STMODEL_POINTER MultiplyProbabilitiesOfFatalityGivenOverdose() const;
        STMODEL_POINTER MultiplyStandardMortalityRatios() const;
        STMODEL_POINTER MultiplyBackgroundMortalityProbabilities() const;
        void SnapshotHistory();
    };

} // namespace synmodels::models

#endif