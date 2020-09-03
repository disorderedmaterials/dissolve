/*
    *** Enum Options
    *** src/base/enumoptions.h
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

#include "base/enumoptionsbase.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

// Enum Options
template <class T> class EnumOptions : public EnumOptionsBase
{
    public:
    EnumOptions() : EnumOptionsBase() {}
    EnumOptions(std::string_view name, const EnumOptionsList &options) : EnumOptionsBase(name, options) {}
    EnumOptions(std::string_view name, const EnumOptionsList &options, T defaultEnumeration)
        : EnumOptionsBase(name, options, defaultEnumeration)
    {
    }

    /*
     * Enum Conversion
     */
    public:
    // Return enumeration in T
    T enumeration(std::string_view keyword) const
    {
        for (int n = 0; n < options_.size(); ++n)
            if (DissolveSys::sameString(keyword, options_[n].keyword()))
                return (T)options_[n].enumeration();

        Messenger::warn("Option '{}' is not recognised, so can't return its enumeration.\n", keyword);

        return (T)-1;
    }
    // Return current enumeration in T
    T enumeration() const
    {
        // Use local index to return enumeration
        if (currentOptionIndex_ == -1)
        {
            Messenger::warn("No current option set in EnumOptions, so can't return an enumeration.\n");
            return (T)-1;
        }

        return (T)options_[currentOptionIndex_].enumeration();
    }
    // Return enumerated keyword
    std::string_view keyword(T enumeration) const
    {
        for (int n = 0; n < options_.size(); ++n)
            if (options_[n].enumeration() == enumeration)
                return options_[n].keyword();
        return "ENUMERATION_NOT_VALID";
    }
    // Return enumerated keyword from uncast integer
    std::string_view keywordFromInt(int uncastEnumeration) const
    {
        for (int n = 0; n < options_.size(); ++n)
            if (options_[n].enumeration() == uncastEnumeration)
                return options_[n].keyword();
        return "ENUMERATION_NOT_VALID";
    }
    // Return option with enumeration specified
    const EnumOption &option(T enumeration) const
    {
        for (int n = 0; n < options_.size(); ++n)
            if (options_[n].enumeration() == enumeration)
                return options_[n];
        return unrecognisedOption_;
    }
    // Return option with keyword specified
    const EnumOption &option(std::string_view keyword) const { return EnumOptionsBase::option(keyword); }
    // Return minimum number of arguments for the specified enumeration
    int minArgs(T enumeration) const
    {
        // Retrieve the relevant EnumOption
        const auto &opt = option(enumeration);
        return opt.minArgs();
    }
    // Return maximum number of arguments for the specified enumeration
    int maxArgs(T enumeration) const
    {
        // Retrieve the relevant EnumOption
        const auto &opt = option(enumeration);
        return opt.maxArgs();
    }
    // Return whether an exact number of arguments is required
    bool exactNArgs(T enumeration) const
    {
        const auto &opt = option(enumeration);
        return opt.minArgs() == opt.maxArgs();
    }
    // Check number of arguments provided to keyword
    bool validNArgs(T enumeration, int nArgsProvided) const
    {
        // Retrieve the relevant EnumOption
        const auto &opt = option(enumeration);

        switch (opt.minArgs())
        {
            case (EnumOption::NoArguments):
                if (nArgsProvided == 0)
                    return true;
                else
                    return Messenger::error("'{}' keyword '{}' does not take any arguments.\n", name(), opt.keyword());
                break;
            case (EnumOption::OneOrMoreArguments):
                if (nArgsProvided > 0)
                    return true;
                else
                    return Messenger::error("'{}' keyword '{}' requires one or more arguments, but none were provided.\n",
                                            name(), opt.keyword());
                break;
            case (EnumOption::EnumOption::OptionalSecondArgument):
                if ((nArgsProvided == 1) || (nArgsProvided == 2))
                    return true;
                else
                    return Messenger::error("'{}' keyword '{}' requires one or two arguments, but {} {} provided.\n", name(),
                                            opt.keyword(), nArgsProvided, nArgsProvided == 1 ? "was" : "were");
                break;
            default:
                // Specific number of arguments required
                if ((nArgsProvided >= opt.minArgs()) && (nArgsProvided <= opt.maxArgs()))
                    return true;
                else
                    return Messenger::error(
                        "'{}s' keyword '{}' requires {} {} {}, but {} {} provided.\n", name(), opt.keyword(),
                        opt.minArgs() == opt.maxArgs() ? "exactly" : nArgsProvided < opt.minArgs() ? "at least" : "at most",
                        nArgsProvided < opt.minArgs() ? opt.minArgs() : opt.maxArgs(),
                        opt.minArgs() == 1 ? "argument" : "arguments", nArgsProvided, nArgsProvided == 1 ? "was" : "were");
                break;
        }

        return false;
    }

    /*
     * Operators
     */
    public:
    EnumOptions<T> &operator=(T value)
    {
        currentOptionIndex_ = value;
        return *this;
    }
};
