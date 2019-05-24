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

#include "base/sysfunc.h"

// Enum Options Base
class EnumOptionsBase
{
	public:
	// Constructor
	EnumOptionsBase(const char* name, int nOptions, const char** options, int currentOption);


	/*
	 * Name for Options
	 */
	private:
	const char* name_;


	/*
	 * Enum Option Data
	 */
	protected:
	// Number of options available
	int nOptions_;
	// Option keywords
	const char** optionKeywords_;
	// Current option index
	int option_;

	public:
	// Return number of options available
	int nOptions() const;
	// Return option keyword with index specified
	const char* option(int index) const;
	// Return current option keyword
	const char* optionKeyword() const;


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
	// Constructor
	EnumOptions(const char* name, int nOptions, const char** options, T currentOption) : EnumOptionsBase(name, nOptions, options, currentOption)
	{
	}


	/*
	 * Enum Conversion
	 */
	public:
	// Return enumeration in T
	T option(const char* keyword) const
	{
		for (int n=0; n<nOptions_; ++n) if (DissolveSys::sameString(keyword, optionKeywords_[n])) return (T) n;
		return (T) nOptions_;
	}
	// Return option keyword with index specified
	const char* option(T index) const
	{
		return EnumOptionsBase::option((int) index);
	}
	// Return current option
	T option() const
	{
		return (T) option_;
	}


	/*
	 * Operators
	 */
	public:
	EnumOptions<T>& operator=(T value)
	{
		option_ = value;
		return *this;
	}
};

#endif
