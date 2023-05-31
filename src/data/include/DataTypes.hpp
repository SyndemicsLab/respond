#ifndef DATA_DATATYPES_HPP_
#define DATA_DATATYPES_HPP_

#include <vector>
#include <Eigen/Eigen>
#include <unsupported/Eigen/CXX11/Tensor>

namespace Data{
    /// @brief Eigen 3d Tensor
    using Matrix3d = Eigen::Tensor<double, 3>;

    /// @brief Specification for each dimension in the Matrix3d
    enum Dimension {
        INTERVENTION = 0,
        OUD = 1,
        DEMOGRAPHIC_COMBO = 2
    };

    /// @brief Eigen 3d Tensor maintaining Time Order
    class Matrix3dOverTime{
    public:
        Matrix3dOverTime(){}
        Matrix3dOverTime(std::vector<Matrix3d> data);
        Matrix3dOverTime(std::vector<Matrix3d> data, std::vector<int> timestepChanges);

        /// @brief Get the Matrix3d at the Specified Timestep
        /// @param timestep Timestep to retrieve the Matrix3d
        /// @return Matrix3d at Specified Timestep
        Matrix3d getMatrix3dAtTimestep(int timestep);

        /// @brief Add the Matrix3d at the specified Timestep
        /// @param datapoint Matrix3d Data
        /// @param timestep Timestep to insert the Matrix3d
        void insert(Matrix3d datapoint, int timestep);

        /// @brief Return all the Matrices in order by their timestep
        /// @return Vector of Matrix3ds in timestep order
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

    /// @brief Struct defining all the History Matrices Across the Duration of the Simulation
    using History = History;

    struct Cost {
        Matrix3dOverTime healthcareCost;
        Matrix3dOverTime nonFatalOverdoseCost;
        Matrix3dOverTime fatalOverdoseCost;
        Matrix3dOverTime pharmaCost;
        Matrix3dOverTime treatmentCost;
    };

    /// @brief Struct defining Cost Matrices Across the Duration of the Simulation
    using Cost = Cost;

    struct Utility {
        Matrix3dOverTime backgroundUtility;
        Matrix3dOverTime oudUtility;
        Matrix3dOverTime settingUtility;
    };

    /// @brief Struct defining Utilities Matrices Across the Duration of the Simulation
    using Utility = Utility;


};

#endif
