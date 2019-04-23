/*
	*** Import Module - Coordinates Functions
	*** src/modules/import/functions_coordinates.cpp
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
#include "base/lineparser.h"

// Read coordinates in specified format
bool ImportModule::readCoordinates(CoordinateImportFileFormat::CoordinateImportFormat format, LineParser& parser, Array< Vec3<double> >& r)
{
	// Check supplied format
	if (format == CoordinateImportFileFormat::XYZCoordinates) return readXYZCoordinates(parser, r);
	else if (format == CoordinateImportFileFormat::DLPOLYCoordinates) return readDLPOLYCoordinates(parser, r);
	else if (format == CoordinateImportFileFormat::EPSRCoordinates) return readEPSRCoordinates(parser, r);

	Messenger::error("Unrecognised coordinate format - '%s'.\nKnown formats are: %s.\n", format, CoordinateImportFileFormat().formats());
	return false;
}

