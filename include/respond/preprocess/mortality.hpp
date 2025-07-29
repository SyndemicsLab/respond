////////////////////////////////////////////////////////////////////////////////
// File: mortality.hpp                                                        //
// Project: respond                                                           //
// Created Date: 2025-07-29                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-29                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_PREPROCESS_MORTALITY_HPP_
#define RESPOND_PREPROCESS_MORTALITY_HPP_

#include <string>

#include <Eigen/Dense>
#include <csv.hpp>

namespace respond {
namespace preprocess {
Eigen::MatrixXd BuildMortality(const std::string &smr_file,
                               const std::string &nvss_file) {
    csv::CSVReader reader(
        smr_file, csv::CSVFormat().delimiter(',').quote('"').header_row(0));
    // I feel like we need to clearly define what the sim inputs looks like?
}
} // namespace preprocess
} // namespace respond

#endif // RESPOND_PREPROCESS_MORTALITY_HPP_