/*
	*** Export Module - Formats
	*** src/modules/export/formats.cpp
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

#include "modules/export/formats.h"
#include "base/sysfunc.h"

/*
 * Coordinate Formats
 */

// Coordinate Type Keywords
const char* CoordinateExportFormatKeywords[] = { "xyz", "dlpoly" };
const char* NiceCoordinateExportFormatKeywords[] = { "XYZ Coordinates", "DL_POLY CONFIG File" };

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

/*
 * Data2D Formats
 */

// Data2D Export Keywords
const char* Data2DExportFormatKeywords[] = { "block" };
const char* NiceData2DExportFormatKeywords[] = { "Block Data" };

// Return number of available formats
int Data2DExportFileFormat::nFormats() const
{
	return Data2DExportFileFormat::nData2DExportFormats;
}

// Return formats array
const char** Data2DExportFileFormat::formats() const
{
	return Data2DExportFormatKeywords;
}

// Return nice formats array
const char** Data2DExportFileFormat::niceFormats() const
{
	return NiceData2DExportFormatKeywords;
}

// Return current format as CoordinateExportFormat
Data2DExportFileFormat::Data2DExportFormat Data2DExportFileFormat::data2DFormat() const
{
	return (Data2DExportFileFormat::Data2DExportFormat) format_;
}

/*
 * PairPotential Formats
 */

// PairPotential Export Keywords
const char* PairPotentialExportFormatKeywords[] = { "block", "TABLE" };
const char* NicePairPotentialExportFormatKeywords[] = { "Block Data", "DL_POLY TABLE file" };

// Return number of available formats
int PairPotentialExportFileFormat::nFormats() const
{
	return PairPotentialExportFileFormat::nPairPotentialExportFormats;
}

// Return formats array
const char** PairPotentialExportFileFormat::formats() const
{
	return PairPotentialExportFormatKeywords;
}

// Return nice formats array
const char** PairPotentialExportFileFormat::niceFormats() const
{
	return NicePairPotentialExportFormatKeywords;
}

// Return current format as CoordinateExportFormat
PairPotentialExportFileFormat::PairPotentialExportFormat PairPotentialExportFileFormat::pairPotentialFormat() const
{
	return (PairPotentialExportFileFormat::PairPotentialExportFormat) format_;
}
