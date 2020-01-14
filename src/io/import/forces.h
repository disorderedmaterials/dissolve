/*
	*** Import - Forces
	*** src/io/import/forces.h
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

#ifndef DISSOLVE_IMPORT_FORCES_H
#define DISSOLVE_IMPORT_FORCES_H

#include "io/fileandformat.h"
#include "templates/array.h"

// Forward Declarations
class ProcessPool;

// Forces Import Formats
class ForceImportFileFormat : public FileAndFormat
{
	public:
	// Forces Formats
	enum ForceImportFormat { XYZForces, DLPOLYForces, nForceImportFormats };
	// Constructor
	ForceImportFileFormat(ForceImportFormat format = XYZForces);
	// Destructor
	~ForceImportFileFormat();


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


	/*
	 * Import Functions
	 */
	private:
	// Import XYZ forces through specified parser
	bool importXYZ(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
	// Import DL_POLY forces through specified parser
	bool importDLPOLY(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);

	public:
	// Import forces using current filename and format
	bool importData(Array<double>& fx, Array<double>& fy, Array<double>& fz, ProcessPool* procPool = NULL);
	// Import forces using supplied parser and current format
	bool importData(LineParser& parser, Array<double>& fx, Array<double>& fy, Array<double>& fz);
};

#endif

