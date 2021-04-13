// SPDX-License-Identifier: GPL-3.0-or-later
// Copyright (c) 2021 Team Dissolve and contributors

#pragma once

#include <optional>

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

    public:
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
     * Operators
     */
    public:
    EnumOptionsBase &operator=(int index);
};
