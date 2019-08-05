/*
	*** Enum Options Provider
	*** src/base/enumoptions.cpp
	Copyright T. Youngs 2012-2019

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

#include "base/charstring.h"
#include "base/enumoptions.h"
#include "base/messenger.h"
#include <stddef.h>

// Static Singleton
UnrecognisedEnumOption EnumOptionsBase::unrecognisedOption_;

/*
 * EnumOption
 */

EnumOption::EnumOption(const int enumeration, const char* keyword, int minArgs, int maxArgs)
{
	enumeration_ = enumeration;
	keyword_ = keyword;
	minArgs_ = minArgs;
	maxArgs_ = (maxArgs == 0 ? minArgs : maxArgs);
}

// Return if the option is valid (true except in derived classes)
bool EnumOption::isValid() const
{
	return true;
}

// Return option enumeration (i.e. from enum value)
int EnumOption::enumeration() const
{
	return enumeration_;
}

// Return option keyword
const char* EnumOption::keyword() const
{
	return keyword_;
}

// Return whether the option has any associated arguments
bool EnumOption::hasArguments() const
{
	return (minArgs_ != 0);
}

// Return minimum number of arguments the option takes
int EnumOption::minArgs() const
{
	return minArgs_;
}

// Return maximum number of arguments the option takes
int EnumOption::maxArgs() const
{
	return maxArgs_;
}

/*
 * EnumOptionsBase
 */

// Constructors
EnumOptionsBase::EnumOptionsBase(const char* name, const EnumOptionsList& options)
{
	name_ = name;
	options_ = options.options();

	currentOptionIndex_ = -1;
}

EnumOptionsBase::EnumOptionsBase(const char* name, const EnumOptionsList& options, int defaultEnumeration)
{
	name_ = name;
	options_ = options.options();;

	currentOptionIndex_ = -1;
	for (int n=0; n<options_.nItems(); ++n) if (options_[n].enumeration() == defaultEnumeration)
	{
		currentOptionIndex_ = n;
		break;
	}
}

// Return name of options (e.g. from source enumeration)
const char* EnumOptionsBase::name() const
{
	return name_;
}

// Return number of options available
int EnumOptionsBase::nOptions() const
{
	return options_.nItems();
}

// Return nth keyword in the list
const char* EnumOptionsBase::keywordByIndex(int index) const
{
	if ((index < 0) || (index >= options_.nItems()))
	{
		Messenger::error("Keyword index %i out of range for EnumOptions '%s'.\n", index, name_);
		return unrecognisedOption_.keyword();
	}

	return options_.constAt(index).keyword();
}

// Return option by keyword
const EnumOption& EnumOptionsBase::option(const char* keyword) const
{
	for (int n=0; n<options_.nItems(); ++n) if (DissolveSys::sameString(keyword, options_.constAt(n).keyword())) return options_.constAt(n);
	return unrecognisedOption_;
}

// Return current option keyword
const char* EnumOptionsBase::currentOptionKeyword() const
{
	if (currentOptionIndex_ == -1) return "UNDEFINED";

	return options_.constAt(currentOptionIndex_).keyword();
}

// Return current option
const EnumOption& EnumOptionsBase::currentOption() const
{
	if (currentOptionIndex_ == -1) return unrecognisedOption_;

	return options_.constAt(currentOptionIndex_);
}

// Return current option index
int EnumOptionsBase::currentOptionIndex() const
{
	return currentOptionIndex_;
}

// Set current option index
void EnumOptionsBase::setCurrentOptionIndex(int index)
{
	currentOptionIndex_ = index;
}

// Set current option from keyword
bool EnumOptionsBase::setCurrentOption(const char* keyword)
{
	for (int n=0; n<options_.nItems(); ++n) if (DissolveSys::sameString(keyword, options_.constAt(n).keyword()))
	{
		currentOptionIndex_ = n;
		return true;
	}

	return false;
}

// Return whether specified option keyword is valid
bool EnumOptionsBase::isValid(const char* keyword) const
{
	for (int n=0; n<options_.nItems(); ++n) if (DissolveSys::sameString(keyword, options_.constAt(n).keyword())) return true;
	return false;
}

// Raise error, printing valid options
bool EnumOptionsBase::errorAndPrintValid(const char* badKeyword) const
{
	CharString validValueString;
	for (int n=0; n<options_.nItems(); ++n) validValueString += CharString(n == 0 ? "%s" : ", %s", options_.constAt(n).keyword());
	Messenger::error("'%s' is not a valid %s.\nValid options are:  %s", badKeyword, name_, validValueString.get());

	return false;
}
