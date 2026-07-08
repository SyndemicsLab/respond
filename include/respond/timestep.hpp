////////////////////////////////////////////////////////////////////////////////
// File: timestep.hpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-06-30                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-09                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TIMESTEP_HPP_
#define RESPOND_TIMESTEP_HPP_

#include <memory>
#include <ostream>
#include <vector>

#include <respond/constants.hpp>
#include <respond/logging.hpp>
#include <respond/transition.hpp>

namespace respond {

/// @brief Represents a single timestep in a simulation, managing a collection
/// of transitions. Each timestep can execute its transitions in sequence,
/// applying their effects to a state vector and updating history records.
/// Transitions can be added, retrieved, and managed within the timestep. The
/// timestep also handles logging for its operations.
class Timestep {
public:
    ////////////////////////////////////////////////////////////////////////////
    //
    // Rule of Five: Copy and Move Semantics
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Default constructor for Timestep. Initializes with default
    /// logger.
    Timestep() : Timestep(RESPOND_DEFAULT_LOG) {}

    /// @brief Default constructor for Timestep with specified logger name.
    /// Initializes with default log file path.
    /// @param log_name String name for the logger to be used by this timestep.
    Timestep(const std::string &log_name)
        : Timestep(log_name, RESPOND_DEFAULT_LOG_FILE) {}

    /// @brief Default constructor for Timestep with specified logger name and
    /// log file path.
    /// @param log_name String name for the logger to be used by this timestep.
    /// @param log_filepath String path for the log file to be used by this
    /// timestep.
    Timestep(const std::string &log_name, const std::string &log_filepath)
        : _log_name(log_name) {
        CreateFileLogger(log_name, log_filepath);
    }

    /// @brief Destructor for Timestep. Default implementation.
    ~Timestep() = default;

    /// @brief Copy constructor for Timestep. Creates a deep copy of the
    /// transitions.
    /// @param other The Timestep instance to copy from.
    Timestep(const Timestep &other) {
        _transitions.clear();
        for (const auto &t : other._transitions) {
            _transitions.push_back(t->clone());
        }
    }

    /// @brief Copy assignment operator for Timestep. Creates a deep copy of the
    /// transitions.
    /// @param other The Timestep instance to copy from.
    /// @return Reference to this Timestep instance after assignment.
    Timestep &operator=(const Timestep &other) {
        if (this != &other) {
            _transitions.clear();
            for (const auto &t : other._transitions) {
                _transitions.push_back(t->clone());
            }
        }
        return *this;
    }

    /// @brief Move constructor for Timestep. Transfers ownership of
    /// transitions.
    /// @param other The Timestep instance to move from.
    Timestep(Timestep &&other) noexcept
        : _transitions(std::move(other._transitions)) {
        other._transitions.clear();
    }

