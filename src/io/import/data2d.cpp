/*
	*** Import - Data2D
	*** src/io/import/data2d.cpp
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

#include "io/import/data2d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Data2D Type Keywords
const char* Data2DImportFormatKeywords[] = { "cartesian" };
const char* NiceData2DImportFormatKeywords[] = { "Cartesian X,Y,f(X,Y) data" };

// Constructor
Data2DImportFileFormat::Data2DImportFileFormat(Data2DImportFormat format) : FileAndFormat(format)
{
	axisRangeSet_.set(false, false, false);
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
 * Additional Options
 */

// Return enum option info for AdditionalOption
EnumOptions<Data2DImportFileFormat::AdditionalOption> Data2DImportFileFormat::additionalOptions()
{
	static EnumOptionsList AdditionalOptionOptions = EnumOptionsList() <<
		EnumOption(Data2DImportFileFormat::XAxisOption, 	"xaxis",	3) <<
		EnumOption(Data2DImportFileFormat::YAxisOption, 	"yaxis",	3);

	static EnumOptions<Data2DImportFileFormat::AdditionalOption> options("Data2DImportOption", AdditionalOptionOptions);

	return options;
}

// Parse additional option
bool Data2DImportFileFormat::parseOption(const char* arg)
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
		case (Data2DImportFileFormat::XAxisOption):
			axisMinimum_.x = valueParser.argd(0);
			axisMaximum_.x = valueParser.argd(1);
			axisDelta_.x = valueParser.argd(2);
			axisRangeSet_.x = true;
			break;
		case (Data2DImportFileFormat::YAxisOption):
			axisMinimum_.y = valueParser.argd(0);
			axisMaximum_.y = valueParser.argd(1);
			axisDelta_.y = valueParser.argd(2);
			axisRangeSet_.y = true;
			break;
		default:
			return Messenger::error("Epic Developer Fail - Don't know how to deal with the %s additional option.\n", key.get());
			break;
	}

	return true;
}

// Return whether this file/format has any additional options to write
bool Data2DImportFileFormat::hasAdditionalOptions() const
{
	return true;
}

// Return additional options as string
const char* Data2DImportFileFormat::additionalOptionsAsString() const
{
	static CharString args;

	args.clear();
	for (int n=0; n<2; ++n) if (axisRangeSet_.get(n)) args.strcatf(" %caxis=%f,%f,%f", 120+n, axisMinimum_.get(n), axisMaximum_.get(n), axisDelta_.get(n));

	return args.get();
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
