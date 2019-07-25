/*
	*** Enum Option
	*** src/base/enumoption.h
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

#ifndef DISSOLVE_ENUMOPTION_H
#define DISSOLVE_ENUMOPTION_H

#include <stddef.h>

// Enum Option
class EnumOption
{
	public:
	// Constructors
	EnumOption(const int enumeration = 0, const char* keyword = NULL, int nArgs = 0);


	/*
	 * Definition
	 */
	private:
	// Option enumeration (i.e. from enum value)
	int enumeration_;
	// Option keyword
	const char* keyword_;
	// Whether the option has any associated arguments
	bool hasArguments_;
	// Number of arguments the option takes
	int nArgs_;

	public:
	// Return if the option is valid (true except in derived classes)
	virtual bool isValid() const;
	// Return option enumeration (i.e. from enum value)
	int enumeration() const;
	// Return option keyword
	const char* keyword() const;
	// Return whether the option has any associated arguments
	bool hasArguments() const;
	// Return number of arguments the option takes
	int nArgs() const;
};

// Unrecognised Enum Option
class UnrecognisedEnumOption : public EnumOption
{
	public:
	// Constructor
	UnrecognisedEnumOption() : EnumOption(0, "UNRECOGNISED_KEYWORD")
	{
	}

	public:
	// Return if the option is valid (true except in derived classes)
	bool isValid() const
	{
		return false;
	}
};

#endif
