/*
	*** Import - Data2D
	*** src/io/import/data2d.h
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

#ifndef DISSOLVE_IMPORT_DATA2D_H
#define DISSOLVE_IMPORT_DATA2D_H

#include "base/fileandformat.h"
#include "base/enumoptions.h"
#include "templates/vector3.h"

// Forward Declarations
class Data2D;
class ProcessPool;

// Data2D Import Formats
class Data2DImportFileFormat : public FileAndFormat
{
	public:
	// Available Data2D formats
	enum Data2DImportFormat { CartesianData2D, nData2DImportFormats };
	// Constructor
	Data2DImportFileFormat(Data2DImportFormat format = CartesianData2D);
	// Destructor
	~Data2DImportFileFormat();


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
	// Return current format as Data2DImportFormat
	Data2DImportFormat data2DFormat() const;


	/*
	 * Additional Options
	 */
	public:
	// Additional Options
	enum AdditionalOption { XAxisOption, YAxisOption };
	// Return enum option info for AdditionalOptions
	static EnumOptions<Data2DImportFileFormat::AdditionalOption> additionalOptions();

	private:
	// Minima, maxima, and bin deltas for axes (if not directly obtainable from data
	Vec3<double> axisMinimum_, axisMaximum_, axisDelta_;
	// Whether any axis information has been set
	Vec3<bool> axisRangeSet_;

	protected:
	// Parse additional option
	bool parseOption(const char* arg);
	// Return whether this file/format has any additional options to write
	bool hasAdditionalOptions() const;
	// Return additional options as string
	const char* additionalOptionsAsString() const;


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
	 * Data Import
	 */
	private:
	// Import cartesian data from supplied parser
	bool importCartesian(LineParser& parser, Data2D& data);

	public:
	// Import Data2D using current filename and format
	bool importData(Data2D& data, ProcessPool* procPool = NULL);
	// Import Data2D using supplied parser and current format
	bool importData(LineParser& parser, Data2D& data);
};

#endif

