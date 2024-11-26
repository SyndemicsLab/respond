//===-- main.cpp - Instruction class definition -----------------*- C++ -*-===//
//
// Part of the RESPOND - Researching Effective Strategies to Prevent Opioid
// Death Project, under the AGPLv3 License. See https://www.gnu.org/licenses/
// for license information.
// SPDX-License-Identifier: AGPLv3
//
//===----------------------------------------------------------------------===//
///
/// \file
/// This file contains the primary main function to start the RESPOND
/// simulation model. It expects 3 command line arguments:
///     (std::string)root_input_directory (int)task_start (int)task_end
/// After verifying args it creates a parallel loop to run tasks from
/// task_start until reaching and including task_end.
///
/// Created Date: Tuesday, June 27th 2023, 10:20:34 am
/// Contact: Benjamin.Linas@bmc.org
///
//===----------------------------------------------------------------------===//

#include "Respond.hpp"
#include "RespondDataStore.hpp"
#include "StateTransitionModel.hpp"

int main() { return 0; }
