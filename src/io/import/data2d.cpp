/*
	*** Import - Data2D
	*** src/io/import/data2d.cpp
	Copyright T. Youngs 2012-2020

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

#include "io/import/data2d.h"
#include "keywords/types.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Data2D Type Keywords
const char* Data2DImportFormatKeywords[] = { "cartesian" };
const char* NiceData2DImportFormatKeywords[] = { "Cartesian X,Y,f(X,Y) data" };

// Constructor
Data2DImportFileFormat::Data2DImportFileFormat(Data2DImportFormat format) : FileAndFormat(format)
{
	keywords_.add("Ranges", new Vec3DoubleKeyword(Vec3<double>(0.0,0.0,0.0)), "XAxis", "Min, max, and delta to assume for x axis");
	keywords_.add("Ranges", new Vec3DoubleKeyword(Vec3<double>(0.0,0.0,0.0)), "YAxis", "Min, max, and delta to assume for y axis");
}

// Destructor
Data2DImportFileFormat::~Data2DImportFileFormat()
{
}

/*
 * Format Access
 */

// Return number of available formats
int Data2DImportFileFormat::nFormats() const
{
	return Data2DImportFileFormat::nData2DImportFormats;
}

// Return formats array
const char** Data2DImportFileFormat::formats() const
{
	return Data2DImportFormatKeywords;
}

// Return nice formats array
const char** Data2DImportFileFormat::niceFormats() const
{
	return NiceData2DImportFormatKeywords;
}

// Return current format as Data2DImportFormat
Data2DImportFileFormat::Data2DImportFormat Data2DImportFileFormat::data2DFormat() const
{
	return (Data2DImportFileFormat::Data2DImportFormat) format_;
}

/*
 * Import Functions
 */

// Import Data2D using current filename and format
bool Data2DImportFileFormat::importData(Data2D& data, ProcessPool* procPool)
{
	// Open file and check that we're OK to proceed importing from it
	LineParser parser(procPool);
	if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open file '%s' for loading Data2D data.\n", filename_.get());

	// Import the data
	bool result = importData(parser, data);

	parser.closeFiles();

	return result;
}

// Import Data2D using supplied parser and current format
bool Data2DImportFileFormat::importData(LineParser& parser, Data2D& data)
{
	// Import the data
	bool result = false;
	if (data2DFormat() == Data2DImportFileFormat::CartesianData2D) result = importCartesian(parser, data);
	else Messenger::error("Don't know how to load Data2D of format '%s'.\n", Data2DImportFileFormat().format(data2DFormat()));

	return result;
}
