/*
	*** Enum Options
	*** src/base/enumoptions.h
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

#ifndef DISSOLVE_ENUMOPTIONS_H
#define DISSOLVE_ENUMOPTIONS_H

#include "base/enumoptionsbase.h"
#include "base/sysfunc.h"

// Enum Options
template <class T> class EnumOptions : public EnumOptionsBase
{
	public:
	// Constructors
	EnumOptions(const char* name, const EnumOptionsList& options) : EnumOptionsBase(name, options)
	{
	}
	EnumOptions(const char* name, const EnumOptionsList& options, T defaultEnumeration) : EnumOptionsBase(name, options, defaultEnumeration)
	{
	}


	/*
	 * Enum Conversion
	 */
	public:
	// Return enumeration in T
	T enumeration(const char* keyword) const
	{
		for (int n=0; n<options_.nItems(); ++n) if (DissolveSys::sameString(keyword, options_.at(n).keyword())) return (T) n;
		return (T) options_.nItems();
	}
	// Return current enumeration in T
	T enumeration() const
	{
		// Use local index to return enumeration
		if (currentOptionIndex_ == -1)
		{
			Messenger::warn("No current option set in EnumOptions, so can't return an enumeration.\n");
			return (T) -1;
		}

		return (T) options_.at(currentOptionIndex_).enumeration();
	}
	// Return enumerated keyword
	const char* keyword(T enumeration) const
	{
		for (int n=0; n<options_.nItems(); ++n) if (options_.at(n).enumeration() == enumeration) return options_.at(n).keyword();
		return "ENUMERATION_NOT_VALID";
	}
	// Return option with enumeration specified
	const EnumOption& option(T enumeration) const
	{
		for (int n=0; n<options_.nItems(); ++n) if (options_.at(n).enumeration() == enumeration) return options_.at(n);
		return unrecognisedOption_;
	}
	// Return option with keyword specified
	const EnumOption& option(const char* keyword) const
	{
		return EnumOptionsBase::option(keyword);
	}
	// Check number of arguments provided to keyword
	bool validNArgs(T enumeration, int nArgsProvided)
	{
		// Retrieve the relevant EnumOption
		const EnumOption& opt = option(enumeration);

		switch (opt.nArgs())
		{
			case (EnumOption::NoArguments):
				if (nArgsProvided == 0) return true;
				else return Messenger::error("'%s' keyword '%s' does not take any arguments.\n", name(), opt.keyword());
				break;
			case (EnumOption::OneOrMoreArguments):
				if (nArgsProvided > 0) return true;
				else return Messenger::error("'%s' keyword '%s' requires one or more arguments, but none were provided.\n", name(), opt.keyword());
				break;
			case (EnumOption::EnumOption::OptionalSecondArgument):
				if ((nArgsProvided == 1) || (nArgsProvided == 2)) return true;
				else return Messenger::error("'%s' keyword '%s' requires one or two arguments, but %i %s provided.\n", name(), opt.keyword(), nArgsProvided, nArgsProvided == 1 ? "was" : "were");
				break;
			default:
				// Specific number of arguments required
				if (opt.nArgs() == nArgsProvided) return true;
				else return Messenger::error("'%s' keyword '%s' requires %i arguments, but %i %s provided.\n", name(), opt.keyword(), opt.nArgs(), nArgsProvided, nArgsProvided == 1 ? "was" : "were");
				break;
		}

		return false;
	}


	/*
	 * Operators
	 */
	public:
	EnumOptions<T>& operator=(T value)
	{
		currentOptionIndex_ = value;
		return *this;
	}
};

#endif
