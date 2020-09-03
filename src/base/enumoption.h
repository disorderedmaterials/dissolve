/*
    *** Enum Option
    *** src/base/enumoption.h
    Copyright T. Youngs 2012-2020

    This file is part of Dissolve.

    Dissolve is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    Dissolve is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with Dissolve.  If not, see <http://www.gnu.org/licenses/>.
*/

#pragma once

#include <string>

// Enum Option
class EnumOption
{
    public:
    EnumOption();
    EnumOption(const int enumeration, std::string_view keyword, int minArgs = 0, int maxArgs = 0);
    EnumOption(const int enumeration, std::string_view keyword, std::string_view description, int minArgs = 0, int maxArgs = 0);
    virtual ~EnumOption() = default;

    /*
     * Definition
     */
    public:
    // Argument Numbers
    enum ArgumentNumber
    {
        NoArguments = 0,
        OneOrMoreArguments = -1,
        OptionalSecondArgument = -2
    };

    private:
    // Option enumeration (i.e. from enum value)
    int enumeration_;
    // Option keyword
    std::string keyword_;
    // Option description / long text
    std::string description_;
    // Whether the option has any associated arguments
    bool hasArguments_;
    // Minimum number of arguments the option takes
    int minArgs_;
    // Maximum number of arguments the option takes
    int maxArgs_;

    public:
    // Return if the option is valid (true except in derived classes)
    virtual bool isValid() const;
    // Return option enumeration (i.e. from enum value)
    int enumeration() const;
    // Return option keyword
    std::string_view keyword() const;
    // Return option description
    std::string_view description() const;
    // Return whether the option has any associated arguments
    bool hasArguments() const;
    // Return minimum number of arguments the option takes
    int minArgs() const;
    // Return maximum number of arguments the option takes
    int maxArgs() const;
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
