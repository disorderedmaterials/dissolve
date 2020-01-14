/*
	*** Import - Coordinates
	*** src/io/import/coordinates.h
	Copyright T. Youngs 2012-2020

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

#ifndef DISSOLVE_IMPORT_COORDINATES_H
#define DISSOLVE_IMPORT_COORDINATES_H

#include "io/fileandformat.h"
#include "templates/array.h"

// Forward Declarations
class ProcessPool;

// Coordinate Import Formats
class CoordinateImportFileFormat : public FileAndFormat
{
	public:
	// Coordinate Import Formats
	enum CoordinateImportFormat { XYZCoordinates, DLPOLYCoordinates, EPSRCoordinates, nCoordinateImportFormats };
	// Constructor
	CoordinateImportFileFormat(CoordinateImportFormat format = XYZCoordinates);
	// Destructor
	~CoordinateImportFileFormat();


	/*
	 * Format Access
	 */
	public:
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as CoordinateImportFormat
	CoordinateImportFormat coordinateFormat() const;


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return true;
	}


	/*
	 * Import Functions
	 */
	private:
	// Import DL_POLY coordinates through specified parser
	bool importDLPOLY(LineParser& parser, Array< Vec3<double> >& r);
	// Import EPSR ATO coordinates through specified parser
	bool importEPSR(LineParser& parser, Array< Vec3<double> >& r);
	// Import xyz coordinates through specified parser
	bool importXYZ(LineParser& parser, Array< Vec3<double> >& r);

	public:
	// Import coordinates using current filename and format
	bool importData(Array< Vec3<double> >& r, ProcessPool* procPool = NULL);
	// Import coordinates using supplied parser and current format
	bool importData(LineParser& parser, Array< Vec3<double> >& r);
};

#endif

