/*
	*** Export - Data1D
	*** src/io/export/data1d.cpp
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

#include "io/export/data1d.h"
#include "math/data1d.h"
#include "base/lineparser.h"
#include "base/sysfunc.h"

// Data1D Export Keywords
const char* Data1DExportFormatKeywords[] = { "xy" };
const char* NiceData1DExportFormatKeywords[] = { "XY Data" };

// Return number of available formats
int Data1DExportFileFormat::nFormats() const
{
	return Data1DExportFileFormat::nData1DExportFormats;
}

// Return formats array
const char** Data1DExportFileFormat::formats() const
{
	return Data1DExportFormatKeywords;
}

// Return nice formats array
const char** Data1DExportFileFormat::niceFormats() const
{
	return NiceData1DExportFormatKeywords;
}

// Return current format as CoordinateExportFormat
Data1DExportFileFormat::Data1DExportFormat Data1DExportFileFormat::data1DFormat() const
{
	return (Data1DExportFileFormat::Data1DExportFormat) format_;
}

// Constructor
Data1DExportFileFormat::Data1DExportFileFormat(const char* filename, Data1DExportFormat format) : FileAndFormat(filename, format)
{
}

/*
 * Export Functions
 */

// Export Data1D as simple XY (or XYE) data
bool Data1DExportFileFormat::exportXY(LineParser& parser, Data1D& data)
{
	const Array<double>& x = data.constXAxis();
	const Array<double>& values = data.constValues();
	if (data.valuesHaveErrors())
	{
		const Array<double>& errors = data.constErrors();
		for (int n = 0; n<x.nItems(); ++n) if (!parser.writeLineF("%16.10e  %16.10e  %16.10e\n", x.constAt(n), values.constAt(n), errors.constAt(n))) return false;
	}
	else for (int n = 0; n<x.nItems(); ++n) if (!parser.writeLineF("%16.10e  %16.10e\n", x.constAt(n), values.constAt(n))) return false;

	return true;
}

// Export Data1D using current filename and format
bool Data1DExportFileFormat::exportData(Data1D& data)
{
	// Open the file
	LineParser parser;
	if (!parser.openOutput(filename_))
	{
		parser.closeFiles();
		return false;
	}

	// Write data
	bool result = false;
	if (data1DFormat() == Data1DExportFileFormat::XYData) result = exportXY(parser, data);
	else Messenger::error("Unrecognised data format.\nKnown formats are: %s.\n", Data1DExportFileFormat().formats());

	return result;
}
