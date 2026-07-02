////////////////////////////////////////////////////////////////////////////////
// File: timestep.hpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-06-30                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-07-02                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TIMESTEP_HPP_
#define RESPOND_TIMESTEP_HPP_

#include <memory>
#include <ostream>
#include <vector>

#include <respond/logging.hpp>
#include <respond/transition.hpp>

namespace respond {
class Timestep {
public:
    Timestep() : Timestep("respond") {}
    Timestep(const std::string &log_name)
        : Timestep(log_name, log_name + ".log") {}
    Timestep(const std::string &log_name, const std::string &log_filepath)
        : _log_name(log_name) {
        CreateFileLogger(log_name, log_filepath);
        _transitions = {};
    }
    ~Timestep() = default;

    const Transition &
    CreateTransition(const std::string &transition_name) const {
        auto transition = Transition::Create(transition_name, _log_name);
        return *transition;
    }

    void AddMatrixToTransition(const size_t &idx,
                               const Eigen::Ref<const Eigen::MatrixXd> &m) {
        if (idx >= _transitions.size()) {
            throw std::out_of_range(
                "Index out of range in AddMatrixToTransition");
        }
        _transitions[idx]->AddTransitionMatrix(m);
    }

    const Transition &GetTransition(const size_t &idx) const {
        if (idx >= _transitions.size()) {
            throw std::out_of_range("Index out of range in GetTransition");
        }
        return *_transitions[idx];
    }

    const Transition &GetTransition(const std::string &transition_name) const {
        for (const auto &t : _transitions) {
            if (t->GetTransitionName() == transition_name) {
                return *t;
            }
        }
        throw std::invalid_argument("Transition not found in GetTransition: " +
                                    transition_name);
    }

    const std::vector<const Transition &> &GetTransitions() const {
        std::vector<const Transition &> _transitions_refs;
        for (const auto &t : _transitions) {
            _transitions_refs.push_back(*t);
        }
        return _transitions_refs;
    }

    const std::unique_ptr<Transition> &
    GetTransitionAtIndex(size_t index) const {
        if (index >= _transitions.size()) {
            throw std::out_of_range(
                "Index out of range in GetTransitionAtIndex");
        }
        return _transitions[index];
    }

    const std::vector<std::string> &GetTransitionNames() const {
        std::vector<std::string> names;
        for (const auto &t : _transitions) {
            names.push_back(t->GetName());
        }
        return names;
    }

    // Copy Constructor and Assignment

    Timestep(const Timestep &other) {
        _transitions.clear();
        for (const auto &t : other._transitions) {
            _transitions.push_back(t->clone());
        }
    }

    Timestep &operator=(const Timestep &other) {
        if (this != &other) {
            _transitions.clear();
            for (const auto &t : other._transitions) {
                _transitions.push_back(t->clone());
            }
        }
        return *this;
    }

    // Move Constructor and Assignment

    Timestep(Timestep &&other) noexcept
        : _transitions(std::move(other._transitions)) {
        other._transitions.clear();
    }

    Timestep &operator=(Timestep &&other) noexcept {
        if (this != &other) {
            _transitions = std::move(other._transitions);
            other._transitions.clear();
        }
        return *this;
    }

    friend std::ostream &operator<<(std::ostream &os, Timestep &other) {
        os << "Timestep with the following transitions:\n";
        for (const auto &t : other._transitions) {
            os << " - " << t->GetName() << "\n";
        }
        return os;
    }

private:
    std::string _log_name;
    std::vector<std::unique_ptr<Transition>> _transitions;
};
} // namespace respond

#endif // RESPOND_TIMESTEP_HPP_