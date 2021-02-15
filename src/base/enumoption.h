// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <optional>
#include <string>

// Enum Option
class EnumOption
{
    public:
    EnumOption();
    EnumOption(const int enumeration, std::string_view keyword, std::optional<int> minArgs = std::nullopt,
               std::optional<int> maxArgs = std::nullopt);
    EnumOption(const int enumeration, std::string_view keyword, std::string_view description,
               std::optional<int> minArgs = std::nullopt, std::optional<int> maxArgs = std::nullopt);
    virtual ~EnumOption() = default;

    /*
     * Definition
     */
    public:
    // Argument Numbers
    enum ArgumentNumber
    {
        OneOrMoreArguments = -1,
        OptionalSecondArgument = -2,
        AnyNumberOfArguments = -3
    };

    private:
    // Option enumeration (i.e. from enum value)
    int enumeration_;
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
    virtual bool isValid() const;
    // Return option enumeration (i.e. from enum value)
    int enumeration() const;
    // Return option keyword
    std::string_view keyword() const;
    // Return option description
    std::string_view description() const;
    // Return minimum number of arguments the option takes
    std::optional<int> minArgs() const;
    // Return maximum number of arguments the option takes
    std::optional<int> maxArgs() const;
};

// Unrecognised Enum Option
class UnrecognisedEnumOption : public EnumOption
{
    public:
    UnrecognisedEnumOption() : EnumOption(0, "UNRECOGNISED_KEYWORD") {}

    public:
    // Return if the option is valid (true except in derived classes)
    bool isValid() const { return false; }
};
