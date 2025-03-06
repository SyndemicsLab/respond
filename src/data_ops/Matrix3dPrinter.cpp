////////////////////////////////////////////////////////////////////////////////
// File: Matrix3dPrinter.cpp                                                  //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-01-14                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#include <respondsimulation/data_ops/Matrix3dPrinter.hpp>

namespace Matrixify {

    void Matrix3dPrinter::Print(Matrixify::Matrix3d mat, std::ostream &stream) {
        Matrixify::Matrix3d::Dimensions dim = mat.dimensions();
        for (int i = 0; i < dim[2]; ++i) {
            // intervention iterated along columns
            for (int j = 0; j < dim[0]; ++j) {
                // oud state is the row
                for (int k = 0; k < dim[1]; ++k) {
                    stream << mat(j, k, i) << " ";
                }
                stream << std::endl;
            }
            if (i != (dim[2] - 1)) {
                stream << "===========================" << std::endl;
            }
        }
    }

    void Matrix3dPrinter::PrintOverTime(Matrixify::Matrix4d matOverTime,
                                        std::ostream &stream) {
        std::vector<Matrixify::Matrix3d> matrices = matOverTime.getMatrices();
        for (Matrixify::Matrix3d mat : matrices) {
            stream << "++++++++ NEW TIMESTEP ++++++++" << std::endl;
            Matrix3dPrinter::Print(mat, stream);
        }
    }

} // namespace Matrixify
