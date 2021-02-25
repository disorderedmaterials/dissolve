// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoptionsbase.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

// Enum Options
template <class E> class EnumOptions : public EnumOptionsBase
{
    public:
    EnumOptions() : EnumOptionsBase() {}
    EnumOptions(std::string_view name, const EnumOptionsList &options) : EnumOptionsBase(name, options) {}
    EnumOptions(std::string_view name, const EnumOptionsList &options, E defaultEnumeration)
        : EnumOptionsBase(name, options, defaultEnumeration)
    {
    }

    /*
     * Enum Conversion
     */
    public:
    // Return enumeration in E
    E enumeration(std::string_view keyword) const
    {
        for (auto n = 0; n < options_.size(); ++n)
            if (DissolveSys::sameString(keyword, options_[n].keyword()))
                return (E)options_[n].enumeration();

        Messenger::warn("Option '{}' is not recognised, so can't return its enumeration.\n", keyword);

        return (E)-1;
    }
    // Return current enumeration in E
    E enumeration() const
    {
        // Use local index to return enumeration
        if (!currentOptionIndex_.has_value())
        {
            Messenger::warn("No current option set in EnumOptions, so can't return an enumeration.\n");
            return (E)-1;
        }

        return (E)options_[currentOptionIndex_.value()].enumeration();
    }
    // Return enumerated keyword
    std::string_view keyword(E enumeration) const
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
    const EnumOption &option(E enumeration) const
    {
        for (int n = 0; n < options_.size(); ++n)
            if (options_[n].enumeration() == enumeration)
                return options_[n];
        return unrecognisedOption_;
    }
    // Return option with keyword specified
    const EnumOption &option(std::string_view keyword) const { return EnumOptionsBase::option(keyword); }
    // Return minimum number of arguments for the specified enumeration
    std::optional<int> minArgs(E enumeration) const
    {
        // Retrieve the relevant EnumOption
        const auto &opt = option(enumeration);
        return opt.minArgs();
    }
    // Return maximum number of arguments for the specified enumeration
    std::optional<int> maxArgs(E enumeration) const
    {
        // Retrieve the relevant EnumOption
        const auto &opt = option(enumeration);
        return opt.maxArgs();
    }
    // Return whether an exact number of arguments is required
    bool exactNArgs(E enumeration) const
    {
        const auto &opt = option(enumeration);
        return opt.minArgs() == opt.maxArgs();
    }
    // Check number of arguments provided to keyword
    bool validNArgs(E enumeration, int nArgsProvided) const
    {
        // Retrieve the relevant EnumOption
        const auto &opt = option(enumeration);

        // If a minimum number of arguments (or special case) has *not* been set, check for zero number of arguments being
        // provided
        if (!opt.minArgs())
        {
            if (nArgsProvided == 0)
                return true;
            else
                return Messenger::error("'{}' keyword '{}' does not take any arguments.\n", name(), opt.keyword());
        }

        // Check the specified minimum value
        switch (opt.minArgs().value())
        {
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
                // If maxArgs has *not* been given, then we check for a specific number of arguments (== minArgs)
                if (!opt.maxArgs())
                {
                    if (nArgsProvided == opt.minArgs().value())
                        return true;
                    else
                        return Messenger::error("'{}s' keyword '{}' requires exactly {} {}, but {} {} provided.\n", name(),
                                                opt.keyword(), opt.minArgs().value(),
                                                opt.minArgs().value() == 1 ? "argument" : "arguments", nArgsProvided,
                                                nArgsProvided == 1 ? "was" : "were");
                }
                else
                {
                    // Range of arguments allowed...
                    if (opt.maxArgs().value() == EnumOption::AnyNumberOfArguments)
                    {
                        // No maximum number of arguments, but must satisfy minimum limit
                        if (nArgsProvided >= opt.minArgs().value())
                            return true;
                        else
                            return Messenger::error("'{}' keyword '{}' requires at least {} {}, but {} {} provided.\n", name(),
                                                    opt.keyword(), opt.minArgs().value(),
                                                    opt.minArgs().value() == 1 ? "argument" : "arguments", nArgsProvided,
                                                    nArgsProvided == 1 ? "was" : "were");
                    }
                    else if ((nArgsProvided >= opt.minArgs().value()) && (nArgsProvided <= opt.maxArgs().value()))
                        return true;
                    else
                        return Messenger::error(
                            "'{}' keyword '{}' requires {} {} {}, but {} {} provided.\n", name(), opt.keyword(),
                            opt.minArgs().value() == opt.maxArgs().value()
                                ? "exactly"
                                : nArgsProvided < opt.minArgs().value() ? "at least" : "at most",
                            nArgsProvided < opt.minArgs().value() ? opt.minArgs().value() : opt.maxArgs().value(),
                            opt.minArgs().value() == 1 ? "argument" : "arguments", nArgsProvided,
                            nArgsProvided == 1 ? "was" : "were");
                }
                break;
        }

        return false;
    }

    /*
     * Operators
     */
    public:
    EnumOptions<E> &operator=(E value)
    {
        currentOptionIndex_ = value;
        return *this;
    }
};
