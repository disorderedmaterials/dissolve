/*
    *** Enum Options Base
    *** src/base/enumoptionsbase.cpp
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

#include "base/enumoptionsbase.h"
#include "base/messenger.h"
#include "base/sysfunc.h"
#include <stddef.h>

EnumOptionsBase::EnumOptionsBase()
{
    name_ = "DummyOptions";

    currentOptionIndex_ = -1;
}

EnumOptionsBase::EnumOptionsBase(std::string_view name, const EnumOptionsList &options)
{
    name_ = name;
    options_ = options.options();

    currentOptionIndex_ = -1;
}

EnumOptionsBase::EnumOptionsBase(std::string_view name, const EnumOptionsList &options, int defaultEnumeration)
{
    name_ = name;
    options_ = options.options();

    currentOptionIndex_ = -1;
    for (int n = 0; n < options_.size(); ++n)
        if (options_[n].enumeration() == defaultEnumeration)
        {
            currentOptionIndex_ = n;
            break;
        }
}

// Return name of options (e.g. from source enumeration)
std::string_view EnumOptionsBase::name() const { return name_; }

// Return number of options available
int EnumOptionsBase::nOptions() const { return options_.size(); }

// Return nth keyword in the list
std::string_view EnumOptionsBase::keywordByIndex(int index) const
{
    if ((index < 0) || (index >= options_.size()))
    {
        Messenger::error("Keyword index {} out of range for EnumOptions '{}'.\n", index, name_);
        return unrecognisedOption_.keyword();
    }

    return options_[index].keyword();
}

// Return description for the nth keyword in the list
std::string_view EnumOptionsBase::descriptionByIndex(int index) const
{
    if ((index < 0) || (index >= options_.size()))
    {
        Messenger::error("Keyword index {} out of range for EnumOptions '{}'.\n", index, name_);
        return unrecognisedOption_.keyword();
    }

    return options_[index].description();
}

// Return option by keyword
const EnumOption &EnumOptionsBase::option(std::string_view keyword) const
{
    for (int n = 0; n < options_.size(); ++n)
        if (DissolveSys::sameString(keyword, options_[n].keyword()))
            return options_[n];
    return unrecognisedOption_;
}

// Return current option keyword
std::string_view EnumOptionsBase::currentOptionKeyword() const
{
    if (currentOptionIndex_ == -1)
        return "UNDEFINED";

    return options_[currentOptionIndex_].keyword();
}

// Return current option
const EnumOption &EnumOptionsBase::currentOption() const
{
    if (currentOptionIndex_ == -1)
        return unrecognisedOption_;

    return options_[currentOptionIndex_];
}

// Return current option index
int EnumOptionsBase::currentOptionIndex() const { return currentOptionIndex_; }

// Set current option index
bool EnumOptionsBase::setCurrentOptionIndex(int index)
{
    if ((index < 0) || (index >= options_.size()))
        return Messenger::error("EnumOptions index {} is out of range for '{}'.\n", index, name());

    currentOptionIndex_ = index;

    return true;
}

// Set current option from keyword
bool EnumOptionsBase::setCurrentOption(std::string_view keyword)
{
    for (int n = 0; n < options_.size(); ++n)
        if (DissolveSys::sameString(keyword, options_[n].keyword()))
        {
            currentOptionIndex_ = n;
            return true;
        }

    return false;
}

// Return whether specified option keyword is valid
bool EnumOptionsBase::isValid(std::string_view keyword) const
{
    for (int n = 0; n < options_.size(); ++n)
        if (DissolveSys::sameString(keyword, options_[n].keyword()))
            return true;
    return false;
}

// Raise error, printing valid options
bool EnumOptionsBase::errorAndPrintValid(std::string_view badKeyword) const
{
    std::string validValueString;
    for (int n = 0; n < options_.size(); ++n)
        validValueString += fmt::format(n == 0 ? "{}" : ", {}", options_[n].keyword());
    Messenger::error("'{}' is not a valid {}.\nValid options are:  {}", badKeyword, name_, validValueString);

    return false;
}
