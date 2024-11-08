#ifndef DATA_RESPONDDATABLOCK_HPP_
#define DATA_RESPONDDATABLOCK_HPP_

#include "DataBlock.hpp"
#include "Tensor3d.hpp"

#include <unordered_map>
#include <vector>

namespace data {
    class IDemographics {
        virtual std::unordered_map<std::string, std::vector<std::string>>
        GetDemographics() = 0;
        virtual std::vector<std::string> GetCombinations() const = 0;
    };

    class IRespondDataBlock : public virtual IDataBlock {
    public:
        virtual Tensor3d GetInitialCohort() const = 0;
        virtual Tensor3d GetMigratingCohort(int timestep) const = 0;
        virtual Tensor3d GetBehaviorTransitions(int timestep) const = 0;
        virtual Tensor3d GetInterventionTransitions(int timestep) const = 0;
        virtual Tensor3d
        GetBehaviorTransitionsAfterInterventionChange(int timestep) const = 0;
        virtual Tensor3d GetOverdoseProbabilities(int timestep) const = 0;
        virtual Tensor3d
        GetProbabilitiesOfOverdoseBeingFatal(int timestep) const = 0;
        virtual Tensor3d GetStandardMortalityRatios(int timestep) const = 0;
        virtual Tensor3d GetBackgroundMortalities(int timestep) const = 0;
        virtual Tensor3d GetStandardMortalityRatios(int timestep) const = 0;
    };
} // namespace data

#endif