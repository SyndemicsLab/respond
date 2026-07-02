////////////////////////////////////////////////////////////////////////////////
// File: timestep.hpp                                                         //
// Project: respond                                                           //
// Created Date: 2026-06-30                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2026-06-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2026 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////
#ifndef RESPOND_TIMESTEP_HPP_
#define RESPOND_TIMESTEP_HPP_

#include <memory>
#include <vector>

#include <respond/transition.hpp>

namespace respond {
class Timestep {
public:
    Timestep() { _transitions = {}; }
    ~Timestep() = default;

    std::vector<std::unique_ptr<Transition>> GetTransitions() const {
        return _transitions;
    }

    const std::unique_ptr<Transition> &
    GetTransitionAtIndex(size_t index) const {
        if (index >= _transitions.size()) {
            throw std::out_of_range(
                "Index out of range in GetTransitionAtIndex");
        }
        return _transitions[index];
    }

    std::vector<std::string> GetTransitionNames() const {
        std::vector<std::string> names;
        for (const auto &t : _transitions) {
            names.push_back(t->GetTransitionName());
        }
        return names;
    }

    void AddTransition(const std::unique_ptr<Transition> &t) {
        _transitions.push_back(t->clone());
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

private:
    std::vector<std::unique_ptr<Transition>> _transitions;
};
} // namespace respond

#endif // RESPOND_TIMESTEP_HPP_