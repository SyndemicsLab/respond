#ifndef DATA_RESPONDDATABLOCK_HPP_
#define DATA_RESPONDDATABLOCK_HPP_

#include "DataBlock.hpp"

#include <memory>
#include <unordered_map>

namespace Eigen {
    template <typename _Scalar, int _Rows, int _Cols, int _Options,
              int _MaxRows, int _MaxCols>
    class Matrix;
    using MatrixXd = Matrix<double, -1, -1, 0, -1, -1>;
    using VectorXd = Matrix<double, -1, 1, 0, -1, 1>;
} // namespace Eigen

namespace data {
    enum class axis { INTERVENTION = 0, BEHAVIOR = 1 };
    using ModelsVec = std::vector<std::shared_ptr<Eigen::MatrixXd>>;
    class IRespondDataBlock : public virtual IDataBlock {
    public:
        virtual std::vector<std::string> GetInterventions() const = 0;
        virtual std::vector<std::string> GetBehaviors() const = 0;
        virtual std::vector<std::string> GetDemographicTypes() const = 0;
        virtual std::vector<std::string> GetDemographicCombinations() const = 0;
        virtual std::shared_ptr<ModelsVec> GetInitialCohort() const = 0;
        virtual std::shared_ptr<ModelsVec> GetMigratingCohort() const = 0;
        virtual std::shared_ptr<ModelsVec> GetBehaviorTransitions() const = 0;
        virtual std::shared_ptr<ModelsVec>
        GetInterventionTransitions() const = 0;
        virtual std::shared_ptr<ModelsVec>
        GetBehaviorTransitionsAfterInterventionChange() const = 0;
        virtual std::shared_ptr<ModelsVec> GetOverdoseProbabilities() const = 0;
        virtual std::shared_ptr<ModelsVec>
        GetProbabilitiesOfOverdoseBeingFatal() const = 0;
        virtual std::shared_ptr<ModelsVec>
        GetStandardMortalityRatios() const = 0;
        virtual std::shared_ptr<ModelsVec> GetBackgroundMortalities() const = 0;
    };

    class RespondDataBlockFactory {
    public:
        static std::shared_ptr<IRespondDataBlock> MakeDataBlock();
    };
} // namespace data

#endif