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
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Data1D Type Keywords
const char* Data1DImportFormatKeywords[] = { "xy", "histogram", "mint" };
const char* NiceData1DImportFormatKeywords[] = { "Simple XY data (x = bin centres)", "Histogrammed Data (x = bin left-boundaries)", "Gudrun output (mint01)" };

// Constructor
Data1DImportFileFormat::Data1DImportFileFormat(Data1DImportFileFormat::Data1DImportFormat format) : FileAndFormat(format)
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
 * Import / Write
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
	else return Messenger::error("Unrecognised or badly formatted additional argument '%s' found for Data1DImportFileFormat.\n", arg);

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

	return result;
}
