#include "Matrix3dPrinter.hpp"

namespace Utilities{

    /// @brief A convenience function useful for viewing `Matrix3d` objects as 2d
    /// slices along the demographics axis -- used for printf debugging
    /// @param toPrint the `Matrix3d` object to print
    void Matrix3dPrinter::Print(Data::Matrix3d mat, std::ostream &stream){
        Data::Matrix3d::Dimensions dim = mat.dimensions();
        for (int i = 0; i < dim[2]; ++i) {
            // intervention iterated along columns
            for (int j = 0; j < dim[0]; ++j) {
                // oud state is the row
                for (int k = 0; k < dim[1]; ++k) {
                    stream << mat(j,k,i) << " ";
                }
                stream << std::endl;
            }
            if (i != (dim[2] - 1)) {
                stream << "===========================" << std::endl;
            }
        }
    }

    void Matrix3dPrinter::PrintOverTime(Data::Matrix3dOverTime matOverTime, std::ostream &stream){
        std::vector<Data::Matrix3d> matrices = matOverTime.getMatrices();
        for(Data::Matrix3d mat : matrices){
            stream << "++++++++ NEW TIMESTEP ++++++++" << std::endl;
            Matrix3dPrinter::Print(mat, stream);
        }
    }
    
}
