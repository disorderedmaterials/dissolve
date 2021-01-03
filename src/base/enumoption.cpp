// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#include "base/enumoptions.h"
#include "base/messenger.h"
#include <stddef.h>

// Static Singleton
UnrecognisedEnumOption EnumOptionsBase::unrecognisedOption_;

EnumOption::EnumOption()
{
    enumeration_ = 0;
    keyword_ = "";
    description_ = "";
    minArgs_ = 0;
    maxArgs_ = 0;
}
EnumOption::EnumOption(const int enumeration, std::string_view keyword, int minArgs, int maxArgs)
{
    enumeration_ = enumeration;
    keyword_ = keyword;
    description_ = "";
    minArgs_ = minArgs;
    maxArgs_ = (maxArgs == 0 ? minArgs : maxArgs);
}
EnumOption::EnumOption(const int enumeration, std::string_view keyword, std::string_view description, int minArgs, int maxArgs)
{
    enumeration_ = enumeration;
    keyword_ = keyword;
    description_ = description;
    minArgs_ = minArgs;
    maxArgs_ = (maxArgs == 0 ? minArgs : maxArgs);
}

// Return if the option is valid (true except in derived classes)
bool EnumOption::isValid() const { return true; }

// Return option enumeration (i.e. from enum value)
int EnumOption::enumeration() const { return enumeration_; }

// Return option keyword
std::string_view EnumOption::keyword() const { return keyword_; }

// Return option description
std::string_view EnumOption::description() const { return description_; }

// Return whether the option has any associated arguments
bool EnumOption::hasArguments() const { return (minArgs_ != 0); }

// Return minimum number of arguments the option takes
int EnumOption::minArgs() const { return minArgs_; }

// Return maximum number of arguments the option takes
int EnumOption::maxArgs() const { return maxArgs_; }
