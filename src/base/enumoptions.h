// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include "base/enumoption.h"
#include "base/enumoptionsbase.h"
#include "base/messenger.h"
#include "base/sysfunc.h"

// Enum Options
template <class E> class EnumOptions : public EnumOptionsBase
{
    public:
    EnumOptions() = default;
    EnumOptions(std::string_view name, const std::vector<EnumOption<E>> &options) : name_(name), options_(options)
    {
        currentOption_ = options_.cbegin();
    }
    EnumOptions(const EnumOptions<E> &source)
    {
        name_ = source.name_;
        options_ = source.options_;
        currentOption_ = options_.cbegin() + source.index();
    }
    EnumOptions<E> &operator=(E value)
    {
        currentOption_ =
            std::find_if(options_.cbegin(), options_.cend(), [value](auto &option) { return option.enumeration() == value; });
        if (currentOption_ == options_.end())
            throw(std::runtime_error(fmt::format("Enumerated options '{}' missing enumeration {}.\n", name_, value)));

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
    // Currently selected option (by index) in local options_ array
    typename std::vector<EnumOption<E>>::const_iterator currentOption_;

    public:
    // Return name of options (e.g. from source enumeration)
    std::string name() const override { return name_; }
    // Return number of options available
    int nOptions() const override { return options_.size(); }
    // Return nth keyword in the list
    std::string keywordByIndex(int index) const override { return options_[index].keyword(); }
    // Return description for the nth keyword in the list
    std::string descriptionByIndex(int index) const override { return options_[index].description(); }
    // Return current option keyword
    std::string keyword() const { return currentOption_ == options_.cend() ? "UNDEFINED" : currentOption_->keyword(); }
    // Set current option keyword
    bool set(std::string_view keyword)
    {
        auto it = std::find_if(options_.cbegin(), options_.cend(),
                               [keyword](auto &option) { return DissolveSys::sameString(keyword, option.keyword()); });
        if (it == options_.cend())
            return false;

        currentOption_ = it;

        return true;
    }
    // Return index of current option
    int index() const override { return currentOption_ - options_.cbegin(); }
    // Set current option from keyword
    void setIndex(int index) override
    {
        assert(index >= 0 && index < options_.size());

        currentOption_ = options_.cbegin() + index;
    }
    // Return whether specified option keyword is valid
    bool isValid(std::string_view keyword) const
    {
        return std::find_if(options_.cbegin(), options_.cend(), [keyword](auto &option) {
                   return DissolveSys::sameString(keyword, option.keyword());
               }) != options_.end();
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
    // Return current enumeration in E
    E enumeration() const
    {
        assert(currentOption_ != options_.cend());

        return currentOption_->enumeration();
    }
    // Return enumerated keyword
    std::string keyword(E enumeration) const
    {
        auto it = std::find_if(options_.cbegin(), options_.cend(),
                               [enumeration](auto &option) { return enumeration == option.enumeration(); });
        if (it == options_.cend())
            throw(std::runtime_error(fmt::format(
                "Enumerated options '{}' missing enumeration {}, so can't return its keyword.\n", name_, enumeration)));

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
            throw(std::runtime_error(fmt::format("No option set for enumeration '{}'.\n", enumeration)));
        return *it;
    }
    // Return minimum number of arguments for the specified enumeration
    std::optional<int> minArgs(E enumeration) const
    {
        auto it = std::find_if(options_.begin(), options_.end(),
                               [enumeration](auto &option) { return enumeration == option.enumeration(); });
        if (it == options_.end())
            return std::nullopt;
        return it->minArgs();
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
            case (OptionArguments::OneOrMore):
                if (nArgsProvided > 0)
                    return true;
                else
                    return Messenger::error("'{}' keyword '{}' requires one or more arguments, but none were provided.\n",
                                            name(), opt.keyword());
                break;
            case (OptionArguments::OptionalSecond):
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
                    if (opt.maxArgs().value() == OptionArguments::AnyNumber)
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
};
