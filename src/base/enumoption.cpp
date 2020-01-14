/*
	*** Enum Options Provider
	*** src/base/enumoptions.cpp
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

#include "base/charstring.h"
#include "base/enumoptions.h"
#include "base/messenger.h"
#include <stddef.h>

// Static Singleton
UnrecognisedEnumOption EnumOptionsBase::unrecognisedOption_;

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
