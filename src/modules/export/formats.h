/*
	*** Export Module - Formats
	*** src/modules/export/formats.h
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

#ifndef DISSOLVE_EXPORTMODULEFORMATS_H
#define DISSOLVE_EXPORTMODULEFORMATS_H

#include "base/fileandformat.h"

// Forward Declarations
/* none */

// Coordinate Export Formats
class CoordinateExportFileFormat : public FileAndFormat
{
	/*
	 * Available Formats
	 */
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


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return false;
	}
};

// Data2D Export Formats
class Data2DExportFileFormat : public FileAndFormat
{
	/*
	 * Available Formats
	 */
	public:
	// Available data formats
	enum Data2DExportFormat { BlockData, nData2DExportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as Data2DExportFormat
	Data2DExportFormat data2DFormat() const;


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return false;
	}
};

// PairPotential Export Formats
class PairPotentialExportFileFormat : public FileAndFormat
{
	/*
	 * Available Formats
	 */
	public:
	// Available data formats
	enum PairPotentialExportFormat { BlockPairPotential, DLPOLYTABLEPairPotential, nPairPotentialExportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as PairPotentialExportFormat
	PairPotentialExportFormat pairPotentialFormat() const;


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return false;
	}
};

// Trajectory Export Formats
class TrajectoryExportFileFormat : public FileAndFormat
{
	/*
	 * Available Formats
	 */
	public:
	// Available trajectory formats
	enum TrajectoryExportFormat { XYZTrajectory, nTrajectoryExportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as TrajectoryExportFormat
	TrajectoryExportFormat trajectoryFormat() const;


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return false;
	}
};

#endif

