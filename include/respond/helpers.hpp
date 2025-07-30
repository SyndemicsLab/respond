////////////////////////////////////////////////////////////////////////////////
// File: helpers.hpp                                                          //
// Project: respond                                                           //
// Created Date: 2025-07-28                                                   //
// Author: Matthew Carroll                                                    //
// -----                                                                      //
// Last Modified: 2025-07-30                                                  //
// Modified By: Matthew Carroll                                               //
// -----                                                                      //
// Copyright (c) 2025 Syndemics Lab at Boston Medical Center                  //
////////////////////////////////////////////////////////////////////////////////

#ifndef RESPOND_HELPERS_HPP_
#define RESPOND_HELPERS_HPP_

namespace respond {

template <class... Args>
inline constexpr bool CheckVectorLengths(Args const &...args) {
    if constexpr (sizeof...(Args) == 0) {
        return true;
    } else {
        return [](auto const &first, auto const &...rest) {
            return ((first.size() == rest.size()) && ...);
        }(args...);
    }
}
} // namespace respond

#endif // RESPOND_HELPERS_HPP_