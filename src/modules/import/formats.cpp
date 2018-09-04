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

// Return number of available formats
int CoordinateImportFileFormat::nFormats() const
{
	return CoordinateImportFileFormat::nCoordinateFormats;
}

// Return formats array
const char** CoordinateImportFileFormat::formats() const
{
	return CoordinateFormatKeywords;
}

// Return nice formats array
const char** CoordinateImportFileFormat::niceFormats() const
{
	return NiceCoordinateFormatKeywords;
}

// Return current format as CoordinateFormat
CoordinateImportFileFormat::CoordinateFormat CoordinateImportFileFormat::coordinateFormat() const
{
	return (CoordinateImportFileFormat::CoordinateFormat) format_;
}

/*
 * Force Formats
 */

// Force Type Keywords
const char* ForcesFormatKeywords[] = { "xyz", "dlpoly" };
const char* NiceForcesFormatKeywords[] = { "XYZ", "DL_POLY" };

// Return number of available formats
int ForceImportFileFormat::nFormats() const
{
	return ForceImportFileFormat::nForceFormats;
}

// Return formats array
const char** ForceImportFileFormat::formats() const
{
	return ForcesFormatKeywords;
}

// Return nice formats array
const char** ForceImportFileFormat::niceFormats() const
{
	return NiceForcesFormatKeywords;
}

// Return current format as ForceFormat
ForceImportFileFormat::ForceFormat ForceImportFileFormat::forceFormat() const
{
	return (ForceImportFileFormat::ForceFormat) format_;
}

/*
 * Trajectory Formats
 */

// Trajectory Type Keywords
const char* TrajectoryFormatKeywords[] = { "xyz" };
const char* NiceTrajectoryFormatKeywords[] = { "XYZ" };

// Return number of available formats
int TrajectoryImportFileFormat::nFormats() const
{
	return TrajectoryImportFileFormat::nTrajectoryFormats;
}

// Return formats array
const char** TrajectoryImportFileFormat::formats() const
{
	return TrajectoryFormatKeywords;
}

// Return nice formats array
const char** TrajectoryImportFileFormat::niceFormats() const
{
	return NiceTrajectoryFormatKeywords;
}

// Return current format as TrajectoryFormat
TrajectoryImportFileFormat::TrajectoryFormat TrajectoryImportFileFormat::trajectoryFormat() const
{
	return (TrajectoryImportFileFormat::TrajectoryFormat) format_;
}
