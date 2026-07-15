////////////////////////////////////////////////////////////////////////////////
// File: model.hpp                                                            //
// Project: respond                                                           //
// Created Date: 2026-02-05                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-14                                                  //
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

#include <respond/constants.hpp>
#include <respond/history.hpp>
#include <respond/timestep.hpp>

namespace respond {
/// @brief Abstract base class representing a state transition model.
/// Models manage a state vector, execute transitions, and maintain history of
/// state changes. Subclasses must implement state management, transition
/// execution, and history tracking.
class Model {
public:
    ////////////////////////////////////////////////////////////////////////////
    //
    // Rule of Five: Copy and Move Semantics
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Factory method to create a Model instance.
    /// @details This method creates a new instance of a Model subclass based on
    /// the provided name. It initializes logging for the model and returns a
    /// unique_ptr to the created instance. Throws an exception if the model
    /// name is unsupported.
    /// @param name The name identifier for the model to create.
    /// @param log_name Name of the logger for this model (default: "console").
    /// @param log_filepath File path for the log file (default: "respond.log").
    /// @return A unique_ptr to the newly created Model instance.
    static std::unique_ptr<Model>
    Create(const std::string &name,
           const std::string &log_name = RESPOND_DEFAULT_LOG,
           const std::string &log_filepath = RESPOND_DEFAULT_LOG_FILE);

    /// @brief Virtual destructor for proper polymorphic cleanup.
    virtual ~Model() = default;

    /// @brief Deleted copy constructor (models are non-copyable by public API).
    Model(const Model &) = delete;
    /// @brief Deleted copy assignment operator (models are non-copyable by
    /// public API).
    Model &operator=(const Model &) = delete;

    /// @brief Creates a deep copy of this model.
    /// @return A unique_ptr to an independent copy of this model.
    virtual std::unique_ptr<Model> clone() const = 0;

    ////////////////////////////////////////////////////////////////////////////
    //
    // Model Behavior Methods: Timestep Execution and History Management
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Helper function to add a single timestep to the model.
    /// @param timestep A shared pointer to a Timestep instance. The model gains
    /// an ownership reference to this timestep and will manage its lifecycle.
    virtual void AddTimestep(const Timestep &timestep) = 0;

    /// @brief Executes the next timestep in the model's sequence.
    virtual void RunTimestep() = 0;

    /// @brief Executes the timestep in the model's sequence.
    virtual void RunTimestep(size_t idx) = 0;

    /// @brief Executes all registered timesteps in sequence, applying their
    /// transitions to the model's state.
    virtual void RunTimesteps() = 0;

    /// @brief Clears all timesteps from the model.
    virtual void ClearTimesteps() = 0;

    /// @brief Clear all history records and reset the history tracking state.
    virtual void ClearHistories() = 0;

    /// @brief Creates default history tracking for the model.
    /// This method initializes standard history records based on the model's
    /// state.
    virtual void CreateDefaultHistories() = 0;

    ////////////////////////////////////////////////////////////////////////////
    //
    // Getters and Setters for Model State and Metadata
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Retrieve a copy of a specific timestep by index.
    /// @param index The zero-based index of the timestep to retrieve.
    /// @return A constant reference to the Timestep at the specified index.
    virtual Timestep GetTimestepAtIndex(size_t index) const = 0;

    /// @brief Retrieves the current state of the model.
    /// @return A reference to the model's internal state. It is limited to
    /// observation and changes cannot be made to it directly.
    virtual const Eigen::Ref<const Eigen::VectorXd> GetState() const = 0;

    /// @brief Retrieves the name identifier for this model.
    /// @return The model's name as a string.
    virtual std::string GetName() const = 0;

    /// @brief Retrieves a reference to the map of all registered histories. The
    /// histories cannot be edited directly and must be saved to their own space
    /// by users before being consumed.
    /// @return A constant reference to a map of history names to History
    /// objects.
    virtual const std::map<std::string, History> &GetHistories() const = 0;

    /// @brief Retrieves the current simulation timestep.
    /// @return The current timestep index. Returns -1 if no timesteps have been
    /// executed yet.
    virtual int GetTimestep() const = 0;

    /// @brief Retrieves the global history capture interval.
    /// @return The active capture interval. A value of 1 means full capture.
    virtual int GetHistoryCaptureInterval() const = 0;

    /// @brief Retrieves the final timestep forced into history output.
    /// @return The configured final simulation timestep, or -1 if unset.
    virtual int GetFinalTimestep() const = 0;

    /// @brief Checks if the initial history has been recorded for this model.
    /// @return True if the initial state has been recorded in history, false
    /// otherwise.
    virtual bool GetInitialHistoryRecorded() const = 0;

    /// @brief Sets the current state of the model.
    /// @param state A constant reference to a vector. This vector is then
    /// applied to the model's internal state. The model may copy or reference
    /// this vector as needed.
    virtual void SetState(const Eigen::Ref<const Eigen::VectorXd> &state) = 0;

    /// @brief Sets the global history capture interval for this model.
    /// @param interval Record every interval timesteps. Values less than 1
    /// default to full capture.
    virtual void SetHistoryCaptureInterval(int interval) = 0;

    /// @brief Sets the final timestep that must always be recorded.
    /// @param final_timestep The final simulation timestep.
    virtual void SetFinalTimestep(int final_timestep) = 0;

    /// @brief Sets whether the initial state has been recorded in history.
    /// @param recorded True if the initial state has been recorded, false
    /// otherwise.
    virtual void SetInitialHistoryRecorded(bool recorded) = 0;

    /// @brief Function to serialize the model's state and metadata to an output
    /// stream.
    /// @details This function is intended to be overridden by subclasses to
    /// provide custom serialization logic. It should write the model's state,
    /// metadata, and any relevant information to the provided output stream.
    /// @note The output format is implementation-defined and may vary between
    /// subclasses. Users should refer to the specific subclass documentation
    /// for details on the serialization format.
    /// @param os The output stream to which the model's serialized data will be
    /// written.
    virtual void Serialize(std::ostream &os) const = 0;

protected:
    /// @brief Protected default constructor for subclass initialization.
    /// Not intended for direct public use.
    Model() = default;
};

/// @brief Overloaded stream insertion operator for Model serialization.
/// @param os The output stream to write to.
/// @param model The Model instance to serialize.
/// @return The output stream after writing the model's serialized data.
inline std::ostream &operator<<(std::ostream &os, const Model &model) {
    model.Serialize(os);
    return os;
}

} // namespace respond

#endif // RESPOND_MODEL_HPP_