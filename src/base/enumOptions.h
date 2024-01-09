// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2024 Team Dissolve and contributors

#pragma once

#include <utility>

#include "base/enumOption.h"
#include "base/enumOptionsBase.h"
#include "base/messenger.h"
#include "base/serialiser.h"
#include "base/sysFunc.h"
#include <cassert>

// Enum Options
template <class E> class EnumOptions : public EnumOptionsBase
{
    public:
    EnumOptions() = default;
    EnumOptions(std::string_view name, std::vector<EnumOption<E>> options) : name_(name), options_(options) {}
    EnumOptions(const EnumOptions<E> &source)
    {
        name_ = source.name_;
        options_ = source.options_;
    }
    EnumOptions<E> &operator=(const EnumOptions<E> &source)
    {
        name_ = source.name_;
        options_ = source.options_;

        return *this;
    }

    /*
     * Option Data
     */
    private:
    // Name of options
    std::string name_;
    // Vector of valid options
    std::vector<EnumOption<E>> options_;

    public:
    // Return name of options (e.g. from source enumeration)
    std::string name() const override { return name_; }
    // Return number of options available
    int nOptions() const override { return options_.size(); }
    // Return nth enumerated value in the list
    E enumerationByIndex(int index) const { return options_[index].enumeration(); }
    // Return nth keyword in the list
    std::string keywordByIndex(int index) const override { return options_[index].keyword(); }
    // Return description for the nth keyword in the list
    std::string descriptionByIndex(int index) const override { return options_[index].description(); }
    // Return whether specified option keyword is valid
    bool isValid(std::string_view keyword) const
    {
        return std::find_if(options_.cbegin(), options_.cend(),
                            [keyword](auto &option)
                            { return DissolveSys::sameString(keyword, option.keyword()); }) != options_.end();
    }

    // Raise error, printing valid options
    bool errorAndPrintValid(std::string_view badKeyword) const
    {
        std::string validValueString;
        for (auto n = 0; n < options_.size(); ++n)
            validValueString += fmt::format(n == 0 ? "{}" : ", {}", options_[n].keyword());

        return Messenger::error("'{}' is not a valid {}.\nValid options are:  {}", badKeyword, name_, validValueString);
    }

    /*
     * Enum Conversion
     */
    public:
    // Return enumeration in E
    E enumeration(std::string_view keyword) const
    {
        auto it = std::find_if(options_.cbegin(), options_.cend(),
                               [keyword](auto &option) { return DissolveSys::sameString(keyword, option.keyword()); });
        if (it != options_.cend())
            return it->enumeration();

        throw(std::runtime_error(fmt::format("Option '{}' is not recognised, so can't return its enumeration.\n", keyword)));
    }
    // Return enumerated keyword
    std::string keyword(E enumeration) const
    {
        auto it = std::find_if(options_.cbegin(), options_.cend(),
                               [enumeration](auto &option) { return enumeration == option.enumeration(); });
        if (it == options_.cend())
            throw(std::runtime_error(fmt::format(
                "Enumerated options '{}' missing enumeration {}, so can't return its keyword.\n", name_, (int)enumeration)));

        return it->keyword();
    }
    // Return enumerated keyword from uncast integer
    std::string keywordFromInt(int uncastEnumeration) const
    {
        auto it = std::find_if(options_.cbegin(), options_.cend(),
                               [uncastEnumeration](auto &option) { return uncastEnumeration == option.enumeration(); });
        if (it == options_.cend())
            throw(std::runtime_error(
                fmt::format("Enumerated options '{}' missing (uncast) enumeration {}, so can't return its keyword.\n", name_,
                            uncastEnumeration)));

        return it->keyword();
    }
    // Return option with enumeration specified
    const EnumOption<E> &option(E enumeration) const
    {
        auto it = std::find_if(options_.cbegin(), options_.cend(),
                               [enumeration](auto &option) { return enumeration == option.enumeration(); });
        if (it == options_.cend())
            throw(std::runtime_error(fmt::format("No option set for enumeration '{}'.\n", (int)enumeration)));
        return *it;
    }
    // Return minimum number of arguments for the specified enumeration
    std::optional<int> minArgs(E enumeration) const
    {
        auto it = std::find_if(options_.begin(), options_.end(),
                               [enumeration](auto &option) { return enumeration == option.enumeration(); });
        if (it == options_.end())
            throw(std::runtime_error(fmt::format("No option set for enumeration '{}'.\n", (int)enumeration)));
        return it->minArgs();
    }
    // Return maximum number of arguments for the specified enumeration
    std::optional<int> maxArgs(E enumeration) const
    {
        auto it = std::find_if(options_.begin(), options_.end(),
                               [enumeration](auto &option) { return enumeration == option.enumeration(); });
        if (it == options_.end())
            throw(std::runtime_error(fmt::format("No option set for enumeration '{}'.\n", (int)enumeration)));
        return it->maxArgs();
    }
    // Check number of arguments provided to keyword
    bool validNArgs(E enumeration, int nArgsProvided, bool quiet = false) const
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
                return quiet ? false
                             : Messenger::error("'{}' keyword '{}' does not take any arguments.\n", name(), opt.keyword());
        }

        // Check the specified minimum value
        switch (opt.minArgs().value())
        {
            case (OptionArguments::OneOrMore):
                if (nArgsProvided > 0)
                    return true;
                else
                    return quiet
                               ? false
                               : Messenger::error("'{}' keyword '{}' requires one or more arguments, but none were provided.\n",
                                                  name(), opt.keyword());
                break;
            case (OptionArguments::OptionalSecond):
                if ((nArgsProvided == 1) || (nArgsProvided == 2))
                    return true;
                else
                    return quiet ? false
                                 : Messenger::error("'{}' keyword '{}' requires one or two arguments, but {} {} provided.\n",
                                                    name(), opt.keyword(), nArgsProvided, nArgsProvided == 1 ? "was" : "were");
                break;
            default:
                // If maxArgs has *not* been given, then we check for a specific number of arguments (== minArgs)
                if (!opt.maxArgs())
                {
                    if (nArgsProvided == opt.minArgs().value())
                        return true;
                    else
                        return quiet ? false
                                     : Messenger::error("'{}s' keyword '{}' requires exactly {} {}, but {} {} provided.\n",
                                                        name(), opt.keyword(), opt.minArgs().value(),
                                                        opt.minArgs().value() == 1 ? "argument" : "arguments", nArgsProvided,
                                                        nArgsProvided == 1 ? "was" : "were");
                }
                else
                {
                    // Range of arguments allowed...
                    if (opt.maxArgs().value() == OptionArguments::AnyNumber)
                    {
                        // No maximum number of arguments, but must satisfy minimum limit
                        if (nArgsProvided >= opt.minArgs().value())
                            return true;
                        else
                            return quiet ? false
                                         : Messenger::error("'{}' keyword '{}' requires at least {} {}, but {} {} provided.\n",
                                                            name(), opt.keyword(), opt.minArgs().value(),
                                                            opt.minArgs().value() == 1 ? "argument" : "arguments",
                                                            nArgsProvided, nArgsProvided == 1 ? "was" : "were");
                    }
                    else if ((nArgsProvided >= opt.minArgs().value()) && (nArgsProvided <= opt.maxArgs().value()))
                        return true;
                    else
                        return quiet
                                   ? false
                                   : Messenger::error(
                                         "'{}' keyword '{}' requires {} {} {}, but {} {} provided.\n", name(), opt.keyword(),
                                         opt.minArgs().value() == opt.maxArgs().value() ? "exactly"
                                         : nArgsProvided < opt.minArgs().value()        ? "at least"
                                                                                        : "at most",
                                         nArgsProvided < opt.minArgs().value() ? opt.minArgs().value() : opt.maxArgs().value(),
                                         opt.minArgs().value() == 1 ? "argument" : "arguments", nArgsProvided,
                                         nArgsProvided == 1 ? "was" : "were");
                }
                break;
        }

        return false;
    }

    SerialisedValue serialise(E value) const { return keyword(value); }

    E deserialise(const SerialisedValue &node) { return enumeration(std::string(node.as_string())); }
};
