/*
	*** Module Keyword Interface
	*** src/modules/modulekeywordinterface.h
	Copyright T. Youngs 2012-2017

	This file is part of dUQ.

	dUQ is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	dUQ is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with dUQ.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef DUQ_MODULEKEYWORDINTERFACE_H
#define DUQ_MODULEKEYWORDINTERFACE_H

#include "templates/array.h"

// Forward Declarations
class GenericList;
class LineParser;
class ModuleKeywordBase;

// Module Keyword Interface
template <class T> class ModuleKeywordInterface
{
	public:
	// Constructor
	ModuleKeywordInterface(T data)
	{
		data_ = data;
		listLimit_ = false;
		minimumLimit_ = false;
		maximumLimit_ = false;
	}


	/*
	 * Data
	 */
	protected:
	// Data type (POD or otherwise) that is to be set by reading from an input file
	T data_;

	public:
	// Return minimum number of arguments accepted
	virtual int minArguments() = 0;
	// Return maxnimum number of arguments accepted
	virtual int maxArguments() = 0;
	// Parse arguments from supplied LineParser, starting at argument offset specified
	virtual bool parseArguments(LineParser& parser, int startArg) = 0;


	/*
	 * Data Validation
	 */
	private:
	// Validation limits to apply (if any)
	bool minimumLimit_, maximumLimit_, listLimit_;
	// Validation range (if appropriate)
	T min_, max_;
	// Validation List (if appropriate)
	Array<T> allowedValues_;

	protected:
	// Set validation range
	void setValidationRange(T minValue, T maxValue)
	{
		minimumLimit_ = true;
		min_ = minValue;
		maximumLimit_ = true;
		max_ = maxValue;
	}
	// Set minimum validation limit
	void setValidationMin(T minValue)
	{
		minimumLimit_ = true;
		min_ = minValue;
	}
	// Set maximum validation limit
	void setValidationMax(T maxValue)
	{
		maximumLimit_ = true;
		max_ = maxValue;
	}
	// Set validation list
	void setValidationList(Array<T> allowedValues)
	{
		listLimit_ = true;
		allowedValues_ = allowedValues;
	}

	public:
	// Validate value
	bool isValid(T value)
	{
		if (listLimit_)
		{
			for (int n=0; n<allowedValues_.nItems(); ++n) if (value == allowedValues_[n]) return true;
			return false;
		}
		else
		{
			// Check minimum limit
			if (minimumLimit_)
			{
				if (value < minimumLimit_) return false;
			}

			// Check maximum limit
			if (maximumLimit_)
			{
				if (value > minimumLimit_) return false;
			}
		}

		return true;
	}
};

#endif

