#ifndef DATA_RESPONDDATABLOCK_HPP_
#define DATA_RESPONDDATABLOCK_HPP_

#include "DataBlock.hpp"
#include "Tensor3d.hpp"

#include <memory>
#include <unordered_map>
#include <vector>

namespace data {
    enum class axis { INTERVENTION = 0, BEHAVIOR = 1, DEMOGRAPHIC = 2 };
    class IRespondDataBlock : public virtual IDataBlock {
    public:
        virtual std::vector<std::string> GetInterventions() const = 0;
        virtual std::vector<std::string> GetBehaviors() const = 0;
        virtual std::vector<std::string> GetDemographicTypes() const = 0;
        virtual std::vector<std::string> GetDemographicCombinations() const = 0;
        virtual std::shared_ptr<Tensor3d> GetInitialCohort() const = 0;
        virtual std::shared_ptr<Tensor3d> GetMigratingCohort() const = 0;
        virtual std::shared_ptr<Tensor3d> GetBehaviorTransitions() const = 0;
        virtual std::shared_ptr<Tensor3d>
        GetInterventionTransitions() const = 0;
        virtual std::shared_ptr<Tensor3d>
        GetBehaviorTransitionsAfterInterventionChange() const = 0;
        virtual std::shared_ptr<Tensor3d> GetOverdoseProbabilities() const = 0;
        virtual std::shared_ptr<Tensor3d>
        GetProbabilitiesOfOverdoseBeingFatal() const = 0;
        virtual std::shared_ptr<Tensor3d>
        GetStandardMortalityRatios() const = 0;
        virtual std::shared_ptr<Tensor3d> GetBackgroundMortalities() const = 0;
    };
} // namespace data

#endif