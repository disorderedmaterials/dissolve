/*
	*** Import Module - Formats
	*** src/modules/import/formats.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/import/formats.h"
#include "base/sysfunc.h"

/*
 * Coordinate Formats
 */

// Coordinate Type Keywords
const char* CoordinateFormatKeywords[] = { "xyz", "dlpoly", "epsr" };
const char* NiceCoordinateFormatKeywords[] = { "XYZ", "DL_POLY", "EPSR (ATO)" };

// Convert text string to CoordinateFormat
ImportModuleFormats::CoordinateFormat ImportModuleFormats::coordinateFormat(const char* s)
{
	for (int n=0; n<ImportModuleFormats::nCoordinateFormats; ++n) if (DissolveSys::sameString(s,CoordinateFormatKeywords[n])) return (ImportModuleFormats::CoordinateFormat) n;
	return ImportModuleFormats::nCoordinateFormats;
}

// Convert CoordinateFormat to text string
const char* ImportModuleFormats::coordinateFormat(ImportModuleFormats::CoordinateFormat id)
{
	return CoordinateFormatKeywords[id];
}

// Return CoordinateFormats array
const char** ImportModuleFormats::niceCoordinateFormats()
{
	return NiceCoordinateFormatKeywords;
}


/*
 * Force Formats
 */

// Force Type Keywords
const char* ForceFormatKeywords[] = { "xyz", "dlpoly" };
const char* NiceForceFormatKeywords[] = { "XYZ", "DL_POLY" };

// Convert text string to ForceFormat
ImportModuleFormats::ForceFormat ImportModuleFormats::forceFormat(const char* s)
{
	for (int n=0; n<ImportModuleFormats::nForceFormats; ++n) if (DissolveSys::sameString(s,ForceFormatKeywords[n])) return (ImportModuleFormats::ForceFormat) n;
	return ImportModuleFormats::nForceFormats;
}

// Convert ForceFormat to text string
const char* ImportModuleFormats::forceFormat(ImportModuleFormats::ForceFormat id)
{
	return ForceFormatKeywords[id];
}

// Return ForceFormats array
const char** ImportModuleFormats::niceForceFormats()
{
	return NiceForceFormatKeywords;
}
