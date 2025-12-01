////////////////////////////////////////////////////////////////////////////////
// File: version.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-03-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-11-26                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_VERSION_HPP_
#define RESPOND_VERSION_HPP_

#define RESPOND_VER_MAJOR 2
#define RESPOND_VER_MINOR 1
#define RESPOND_VER_PATCH 0

#define RESPOND_TO_VERSION(major, minor, patch)                                \
    (major * 10000 + minor * 100 + patch)
#define RESPOND_VERSION                                                        \
    RESPOND_TO_VERSION(RESPOND_VER_MAJOR, RESPOND_VER_MINOR, RESPOND_VER_PATCH)

#endif // RESPOND_VERSION_HPP_