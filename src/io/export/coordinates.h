/*
	*** Export - Coordinates
	*** src/io/export/coordinates.h
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

#ifndef DISSOLVE_EXPORT_COORDINATES_H
#define DISSOLVE_EXPORT_COORDINATES_H

#include "io/fileandformat.h"

// Forward Declarations
class Configuration;

// Coordinate Export Formats
class CoordinateExportFileFormat : public FileAndFormat
{
	public:
	// Available coordinate formats
	enum CoordinateExportFormat { XYZCoordinates, DLPOLYCoordinates, nCoordinateExportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as CoordinateExportFormat
	CoordinateExportFormat coordinateFormat() const;
	// Constructor
	CoordinateExportFileFormat(const char* filename = NULL, CoordinateExportFormat format = XYZCoordinates);


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return false;
	}


	/*
	 * Export Functions
	 */
	private:
	// Export Configuration as XYZ
	bool exportXYZ(LineParser& parser, Configuration* cfg);
	// Export Configuration as DL_POLY CONFIG
	bool exportDLPOLY(LineParser& parser, Configuration* cfg);

	public:
	// Export Configuration using current filename and format
	bool exportData(Configuration* cfg);
};

#endif

