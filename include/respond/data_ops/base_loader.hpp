////////////////////////////////////////////////////////////////////////////////
// File: base_loader.hpp                                                      //
// Project: data_ops                                                          //
// Created Date: 2025-06-23                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-06-23                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_DATAOPS_BASELOADER_HPP_
#define RESPOND_DATAOPS_BASELOADER_HPP_

#include <datamanagement/DataManagement.hpp>

namespace respond {
namespace data_ops {
class BaseLoader {
public:
    virtual Data::IDataTablePtr LoadDataTable(const std::string &path,
                                              bool headers = true) = 0;
    virtual void SetConfig(const std::string &config_file) = 0;
    virtual Data::IConfigablePtr GetConfig() const = 0;
};
} // namespace data_ops
} // namespace respond

#endif // RESPOND_DATAOPS_BASELOADER_HPP_