/*
	*** Keyword - Integer
	*** src/module/keywordtypes/integer.h
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

#ifndef DISSOLVE_KEYWORD_INTEGER_H
#define DISSOLVE_KEYWORD_INTEGER_H

#include "keywords/data.h"
#include "keywords/base.h"

// Forward Declarations
/* none */

// Keyword with Integer Data
class IntegerModuleKeyword : public ModuleKeywordData<int>
{
	public:
	// Constructors
	IntegerModuleKeyword(int value);
	IntegerModuleKeyword(int value, int minValue);
	IntegerModuleKeyword(int value, int minValue, int maxValue);
	// Destructor
	~IntegerModuleKeyword();


	/*
	 * Data Validation
	 */
	private:
	// Validation limits to apply (if any)
	bool minimumLimit_, maximumLimit_;
	// Validation range (if appropriate)
	int min_, max_;

	public:
	// Return whether a minimum validation limit has been set
	bool hasValidationMin();
	// Return validation minimum limit
	int validationMin();
	// Return whether a maximum validation limit has been set
	bool hasValidationMax();
	// Return validation maximum limit
	int validationMax();
	// Validate supplied value
	bool isValid(int value);


	/*
	 * Arguments
	 */
	public:
	// Return minimum number of arguments accepted
	int minArguments();
	// Return maximum number of arguments accepted
	int maxArguments();
	// Parse arguments from supplied LineParser, starting at given argument offset, utilising specified ProcessPool if required
	bool read(LineParser& parser, int startArg, const CoreData& coreData, ProcessPool& procPool);
	// Write keyword data to specified LineParser
	bool write(LineParser& parser, const char* prefix);


	/*
	 * Conversion
	 */
	public:
	// Return value (as bool)
	bool asBool();
	// Return value (as int)
	int asInt();
	// Return value (as double)
	double asDouble();
	// Return value (as string)
	const char* asString();
};

#endif

