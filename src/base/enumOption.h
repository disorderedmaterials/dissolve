// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include "base/arguments.h"
#include <optional>
#include <string>

// Enum Option
template <class E> class EnumOption
{
    public:
    EnumOption();
    EnumOption(const E enumeration, std::string_view keyword, std::optional<int> minArgs = std::nullopt,
               std::optional<int> maxArgs = std::nullopt)
        : enumeration_(enumeration), keyword_(keyword), minArgs_(minArgs), maxArgs_(maxArgs)
    {
    }
    EnumOption(const E enumeration, std::string_view keyword, std::string_view description,
               std::optional<int> minArgs = std::nullopt, std::optional<int> maxArgs = std::nullopt)
        : enumeration_(enumeration), keyword_(keyword), description_(description), minArgs_(minArgs), maxArgs_(maxArgs)
    {
    }
    virtual ~EnumOption() = default;

    /*
     * Definition
     */
    private:
    // Option enumeration (i.e. from enum value)
    E enumeration_;
    // Option keyword
    std::string keyword_;
    // Option description / long text
    std::string description_;
    // Minimum number of arguments the option takes
    std::optional<int> minArgs_;
    // Maximum number of arguments the option takes
    std::optional<int> maxArgs_;

    public:
    // Return if the option is valid (true except in derived classes)
    virtual bool isValid() const { return true; }
    // Return option enumeration (i.e. from enum value)
    E enumeration() const { return enumeration_; }
    // Return option keyword
    std::string keyword() const { return keyword_; }
    // Return option description
    std::string description() const { return description_; }
    // Return minimum number of arguments the option takes
    std::optional<int> minArgs() const { return minArgs_; }
    // Return maximum number of arguments the option takes
    std::optional<int> maxArgs() const { return maxArgs_; }
};
