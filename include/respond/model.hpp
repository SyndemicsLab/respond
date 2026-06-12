////////////////////////////////////////////////////////////////////////////////
// File: model.hpp                                                            //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-02-12                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_MODEL_HPP_
#define RESPOND_MODEL_HPP_

#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <respond/history.hpp>
#include <respond/transition.hpp>

namespace respond {
/// @brief Abstract base class representing a state transition model.
/// Models manage a state vector, execute transitions, and maintain history of
/// state changes. Subclasses must implement state management, transition
/// execution, and history tracking.
class Model {
public:
    /// @brief Virtual destructor for proper polymorphic cleanup.
    virtual ~Model() = default;

    /// @brief Sets the current state of the model.
    /// @param state The state vector to set. A copy is made internally.
    virtual void SetState(const Eigen::VectorXd &state) = 0;

    /// @brief Retrieves the current state of the model.
    /// @return A copy of the current state vector (limited to observation).
    virtual Eigen::VectorXd GetState() const = 0;

    /// @brief Executes all registered transitions on the current state.
    /// Transitions are applied in the order they were added and may modify
    /// history.
    virtual void RunTransitions() = 0;

    /// @brief Adds a transition to the model.
    /// @param t A unique_ptr to a Transition object. The model assumes
    /// ownership.
    virtual void AddTransition(const std::unique_ptr<Transition> &t) = 0;

    /// @brief Retrieves the names of all registered transitions.
    /// @return Vector of transition names in the order they were added.
    virtual std::vector<std::string> GetTransitionNames() const = 0;

    /// @brief Clears all registered transitions.
    /// Deletes all stored Transition unique_ptrs.
    virtual void ClearTransitions() = 0;

    /// @brief Retrieves the history records for all state variables.
    /// @return A map of history names to History objects containing state
    /// trajectories.
    virtual std::map<std::string, History> GetHistories() const = 0;

    /// @brief Creates default history tracking for the model.
    /// This method initializes standard history records based on the model's
    /// state.
    virtual void CreateDefaultHistories() = 0;

    /// @brief Sets the history records for the model.
    /// @param h A map of history names to History objects.
    virtual void SetHistories(const std::map<std::string, History> &h) = 0;

    /// @brief Clears all history records and resets history tracking state.
    virtual void ClearHistories() = 0;

    /// @brief Sets the global history capture interval for this model.
    /// @param interval Record every interval timesteps. Values less than 1
    /// default to full capture.
    virtual void SetHistoryCaptureInterval(int interval) = 0;

    /// @brief Retrieves the global history capture interval.
    /// @return The active capture interval. A value of 1 means full capture.
    virtual int GetHistoryCaptureInterval() const = 0;

    /// @brief Sets the final timestep that must always be recorded.
    /// @param final_timestep The final simulation timestep.
    virtual void SetFinalTimestep(int final_timestep) = 0;

    /// @brief Retrieves the final timestep forced into history output.
    /// @return The configured final simulation timestep, or -1 if unset.
    virtual int GetFinalTimestep() const = 0;

    /// @brief Retrieves the name identifier for this model.
    /// @return The model's name as a string.
    virtual std::string GetModelName() const = 0;

    /// @brief Retrieves the logger name used by this model.
    /// @return The name of the associated logger.
    virtual std::string GetLogName() const = 0;

    /// @brief Factory method to create a Model instance.
    /// @param name The name identifier for the model to create.
    /// @param log_name Name of the logger for this model (default: "console").
    /// @return A unique_ptr to the newly created Model instance.
    static std::unique_ptr<Model>
    Create(const std::string &name, const std::string &log_name = "console");

    /// @brief Deleted copy constructor (models are non-copyable by public API).
    Model(const Model &) = delete;
    /// @brief Deleted copy assignment operator (models are non-copyable by
    /// public API).
    Model &operator=(const Model &) = delete;

    /// @brief Creates a deep copy of this model.
    /// @return A unique_ptr to an independent copy of this model.
    virtual std::unique_ptr<Model> clone() const = 0;

protected:
    /// @brief Protected default constructor for subclass initialization.
    /// Not intended for direct public use.
    Model() = default;
};
} // namespace respond

#endif // RESPOND_MODEL_HPP_