/*
	*** Import Module - Data2D Functions
	*** src/modules/import/functions_data2d.cpp
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

#include "modules/import/import.h"
#include "math/data2d.h"
#include "math/filters.h"
#include "base/lineparser.h"

// Read Data2D in specified format
bool ImportModule::readData2D(Data2DImportFileFormat::Data2DImportFormat format, LineParser& parser, Data2D& data)
{
	// Check supplied format
	if (format == Data2DImportFileFormat::CartesianData2D) return readCartesianData2D(parser, data);

	Messenger::error("Don't know how to load Data2D of format '%s'.\n", Data2DImportFileFormat().format(format));

	return false;
}

// Read cartesian XYV data from specified file
bool ImportModule::readCartesianData2D(LineParser& parser, Data2D& data)
{
	return data.load(parser);
}

