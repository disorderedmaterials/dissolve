/*
	*** Import - Data3D
	*** src/io/import/data3d.cpp
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

#include "io/import/data3d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Data3D Type Keywords
const char* Data3DImportFormatKeywords[] = { "cartesian" };
const char* NiceData3DImportFormatKeywords[] = { "Cartesian X,Y,Z,f(x,y,z) data" };

// Constructor
Data3DImportFileFormat::Data3DImportFileFormat(Data3DImportFormat format) : FileAndFormat(format)
{
}

// Destructor
Data3DImportFileFormat::~Data3DImportFileFormat()
{
}

/*
 * Format Access
 */

// Return number of available formats
int Data3DImportFileFormat::nFormats() const
{
	return Data3DImportFileFormat::nData3DImportFormats;
}

// Return formats array
const char** Data3DImportFileFormat::formats() const
{
	return Data3DImportFormatKeywords;
}

// Return nice formats array
const char** Data3DImportFileFormat::niceFormats() const
{
	return NiceData3DImportFormatKeywords;
}

// Return current format as Data3DImportFormat
Data3DImportFileFormat::Data3DImportFormat Data3DImportFileFormat::data3DFormat() const
{
	return (Data3DImportFileFormat::Data3DImportFormat) format_;
}

/*
 * Import / Write
 */

// Parse additional argument
bool Data3DImportFileFormat::parseArgument(const char* arg)
{
	// Split arg into parts before and after the '='
	CharString key = DissolveSys::beforeChar(arg, '=');
	CharString value = DissolveSys::afterChar(arg, '=');
	if (key == "template") templateSourceObjectTag_ = value;
	else return false;

	return true;
}

// Return whether this file/format has any additional arguments to write
bool Data3DImportFileFormat::hasAdditionalArguments() const
{
	return true;
}

// Return additional arguments as string
const char* Data3DImportFileFormat::additionalArguments() const
{
	static CharString args;

	args.clear();
	if (!templateSourceObjectTag_.isEmpty()) args.sprintf("template='%s'", templateSourceObjectTag_.get());

	return args.get();
}

/*
 * Import Functions
 */

// Import Data3D using current filename and format
bool Data3DImportFileFormat::importData(Data3D& data, ProcessPool* procPool)
{
	// Open file and check that we're OK to proceed importing from it
	LineParser parser(procPool);
	if ((!parser.openInput(filename_)) || (!parser.isFileGoodForReading())) return Messenger::error("Couldn't open file '%s' for loading Data3D data.\n", filename_.get());

	// Import the data
	bool result = importData(parser, data);

	parser.closeFiles();

	return result;
}

// Import Data3D using supplied parser and current format
bool Data3DImportFileFormat::importData(LineParser& parser, Data3D& data)
{
	// Import the data
	bool result = false;
// 	if (data3DFormat() == Data3DImportFileFormat::CartesianData3D) result = importCartesian(parser, data);
	Messenger::error("Don't know how to load Data3D of format '%s'.\n", Data3DImportFileFormat().format(data3DFormat()));

	return result;
}
