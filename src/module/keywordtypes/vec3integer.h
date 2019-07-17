/*
	*** Module Keyword - Integer Triplet
	*** src/module/keywordtypes/vec3integer.h
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

#ifndef DISSOLVE_MODULEKEYWORD_VEC3INTEGER_H
#define DISSOLVE_MODULEKEYWORD_VEC3INTEGER_H

#include "module/keyworddata.h"
#include "module/keywordbase.h"

// Forward Declarations
/* none */

// Keyword with Integer Triplet Data
class Vec3IntegerModuleKeyword : public ModuleKeywordData< Vec3<int> >
{
	public:
	// Constructors
	Vec3IntegerModuleKeyword(Vec3<int> value);
	Vec3IntegerModuleKeyword(Vec3<int> value, Vec3<int> minValue);
	Vec3IntegerModuleKeyword(Vec3<int> value, Vec3<int> minValue, Vec3<int> maxValue);
	// Destructor
	~Vec3IntegerModuleKeyword();


	/*
	 * Data Validation
	 */
	private:
	// Validation limits to apply (if any)
	Vec3<bool> minimumLimit_, maximumLimit_;
	// Validation range (if appropriate)
	Vec3<int> min_, max_;

	public:
	// Return whether a minimum validation limit has been set for supplied index
	bool hasValidationMin(int index);
	// Return validation minimum limit for supplied index
	int validationMin(int index);
	// Return whether a maximum validation limit has been set for supplied index
	bool hasValidationMax(int index);
	// Return validation maximum limit for supplied index
	int validationMax(int index);
	// Validate supplied value
	bool isValid(Vec3<int> value);
	// Validate supplied single
	bool isValid(int index, int value);


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
	// Return value (as Vec3<int>)
	Vec3<int> asVec3Int();
	// Return value (as Vec3<double>)
	Vec3<double> asVec3Double();
};

#endif