    /// @brief Move assignment operator for Timestep. Transfers ownership of
    /// transitions.
    /// @param other The Timestep instance to move from.
    /// @return Reference to this Timestep instance after assignment.
    Timestep &operator=(Timestep &&other) noexcept {
        if (this != &other) {
            _transitions = std::move(other._transitions);
            other._transitions.clear();
        }
        return *this;
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Timestep Behavior Methods: Transition Management
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Creates a transition of the specified type and adds it to this
    /// timestep.
    /// @param transition_name The type of transition to create. Supported types
    /// (case-insensitive):
    ///        - "migration": Population migration transitions
    ///        - "behavior": Behavioral state transitions
    ///        - "intervention": Intervention-driven transitions
    ///        - "overdose": Overdose-related transitions
    ///        - "background_death": Background mortality transitions
    /// @return A constant reference to the created Transition. Throws an
    /// exception if the transition type is unsupported.
    const std::unique_ptr<Transition> &
    CreateTransition(const std::string &transition_name) {
        _transitions.push_back(
            Transition::Create(transition_name, transition_name, _log_name));
        return _transitions.back();
    }

    std::unique_ptr<Transition> RemoveTransition(size_t idx) {
        if (idx >= _transitions.size()) {
            LogWarning(_log_name, "Index out of range in RemoveTransition: " +
                                      std::to_string(idx));
            throw std::out_of_range(
                "Error attempting to RemoveTransition by index.");
        }
        auto removed_transition = std::move(_transitions[idx]);
        _transitions.erase(_transitions.begin() + idx);
        return removed_transition;
    }

    /// @brief Adds a matrix to an existing transition in this timestep by
    /// index.
    /// @param idx The index of the transition to which the matrix will be
    /// added.
    /// @param m The transition matrix to add (not modified by this transition).
    void AddMatrixToTransition(const size_t &idx,
                               const Eigen::Ref<const Eigen::MatrixXd> &m) {
        if (idx >= _transitions.size()) {
            LogWarning(_log_name,
                       "Index out of range in AddMatrixToTransition: " +
                           std::to_string(idx));
        }
        _transitions[idx]->AddMatrix(m);
    }

    /// @brief Adds a matrix to an existing transition in this timestep by
    /// name.
    /// @param transition_name The name of the transition to which the matrix
    /// will be added.
    /// @param m The transition matrix to add (not modified by this transition).
    void AddMatrixToTransition(const std::string &transition_name,
                               const Eigen::Ref<const Eigen::MatrixXd> &m) {
        for (size_t i = 0; i < _transitions.size(); ++i) {
            if (_transitions[i]->GetName() == transition_name) {
                _transitions[i]->AddMatrix(m);
                return;
            }
        }
        LogWarning(_log_name,
                   "Transition not found in AddMatrixToTransition: " +
                       transition_name);
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Getters and Setters for Transitions and Metadata
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Retrieves a constant reference to a transition in this timestep
    /// by index.
    /// @param idx The index of the transition to retrieve.
    /// @return A constant reference to the Transition at the specified index.
    /// Throws an error if the index is out of range.
    const std::unique_ptr<Transition> &GetTransition(const size_t &idx) const {
        if (idx >= _transitions.size()) {
            LogError(_log_name, "Index out of range in GetTransition: " +
                                    std::to_string(idx));
            throw std::out_of_range(
                "Error attempting to GetTransition by index.");
        }
        return _transitions[idx];
    }

    /// @brief Gets a constant reference to a transition in this timestep by
    /// name.
    /// @param transition_name The name of the transition to retrieve.
    /// @return A constant reference to the Transition with the specified name.
    /// Throws an error if the transition is not found.
    const std::unique_ptr<Transition> &
    GetTransition(const std::string &transition_name) const {
        for (const auto &t : _transitions) {
            if (t->GetName() == transition_name) {
                return t;
            }
        }
        LogError(_log_name,
                 "Transition not found in GetTransition: " + transition_name);
        throw std::invalid_argument(
            "Error attempting to GetTransition by name.");
    }

    /// @brief Gets a vector of unique_ptrs to all transitions in this timestep.
    /// The returned vector contains deep copies of the transitions, ensuring
    /// that modifications to the returned transitions do not affect the
    /// original transitions in the timestep.
    /// @return A vector of unique_ptrs to the transitions in this timestep.
    std::vector<std::unique_ptr<Transition>> GetTransitions() const {
        std::vector<std::unique_ptr<Transition>> _ret;
        for (const auto &t : _transitions) {
            _ret.push_back(t->clone());
        }
        return _ret;
    }

    /// @brief Gets a vector of the names of all transitions in this timestep.
    /// @return A vector of strings containing the names of the transitions in
    /// this timestep.
    std::vector<std::string> GetTransitionNames() const {
        std::vector<std::string> names;
        for (const auto &t : _transitions) {
            names.push_back(t->GetName());
        }
        return names;
    }

    ////////////////////////////////////////////////////////////////////////////
    //
    // Logging and Output Methods
    //
    ////////////////////////////////////////////////////////////////////////////

    /// @brief Overloaded stream insertion operator for Timestep. Outputs the
    /// names of all transitions in the timestep to the provided output stream.
    /// @details This operator allows for easy logging and debugging of the
    /// transitions contained within a Timestep instance. It outputs a list of
    /// transition names, each prefixed with a dash for clarity.
    /// @param os The output stream to which the transition names will be
    /// written.
    /// @param other The Timestep instance whose transitions are to be output.
    /// @return A reference to the output stream after writing the transition
    /// names.
    friend std::ostream &operator<<(std::ostream &os, Timestep &other) {
        os << "Timestep with the following transitions:\n";
        for (const auto &t : other._transitions) {
            os << " - " << t->GetName() << "\n";
        }
        return os;
    }

    /// @brief Overloaded equality operator for Timestep. Compares two Timestep
    /// instances for equality based on their transitions and transition
    /// matrices.
    /// @param lhs The left-hand side Timestep instance to compare.
    /// @param rhs The right-hand side Timestep instance to compare.
    /// @return True if the two Timestep instances are equal (same transitions
    /// and transition matrices), false otherwise.
    friend bool operator==(const Timestep &lhs, const Timestep &rhs) {
        if (lhs._transitions.size() != rhs._transitions.size()) {
            return false;
        }
        for (size_t i = 0; i < lhs._transitions.size(); ++i) {
            if (lhs._transitions[i]->GetName() !=
                rhs._transitions[i]->GetName()) {
                return false;
            }
            if (lhs._transitions[i]->GetMatrices().size() !=
                rhs._transitions[i]->GetMatrices().size()) {
                return false;
            }
            for (size_t j = 0; j < lhs._transitions[i]->GetMatrices().size();
                 ++j) {
                if (!lhs._transitions[i]->GetMatrices()[j].isApprox(
                        rhs._transitions[i]->GetMatrices()[j])) {
                    return false;
                }
            }
        }
        return true;
    }

    /// @brief Overloaded inequality operator for Timestep. Compares two
    /// Timestep instances for inequality based on their transitions and
    /// transition matrices.
    /// @param lhs The left-hand side Timestep instance to compare.
    /// @param rhs The right-hand side Timestep instance to compare.
    /// @return True if the two Timestep instances are not equal (different
    /// transitions or transition matrices), false otherwise.
    friend bool operator!=(const Timestep &lhs, const Timestep &rhs) {
        return !(lhs == rhs);
    }

private:
    std::string _log_name;
    std::vector<std::unique_ptr<Transition>> _transitions;
};
} // namespace respond

#endif // RESPOND_TIMESTEP_HPP_