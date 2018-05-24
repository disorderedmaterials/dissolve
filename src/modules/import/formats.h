/*
	*** Import Module - Formats
	*** src/modules/import/formats.h
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

#ifndef DISSOLVE_IMPORTMODULEFORMATS_H
#define DISSOLVE_IMPORTMODULEFORMATS_H

// Forward Declarations
/* none */

namespace ImportModuleFormats
{
	/*
	 * Coordinate Formats
	 */
	enum CoordinateFormat { XYZCoordinates, DLPOLYCoordinates, EPSRCoordinates, nCoordinateFormats};
	// Convert text string to CoordinateFormat
	CoordinateFormat coordinateFormat(const char* s);
	// Convert CoordinateFormat to text string
	const char* coordinateFormat(CoordinateFormat id);
	// Return nice CoordinateFormat array
	const char** niceCoordinateFormats();


	/*
	 * Forces Formats
	 */
	enum ForceFormat { XYZForces, DLPOLYForces, nForceFormats};
	// Convert text string to ForceFormat
	ForceFormat forceFormat(const char* s);
	// Convert ForceFormat to text string
	const char* forceFormat(ForceFormat id);
	// Return nice ForceFormat array
	const char** niceForceFormats();
};

#endif

