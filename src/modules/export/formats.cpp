/*
	*** Export Module - Formats
	*** src/modules/export/formats.cpp
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

#include "modules/export/formats.h"
#include "base/sysfunc.h"

/*
 * Coordinate Formats
 */

// Coordinate Type Keywords
const char* CoordinateExportFormatKeywords[] = { "xyz", "dlpoly" };
const char* NiceCoordinateExportFormatKeywords[] = { "XYZ", "DL_POLY" };

// Return number of available formats
int CoordinateExportFileFormat::nFormats() const
{
	return CoordinateExportFileFormat::nCoordinateExportFormats;
}

// Return formats array
const char** CoordinateExportFileFormat::formats() const
{
	return CoordinateExportFormatKeywords;
}

// Return nice formats array
const char** CoordinateExportFileFormat::niceFormats() const
{
	return NiceCoordinateExportFormatKeywords;
}

// Return current format as CoordinateExportFormat
CoordinateExportFileFormat::CoordinateExportFormat CoordinateExportFileFormat::coordinateFormat() const
{
	return (CoordinateExportFileFormat::CoordinateExportFormat) format_;
}

