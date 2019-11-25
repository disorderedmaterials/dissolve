/*
	*** Export - Data1D
	*** src/io/export/data2d.h
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

#ifndef DISSOLVE_EXPORT_DATA1D_H
#define DISSOLVE_EXPORT_DATA1D_H

#include "io/fileandformat.h"

// Forward Declarations
class Data1D;

// Data1D Export Formats
class Data1DExportFileFormat : public FileAndFormat
{
	public:
	// Available data formats
	enum Data1DExportFormat { XYData, nData1DExportFormats };
	// Return number of available formats
	int nFormats() const;
	// Return formats array
	const char** formats() const;
	// Return nice formats array
	const char** niceFormats() const;
	// Return current format as Data1DExportFormat
	Data1DExportFormat data1DFormat() const;
	// Constructor
	Data1DExportFileFormat(const char* filename = NULL, Data1DExportFormat format = Data1DExportFileFormat::XYData);


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
	// Export Data1D as simple XY (or XYE) data
	bool exportXY(LineParser& parser, const Data1D& data);

	public:
	// Export Data1D using current filename and format
	bool exportData(const Data1D& data);
};

#endif

