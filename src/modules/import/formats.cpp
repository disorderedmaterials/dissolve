/*
	*** Import Module - Formats
	*** src/modules/import/formats.cpp
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

#include "modules/import/formats.h"
#include "base/sysfunc.h"

/*
 * Coordinate Formats
 */

// Coordinate Type Keywords
const char* CoordinateImportFormatKeywords[] = { "xyz", "dlpoly", "epsr" };
const char* NiceCoordinateImportFormatKeywords[] = { "XYZ", "DL_POLY", "EPSR (ATO)" };

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
 * Force Formats
 */

// Force Type Keywords
const char* ForcesFormatKeywords[] = { "xyz", "dlpoly" };
const char* NiceForcesFormatKeywords[] = { "XYZ", "DL_POLY" };

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
 * Trajectory Formats
 */

// Trajectory Type Keywords
const char* TrajectoryImportFormatKeywords[] = { "xyz" };
const char* NiceTrajectoryImportFormatKeywords[] = { "XYZ" };

// Return number of available formats
int TrajectoryImportFileFormat::nFormats() const
{
	return TrajectoryImportFileFormat::nTrajectoryImportFormats;
}

// Return formats array
const char** TrajectoryImportFileFormat::formats() const
{
	return TrajectoryImportFormatKeywords;
}

// Return nice formats array
const char** TrajectoryImportFileFormat::niceFormats() const
{
	return NiceTrajectoryImportFormatKeywords;
}

// Return current format as TrajectoryImportFormat
TrajectoryImportFileFormat::TrajectoryImportFormat TrajectoryImportFileFormat::trajectoryFormat() const
{
	return (TrajectoryImportFileFormat::TrajectoryImportFormat) format_;
}

/*
 * Data1D Formats
 */

// Data1D Type Keywords
const char* Data1DImportFormatKeywords[] = { "xy", "histogram", "mint" };
const char* NiceData1DImportFormatKeywords[] = { "Simple XY data (x = bin centres)", "Histogrammed Data (x = bin left-boundaries)", "Gudrun output (mint01)" };

// Return number of available formats
int Data1DImportFileFormat::nFormats() const
{
	return Data1DImportFileFormat::nData1DImportFormats;
}

// Return formats array
const char** Data1DImportFileFormat::formats() const
{
	return Data1DImportFormatKeywords;
}

// Return nice formats array
const char** Data1DImportFileFormat::niceFormats() const
{
	return NiceData1DImportFormatKeywords;
}

// Return current format as Data1DImportFormat
Data1DImportFileFormat::Data1DImportFormat Data1DImportFileFormat::data1DFormat() const
{
	return (Data1DImportFileFormat::Data1DImportFormat) format_;
}

// Constructor
Data1DImportFileFormat::Data1DImportFileFormat()
{
	xColumn_ = 0;
	yColumn_ = 1;
	errorColumn_ = -1;
}

// Destructor
Data1DImportFileFormat::~Data1DImportFileFormat()
{
}

/*
 * Column Designation
 */

// Return column used for x values
int Data1DImportFileFormat::xColumn() const
{
	return xColumn_;
}

// Return column used for y values
int Data1DImportFileFormat::yColumn() const
{
	return yColumn_;
}

// Return column used for errors
int Data1DImportFileFormat::errorColumn() const
{
	return errorColumn_;
}


/*
 * Read / Write
 */

// Parse additional argument
bool Data1DImportFileFormat::parseArgument(const char* arg)
{
	// Split arg into parts before and after the '='
	CharString key = DissolveSys::beforeChar(arg, '=');
	CharString value = DissolveSys::afterChar(arg, '=');
	if ((key == "x") || (key == "X")) xColumn_ = value.asInteger() - 1;
	else if ((key == "y") || (key == "Y")) yColumn_ = value.asInteger() - 1;
	else if ((key == "e") || (key == "E")) errorColumn_ = value.asInteger() - 1;
	else return false;

	return true;
}

// Return whether this file/format has any additional arguments to write
bool Data1DImportFileFormat::hasAdditionalArguments() const
{
	return true;
}

// Return additional arguments as string
const char* Data1DImportFileFormat::additionalArguments() const
{
	static CharString args;

	args.clear();
	args.sprintf("x=%i  y=%i  e=%i", xColumn_+1, yColumn_+1, errorColumn_+1);

	return args.get();
}
