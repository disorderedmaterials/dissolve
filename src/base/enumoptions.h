/*
	*** Enum Options Provider
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

#include "base/messenger.h"
#include "base/sysfunc.h"
#include "templates/array.h"

// Enum Option
class EnumOption
{
	public:
	// Constructors
	EnumOption(const int enumeration = 0, const char* keyword = NULL, int nArgs = 0);

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

class EnumOptionsList
{
	private:
	// List of options
	Array<EnumOption> options_;

	public:
	// Return list of options
	const Array<EnumOption>& options() const
	{
		return options_;
	}
	// Add item
	EnumOptionsList& operator<<(EnumOption option)
	{
		options_.add(option);
		return (*this);
	}
};

// Enum Options Base
class EnumOptionsBase
{
	public:
	// Constructors
	EnumOptionsBase(const char* name, const EnumOptionsList& options);
	EnumOptionsBase(const char* name, const EnumOptionsList& options, int defaultEnumeration);


	/*
	 * Name
	 */
	protected:
	// Name of options (e.g. from source enumeration)
	const char* name_;

	protected:
	// Unrecognised option
	static UnrecognisedEnumOption unrecognisedOption_;

	public:
	// Return name of options (e.g. from source enumeration)
	const char* name() const;


	/*
	 * Enum Option Data
	 */
	protected:
	// Options
	Array<EnumOption> options_;
	// Current option index in local options_ array
	int currentOptionIndex_;

	public:
	// Return number of options available
	int nOptions() const;
	// Return nth keyword in the list
	const char* keywordByIndex(int index) const;
	// Return option by keyword
	const EnumOption& option(const char* keyword) const;
	// Return current option keyword
	const char* currentOptionKeyword() const;
	// Return current option
	const EnumOption& currentOption() const;
	// Return current option index
	int currentOptionIndex() const;
	// Set current option index
	void setCurrentOptionIndex(int index);
	// Set current option from keyword
	bool setCurrentOption(const char* keyword);
	// Return whether specified option keyword is valid
	bool isValid(const char* keyword) const;


	/*
	 * Error Reporting
	 */
	public:
	// Raise error, printing valid options
	bool errorAndPrintValid(const char* badKeyword) const;


	/*
	 * Operators
	 */
	public:
	EnumOptionsBase& operator=(int index);
};

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

		// No arguments?
		if (opt.nArgs() == 0)
		{
			if (nArgsProvided == 0) return true;
			else return Messenger::error("'%s' keyword '%s' does not take any arguments.\n", name(), opt.keyword());
		}

		// Some number of arguments
		if (opt.nArgs() == nArgsProvided) return true;
		else return Messenger::error("'%s' keyword '%s' requires %i arguments, but %i %s provided.\n", name(), opt.keyword(), opt.nArgs(), nArgsProvided == 1 ? "was" : "were");
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
