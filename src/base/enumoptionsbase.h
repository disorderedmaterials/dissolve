// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2022 Team Dissolve and contributors

#pragma once

#include <optional>
#include <string>

// Enum Options Base
class EnumOptionsBase
{
    public:
    EnumOptionsBase() = default;
    ~EnumOptionsBase() = default;

    /*
     * Options Data
     */
    public:
    // Return name of options (e.g. from source enumeration)
    virtual std::string name() const = 0;
    // Return number of options available
    virtual int nOptions() const = 0;
    // Return nth keyword in the list
    virtual std::string keywordByIndex(int index) const = 0;
    // Return description for the nth keyword in the list
    virtual std::string descriptionByIndex(int index) const = 0;
    // Return index of current option
    virtual int index() const = 0;
    // Set current option index
    virtual void setIndex(int index) = 0;

    /*
     * Search
     */
    public:
    // Return index of matching keyword, if it exists
    std::optional<int> keywordIndex(std::string_view keyword) const;
};
