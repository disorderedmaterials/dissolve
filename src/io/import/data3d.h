/*
	*** Import - Data3D
	*** src/io/import/data3d.h
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

#ifndef DISSOLVE_IMPORT_DATA3D_H
#define DISSOLVE_IMPORT_DATA3D_H

#include "io/fileandformat.h"

// Forward Declarations
class Data3D;
class ProcessPool;

// Data3D Import Formats
class Data3DImportFileFormat : public FileAndFormat
{
	public:
	// Available Data3D formats
	enum Data3DImportFormat { CartesianData3D, nData3DImportFormats };
	// Constructor
	Data3DImportFileFormat(Data3DImportFormat format = CartesianData3D);
	// Destructor
	~Data3DImportFileFormat();
	// Copy Constructor
	Data3DImportFileFormat(const Data3DImportFileFormat& source);
	// Assignment Operator
	void operator=(const Data3DImportFileFormat& source);


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
	// Return current format as Data3DImportFormat
	Data3DImportFormat data3DFormat() const;


	/*
	 * Templating
	 */
	private:
	// Object tag of Data3D upon which to template arrays before importing
	CharString templateSourceObjectTag_;


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
	 * Import / Write
	 */
	protected:
	// Parse additional argument
	bool parseArgument(const char* arg);
	// Return whether this file/format has any additional arguments to write
	bool hasAdditionalArguments() const;
	// Return additional arguments as string
	const char* additionalArguments() const;


	/*
	 * Data Import
	 */
	private:
	// Import cartesian data from supplied parser
	bool importCartesian(LineParser& parser, Data3D& data);

	public:
	// Import Data3D using current filename and format
	bool importData(Data3D& data, ProcessPool* procPool = NULL);
	// Import Data3D using supplied parser and current format
	bool importData(LineParser& parser, Data3D& data);
};

#endif

