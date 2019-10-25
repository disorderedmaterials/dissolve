/*
	*** Import - Forces
	*** src/io/import/forces.cpp
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

#include "io/import/forces.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Force Type Keywords
const char* ForcesFormatKeywords[] = { "xyz", "dlpoly" };
const char* NiceForcesFormatKeywords[] = { "XYZ", "DL_POLY" };


// Constructor
ForceImportFileFormat::ForceImportFileFormat(ForceImportFormat format) : FileAndFormat(format)
{
}

// Destructor
ForceImportFileFormat::~ForceImportFileFormat()
{
}

/*
 * Format Access
 */

// Return number of available formats
int ForceImportFileFormat::nFormats() const
{
	return ForceImportFileFormat::nForceImportFormats;
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

// Return current format as ForceImportFormat
ForceImportFileFormat::ForceImportFormat ForceImportFileFormat::forceFormat() const
{
	return (ForceImportFileFormat::ForceImportFormat) format_;
}

/*
 * Import Functions
 */

// Read forces using current filename and format
bool ForceImportFileFormat::importData(Array<double>& fx, Array<double>& fy, Array<double>& fz, ProcessPool* procPool)
{
	// Open file and check that we're OK to proceed importing from it
	LineParser parser(procPool);
	if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open file '%s' for loading Data2D data.\n", filename_.get());

	// Import the data
	bool result = importData(parser, fx, fy, fz);

	parser.closeFiles();

	return result;
}

// Import forces using supplied parser and current format
bool ForceImportFileFormat::importData(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz)
{
	// Import the data
	bool result = false;
	if (forceFormat() == ForceImportFileFormat::XYZForces) result = importXYZ(parser, fx, fy, fz);
	else if (forceFormat() == ForceImportFileFormat::DLPOLYForces) return importDLPOLY(parser, fx, fy, fz);
	else Messenger::error("Don't know how to load forces in format '%s'.\n", ForceImportFileFormat().format(forceFormat()));

	return result;
}
