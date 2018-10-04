/*
	*** Export Module - Data2D Functions
	*** src/modules/export/functions_data2d.cpp
	Copyright T. Youngs 2012-2018

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

#include "modules/export/export.h"
#include "math/data2d.h"
#include "base/lineparser.h"

// Write Data2D in specified format
bool ExportModule::writeData2D(Data2DExportFileFormat::Data2DExportFormat format, LineParser& parser, Data2D& data)
{
	// Check supplied format
	if (format == Data2DExportFileFormat::BlockData) return writeBlockData2D(parser, data);
// 	else if (format == CoordinateExportFileFormat::DLPOLYCoordinates) return writeConfigurationDLPOLY(parser, cfg);

	Messenger::error("Unrecognised data format - '%s'.\nKnown formats are: %s.\n", format, Data2DExportFileFormat().formats());
	return false;
}

// Write Data2D in specified format to file
bool ExportModule::writeData2D(Data2DExportFileFormat::Data2DExportFormat format, const char* filename, Data2D& data)
{
	// Open the file
	LineParser parser;
	if (!parser.openOutput(filename))
	{
		parser.closeFiles();
		return false;
	}

	bool result = writeData2D(format, parser, data);

	parser.closeFiles();

	return result;
}

// Write Data2D as simple block data
bool ExportModule::writeBlockData2D(LineParser& parser, Data2D& data)
{
	// Write header comment
	if (!parser.writeLineF("# %i blocks (nX) of %i points (nY).\n", data.xAxis().nItems(), data.yAxis().nItems())) return false;

	// Write datapoints, separating each block of a specific x value with a single blank line
	const Array2D<double>& values = data.constValues2D();
	for (int x=0; x<values.nRows(); ++x)
	{
		for (int y=0; y<values.nColumns(); ++y) if (!parser.writeLineF("%15.9f\n", values.constAt(x,y))) return false;
		if (!parser.writeLineF("\n")) return false;
	}

	return true;
}
