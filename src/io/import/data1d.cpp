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
	xColumn_ = 0;
	yColumn_ = 1;
	errorColumn_ = -1;
	removeAverageLevelXMin_ = -1.0;
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
 * Additional Options
 */

// Return enum option info for AdditionalOption
EnumOptions<Data1DImportFileFormat::AdditionalOption> Data1DImportFileFormat::additionalOptions()
{
	static EnumOptionsList AdditionalOptionOptions = EnumOptionsList() <<
		EnumOption(Data1DImportFileFormat::ErrorColumnOption, 	"e",			1) <<
		EnumOption(Data1DImportFileFormat::RemoveAverageOption, 	"removeaverage",	1) <<
		EnumOption(Data1DImportFileFormat::XColumnOption, 		"x",			1) <<
		EnumOption(Data1DImportFileFormat::YColumnOption, 		"y",			1);

	static EnumOptions<Data1DImportFileFormat::AdditionalOption> options("Data1DImportOption", AdditionalOptionOptions);

	return options;
}

// Parse additional option
bool Data1DImportFileFormat::parseOption(const char* arg)
{
	// Split arg into parts before and after the '=', and parse the value string into comma-delimited parts
	CharString key = DissolveSys::beforeChar(arg, '=');
	CharString value = DissolveSys::afterChar(arg, '=');
	LineParser valueParser;
	valueParser.getArgsDelim(LineParser::CommasAreDelimiters, value.get());

	// Check if we have a valid key and, if so, have appropriately been provided a value
	if (!additionalOptions().isValid(key)) return additionalOptions().errorAndPrintValid(key);
	AdditionalOption aa = additionalOptions().enumeration(key);
	if (!additionalOptions().validNArgs(aa, valueParser.nArgs())) return false;

	// Act on the option
	switch (aa)
	{
		case (Data1DImportFileFormat::ErrorColumnOption):
			errorColumn_ = value.asInteger() - 1;
			break;
		case (Data1DImportFileFormat::RemoveAverageOption):
			removeAverageLevelXMin_ = value.asDouble();
			break;
		case (Data1DImportFileFormat::XColumnOption):
			xColumn_ = value.asInteger() - 1;
			break;
		case (Data1DImportFileFormat::YColumnOption):
			yColumn_ = value.asInteger() - 1;
			break;
		default:
			return Messenger::error("Epic Developer Fail - Don't know how to deal with the %s additional option.\n", key.get());
			break;
	}

	return true;
}

// Return whether this file/format has any additional options to write
bool Data1DImportFileFormat::hasAdditionalOptions() const
{
	return true;
}

// Return additional options as string
const char* Data1DImportFileFormat::additionalOptionsAsString() const
{
	static CharString args;

	args.clear();
	if (xColumn_ != 0) args.strcatf(" x=%i", xColumn_+1);
	if (yColumn_ != 1) args.strcatf(" y=%i", yColumn_+1);
	if (errorColumn_ != -1) args.strcatf(" e=%i", errorColumn_+1);
	if (removeAverageLevelXMin_ > 0.0) args.strcatf(" removeaverage=%f", removeAverageLevelXMin_);

	return args.get();
}

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
	if (removeAverageLevelXMin_ > 0.0)
	{
		double level = Filters::subtractAverage(data, removeAverageLevelXMin_);
		Messenger::print("Removed average level of %f from data, forming average over x >= %f.\n", level, removeAverageLevelXMin_);
	}

	return result;
}
