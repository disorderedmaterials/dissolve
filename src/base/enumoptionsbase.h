// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2020 Team Dissolve and contributors

#pragma once

#include "base/enumoptionslist.h"
#include <optional>

// Enum Options Base
class EnumOptionsBase
{
    public:
    EnumOptionsBase();
    EnumOptionsBase(std::string_view name, const EnumOptionsList &options);
    EnumOptionsBase(std::string_view name, const EnumOptionsList &options, int defaultEnumeration);

    /*
     * Name
     */
    protected:
    // Name of options (e.g. from source enumeration)
    std::string name_;

    protected:
    // Unrecognised option
    static UnrecognisedEnumOption unrecognisedOption_;

    public:
    // Return name of options (e.g. from source enumeration)
    std::string_view name() const;

    /*
     * Enum Option Data
     */
    protected:
    // Options
    std::vector<EnumOption> options_;
    // Current option index in local options_ array
    std::optional<int> currentOptionIndex_;

    public:
    // Return number of options available
    int nOptions() const;
    // Return nth keyword in the list
    std::string_view keywordByIndex(int index) const;
    // Return description for the nth keyword in the list
    std::string_view descriptionByIndex(int index) const;
    // Return option by keyword
    const EnumOption &option(std::string_view keyword) const;
    // Return current option keyword
    std::string_view currentOptionKeyword() const;
    // Return current option
    const EnumOption &currentOption() const;
    // Return current option index
    int currentOptionIndex() const;
    // Set current option index
    bool setCurrentOptionIndex(int index);
    // Set current option from keyword
    bool setCurrentOption(std::string_view keyword);
    // Return whether specified option keyword is valid
    bool isValid(std::string_view keyword) const;

    /*
     * Error Reporting
     */
    public:
    // Raise error, printing valid options
    bool errorAndPrintValid(std::string_view badKeyword) const;

    /*
     * Operators
     */
    public:
    EnumOptionsBase &operator=(int index);
};
