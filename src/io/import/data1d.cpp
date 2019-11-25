/*
	*** Import - Data1D
	*** src/io/import/data1d.cpp
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

#include "io/import/data1d.h"
#include "math/filters.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Data1D Type Keywords
const char* Data1DImportFormatKeywords[] = { "xy", "histogram", "mint" };
const char* NiceData1DImportFormatKeywords[] = { "Simple XY data (x = bin centres)", "Histogrammed Data (x = bin left-boundaries)", "Gudrun output (mint01)" };

// Constructor
Data1DImportFileFormat::Data1DImportFileFormat(Data1DImportFileFormat::Data1DImportFormat format) : FileAndFormat(format)
{
	keywords_.add("Columns", new IntegerKeyword(1, 1), "X", "Column index to use for x values");
	keywords_.add("Columns", new IntegerKeyword(2, 1), "Y", "Column index to use for y values");
	keywords_.add("Columns", new IntegerKeyword(0, 0), "Error", "Column index to use for error values");
	keywords_.add("Manipulations", new DoubleKeyword(-1.0, -1.0), "RemoveAverage", "X axis value from which to form average value to subtract from data (-1 for no subtraction)");
}

// Destructor
Data1DImportFileFormat::~Data1DImportFileFormat()
{
}

/*
 * Format Access
 */

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

/*
 * Import Functions
 */

// Import Data1D using current filename and format
bool Data1DImportFileFormat::importData(Data1D& data, ProcessPool* procPool)
{
	// Open file and check that we're OK to proceed importing from it
	LineParser parser(procPool);
	if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open file '%s' for loading Data1D data.\n", filename_.get());

	// Import the data
	bool result = importData(parser, data);

	parser.closeFiles();

	return result;
}

// Import Data1D using supplied parser and current format
bool Data1DImportFileFormat::importData(LineParser& parser, Data1D& data)
{
	// Import the data
	bool result = false;
	if (data1DFormat() == Data1DImportFileFormat::XYData1D) result = importXY(parser, data);
	else if (data1DFormat() == Data1DImportFileFormat::HistogramData1D) result = importHistogram(parser, data);
	else if (data1DFormat() == Data1DImportFileFormat::GudrunMintData1D) result = importGudrunMint(parser, data);
	else Messenger::error("Don't know how to load Data1D of format '%s'.\n", Data1DImportFileFormat().format(data1DFormat()));

	// If we failed, may as well return now
	if (!result) return false;

	// Handle any additional options
	// --Subtract average level from data?
	const double removeAverage = keywords_.asDouble("RemoveAverage");
	if (removeAverage > 0.0)
	{
		double level = Filters::subtractAverage(data, removeAverage);
		Messenger::print("Removed average level of %f from data, forming average over x >= %f.\n", level, removeAverage);
	}

	return result;
}
