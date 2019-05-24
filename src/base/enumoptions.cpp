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

#include "base/enumoptions.h"
#include "base/messenger.h"

// Constructor
EnumOptionsBase::EnumOptionsBase(const char* name, int nOptions, const char** optionKeywords, int currentOption)
{
	name_ = name;
	nOptions_ = nOptions;
	optionKeywords_ = optionKeywords;
	option_ = currentOption;
}

/*
 * Enum Options
 */

// Return number of options available
int EnumOptionsBase::nOptions() const
{
	return nOptions_;
}

// Return option keyword with index specified
const char* EnumOptionsBase::option(int index) const
{
	if ((index < 0) || (index >= nOptions_))
	{
		Messenger::error("Index %i is out of range for enum %s.\n", index, name_);
		return "???";
	}

	return optionKeywords_[index];
}

// Return current option keyword
const char* EnumOptionsBase::optionKeyword() const
{
	return optionKeywords_[option_];
}
