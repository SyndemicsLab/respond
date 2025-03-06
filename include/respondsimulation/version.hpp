////////////////////////////////////////////////////////////////////////////////
// File: version.hpp                                                          //
// Project: RESPONDSimulationv2                                               //
// Created Date: 2025-03-06                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-03-06                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPONDSIMULATION_VERSION_HPP_
#define RESPONDSIMULATION_VERSION_HPP_

#define RESPONDSIMULATION_VER_MAJOR 2
#define RESPONDSIMULATION_VER_MINOR 0
#define RESPONDSIMULATION_VER_PATCH 1

#define RESPONDSIMULATION_TO_VERSION(major, minor, patch)                      \
    (major * 10000 + minor * 100 + patch)
#define RESPONDSIMULATION_VERSION                                              \
    RESPONDSIMULATION_TO_VERSION(RESPONDSIMULATION_VER_MAJOR,                  \
                                 RESPONDSIMULATION_VER_MINOR,                  \
                                 RESPONDSIMULATION_VER_PATCH)

#endif // RESPONDSIMULATION_VERSION_HPP_