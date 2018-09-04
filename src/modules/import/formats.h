/*
	*** Import Module - Formats
	*** src/modules/import/formats.h
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

#ifndef DISSOLVE_IMPORTMODULEFORMATS_H
#define DISSOLVE_IMPORTMODULEFORMATS_H

#include "base/fileandformat.h"

// Forward Declarations
/* none */

// Coordinate Import Formats
class CoordinateImportFileFormat : public FileAndFormat
{
	public:
	// Available coordinate formats
 	enum CoordinateFormat { XYZCoordinates, DLPOLYCoordinates, EPSRCoordinates, nCoordinateFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as CoordinateFormat
	CoordinateFormat coordinateFormat() const;
};

// Forces Import Formats
class ForceImportFileFormat : public FileAndFormat
{
	public:
	// Available forces formats
 	enum ForceFormat { XYZForces, DLPOLYForces, nForceFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as ForceFormat
	ForceFormat forceFormat() const;
};

// Trajectory Import Formats
class TrajectoryImportFileFormat : public FileAndFormat
{
	public:
	// Available forces formats
 	enum TrajectoryFormat { XYZTrajectory, nTrajectoryFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as TrajectoryFormat
	TrajectoryFormat trajectoryFormat() const;
};

#endif

