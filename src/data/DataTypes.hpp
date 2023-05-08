#ifndef DATA_DATATYPES_HPP_
#define DATA_DATATYPES_HPP_

#include <vector>
#include <eigen3/Eigen/Eigen>
#include <eigen3/unsupported/Eigen/CXX11/Tensor>

namespace Data{
    /// @brief
    using Matrix3d = Eigen::Tensor<double, 3>;

    /// @brief
    enum Dimension {
        INTERVENTION = 0,
        OUD = 1,
        DEMOGRAPHIC_COMBO = 2
    };

    /// @brief
    class Matrix3dOverTime{
    public:
        Matrix3dOverTime(){}
        Matrix3dOverTime(std::vector<Matrix3d> data);
        Matrix3dOverTime(std::vector<Matrix3d> data, std::vector<int> timestepChanges);

        /// @brief
        /// @param timestep
        /// @return
        Matrix3d getMatrix3dAtTimestep(int timestep);

        /// @brief
        /// @param datapoint
        /// @param timestep
        void insert(Matrix3d datapoint, int timestep);

        /// @brief
        /// @return
        std::vector<Matrix3d> getMatrices();
    private:
        std::map<int, Matrix3d> data;
    };

    struct History {
        Matrix3dOverTime stateHistory;
        Matrix3dOverTime overdoseHistory;
        Matrix3dOverTime fatalOverdoseHistory;
        Matrix3dOverTime mortalityHistory;
    };

    /// @brief
    using History = History;

    struct Cost {
        Matrix3dOverTime healthcareCost;
        Matrix3dOverTime nonFatalOverdoseCost;
        Matrix3dOverTime fatalOverdoseCost;
        Matrix3dOverTime pharmaCost;
        Matrix3dOverTime treatmentCost;
    };

    /// @brief
    using Cost = Cost;


};

#endif
