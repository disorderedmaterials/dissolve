/*
	*** Keyword - EnumOptions
	*** src/keywords/enumoptions.h
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

#ifndef DISSOLVE_KEYWORD_ENUMOPTIONS_H
#define DISSOLVE_KEYWORD_ENUMOPTIONS_H

#include "keywords/data.h"
#include "base/enumoptions.h"
#include "base/lineparser.h"

// Forward Declarations
/* none */

// EnumOptionsKeyword base class
class EnumOptionsBaseKeyword
{
	public:
	// Constructor
	EnumOptionsBaseKeyword(EnumOptionsBase& baseOptions) : baseOptions_(baseOptions)
	{
	}


	/*
	 * Source Options
	 */
	private:
	// Source EnumBaseOptions
	EnumOptionsBase& baseOptions_;

	public:
	// Return EnumBaseOptions
	const EnumOptionsBase& baseOptions() const
	{
		return baseOptions_;
	}


	/*
	 * Set
	 */
	public:
	// Set new option index, informing KeywordBase
	virtual void setEnumerationByIndex(int optionIndex) = 0;
};

// Keyword based on EnumOptions
template <class E> class EnumOptionsKeyword : public EnumOptionsBaseKeyword, public KeywordData< EnumOptions<E> >
{
	public:
	// Constructor
	EnumOptionsKeyword(EnumOptions<E> options) : KeywordData< EnumOptions<E> >(KeywordBase::EnumOptionsData, options), EnumOptionsBaseKeyword(KeywordData< EnumOptions<E> >::data_)
	{
		// Set our array of valid values
		for (int n=0; n<KeywordData< EnumOptions<E> >::data_.nOptions(); ++n) validKeywords_.add(KeywordData< EnumOptions<E> >::data_.keywordByIndex(n));
	}
	// Destructor
	~EnumOptionsKeyword()
	{
	}


	/*
	 * Data Validation
	 */
	private:
	// List of valid keyword values
	Array<CharString> validKeywords_;

	public:
	// Return validation list
	const Array<CharString>& validationList()
	{
		return validKeywords_;
	}
	// Validate supplied value
	bool isValid(CharString value)
	{
		return KeywordData< EnumOptions<E> >::data_.isValid(value.get());
	}


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments() const
	{
		return 1;
	}
	// Return maximum number of arguments accepted
	int maxArguments() const
	{
		return 1;
	}
	// Parse arguments from supplied LineParser, starting at given argument offset
	bool read(LineParser& parser, int startArg, const CoreData& coreData)
	{
		if (parser.hasArg(startArg))
		{
			// Check validity of the supplied keyword...
			if (!KeywordData< EnumOptions<E> >::data_.isValid(parser.argc(startArg))) return KeywordData< EnumOptions<E> >::data_.errorAndPrintValid(parser.argc(startArg));

			// Keyword recognised...
			EnumOptions<E> newOptions(KeywordData< EnumOptions<E> >::data_);
			newOptions.setCurrentOption(parser.argc(startArg));
			if (!KeywordData< EnumOptions<E> >::setData(newOptions)) return Messenger::error("An odd thing happened....\n");

			return true;
		}

		return false;
	}
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* keywordName, const char* prefix)
	{
		return parser.writeLineF("%s%s  '%s'\n", prefix, KeywordData< EnumOptions<E> >::name(), KeywordData< EnumOptions<E> >::data_.currentOptionKeyword());
	}


	/*
	 * Conversion
	 */
	public:
	// Return value (as string)
	const char* asString()
	{
		return KeywordData< EnumOptions<E> >::data_.currentOptionKeyword();
	}


	/*
	 * Set (implementing pure virtual from EnumOptionsBaseKeyword)
	 */
	public:
	// Set new option index, informing KeywordBase
	void setEnumerationByIndex(int optionIndex)
	{
		KeywordData< EnumOptions<E> >::data_.setCurrentOptionIndex(optionIndex);
		KeywordData< EnumOptions<E> >::dataHasBeenSet();
	}
};

#endif

