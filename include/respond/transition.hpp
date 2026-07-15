////////////////////////////////////////////////////////////////////////////////
// File: transition.hpp                                                       //
// Project: respond                                                           //
// Created Date: 2026-02-02                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-14                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_TRANSITION_HPP_
#define RESPOND_TRANSITION_HPP_

#include <map>
#include <memory>
#include <string>
#include <vector>

#include <Eigen/Dense>

#include <respond/constants.hpp>
#include <respond/history.hpp>

namespace respond {

/// @brief Abstract base class representing a state transition operation.
/// Transitions apply transformation matrices to state vectors and update
/// history records. Subclasses define specific types of transitions (e.g.,
/// Markov, background death, behavior).
class Transition {
public:
    /// @brief Virtual destructor for proper polymorphic cleanup.
    virtual ~Transition() = default;

    /// @brief Executes this transition, applying it to a state vector.
    /// The input state is not modified; history records are updated with the
    /// transition effects.
    /// @param s The current state vector (not modified).
    /// @param h The history records to update (may be modified by this
    /// transition).
    /// @return The resulting state vector after applying this transition.
    virtual Eigen::VectorXd
    Execute(const Eigen::Ref<const Eigen::VectorXd> &s,
            std::map<std::string, History> &h) const = 0;

    /// @brief Adds a transformation matrix to this transition.
    /// The matrix is stored for use during Execute() calls.
    /// @param m The transition matrix to add (not modified by this transition).
    virtual void AddMatrix(Eigen::Ref<const Eigen::MatrixXd> m) = 0;

    /// @brief Retrieves the stored transition matrices for this transition.
    /// @return A vector of references to the stored transition matrices.
    virtual std::vector<Eigen::MatrixXd> GetMatrices() const = 0;

    /// @brief Retrieves the name/type of this transition.
    /// @return The transition's identifier as a string.
    virtual std::string GetName() const = 0;

    /// @brief Clears all stored transition matrices.
    virtual void ClearMatrices() = 0;

    /// @brief Deleted copy constructor (transitions are non-copyable by public
    /// API).
    Transition(const Transition &) = delete;
    /// @brief Deleted copy assignment operator (transitions are non-copyable by
    /// public API).
    Transition &operator=(const Transition &) = delete;

    /// @brief Creates a deep copy of this transition.
    /// @return A unique_ptr to an independent copy of this transition.
    virtual std::unique_ptr<Transition> clone() const = 0;

    /// @brief Creates a transition of the specified type.
    /// @param type The type of transition to create. Supported types
    /// (case-insensitive):
    ///        - "migration": Population migration transitions
    ///        - "behavior": Behavioral state transitions
    ///        - "intervention": Intervention-driven transitions
    ///        - "overdose": Overdose-related transitions
    ///        - "background_death": Background mortality transitions
    /// @param log_name The logger name for error reporting (e.g., "console").
    /// @return A unique_ptr to the created Transition, or nullptr if type is
    /// unsupported.
    static std::unique_ptr<Transition>
    Create(const std::string &type,
           const std::string &name = RESPOND_DEFAULT_TRANSITION_NAME,
           const std::string &log_name = RESPOND_DEFAULT_LOG,
           const std::string &log_file = RESPOND_DEFAULT_LOG_FILE);

    /// @brief Helper function to overload to the stream insertion operator for
    /// Transition serialization.
    /// @details This function is intended to be overridden by subclasses to
    /// provide custom serialization logic. It should write the transition's
    /// state, metadata, and any relevant information to the provided output
    /// stream.
    /// @note The output format is implementation-defined and may vary between
    /// subclasses. Users should refer to the specific subclass documentation
    /// for details on the serialization format.
    /// @param os The output stream to which the transition's serialized data
    /// will be written.
    virtual void Serialize(std::ostream &os) const = 0;

protected:
    /// @brief Protected default constructor for subclass initialization.
    /// Not intended for direct public use.
    Transition() = default;
};

/// @brief Overloaded stream insertion operator for Model serialization.
/// @param os The output stream to write to.
/// @param model The Model instance to serialize.
/// @return The output stream after writing the model's serialized data.
inline std::ostream &operator<<(std::ostream &os,
                                const Transition &transition) {
    transition.Serialize(os);
    return os;
}

} // namespace respond

#endif