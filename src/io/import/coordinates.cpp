/*
	*** Import - Coordinate
	*** src/io/import/coordinates.cpp
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

#include "io/import/coordinates.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Coordinate Type Keywords
const char* CoordinateImportFormatKeywords[] = { "xyz", "dlpoly", "epsr" };
const char* NiceCoordinateImportFormatKeywords[] = { "XYZ", "DL_POLY", "EPSR (ATO)" };

// Constructor
CoordinateImportFileFormat::CoordinateImportFileFormat(CoordinateImportFormat format) : FileAndFormat(format)
{
}

// Destructor
CoordinateImportFileFormat::~CoordinateImportFileFormat()
{
}

/*
 * Format Access
 */

// Return number of available formats
int CoordinateImportFileFormat::nFormats() const
{
	return CoordinateImportFileFormat::nCoordinateImportFormats;
}

// Return formats array
const char** CoordinateImportFileFormat::formats() const
{
	return CoordinateImportFormatKeywords;
}

// Return nice formats array
const char** CoordinateImportFileFormat::niceFormats() const
{
	return NiceCoordinateImportFormatKeywords;
}

// Return current format as CoordinateImportFormat
CoordinateImportFileFormat::CoordinateImportFormat CoordinateImportFileFormat::coordinateFormat() const
{
	return (CoordinateImportFileFormat::CoordinateImportFormat) format_;
}

/*
 * Import Functions
 */

// Import coordinates using current filename and format
bool CoordinateImportFileFormat::importData(Array< Vec3<double> >& r, ProcessPool* procPool)
{
	// Open file and check that we're OK to proceed importing from it
	LineParser parser(procPool);
	if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open file '%s' for loading Data2D data.\n", filename_.get());

	// Import the data
	bool result = importData(parser, r);

	parser.closeFiles();

	return result;
}

// Import coordinates using supplied parser and current format
bool CoordinateImportFileFormat::importData(LineParser& parser, Array< Vec3<double> >& r)
{
	// Import the data
	bool result = false;
	if (coordinateFormat() == CoordinateImportFileFormat::XYZCoordinates) result = importXYZ(parser, r);
	else if (coordinateFormat() == CoordinateImportFileFormat::DLPOLYCoordinates) result = importDLPOLY(parser, r);
	else if (coordinateFormat() == CoordinateImportFileFormat::EPSRCoordinates) result = importEPSR(parser, r);
	else Messenger::error("Don't know how to load coordinates in format '%s'.\n", CoordinateImportFileFormat().format(coordinateFormat()));

	return result;
}
