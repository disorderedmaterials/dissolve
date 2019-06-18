/*
	*** Import Module - Formats
	*** src/modules/import/formats.h
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

#ifndef DISSOLVE_IMPORTMODULEFORMATS_H
#define DISSOLVE_IMPORTMODULEFORMATS_H

#include "base/fileandformat.h"

// Forward Declarations
/* none */

// Coordinate Import Formats
class CoordinateImportFileFormat : public FileAndFormat
{
	/*
	 * Available Formats
	 */
	public:
	// Available coordinate formats
 	enum CoordinateImportFormat { XYZCoordinates, DLPOLYCoordinates, EPSRCoordinates, nCoordinateImportFormats };
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
};

// Forces Import Formats
class ForceImportFileFormat : public FileAndFormat
{
	/*
	 * Available Formats
	 */
	public:
	// Available forces formats
	enum ForceImportFormat { XYZForces, DLPOLYForces, nForceImportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as ForceImportFormat
	ForceImportFormat forceFormat() const;


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return true;
	}
};

// Trajectory Import Formats
class TrajectoryImportFileFormat : public FileAndFormat
{
	/*
	 * Available Formats
	 */
	public:
	// Available trajectory formats
	enum TrajectoryImportFormat { XYZTrajectory, nTrajectoryImportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as TrajectoryImportFormat
	TrajectoryImportFormat trajectoryFormat() const;


	/*
	 * Filename / Basename
	 */
	public:
	// Return whether the file must exist
	bool fileMustExist() const
	{
		return true;
	}
};

// Data1D Import Formats
class Data1DImportFileFormat : public FileAndFormat
{
	public:
	// Constructor
	Data1DImportFileFormat();
	// Destructor
	~Data1DImportFileFormat();


	/*
	 * Available Formats
	 */
	public:
	// Available Data1D formats
	enum Data1DImportFormat { XYData1D, HistogramData1D, GudrunMintData1D, nData1DImportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as Data1DImportFormat
	Data1DImportFormat data1DFormat() const;


	/*
	 * Column Designations
	 */
	private:
	// Column to use for x values
	int xColumn_;
	// Column to use for y values
	int yColumn_;
	// Column to use for errors
	int errorColumn_;
	
	public:
	// Return column used for x values
	int xColumn() const;
	// Return column used for y values
	int yColumn() const;
	// Return column used for errors
	int errorColumn() const;


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
	 * Read / Write
	 */
	protected:
	// Parse additional argument
	bool parseArgument(const char* arg);
	// Return whether this file/format has any additional arguments to write
	bool hasAdditionalArguments() const;
	// Return additional arguments as string
	const char* additionalArguments() const;
};

#endif

