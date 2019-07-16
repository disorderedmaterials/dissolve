/*
	*** Export - Data2D
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

#ifndef DISSOLVE_EXPORT_DATA2D_H
#define DISSOLVE_EXPORT_DATA2D_H

#include "base/fileandformat.h"

// Forward Declarations
class Data2D;

// Data2D Export Formats
class Data2DExportFileFormat : public FileAndFormat
{
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
	// Constructor
	Data2DExportFileFormat(const char* filename = NULL, Data2DExportFormat format = Data2DExportFileFormat::BlockData);


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
	// Export Data2D as simple block data
	bool exportBlockData(LineParser& parser, Data2D& data);

	public:
	// Export Data2D using current filename and format
	bool exportData(Data2D& data);
};

#endif

