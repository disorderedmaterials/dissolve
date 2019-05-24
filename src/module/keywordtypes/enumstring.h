/*
	*** Module Keyword - Enum String
	*** src/module/keywordtypes/enumstring.h
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

#ifndef DISSOLVE_MODULEKEYWORDENUMSTRING_H
#define DISSOLVE_MODULEKEYWORDENUMSTRING_H

#include "module/keyworddata.h"
#include "module/keywordbase.h"
#include "base/enumoptions.h"

// Forward Declarations
/* none */

// Keyword with Enum Value
class EnumStringModuleKeyword : public ModuleKeywordBase, public ModuleKeywordData<CharString>
{
	public:
	// Constructor
	EnumStringModuleKeyword(int value, int nOptions, const char** options);
	EnumStringModuleKeyword(EnumOptionsBase options);
	// Destructor
	~EnumStringModuleKeyword();


	/*
	 * Data
	 */
	public:
	// Return whether the current data value has ever been set
	bool isSet();


	/*
	 * Data Validation
	 */
	private:
	// List of valid values (if appropriate)
	Array<CharString> validValues_;

	public:
	// Return whether a validation list has been set
	bool hasValidationList();
	// Return validation list
	const Array<CharString>& validationList();
	// Validate supplied value
	bool isValid(CharString value);


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

