/*
	*** Keyword Data
	*** src/keywords/data.h
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

#ifndef DISSOLVE_KEYWORD_DATA_H
#define DISSOLVE_KEYWORD_DATA_H

#include "templates/array.h"
#include "keywords/base.h"

// Forward Declarations
class GenericList;
class LineParser;

// Keyword Interface
template <class T> class KeywordData : public KeywordBase
{
	public:
	// Constructor
	KeywordData(KeywordBase::KeywordDataType type, T data) : KeywordBase(type), data_(data)
	{
	}


	/*
	 * Data
	 */
	protected:
	// Data (POD or otherwise) that is to be set by reading from an input file
	T data_;

	protected:
	// Determine whether current data is 'empty', and should be considered as 'not set'
	virtual bool isDataEmpty() const
	{
		// Override this function to handle cases where, for instance, checks for empty lists need to be made.
		return false;
	}

	public:
	// Set data, validating as necessary
	bool setData(T value)
	{
		if (isValid(value))
		{
			// Data is valid, so store it
			data_ = value;

			// Check here if the data is 'empty', in which case it is not strictly 'set'
			set_ = isDataEmpty() ? false : true;

			return true;
		}

		return false;
	}
	// Return data
	T& data()
	{
		return data_;
	}
	// Flag that data has been set by some other means
	void dataHasBeenSet()
	{
		set_ = true;
	}


	/*
	 * Data Validation
	 */
	public:
	// Validate supplied value
	virtual bool isValid(T value)
	{
		return true;
	}
};

#endif

